/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_comm.h"
#include "types_comm.h"
#include "config_comm.h"

#include "types_protocol.h"

/** 在这里添加通信端口头文件 */
#include "rs485_1.h"
#include "rs485_2.h"
#include "optical.h"
#include "module.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  端口记录
  */
struct __port_entry
{
	enum __port_type					port_type;
	uint16_t							pf;
	const struct __serial				*serial;
};

/**
  * @brief  端口插拔事件
  */
struct __port_callback
{
    void			(*callback)(uint8_t, bool);
	unsigned long	check;
};


/* Private define ------------------------------------------------------------*/
#define MAX_CALLBACKS		((uint8_t)2)

/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;

/**
  * @brief  将所有的端口以及端口类型记录到结构体中 
  */
static const struct __port_entry port_table[] = 
{
	/** 端口类型		支持的协议								端口结构体指针 */
	{PORT_MODULE,	(uint16_t)(PF_DLMS | PF_ATCMD),				(struct __serial *)&module},
	{PORT_RS485,	(uint16_t)(PF_DLMS | PF_XMODEM),			&rs485_1},
	{PORT_RS485,	(uint16_t)(PF_DLMS | PF_XMODEM),			&rs485_2},
	{PORT_OPTICAL,	(uint16_t)(PF_DLMS),						&optical},
};

/* Private macro -------------------------------------------------------------*/
#define PORT_AMOUNT			((uint8_t)(sizeof(port_table) / sizeof(struct __port_entry)))

/* Private variables ---------------------------------------------------------*/
static uint8_t *buff = (uint8_t *)0;
static volatile uint8_t current = 0xff;
static bool detected[PORT_AMOUNT] = {true};
static struct __port_callback callbacks[MAX_CALLBACKS];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  获取当前通道
  */
static uint8_t serial_channel(void)
{
	return(current);
}

/**
  * @brief  获取端口类型 
  */
static enum __port_type serial_porttype(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __port_type)0xff);
    }
    
    return(port_table[channel].port_type);
}

/**
  * @brief  获取端口支持的协议
  */
static uint16_t serial_port_protocol(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return(0);
    }
    
    return(port_table[channel].pf);
}

/**
  * @brief  获取端口状态 
  */
static enum __port_status serial_portstatus(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __port_status)0xff);
    }
    
    if(port_table[channel].serial->status() != BUS_IDLE)
    {
    	return(PORT_BUSY);
    }
    
    return(PORT_IDLE);
}

/**
  * @brief  获取链路连接状态 
  */
static bool serial_link(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return(false);
    }
	
	//只有模块端口才有链路状态
	if(port_table[channel].port_type == PORT_MODULE)
	{
		return(((struct __module *)(port_table[channel].serial))->linked());
	}
	
	return(true);
}

/**
  * @brief  设置帧超时时间 
  */
static uint16_t serial_timeout_set(uint8_t channel, uint16_t msecond)
{
    if(channel >= PORT_AMOUNT)
    {
        return(0);
    }
    
    return(port_table[channel].serial->timeout.set(msecond));
}

/**
  * @brief  获取帧超时时间 
  */
static uint16_t serial_timeout_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return(0);
    }
    
    return(port_table[channel].serial->timeout.get());
}

/**
  * @brief  设置波特率 
  */
static enum __baud serial_baudrate_set(uint8_t channel, enum __baud baudrate)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __baud)0xff);
    }
    
    return(port_table[channel].serial->uart->baudrate.set(baudrate));
}

/**
  * @brief  获取波特率 
  */
static enum __baud serial_baudrate_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __baud)0xffff);
    }
    
    return(port_table[channel].serial->uart->baudrate.get());
}

/**
  * @brief  设置校验位 
  */
static enum __parity serial_parity_set(uint8_t channel, enum __parity parity)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __parity)0xff);
    }
    
    return(port_table[channel].serial->uart->parity.set(parity));
}

/**
  * @brief  获取校验位 
  */
static enum __parity serial_parity_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __parity)0xff);
    }
    
    return(port_table[channel].serial->uart->parity.get());
}

