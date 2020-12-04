/**
 * @brief		
 * @details		
 * @date		2016-08-13
 **/

/* Includes ------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "allocator.h"
#include "allocator_ctrl.h"
#include "tasks.h"
#include "trace.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  内存申请记录
  */
struct __mem_entry
{
    uint32_t				size;//内存大小
    unsigned long			magic;//幻数，用于标记是哪个task申请的内存
	struct __mem_entry		*next;
};

/* Private define ------------------------------------------------------------*/
#define MAX_SHEAP_PER_TASK      ((uint32_t)(1024*6)) //每个任务最大分配的专用内存数
#define MAX_DHEAP               ((uint32_t)(1024*8)) //最大分配的短时内存数

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __mem_entry *slist = (struct __mem_entry *)0;
static struct __mem_entry *dlist = (struct __mem_entry *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  专用动态内存初始化
  */
static void heap_sinit(void)
{
    struct __mem_entry *p;
    
    while(slist)
    {
        p = slist;
        slist = slist->next;
        free(p);
    }
}

/**
  * @brief  短时动态内存初始化
  */
static void heap_dinit(void)
{
    struct __mem_entry *p;
    
    while(dlist)
    {
        p = dlist;
        dlist = dlist->next;
        free(p);
    }
}

/**
  * @brief  专用动态内存回收
  */
static void heap_recycle(void)
{
    struct __mem_entry *p = slist;
    struct __mem_entry *previous = slist;
    unsigned long magic;
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

	magic = (unsigned long)task_ctrl.current();

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
    
    if(!magic)
    {
        return;
    }
    
    while(p)
    {
        if((p->magic == magic) || (p->magic == 0))
        {
        	TRACE(TRACE_INFO,\
			"Heap salloc memory freed automatically, magic: %08X, address: %08X, size: %08X.",\
			p->magic,\
            p,\
            p->size + sizeof(struct __mem_entry));
            
        	if(p != slist)
        	{
            	previous->next = p->next;
	            free(p);
	            p = previous->next;
        	}
        	else
        	{
        		slist = p->next;
        		previous = slist;
	            free(p);
	            p = slist;
        	}
        }
        else
        {
        	previous = p;
            p = p->next;
        }
    }
}

/**
  * @brief  内存控制
  */
const struct __heap_ctrl heap_ctrl = 
{
    .sinit              = heap_sinit,
    .dinit              = heap_dinit,
    .recycle            = heap_recycle,
};

/**
  * @brief  专用动态内存申请函数，会在该任务退出（exit）或者复位（exit）时自动释放
  */
static void *heap_salloc(const char *name, uint32_t size)
{
    void *address;
    struct __mem_entry *p = slist;
    uint32_t mem_allcoked = 0;
    unsigned long magic;
    
    if(!name)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (name invalid).");
        return((void *)0);
    }
    
    if(!size)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (size invalid), task name is: %s.",\
		name);
        return((void *)0);
    }
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif
    
    magic = (unsigned long)task_ctrl.search(name);
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
    
    if(!magic)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (task not found), task name is: %s.",\
		name);
        return((void *)0);
    }
    
    while(p)
    {
        if(p->magic == magic)
        {
            mem_allcoked += p->size;
        }
        
        p = p->next;
    }
    
    if((mem_allcoked + size) > MAX_SHEAP_PER_TASK)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (heap overflow), task name is: %s.",\
		name);
        return((void *)0);
    }
    
    
    address = malloc(size + sizeof(struct __mem_entry));
    if(!address)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (system memory overflow), task name is: %s.",\
		name);
        return((void *)0);
    }

    ((struct __mem_entry *)address)->magic = magic;
	((struct __mem_entry *)address)->size = size;
    ((struct __mem_entry *)address)->next = slist;
    slist = ((struct __mem_entry *)address);
    
    TRACE(TRACE_INFO,\
	"Heap salloc success, task name: %s, address: %08X, size: %08X.",\
	name,\
	address,\
	size + sizeof(struct __mem_entry));

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

    return((void *)(((unsigned long)address + sizeof(struct __mem_entry))));

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
}

/**
  * @brief  
  */
static void *heap_szalloc(const char *name, uint32_t size)
{
	void *result = heap_salloc(name, size);
	
	if(!result)
	{
		memset(result, 0, size);
	}
	
	return(result);
}

/**
  * @brief  专用动态内存申请函数，会在该任务退出（exit）或者复位（exit）时自动释放
  */
static void *heap_scalloc(const char *name, uint32_t n, uint32_t size)
{
    return(heap_salloc(name, n * size));
}

/**
  * @brief  短时动态内存申请函数，会在该任务轮询完成后自动释放
  */
