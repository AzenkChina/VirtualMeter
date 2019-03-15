/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "tasks.h"
#include "string.h"
#include "trace.h"
#include "cpu.h"
#include "allocator_ctrl.h"
#include "api.h"

#if defined ( __TASKS_MONITOR )
#include "jiffy.h"
#include "allocator.h"
#endif

/** 在这里添加所有tasks的头文件 */
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include "task_console.h"
#endif
#include "task_calendar.h"
#include "task_comm.h"
#include "task_protocol.h"
#include "task_timed.h"
#include "task_metering.h"
#include "task_display.h"
#include "task_keyboard.h"
#include "task_xmodem.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  任务列表
  */
struct __task_table
{
    struct __task_order
    {
        uint8_t                     init;//初始化顺序
        uint8_t                     exit;//掉电顺序
        uint8_t                     reset;//复位顺序
        uint8_t                     loop;//轮询顺序
        
    }                               order;
    
    const struct    __task_sched    *const task;
};

/* Private define ------------------------------------------------------------*/
#define TASKS_MAX                   ((uint8_t)(128)) //最大task数量
#define API_STACK_MAX				((uint8_t)(16)) //最大api调用深度

/* Private macro -------------------------------------------------------------*/
#define TASKS_COMP      ((uint8_t)(sizeof(task_tables) / sizeof(struct __task_table)))
#define TASK_AMOUNT     ((TASKS_COMP > TASKS_MAX)? TASKS_MAX : TASKS_COMP)

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  注册到系统中的应用，最多 TASKS_MAX 个
  */
static const struct __task_table task_tables[] = 
{
    //内容依次为
    //初始化顺序（（1~254）越小优先级越高）
    //掉电顺序（（1~254）越小优先级越高）
    //复位顺序（（1~254）越小优先级越高）
    //轮询顺序（（1~254）越小优先级越高）
    //任务控制块
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	{ {0xfe, 0xfe, 0xfe, 0xfe}, &task_console },
#endif
    
    /** 在这里添加所有tasks的结构体地址 */
    { {0x03, 0x02, 0x02, 0xfe}, &task_timed },
    { {0x04, 0x01, 0x02, 0xfe}, &task_metering },
    { {0x05, 0x05, 0x02, 0xfe}, &task_display },
    { {0x06, 0x04, 0x02, 0xfe}, &task_keyboard },
    { {0x10, 0x10, 0x02, 0xfe}, &task_protocol },
    { {0x11, 0x11, 0x02, 0xfe}, &task_comm },
    { {0x12, 0x12, 0x02, 0xfe}, &task_xmodem },
};

static uint16_t task_id = 0xffff;
static uint16_t api_id = 0xffff;
static uint16_t api_stack[API_STACK_MAX] = { 0 };
static uint8_t api_stack_top = 0;
static uint8_t task_hang[TASKS_COMP] = {0};

#if defined ( __TASKS_MONITOR )
static uint32_t timing = 0;
static uint16_t usage_init[TASK_AMOUNT] = {0};
static uint16_t usage_exit[TASK_AMOUNT] = {0};
static uint16_t usage_loop[TASK_AMOUNT] = {0};
static uint16_t usage_loop_max[TASK_AMOUNT] = {0};
#endif


/* Private function prototypes -----------------------------------------------*/
#if defined ( __TASKS_MONITOR )
static void __monitor_start(uint8_t index);
static void __monitor_stop_init(uint8_t index);
static void __monitor_stop_exit(uint8_t index);
static void __monitor_stop_loop(uint8_t index);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static void tasks_init(void)
{
    uint8_t loop;
    uint8_t cnt;
    
    cpu.watchdog.feed();
    heap_ctrl.sinit();
    heap_ctrl.dinit();
    
    cpu.watchdog.feed();
    disk_ctrl.start();
    
    TRACE(TRACE_INFO, "Tasks initializing.");
    
    for(loop = 1; loop < 255; loop ++)
    {
        for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
        {
            if(task_tables[cnt].order.init != loop)
            {
                continue;
            }
            
            if(task_tables[cnt].task && task_tables[cnt].task->init)
            {
                task_id = cnt;
				api_id = task_id;
				api_stack_top = 0;
                cpu.watchdog.feed();
#if defined ( __TASKS_MONITOR )
                __monitor_start(cnt);
#endif
                heap_ctrl.unlock(HEAP_UNLOCK_ALLOC);
                disk_ctrl.unlock();
                task_tables[cnt].task->init();
                heap_ctrl.lock();
                disk_ctrl.lock();
#if defined ( __TASKS_MONITOR )
                __monitor_stop_init(cnt);
#endif
                task_id = 0xffff;
				api_id = task_id;
                
                task_hang[cnt] = 0;
            }
        }
    }
    
    TRACE(TRACE_INFO, "Tasks initialized.");
}

