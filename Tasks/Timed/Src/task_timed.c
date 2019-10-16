/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_timed.h"
#include "types_timed.h"
#include "config_timed.h"

#include "rtc.h"
#include "crc.h"

/* Private typedef -----------------------------------------------------------*/
struct __timed_entry
{
    uint16_t                        counter; //计时器
    struct      __timed_conf        conf; //保存定时器配置
    uint16_t                        check; //数据校验
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;
static struct __timed_entry *entries = (struct __timed_entry *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static enum __timd_status timed_create(const struct __timed_conf *conf)
{
	uint8_t cnt;
	 
	if((!conf) || (!conf->callback))
	{
		return(TIMD_NOEXIST);
	}
    
    if(!entries)
    {
        return(TIMD_CONFLICT);
    }
	
    for(cnt=0; cnt<TIMD_CONF_ENTRY_MAX; cnt++)
    {
        if(entries[cnt].conf.callback == conf->callback)
        {
        	return(TIMD_CONFLICT);
        }
    }
    
    for(cnt=0; cnt<TIMD_CONF_ENTRY_MAX; cnt++)
    {
        if(entries[cnt].conf.callback)
        {
            continue;
        }
        
        heap.set((void *)&entries[cnt], 0, sizeof(struct __timed_entry));
        heap.copy((void *)&entries[cnt].conf, (void *)conf, sizeof(entries[cnt].conf));
        entries[cnt].check = crc16((const uint8_t *)&entries[cnt], (sizeof(struct __timed_entry) - sizeof(entries[cnt].check)));
        
        return(TIMD_SUCCESS);
    }
    
    return(TIMD_NOEXIST);
}

/**
  * @brief  
  */
static enum __timd_status timed_query(const struct __timed_conf *conf)
{
	uint8_t cnt;
	
	if((!conf) || (!conf->callback))
	{
		return(TIMD_NOEXIST);
	}
    
    if(!entries)
    {
        return(TIMD_CONFLICT);
    }
	
    for(cnt=0; cnt<TIMD_CONF_ENTRY_MAX; cnt++)
    {
        if(!entries[cnt].conf.callback)
        {
            continue;
        }
        
        if(entries[cnt].conf.callback != conf->callback)
        {
			continue;
        }
        
        if(entries[cnt].check != crc16((const uint8_t *)&entries[cnt], (sizeof(struct __timed_entry) - sizeof(entries[cnt].check))))
        {
            heap.set((void *)&entries[cnt], 0, sizeof(struct __timed_entry));
            continue;
        }
        
    	heap.copy((void *)conf, (void *)&entries[cnt].conf, sizeof(entries[cnt].conf));
    	
    	return(TIMD_SUCCESS);
    }
    
    return(TIMD_NOEXIST);
}

/**
  * @brief  
  */
static enum __timd_status timed_remove(const struct __timed_conf *conf)
{
	uint8_t cnt;
	
	if((!conf) || (!conf->callback))
	{
		return(TIMD_NOEXIST);
	}
    
    if(!entries)
    {
        return(TIMD_CONFLICT);
    }
	
    for(cnt=0; cnt<TIMD_CONF_ENTRY_MAX; cnt++)
    {
        if(!entries[cnt].conf.callback)
        {
            continue;
        }
        
        if(entries[cnt].conf.callback == conf->callback)
        {
            heap.set((void *)&entries[cnt], 0, sizeof(struct __timed_entry));
            
            return(TIMD_SUCCESS);
        }
    }
    
    return(TIMD_NOEXIST);
}

/**
  * @brief  
  */
static enum __timd_status timed_empty(void)
{
    if(!entries)
    {
        return(TIMD_CONFLICT);
    }
    
    heap.set((void *)entries, 0, (sizeof(struct __timed_entry)*TIMD_CONF_ENTRY_MAX));
    
    return(TIMD_SUCCESS);
}

/**
  * @brief  
  */
static const struct __timed timed = 
{
    .create				= timed_create,
    .query				= timed_query,
    .remove				= timed_remove,
    .empty				= timed_empty,
};






/**
  * @brief  
  */
static void timed_init(void)
{
    entries = (struct __timed_entry *)0;
    
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
        entries = heap.salloc(NAME_TIMED, sizeof(struct __timed_entry)*TIMD_CONF_ENTRY_MAX);
        if(!entries)
        {
            status = TASK_ERROR;
            return;
        }
        
        heap.set((void *)entries, 0, (sizeof(struct __timed_entry)*TIMD_CONF_ENTRY_MAX));
        
        status = TASK_INIT;
        
        rtc.control.init(DEVICE_NORMAL);
        
        TRACE(TRACE_INFO, "Task timed initialized.");
	}
}

/**
  * @brief  
  */
static void timed_loop(void)
{
    uint8_t cnt;
    
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
    	//...在这里判断秒变化
        
        if(!entries)
        {
            status = TASK_ERROR;
            return;
        }
        
	    for(cnt=0; cnt<TIMD_CONF_ENTRY_MAX; cnt++)
	    {
	        if(!entries[cnt].conf.callback)
	        {
	            continue;
	        }
	        
	        if(entries[cnt].check != crc16((const uint8_t *)&entries[cnt], (sizeof(struct __timed_entry)) - sizeof(entries[cnt].check)))
	        {
                heap.set((void *)&entries[cnt], 0, sizeof(struct __timed_entry));
	            continue;
	        }
	    }
        
        status = TASK_RUN;
	}
}

/**
  * @brief  
  */
static void timed_exit(void)
{
	status = TASK_SUSPEND;
    
    TRACE(TRACE_INFO, "Task timed exited.");
}

/**
  * @brief  
  */
static void timed_reset(void)
{
	status = TASK_NOTINIT;
    
    TRACE(TRACE_INFO, "Task timed reset.");
}

/**
  * @brief  
  */
static enum __task_status timed_status(void)
{
    return(status);
}


/**
  * @brief  
  */
const struct __task_sched task_timed = 
{
    .name               = NAME_TIMED,
    .init               = timed_init,
    .loop               = timed_loop,
    .exit               = timed_exit,
    .reset              = timed_reset,
    .status             = timed_status,
    .api                = (void *)&timed,
};
