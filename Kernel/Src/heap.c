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
  * @brief  �ڴ������¼
  */
struct __mem_entry
{
    uint32_t				size;//�ڴ��С
    unsigned long			magic;//���������ڱ�����ĸ�task������ڴ�
	struct __mem_entry		*next;
};

/* Private define ------------------------------------------------------------*/
#define MAX_SHEAP_PER_TASK      ((uint32_t)(1024*6)) //ÿ�������������ר���ڴ���
#define MAX_DHEAP               ((uint32_t)(1024*8)) //������Ķ�ʱ�ڴ���

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __mem_entry *slist = (struct __mem_entry *)0;
static struct __mem_entry *dlist = (struct __mem_entry *)0;
static uint8_t lock = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ר�ö�̬�ڴ��ʼ��
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
  * @brief  ��ʱ��̬�ڴ��ʼ��
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
  * @brief  ר�ö�̬�ڴ����
  */
static void heap_unlock(enum __heap_unlock fun)
{
    lock = 0;
    lock |= fun;
}

/**
  * @brief  ר�ö�̬�ڴ�����
  */
static void heap_lock(void)
{
    lock = 0;
}

/**
  * @brief  ר�ö�̬�ڴ����
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

	magic = (unsigned long)task_trace.current();

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
  * @brief  �ڴ����
  */
const struct __heap_ctrl heap_ctrl = 
{
    .sinit              = heap_sinit,
    .dinit              = heap_dinit,
    .unlock             = heap_unlock,
    .lock               = heap_lock,
    .recycle            = heap_recycle,
};

/**
  * @brief  ר�ö�̬�ڴ����뺯����ֻ������ struct __task_sched ʵ���� init ������ loop ������
  */