/**
  * @brief  
  */
static void tasks_loop(void)
{
    uint8_t loop;
    uint8_t cnt;
    
    cpu.watchdog.feed();
    heap_ctrl.dinit();
    
    for(loop = 1; loop < 255; loop ++)
    {
        for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
        {
            if(task_tables[cnt].order.loop != loop)
            {
                continue;
            }
            
            if(task_tables[cnt].task->status)
            {
                enum  __task_status task_status = task_tables[cnt].task->status();
                
                if((task_status != TASK_INIT) && (task_status != TASK_RUN))
                {
                    continue;
                }
            }
            
            if(task_hang[cnt] == 0x8f)
            {
            	continue;
            }
            
            if(task_tables[cnt].task && task_tables[cnt].task->loop)
            {
                task_id = cnt;
				api_id = task_id;
				api_stack_top = 0;
                cpu.watchdog.feed();
#if defined ( __TASKS_MONITOR )
                __monitor_start(cnt);
#endif
                heap_ctrl.unlock((enum __heap_unlock)(HEAP_UNLOCK_ALLOC | HEAP_UNLOCK_FREE));
                task_tables[cnt].task->loop();
                heap_ctrl.lock();
#if defined ( __TASKS_MONITOR )
                __monitor_stop_loop(cnt);
#endif
                task_id = 0xffff;
				api_id = task_id;
            }
        }
    }
}

/**
  * @brief  
  */
static void tasks_exit(void)
{
    uint16_t loop;
    uint8_t cnt;
    
    cpu.watchdog.feed();
    heap_ctrl.dinit();
    
    TRACE(TRACE_INFO, "Tasks quitting.");
    
    for(loop = 1; loop < 255; loop ++)
    {
        for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
        {
            if(task_tables[cnt].order.exit != loop)
            {
                continue;
            }
            
            if(task_tables[cnt].task && task_tables[cnt].task->exit)
            {
                task_id = cnt;
				api_id = task_id;
				api_stack_top = 0;
                cpu.watchdog.feed();
#if defined ( __TASKS_MONITOR )
                __monitor_start(cnt);
#endif
                heap_ctrl.unlock(HEAP_UNLOCK_FREE);
                task_tables[cnt].task->exit();
                heap_ctrl.lock();
                heap_ctrl.recycle();
#if defined ( __TASKS_MONITOR )
                __monitor_stop_exit(cnt);
#endif
                task_id = 0xffff;
				api_id = task_id;
            }
        }
    }
    
    cpu.watchdog.feed();
    disk_ctrl.idle();
    
    TRACE(TRACE_INFO, "Tasks quitted.");
}

/**
  * @brief  
  */
static void tasks_reset(void)
{
    uint8_t loop;
    uint8_t cnt;
    
    cpu.watchdog.feed();
    disk_ctrl.format();
    cpu.watchdog.feed();
    heap_ctrl.dinit();
    
    TRACE(TRACE_INFO, "Tasks resetting.");
    
    for(loop = 1; loop < 255; loop ++)
    {
        for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
        {
            if(task_tables[cnt].order.reset != loop)
            {
                continue;
            }
            
            if(task_tables[cnt].task && task_tables[cnt].task->reset)
            {
                task_id = cnt;
				api_id = task_id;
				api_stack_top = 0;
                cpu.watchdog.feed();
                disk_ctrl.unlock();
                task_tables[cnt].task->reset();
                disk_ctrl.lock();
                heap_ctrl.recycle();
                task_id = 0xffff;
				api_id = task_id;
            }
        }
    }
    
    TRACE(TRACE_INFO, "Tasks reset.");
}