/**
  * @brief  设置停止位 
  */
static enum __stop serial_stop_set(uint8_t channel, enum __stop stop)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __stop)0xff);
    }
    
    return(port_table[channel].serial->uart->stop.set(stop));
}

/**
  * @brief  获取停止位 
  */
static enum __stop serial_stop_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __stop)0xff);
    }
    
    return(port_table[channel].serial->uart->stop.get());
}

/**
  * @brief   添加模块插拔回调
  */
static bool serial_monitor_add(void (*callback)(uint8_t, bool))
{
	uint8_t n;
	
	if(!callback)
	{
		return(false);
	}
	
	for(n=0; n<MAX_CALLBACKS; n++)
	{
		if((unsigned long)callbacks[n].callback != callbacks[n].check)
		{
			callbacks[n].callback = 0;
			callbacks[n].check = 0;
		}
		
		if(callbacks[n].callback == callback)
		{
			return(true);
		}
	}
	
	for(n=0; n<MAX_CALLBACKS; n++)
	{
		if((!callbacks[n].callback) || (!callbacks[n].check))
		{
			callbacks[n].callback = callback;
			callbacks[n].check = (unsigned long)callback;
			return(true);
		}
	}
	
	return(false);
}

/**
  * @brief   删除模块插拔回调
  */
static bool serial_monitor_remove(void (*callback)(uint8_t, bool))
{
	uint8_t n;
	
	if(!callback)
	{
		return(false);
	}
	
	for(n=0; n<MAX_CALLBACKS; n++)
	{
		if((unsigned long)callbacks[n].callback != callbacks[n].check)
		{
			callbacks[n].callback = 0;
			callbacks[n].check = 0;
		}
		
		if(callbacks[n].callback == callback)
		{
			callbacks[n].callback = 0;
			callbacks[n].check = 0;
			return(true);
		}
	}
	
	return(false);
}


/**
  * @brief  配置信息改变
  */
static void serial_config(bool state)
{
	uint8_t cnt;
	
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		if(port_table[cnt].port_type != PORT_MODULE)
		{
			continue;
		}
		
		((struct __module *)(port_table[cnt].serial))->config(state);
	}
}

/**
  * @brief  复位
  */
static void serial_reset(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return;
    }
	
	//模块端口硬复位
	if(port_table[channel].port_type == PORT_MODULE)
	{
		((struct __module *)(port_table[channel].serial))->reset();
	}
	
	//...TODO 重新初始化端口
}

/**
  * @brief  
  */
static const struct __comm comm = 
{
    .attrib				= 
    {
		.channel		= serial_channel,
		.type			= serial_porttype,
		.protocol		= serial_port_protocol,
		.status			= serial_portstatus,
		.link			= serial_link,
    },
    
    .timeout            = 
    {
        .set            = serial_timeout_set,
        .get            = serial_timeout_get,
    },
    
    .baudrate           =
    {
        .set            = serial_baudrate_set,
        .get            = serial_baudrate_get,
    },
    
    .parity             =
    {
        .set            = serial_parity_set,
        .get            = serial_parity_get,
    },
    
    .stop               =
    {
        .set            = serial_stop_set,
        .get            = serial_stop_get,
    },
	
	.monitor			=
	{
		.add			= serial_monitor_add,
		.remove			= serial_monitor_remove,
	},
	
	.config				= serial_config,
	.reset				= serial_reset,
};








/**
  * @brief  task 初始化 
  */
static void comm_init(void)
{
	uint8_t cnt;
	
	heap.set(callbacks, 0, sizeof(callbacks));
    buff = (uint8_t *)0;
    
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
        buff = heap.salloc(NAME_COMM, (PORT_AMOUNT*2 + 1)*COMM_CONF_BUFF);
        
        if(!buff)
        {
            status = TASK_ERROR;
            return;
        }
        
    	//初始化所有串行总线
    	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
    	{
    		port_table[cnt].serial->control.init(DEVICE_NORMAL);
    		port_table[cnt].serial->timeout.set(50);
    		port_table[cnt].serial->rxbuff.set(COMM_CONF_BUFF, (buff+COMM_CONF_BUFF*(1+cnt*2+0)));
    		port_table[cnt].serial->txbuff.set(COMM_CONF_BUFF, (buff+COMM_CONF_BUFF*(1+cnt*2+1)));
    	}
	    
	    status = TASK_INIT;
		TRACE(TRACE_INFO, "Task comm initialized.");
    }
}

