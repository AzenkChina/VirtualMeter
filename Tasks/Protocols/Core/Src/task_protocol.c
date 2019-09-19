/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_protocol.h"
#include "types_protocol.h"
#include "config_protocol.h"

/** 在这里添加协议栈头文件 */
#include "proto_dlms.h"
#include "proto_xmodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;

/**
  * @brief  将所有的协议栈接口记录到结构体中 
  */
static const struct __protocol_registrable *proto_table[] = 
{
	&proto_dlms,
    &proto_xmodem,
};

/* Private macro -------------------------------------------------------------*/
#define PROTO_AMOUNT		((uint8_t)(sizeof(proto_table) / sizeof(struct __protocol_registrable *)))

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  初始化注册的规约
  */
static void proto_init(void)
{
	uint8_t cnt;
    
    for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
    {
    	if(proto_table[cnt]->sched.init)
    	{
			proto_table[cnt]->sched.init();
		}
    }
}

/**
  * @brief  轮询注册的规约
  */
static void proto_loop(void)
{
	uint8_t cnt;
    
    for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
    {
    	if(proto_table[cnt]->sched.loop)
    	{
			proto_table[cnt]->sched.loop();
		}
    }
}

/**
  * @brief  退出注册的规约
  */
static void proto_exit(void)
{
	uint8_t cnt;
    
    for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
    {
    	if(proto_table[cnt]->sched.exit)
    	{
			proto_table[cnt]->sched.exit();
		}
    }
}

/**
  * @brief  复位注册的规约
  */
static void proto_reset(void)
{
	uint8_t cnt;
    
    for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
    {
    	if(proto_table[cnt]->sched.reset)
    	{
			proto_table[cnt]->sched.reset();
		}
    }
}

static enum __task_status proto_status(void)
{
	return(TASK_INIT);
}

/**
  * @brief  协议栈 读取一个数据项
  */
static uint16_t proto_read(uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *mid)
{
	uint8_t cnt;
    uint16_t length;
	
	for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
	{
    	if(proto_table[cnt]->protocol.read)
    	{
			length = proto_table[cnt]->protocol.read(descriptor, buff, size, mid);
		}
        else
        {
            length = 0;
        }
        
        if(length)
        {
            return(length);
        }
	}
    
    return(0);
}

/**
  * @brief  协议栈 设置一个数据项
  */
static uint16_t proto_write(uint8_t *descriptor, uint8_t *buff, uint16_t size)
{
	uint8_t cnt;
    uint16_t length;
	
	for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
	{
    	if(proto_table[cnt]->protocol.write)
    	{
			length = proto_table[cnt]->protocol.write(descriptor, buff, size);
		}
        else
        {
            length = 0;
        }
        
        if(!length)
        {
            return(length);
        }
	}
    
    return(0);
}

/**
  * @brief  输入流
  */
static uint16_t proto_stream_in(uint8_t channel, const uint8_t *frame, uint16_t length)
{
	uint8_t cnt;
	
	for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
	{
    	if(proto_table[cnt]->protocol.stream.in)
    	{
			proto_table[cnt]->protocol.stream.in(channel, frame, length);
		}
	}
    
    return(0);
}

/**
  * @brief  输出流
  */
static uint16_t proto_stream_out(uint8_t channel, uint8_t *buff, uint16_t length)
{
	uint8_t cnt;
    uint16_t result;
	
	for(cnt=0; cnt<PROTO_AMOUNT; cnt++)
	{
    	if(proto_table[cnt]->protocol.stream.out)
    	{
			result = proto_table[cnt]->protocol.stream.out(channel, buff, length);
		}
        else
        {
            result = 0;
        }
        
        if(result)
        {
            return(result);
        }
	}
    
    return(0);
}

/**
  * @brief  接口
  */
static const struct __protocol_registrable protocol = 
{
    .protocol               = 
    {
        .pf                 = PF_ALL,
        
        .read               = proto_read,
        .write              = proto_write,
    
        .stream             = 
        {
            .in             = proto_stream_in,
            .out            = proto_stream_out,
        },
    },
    
    .sched                  = 
    {
        .name               = "PROTO_ALL",
        .init               = proto_init,
        .loop               = proto_loop,
        .exit               = proto_exit,
        .reset              = proto_reset,
        .status             = proto_status,
        .api                = (void *)0,
    },
};










/**
  * @brief  task 初始化 
  */
static void protocol_init(void)
{
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
    	//初始化所有协议栈
    	proto_init();
	    
	    TRACE(TRACE_INFO, "Task protocol initialized.");
	    
	    status = TASK_INIT;
    }
}

/**
  * @brief  task 轮询 
  */
static void protocol_loop(void)
{
    if(system_status() == SYSTEM_RUN)
    {
    	proto_loop();
	    status = TASK_RUN;
    }
}

/**
  * @brief  task 退出 
  */
static void protocol_exit(void)
{
	//退出所有协议栈
    proto_exit();
    
    TRACE(TRACE_INFO, "Task protocol exited.");
    status = TASK_SUSPEND;
}

/**
  * @brief  task 复位
  */
static void protocol_reset(void)
{
	//退出所有协议栈
    proto_reset();
    
    TRACE(TRACE_INFO, "Task protocol reset.");
    status = TASK_NOTINIT;
}

/**
  * @brief  task 状态
  */
static enum __task_status protocol_status(void)
{
    return(status);
}


/**
  * @brief  
  */
const struct __task_sched task_protocol = 
{
    .name               = NAME_PROTOCOL,
    .init               = protocol_init,
    .loop               = protocol_loop,
    .exit               = protocol_exit,
    .reset              = protocol_reset,
    .status             = protocol_status,
    .api                = (void *)&protocol,
};
