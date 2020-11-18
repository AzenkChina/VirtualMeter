/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "string.h"
#include "task_logger.h"
#include "types_logger.h"
#include "config_logger.h"
#include "crc.h"

/* Private define ------------------------------------------------------------*/
#define MAX_MONITORS	((uint8_t)16)

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  事件行为
  */
enum __event_behaviors
{
	EVB_PU		= 0x0001, //正常运行时可触发
	EVB_PD		= 0x0002, //低功耗时可触发
	EVB_STR		= 0x0004, //事件开始可触发
	EVB_END		= 0x0008, //事件结束可触发
	EVB_STRPU	= 0x0010, //上电时强制触发开始
	EVB_ENDPU	= 0x0020, //上电时强制触发结束
	EVB_STRPD	= 0x0040, //掉电时强制触发开始（掉电时事件状态强制设置位 EVS_STARTING，下次上电处理）
	EVB_ENDPD	= 0x0080, //掉电时强制触发结束（掉电时事件状态强制设置位 EVS_ENDING，下次上电处理）
};

/**
  * @brief  事件配置表
  */
struct __logger_table
{
	enum __event_id id; //事件ID
	uint16_t mapping; //映射后的ID
	uint16_t behaviors; //事件配置
};

/**
  * @brief  事件当前状态
  */
struct __logger_status
{
	uint8_t value[(EVI_MAX*2+7)/8];
	uint32_t check;
};

/**
  * @brief  事件监听器
  */
struct __logger_monitor
{
	void (*callback[MAX_MONITORS])(enum __event_id, enum __event_status, uint16_t);
	uint32_t check;
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __task_status task_status = TASK_NOTINIT;

/**
  * @brief  事件配置表
  */
static const struct __logger_table table[] = 
{
	/** 事件标识		映射标识	事件行为 */
	{EVI_POWERUP,		11,			(EVB_PU | EVB_STR | EVB_ENDPD | EVB_STRPU)},
	{EVI_POWERDOWN,		12,			(EVB_PU | EVB_STR | EVB_ENDPU | EVB_STRPD)},
};

/**
  * @brief  请求的事件状态字，每个事件占用2个bit，与 enum __event_id 中顺序一致
  */
static struct __logger_status logger_status;

/**
  * @brief  事件监听器
  */
static struct __logger_monitor monitors;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  获取指定的事件的状态
  */
static enum __event_status logger_event_status_read(enum __event_id id)
{
	enum __event_id val = EVI_ALL;
	
	if(logger_status.check != crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check))))
	{
		return(EVS_ENDED);
	}
	
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].id == id)
		{
			val = id;
		}
	}
	
    return((enum __event_status)((logger_status.value[val/4] >> ((val%4)*2)) & 0x03));
}

/**
  * @brief  更新指定的事件的状态
  */
static bool logger_event_status_toggle(enum __event_id id, enum __event_status status)
{
	uint16_t behaviors = 0;
	enum __event_status status_current;
	
	if((id > EVI_MAX) || (id <= EVI_ALL))
	{
		return(false);
	}
	
	if(logger_status.check != crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check))))
	{
		return(false);
	}
	
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].id == id)
		{
			behaviors = table[n].behaviors;
			break;
		}
	}
	
	if(!behaviors)
	{
		return(false);
	}
	
	status_current = (enum __event_status)((logger_status.value[id/4] >> ((id%4)*2)) & 0x03);
	
	//正常状态下，EVB_PU 未置位 不处理
    if((system_status() == SYSTEM_RUN) && !(behaviors & EVB_PU))
    {
		return(false);
	}
	
	//低功耗状态下，EVB_PD 未置位 不处理
    if(((system_status() == SYSTEM_SLEEP) || (system_status() == SYSTEM_WAKEUP)) && !(behaviors & EVB_PD))
    {
		return(false);
	}
	
	//更新状态
	logger_status.value[id/4] &= ~(0x03 << ((id%4)*2));
	logger_status.value[id/4] |= (((uint8_t)status) << ((id%4)*2));
	logger_status.check = crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check)));
	
	//事件开始可触发
	if(behaviors & EVB_STR)
	{
		
	}
	
	//事件结束可触发
	if(behaviors & EVB_END)
	{
		
	}
	
    return(true);
}