static void *heap_salloc(uint32_t size)
{
    void *address;
    struct __mem_entry *p = slist;
    uint32_t mem_allcoked = 0;
    
    if((lock & HEAP_UNLOCK_ALLOC) != HEAP_UNLOCK_ALLOC)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (lock not available), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
    if(!task_trace.belong())
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (task id invalid), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
    if(!size)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (size invalid), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

        if(p->magic == (unsigned long)task_trace.belong())
        {
            mem_allcoked += p->size;
        }

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    if((mem_allcoked + size) > MAX_SHEAP_PER_TASK)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (heap overflow), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
    
    address = malloc(size + sizeof(struct __mem_entry));
    if(!address)
    {
        TRACE(TRACE_ERR,\
		"Heap salloc failed (system memory overflow), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

    ((struct __mem_entry *)address)->magic = (unsigned long)task_trace.belong();

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

	((struct __mem_entry *)address)->size = size;
    ((struct __mem_entry *)address)->next = slist;
    slist = ((struct __mem_entry *)address);
    
    TRACE(TRACE_INFO,\
	"Heap salloc success, magic: %08X, address: %08X, size: %08X.",\
	task_trace.belong(),\
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
  * @brief  ר�ö�̬�ڴ����뺯����ֻ������ struct __task_sched ʵ���� init ������ loop ������
  */
static void *heap_scalloc(uint32_t n, uint32_t size)
{
    return(heap_salloc(n * size));
}

/**
  * @brief  ��ʱ��̬�ڴ����뺯���������˳�����Զ��ͷ�
  */
static void *heap_dalloc(uint32_t size)
{
    void *address;
    struct __mem_entry *p = dlist;
    uint32_t mem_allcoked = 0;
    
    if(!size)
    {
        TRACE(TRACE_ERR,\
		"Heap dalloc failed (size invalid), magic: %08X.",\
		task_trace.belong());
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
		"Heap dalloc failed (heap overflow), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
    address = malloc(size + sizeof(struct __mem_entry));
    if(!address)
    {
        TRACE(TRACE_ERR,\
		"Heap dalloc failed (system memory overflow), magic: %08X.",\
		task_trace.belong());
        return((void *)0);
    }
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

    ((struct __mem_entry *)address)->magic = (unsigned long)task_trace.belong() + 3;

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

	((struct __mem_entry *)address)->size = size;
    ((struct __mem_entry *)address)->next = dlist;
    dlist = ((struct __mem_entry *)address);
    
    TRACE(TRACE_INFO,\
	"Heap dalloc success, magic: %08X, address: %08X, size: %08X.",\
	task_trace.belong(),\
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
  * @brief  ��ʱ��̬�ڴ����뺯���������˳�����Զ��ͷ�
  */
static void *heap_dcalloc(uint32_t n, uint32_t size)
{
    return(heap_dalloc(n * size));
}

/**
  * @brief  ��̬�ڴ��ͷź���
  *         1 ʹ�� heap_salloc ������ڴ�ֻ���� struct __task_sched ʵ���� exit �������ֶ��ͷţ�����
  *         ��ϵͳ���ø� task �� exit ���� ���� reset ��������ϵͳ�Զ�����
  *         2 ʹ�� heap_dalloc ������ڴ������κεط��ֶ��ͷ�
  */
static void heap_free(void *address)
{
    struct __mem_entry *p;
    struct __mem_entry *previous;
    unsigned long magic;
    
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
			"Heap dalloc memory freed manually, magic: %08X, address: %08X, size: %08X.",\
			p->magic,\
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
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
    
    if((lock & HEAP_UNLOCK_FREE) != HEAP_UNLOCK_FREE)
    {
        return;
    }
    
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

	magic = (unsigned long)task_trace.belong();
    
    if(!magic)
    {
        return;
    }
    
    p = slist;
    previous = slist;
    
    while(p)
    {
        if((unsigned long)address >= ((unsigned long)p + sizeof(struct __mem_entry)) && \
		(unsigned long)address < ((unsigned long)p + sizeof(struct __mem_entry) + p->size) && \
		((p->magic == magic) || (p->magic == 0)))
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
  * @brief  ��ȫ�ڴ濽����������ֹ�ڴ������¼��Ϣ struct __mem_entry ������
  */
static uint32_t heap_copy(void *dst, const void *src, uint32_t count)
{
    struct __mem_entry *p = slist;
    
    if(!dst || !src || !count)
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
		(((unsigned long)dst + count) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap copy address conflict, magic: %08X.",\
			task_trace.belong());
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    p = dlist;
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

		if((((unsigned long)dst) < ((unsigned long)p + sizeof(struct __mem_entry))) && \
		(((unsigned long)dst + count) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap copy address conflict, magic: %08X.",\
			task_trace.belong());
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    memcpy(dst, src, count);
    
    return(count);
}

/**
  * @brief  ��ȫ�ڴ��ʼ����������ֹ�ڴ������¼��Ϣ struct __mem_entry ������
  */
static uint32_t heap_set(void *address, uint8_t ch, uint32_t count)
{
    struct __mem_entry *p = slist;
    
    if(!address || !count)
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
		(((unsigned long)address + count) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap set address conflict, magic: %08X.",\
			task_trace.belong());
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    p = dlist;
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

		if((((unsigned long)address) < ((unsigned long)p + sizeof(struct __mem_entry))) && \
		(((unsigned long)address + count) > ((unsigned long)p)))
		{
            TRACE(TRACE_ERR,\
			"Heap set address conflict, magic: %08X.",\
			task_trace.belong());
			return(0);
		}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
        p = p->next;
    }
    
    memset(address, ch, count);
    
    return(count);
}

/**
  * @brief   
  */
static uint32_t heap_szone_size(void)
{
    return(MAX_SHEAP_PER_TASK);
}

/**
  * @brief   
  */
static uint32_t heap_szone_available(void)
{
    struct __mem_entry *p = slist;
    uint32_t mem_allcoked = 0;
    
    while(p)
    {
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

        if(p->magic == (unsigned long)task_trace.belong())
        {
            mem_allcoked += p->size;
        }

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

        p = p->next;
    }
    
    if(mem_allcoked <= MAX_SHEAP_PER_TASK)
    {
        return(MAX_SHEAP_PER_TASK - mem_allcoked);
    }
    else
    {
        return(0);
    }
}

/**
  * @brief   
  */
static uint32_t heap_dzone_size(void)
{
    return(MAX_DHEAP);
}

/**
  * @brief   
  */
static uint32_t heap_dzone_available(void)
{
    struct __mem_entry *p = dlist;
    uint32_t mem_allcoked = 0;
    
    while(p)
    {
        mem_allcoked += p->size;
        p = p->next;
    }
    
    if(mem_allcoked <= MAX_DHEAP)
    {
        return(MAX_DHEAP - mem_allcoked);
    }
    else
    {
        return(0);
    }
}

/**
  * @brief  �ڴ��������
  */
struct __heap heap = 
{
    .salloc             = heap_salloc,
    .scalloc            = heap_scalloc,
    .dalloc             = heap_dalloc,
    .dcalloc            = heap_dcalloc,
    .free               = heap_free,
    .copy               = heap_copy,
    .set                = heap_set,
    .szone              = 
    {
        .size           = heap_szone_size,
        .available      = heap_szone_available,
    },
    .dzone              = 
    {
        .size           = heap_dzone_size,
        .available      = heap_dzone_available,
    },
};