/**
  * @brief  
  */
static enum __task_status tasks_status(void)
{
    return(TASK_RUN);
}



/**
  * @brief  
  */
const struct __task_sched tasks = 
{
    .name           = "task controler",
    .init           = tasks_init,
    .loop           = tasks_loop,
    .exit           = tasks_exit,
    .reset          = tasks_reset,
    .status         = tasks_status,
    .api            = (void *)0,
};

/**
  * @brief  
  */
static enum  __sched_status task_sched_hang(const char *name)
{
    uint8_t cnt;
   
    for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
    {
        if(!task_tables[cnt].task->name)
        {
            continue;
        }
		
        if(strcmp(task_tables[cnt].task->name, name) != 0)
        {
            continue;
        }
        
        task_hang[cnt] = 0x8f;
        
        return(SCHED_HANGED);
    }
    
    return(SCHED_NODEF);
}

/**
  * @brief  
  */
static enum  __sched_status task_sched_reset(const char *name)
{
    uint8_t cnt;
	uint16_t task_id_backup;
	uint16_t api_id_backup;
   
    for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
    {
        if(!task_tables[cnt].task->name)
        {
            continue;
        }
		
        if(strcmp(task_tables[cnt].task->name, name) != 0)
        {
            if(task_tables[cnt].task && task_tables[cnt].task->reset && task_tables[cnt].task->init)
            {
				task_id_backup = task_id;
				api_id_backup = api_id;
				
				task_id = cnt;
				api_id = task_id;
				api_stack_top = 0;
				disk_ctrl.unlock();
				cpu.watchdog.feed();
				task_tables[cnt].task->reset();
				heap_ctrl.recycle();
                heap_ctrl.unlock(HEAP_UNLOCK_ALLOC);
				cpu.watchdog.feed();
                task_tables[cnt].task->init();
                heap_ctrl.lock();
				disk_ctrl.lock();
				
				task_id = task_id_backup;
				api_id = api_id_backup;
            }
        }
    }
    
    return(SCHED_NODEF);
}

/**
  * @brief  
  */
static enum  __sched_status task_sched_run(const char *name)
{
    uint8_t cnt;
   
    for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
    {
        if(!task_tables[cnt].task->name)
        {
            continue;
        }
		
        if(strcmp(task_tables[cnt].task->name, name) != 0)
        {
            continue;
        }
        
        task_hang[cnt] = 0;
        
        return(SCHED_RUNNING);
    }
    
    return(SCHED_NODEF);
}

/**
  * @brief  
  */
static enum  __sched_status task_sched_status(const char *name)
{
    uint8_t cnt;
   
    for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
    {
        if(!task_tables[cnt].task->name)
        {
            continue;
        }
		
        if(strcmp(task_tables[cnt].task->name, name) != 0)
        {
            continue;
        }
        
        if(task_hang[cnt] != 0x8f)
        {
            return(SCHED_RUNNING);
        }
        else
        {
            return(SCHED_HANGED);
        }
    }
    
    return(SCHED_NODEF);
}

/**
  * @brief  
  */
const struct __task_ctrl task_ctrl = 
{
    .hang           = task_sched_hang,
	.reset			= task_sched_reset,
    .run            = task_sched_run,
    .status         = task_sched_status,
};



/**
  * @brief  
  */
static uint8_t task_amount(void)
{
    return(TASK_AMOUNT);
}

/**
  * @brief  
  */
static const struct __task_sched *task_current(void)
{
    if(task_id < TASK_AMOUNT)
    {
        return(task_tables[task_id].task);
    }
    
    return((const struct __task_sched *)0);
}

/**
  * @brief  
  */
static const struct __task_sched *task_belong(void)
{
    if(api_id < TASK_AMOUNT)
    {
        return(task_tables[api_id].task);
    }
    
    return((const struct __task_sched *)0);
}

/**
  * @brief  
  */
static uint8_t task_exist(const char *name)
{
    uint8_t cnt;
   
    for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
    {
        if(strcmp(task_tables[cnt].task->name, name) != 0)
        {
            continue;
        }
        
        return(0xff);
    }
    
    return(0);
}