static void *heap_dalloc(uint32_t size)
{
    void *address;
    struct __mem_entry *p = dlist;
    uint32_t mem_allcoked = 0;
    
    if(!size)
    {
        TRACE(TRACE_ERR,\
		"Heap dalloc failed (size invalid).");
        return((void *)0);
    }
    
    while(p)
    {
        mem_allcoked += p->size;
        p = p->next;
    }
    
    if((mem_allcoked + size) > MAX_DHEAP)
    {
        TRACE(TRACE_ERR,\
		"Heap dalloc failed (heap overflow).");
        return((void *)0);
    }
    
    address = malloc(size + sizeof(struct __mem_entry));
    if(!address)
    {
        TRACE(TRACE_ERR,\
		"Heap dalloc failed (system memory overflow).");
        return((void *)0);
    }
    
    ((struct __mem_entry *)address)->magic = 0;
	((struct __mem_entry *)address)->size = size;
    ((struct __mem_entry *)address)->next = dlist;
    dlist = ((struct __mem_entry *)address);
    
    TRACE(TRACE_INFO,\
	"Heap dalloc success, address: %08X, size: %08X.",\
	address,\
	size + sizeof(struct __mem_entry));

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

    return((void *)(((unsigned long)address + sizeof(struct __mem_entry))));

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
}

/**
  * @brief  
  */
static void *heap_dzalloc(uint32_t size)
{
	void *result = heap_dalloc(size);
	
	if(!result)
	{
		memset(result, 0, size);
	}
	
	return(result);
}

/**
  * @brief  短时动态内存申请函数，会在该任务轮询完成后自动释放
  */
static void *heap_dcalloc(uint32_t n, uint32_t size)
{
    return(heap_dalloc(n * size));
}

/**
  * @brief  动态内存释放函数
  */
static void heap_free(void *address)
{
    struct __mem_entry *p;
    struct __mem_entry *previous;
    
    if(!address)
    {
    	return;
	}
    
    p = dlist;
    previous = dlist;
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif
    
    while(p)
    {
        if((unsigned long)address >= ((unsigned long)p + sizeof(struct __mem_entry)) && \
		(unsigned long)address < ((unsigned long)p + sizeof(struct __mem_entry) + p->size))
        {
            TRACE(TRACE_INFO,\
			"Heap dalloc memory freed manually, address: %08X, size: %08X.",\
			p,\
			p->size + sizeof(struct __mem_entry));
            
        	if(p != dlist)
        	{
            	previous->next = p->next;
	            free(p);
	            p = previous->next;
                return;
        	}
        	else
        	{
        		dlist = p->next;
        		previous = dlist;
	            free(p);
	            p = dlist;
                return;
        	}
        }
        else
        {
        	previous = p;
            p = p->next;
        }
    }
    
    p = slist;
    previous = slist;
    
    while(p)
    {
        if((unsigned long)address >= ((unsigned long)p + sizeof(struct __mem_entry)) && \
		(unsigned long)address < ((unsigned long)p + sizeof(struct __mem_entry) + p->size))
        {
            TRACE(TRACE_INFO,\
			"Heap salloc memory freed manually, magic: %08X, address: %08X, size: %08X.",\
			p->magic,\
			p,\
			p->size + sizeof(struct __mem_entry));
            
        	if(p != slist)
        	{
            	previous->next = p->next;
	            free(p);
	            p = previous->next;
        	}
        	else
        	{
        		slist = p->next;
        		previous = slist;
	            free(p);
	            p = slist;
        	}
        }
        else
        {
        	previous = p;
            p = p->next;
        }
    }
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

}

/**
  * @brief  安全内存拷贝函数，防止内存申请记录信息 struct __mem_entry 被覆盖
  */
static uint32_t heap_copy(void *dst, const void *src, uint32_t size)
{
    struct __mem_entry *p = dlist;
    
    if(!dst || !src || !size)
    {
    	return(0);
	}
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

		if((((unsigned long)dst) < ((unsigned long)p + sizeof(struct __mem_entry))) && \
		(((unsigned long)dst + size) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap copy address conflict.");
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    p = slist;
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

		if((((unsigned long)dst) < ((unsigned long)p + sizeof(struct __mem_entry))) && \
		(((unsigned long)dst + size) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap copy address conflict.");
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    memcpy(dst, src, size);
    
    return(size);
}

/**
  * @brief  安全内存初始化函数，防止内存申请记录信息 struct __mem_entry 被覆盖
  */
static uint32_t heap_set(void *address, uint8_t ch, uint32_t size)
{
    struct __mem_entry *p = dlist;
    
    if(!address || !size)
    {
    	return(0);
	}
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

		if((((unsigned long)address) < ((unsigned long)p + sizeof(struct __mem_entry))) && \
		(((unsigned long)address + size) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap set address conflict.");
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    p = slist;
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

		if((((unsigned long)address) < ((unsigned long)p + sizeof(struct __mem_entry))) && \
		(((unsigned long)address + size) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap set address conflict.");
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    memset(address, ch, size);
    
    return(size);
}

/**
  * @brief  内存操作函数
  */
struct __heap heap = 
{
    .salloc             = heap_salloc,
    .szalloc			= heap_szalloc,
    .scalloc            = heap_scalloc,
    .dalloc             = heap_dalloc,
    .dzalloc			= heap_dzalloc,
    .dcalloc            = heap_dcalloc,
    .free               = heap_free,
    .copy               = heap_copy,
    .set                = heap_set,
};