/**
  * @brief  task 轮询
  */
static void comm_loop(void)
{
	static uint16_t periods = 0;
	uint8_t n;
	uint8_t cnt;
	uint16_t length;
	
	uint8_t *pbuff;
	uint16_t pbuff_length;
    
    struct __protocol *api_stream;
	
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() != SYSTEM_RUN)
    {
    	return;
    }
    
    //给驱动提供时钟
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		port_table[cnt].serial->runner(KERNEL_PERIOD);
	}
	
	//定时检测模块插拔状态
	periods += KERNEL_PERIOD;
	
	if(periods > 3000)
	{
		periods = 0;
		
		for(cnt=0; cnt<PORT_AMOUNT; cnt++)
		{
			if(port_table[cnt].port_type != PORT_MODULE)
			{
				continue;
			}
			
			if(((struct __module *)(port_table[cnt].serial))->detect() == detected[cnt])
			{
				continue;
			}
			
			detected[cnt] = ((struct __module *)(port_table[cnt].serial))->detect();
			
			for(n=0; n<MAX_CALLBACKS; n++)
			{
				if(!callbacks[n].callback)
				{
					continue;
				}
				if((unsigned long)callbacks[n].callback != callbacks[n].check)
				{
					callbacks[n].callback = 0;
					callbacks[n].check = 0;
					continue;
				}
				callbacks[n].callback(cnt, detected[cnt]);
			}
		}
	}
    
    //获取协议栈接口
    api_stream = (struct __protocol *)api("task_protocol");
    
    //从总线读取数据帧
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		//从总线读取
		length = port_table[cnt].serial->read(COMM_CONF_BUFF, buff);
        
	    if(length)
	    {
            //数据发送到协议栈
            if(api_stream)
            {
				current = cnt;
                api_stream->stream.in(cnt, buff, length);
				current = 0xff;
            }
		}
	}
    
    if(!api_stream)
    {
        return;
    }
    
    //往总线写入数据帧
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		//如果这个总线忙，则跳过 
        if(port_table[cnt].serial->status() != BUS_IDLE)
        {
            continue;
        }
        
        //获取这个总线的发送缓冲和缓冲长度 
        pbuff_length = port_table[cnt].serial->txbuff.get(&pbuff);
        
        if((!pbuff_length) || (!pbuff))
        {
            continue;
        }
        
        //从协议栈读取数据
		current = cnt;
        length = api_stream->stream.out(cnt, pbuff, pbuff_length);
        current = 0xff;
		
        if(length)
        {
            port_table[cnt].serial->write(length);
        }
	}
    
    status = TASK_RUN;
}

/**
  * @brief  task 退出 
  */
static void comm_exit(void)
{
	uint8_t cnt;
	
	//挂起所有串行总线
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		port_table[cnt].serial->control.suspend();
	}
    
    status = TASK_SUSPEND;
	TRACE(TRACE_INFO, "Task comm exited.");
}

/**
  * @brief  task 复位 
  */
static void comm_reset(void)
{
	uint8_t cnt;
	
	//挂起所有串行总线
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		port_table[cnt].serial->control.suspend();
	}
    
    status = TASK_NOTINIT;
	TRACE(TRACE_INFO, "Task comm reset.");
}

/**
  * @brief  task 状态
  */
static enum __task_status comm_status(void)
{
    return(status);
}


/**
  * @brief  
  */
const struct __task_sched task_comm = 
{
    .name               = NAME_COMM,
    .init               = comm_init,
    .loop               = comm_loop,
    .exit               = comm_exit,
    .reset              = comm_reset,
    .status             = comm_status,
    .api                = (void *)&comm,
};