#if defined ( __TASKS_MONITOR )
static void __monitor_start(uint8_t index)
{
    timing = jiffy.value();
}

static void __monitor_stop_init(uint8_t index)
{
    uint32_t usage_ms;
    
    usage_ms = jiffy.after(timing);
    
    if(usage_ms > 9999)
    {
        usage_ms = 9999;
    }
    
    usage_init[index] = usage_ms;
    usage_exit[index] = 0;
    usage_loop[index] = 0;
}

static void __monitor_stop_exit(uint8_t index)
{
    uint32_t usage_ms;
    
    usage_ms = jiffy.after(timing);
    
    if(usage_ms > 9999)
    {
        usage_ms = 9999;
    }
    
    usage_exit[index] = usage_ms;
}

static void __monitor_stop_loop(uint8_t index)
{
    uint32_t usage_ms;
    
    usage_ms = jiffy.after(timing);
    
    if(usage_ms > 9999)
    {
        usage_ms = 9999;
    }
    
    usage_loop[index] = usage_ms;
    
    if(usage_loop[index] > usage_loop_max[index])
    {
        usage_loop_max[index] = usage_loop[index];
    }
}

/**
  * @brief  输出task信息
  *         格式：task_id task_name task_status init_seq       exit_seq   reset_seq  loop_seq    init_ms     exit_ms  loop_ms  loop_max_ms
  *               任务ID  任务名称  任务状态    初始化优先级   退出优先级 重置优先级 轮询优先级   初始化用时 退出用时 轮询用时 轮询最大用时
  */