/**
  * @brief  添加一个监测回调
  */
static bool logger_event_add_monitor(void (*callback)(enum __event_id, enum __event_status, uint16_t))
{
	uint16_t blank = 0xffff;
	
	if(!callback)
	{
		return(false);
	}
	
	if(monitors.check != crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check))))
	{
		return(false);
	}
	
	for(uint8_t n=0; n<MAX_MONITORS; n++)
	{
		if((void *)(monitors.callback[n]) == (void *)0)
		{
			if(blank == 0xffff)
			{
				blank = n;
			}
			continue;
		}
		
		if(monitors.callback[n] == callback)
		{
			return(false);
		}
	}
	
	if(blank >= MAX_MONITORS)
	{
		return(false);
	}
	
	monitors.callback[blank] = callback;
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	return(true);
}

/**
  * @brief  删除一个监测回调
  */
static bool logger_event_remove_monitor(void (*callback)(enum __event_id, enum __event_status, uint16_t))
{
	if(!callback)
	{
		return(false);
	}
	
	if(monitors.check != crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check))))
	{
		return(false);
	}
	
	for(uint8_t n=0; n<MAX_MONITORS; n++)
	{
		if((void *)(monitors.callback[n]) == (void *)0)
		{
			continue;
		}
		
		if(monitors.callback[n] == callback)
		{
			monitors.callback[n] = (void *)0;
		}
	}
	
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	return(true);
}

/**
  * @brief  获取映射后的ID
  */
static uint16_t logger_event_map(enum __event_id id)
{
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].id == id)
		{
			return(table[n].mapping);
		}
	}
	
	return(0);
}

/**
  * @brief  获取映射前的ID
  */
static enum __event_id logger_event_unmap(uint16_t mapping)
{
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].mapping == mapping)
		{
			return(table[n].id);
		}
	}
	
	return(EVI_ALL);
}

/**
  * @brief  
  */
static const struct __logger logger = 
{
	.status			= 
	{
    	.read		= logger_event_status_read,
		.toggle		= logger_event_status_toggle,
	},
	
	.monitor		= 
	{
		.add		= logger_event_add_monitor,
		.remove		= logger_event_remove_monitor,
	},
	
	.map			= logger_event_map,
	.unmap			= logger_event_unmap,
};








/**
  * @brief  
  */
static void task_logger_init(void)
{
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
	    task_status = TASK_INIT;
    }
	
	//初始化监听列表
    memset(&monitors, 0, sizeof(monitors));
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	
	//...加载事件状态
	memset(&logger_status, 0, sizeof(logger_status));
	
	//...记录上电时需要处理的事件，延迟处理
}

/**
  * @brief  
  */
static void task_logger_loop(void)
{
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
	    task_status = TASK_INIT;
    }
}

/**
  * @brief  
  */
static void task_logger_exit(void)
{
	//...保存事件状态
	memset(&logger_status, 0, sizeof(logger_status));
	
	task_status = TASK_NOTINIT;
}

/**
  * @brief  
  */
static void task_logger_reset(void)
{
	//初始化监听列表
    memset(&monitors, 0, sizeof(monitors));
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	
	//...初始化事件状态
	memset(&logger_status, 0, sizeof(logger_status));
	logger_status.check = crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check)));
	//...写回
	
	task_status = TASK_NOTINIT;
}

/**
  * @brief  
  */
static enum __task_status task_logger_status(void)
{
    return(task_status);
}


/**
  * @brief  
  */
const struct __task_sched task_logger = 
{
    .name               = NAME_LOGGER,
    .init               = task_logger_init,
    .loop               = task_logger_loop,
    .exit               = task_logger_exit,
    .reset              = task_logger_reset,
    .status             = task_logger_status,
    .api                = (void *)&logger,
};