static const char *task_info(uint8_t index)
{
    static uint8_t task_info[128];
    uint8_t *pinfo = task_info;
    
    heap.set((void *)task_info, 0x20, sizeof(task_info));
    
    if(index >= 100)
    {
        *(pinfo ++) = ((index / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(index >= 10)
    {
        *(pinfo ++) = ((index / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((index / 1) % 10) + '0';
    
    pinfo += 4;
    
    if(index >= TASK_AMOUNT)
    {
        *(pinfo ++) = 'E';
        *(pinfo ++) = 'R';
        *(pinfo ++) = 'R';
        *(pinfo ++) = 'O';
        *(pinfo ++) = 'R';
        *(pinfo ++) = 0x0a;
        *(pinfo ++) = 0;
        
        return((char *)task_info);
    }
    
    if(strlen(task_tables[index].task->name) > 16)
    {
        heap.copy((void *)pinfo, (const void *)task_tables[index].task->name, 16);
    }
    else
    {
        heap.copy((void *)pinfo, (const void *)task_tables[index].task->name, strlen(task_tables[index].task->name));
    }
    
    pinfo += 16;
    pinfo += 4;
    
    if(task_tables[index].task->status)
    {
    	enum  __task_status task_status = task_tables[index].task->status();
    	
    	if(task_status == TASK_NOTINIT)
    	{
    		*(pinfo ++) = 'N';
    	}
    	else if(task_status == TASK_INIT)
    	{
    		*(pinfo ++) = 'I';
    	}
    	else if(task_status == TASK_RUN)
    	{
            if(task_hang[index] != 0x8f)
            {
                *(pinfo ++) = 'R';
            }
            else
            {
                *(pinfo ++) = 'H';
            }
    	}
    	else if(task_status == TASK_SUSPEND)
    	{
    		*(pinfo ++) = 'S';
    	}
    	else if(task_status == TASK_ERROR)
    	{
    		*(pinfo ++) = 'E';
    	}
		else
	    {
	    	*(pinfo ++) = 'U';
	    }
    }
    else
    {
    	*(pinfo ++) = 'U';
    }
    pinfo += 4;
    
    if(task_tables[index].order.init >= 100)
    {
        *(pinfo ++) = ((task_tables[index].order.init / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(task_tables[index].order.init >= 10)
    {
        *(pinfo ++) = ((task_tables[index].order.init / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((task_tables[index].order.init / 1) % 10) + '0';
    pinfo += 4;
    
    if(task_tables[index].order.exit >= 100)
    {
        *(pinfo ++) = ((task_tables[index].order.exit / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(task_tables[index].order.exit >= 10)
    {
        *(pinfo ++) = ((task_tables[index].order.exit / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((task_tables[index].order.exit / 1) % 10) + '0';
    pinfo += 4;
    
    if(task_tables[index].order.reset >= 100)
    {
        *(pinfo ++) = ((task_tables[index].order.reset / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(task_tables[index].order.reset >= 10)
    {
        *(pinfo ++) = ((task_tables[index].order.reset / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((task_tables[index].order.reset / 1) % 10) + '0';
    pinfo += 4;
    
    if(task_tables[index].order.loop >= 100)
    {
        *(pinfo ++) = ((task_tables[index].order.loop / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(task_tables[index].order.loop >= 10)
    {
        *(pinfo ++) = ((task_tables[index].order.loop / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((task_tables[index].order.loop / 1) % 10) + '0';
    pinfo += 4;
    
    if(usage_init[index] >= 1000)
    {
        *(pinfo ++) = ((usage_init[index] / 1000) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_init[index] >= 100)
    {
        *(pinfo ++) = ((usage_init[index] / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_init[index] >= 10)
    {
        *(pinfo ++) = ((usage_init[index] / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((usage_init[index] / 1) % 10) + '0';
    pinfo += 4;
    
    if(usage_exit[index] >= 1000)
    {
        *(pinfo ++) = ((usage_exit[index] / 1000) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_exit[index] >= 100)
    {
        *(pinfo ++) = ((usage_exit[index] / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_exit[index] >= 10)
    {
        *(pinfo ++) = ((usage_exit[index] / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((usage_loop[index] / 1) % 10) + '0';
    pinfo += 4;
    
    if(usage_loop[index] >= 1000)
    {
        *(pinfo ++) = ((usage_loop[index] / 1000) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_loop[index] >= 100)
    {
        *(pinfo ++) = ((usage_loop[index] / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_loop[index] >= 10)
    {
        *(pinfo ++) = ((usage_loop[index] / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((usage_loop[index] / 1) % 10) + '0';
    pinfo += 4;
    
    if(usage_loop_max[index] >= 1000)
    {
        *(pinfo ++) = ((usage_loop_max[index] / 1000) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_loop_max[index] >= 100)
    {
        *(pinfo ++) = ((usage_loop_max[index] / 100) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    if(usage_loop_max[index] >= 10)
    {
        *(pinfo ++) = ((usage_loop_max[index] / 10) % 10) + '0';
    }
    else
    {
    	pinfo ++;
    }
    *(pinfo ++) = ((usage_loop_max[index] / 1) % 10) + '0';
    
    *(pinfo ++) = 0x0a;
    *(pinfo ++) = 0;
    
    return((char *)task_info);
}

#else

static const char *task_info(uint8_t index)
{
    return("");
}

#endif

/**
  * @brief  
  */
const struct __task_trace task_trace = 
{
    .amount         = task_amount,
    .current        = task_current,
    .belong         = task_belong,
    .exist          = task_exist,
    .info           = task_info,
};



/**
  * @brief  
  */
static void * api_query(char *name)
{
    uint8_t cnt;
   
    for(cnt = 0; cnt < TASK_AMOUNT; cnt ++)
    {
        if(strcmp(task_tables[cnt].task->name, name) == 0)
        {
			if(api_stack_top >= API_STACK_MAX)
			{
				TRACE(TRACE_ERR, "Api call stack overflow.");
				api_id = cnt;
				return(task_tables[cnt].task->api);
			}

			api_stack[api_stack_top] = api_id;
			api_stack_top += 1;
			api_id = cnt;
            return(task_tables[cnt].task->api);
        }
    }
    
    return((void *)0);
}

/**
  * @brief  
  */
static void api_release(void)
{
	if((api_stack_top) && (api_stack_top < API_STACK_MAX))
	{
		api_stack_top -= 1;
		api_id = api_stack[api_stack_top];
	}
	else
	{
		TRACE(TRACE_ERR, "Api call stack pointer is invalid: %d.", api_stack_top);
		api_id = task_id;
	}
}

/**
  * @brief  
  */
const struct __task_api api = 
{
    .query              = api_query,
    .release            = api_release,
};
