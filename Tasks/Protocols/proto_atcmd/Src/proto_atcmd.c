/**
 * @brief		
 * @details     
 * @date        2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "config_protocol.h"
#include "proto_atcmd.h"
#include "info.h"
#include "stdbool.h"
#include "string.h"
#include "types_comm.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  命令索引
  */
struct __atcmd_entry
{
    char		*command;
	void		(*execute)(const char *);
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void atcmd_execute_getid(const char *s);
static void atcmd_execute_getgap(const char *s);
static void atcmd_execute_setimei(const char *s);
static void atcmd_execute_setmac(const char *s);
static void atcmd_execute_setsig(const char *s);
static void atcmd_execute_getlip(const char *s);
static void atcmd_execute_setlip(const char *s);
static void atcmd_execute_getmask(const char *s);
static void atcmd_execute_setmask(const char *s);
static void atcmd_execute_getgate(const char *s);
static void atcmd_execute_setgate(const char *s);
static void atcmd_execute_getadns(const char *s);
static void atcmd_execute_getbdns(const char *s);
static void atcmd_execute_getrip(const char *s);
static void atcmd_execute_getport(const char *s);
static void atcmd_execute_getpin(const char *s);
static void atcmd_execute_getapn(const char *s);
static void atcmd_execute_getpassword(const char *s);

/* Private variables ---------------------------------------------------------*/
static const struct __atcmd_entry atcmd_entry[] = 
{
	{"AT#ID?",			atcmd_execute_getid},
	{"AT#GAP?",			atcmd_execute_getgap},
	{"AT#IMEI",			atcmd_execute_setimei},
	{"AT#MAC",			atcmd_execute_setmac},
	{"AT#SIG",			atcmd_execute_setsig},
	{"AT#LIP?",			atcmd_execute_getlip},
	{"AT#LIP",			atcmd_execute_setlip},
	{"AT#MASK?",		atcmd_execute_getmask},
	{"AT#MASK",			atcmd_execute_setmask},
	{"AT#GATE?",		atcmd_execute_getgate},
	{"AT#GATE",			atcmd_execute_setgate},
	{"AT#ADNS?",		atcmd_execute_getadns},
	{"AT#BDNS?",		atcmd_execute_getbdns},
	{"AT#RIP?",			atcmd_execute_getrip},
	{"AT#PORT?",		atcmd_execute_getport},
	{"AT#PIN?",			atcmd_execute_getpin},
	{"AT#APN?",			atcmd_execute_getapn},
	{"AT#PWD?",			atcmd_execute_getpassword},
};

static char *result = (char *)0;
static uint8_t result_channel = 0xff;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static void atcmd_execute_getid(const char *s)
{

}

/**
  * @brief  
  */
static void atcmd_execute_getgap(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_setimei(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_setmac(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_setsig(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getlip(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_setlip(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getmask(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_setmask(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getgate(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_setgate(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getadns(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getbdns(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getrip(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getport(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getpin(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getapn(const char *s)
{
	
}

/**
  * @brief  
  */
static void atcmd_execute_getpassword(const char *s)
{
	
}





/**
  * @brief  
  */
static void atcmd_init(void)
{
	
}

/**
  * @brief  
  */
static void atcmd_loop(void)
{
	
}

/**
  * @brief  
  */
static void atcmd_exit(void)
{
	
}

/**
  * @brief  
  */
static void atcmd_reset(void)
{
	
}

/**
  * @brief  
  */
static enum __task_status atcmd_status(void)
{
	return(TASK_INIT);
}


/**
  * @brief  
  */
static uint16_t atcmd_read(enum  __protocol_family pf, uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *id)
{
	if(!(pf & PF_ATCMD))
	{
		return(0);
	}
	
	return(0);
}

/**
  * @brief  
  */
static uint16_t atcmd_write(enum  __protocol_family pf, uint8_t *descriptor, uint8_t *buff, uint16_t size)
{
	if(!(pf & PF_ATCMD))
	{
		return(0);
	}
	
    return(0);
}

/**
  * @brief  
  */
static uint16_t atcmd_stream_in(uint8_t channel, const uint8_t *frame, uint16_t frame_length)
{
	char buff[128];
    char *p;
	uint8_t cnt;
	struct __comm *comm = api("task_comm");
	
	if(!comm)
	{
		return(0);
	}
	
	if(!(comm->attrib.protocol(channel) & PF_ATCMD))
	{
		return(0);
	}
	
	//上次的回复未发送
	if(result)
	{
		return(0);
	}
	
	//最短命令长度
	if(frame_length < 4)
	{
		return(0);
	}
	
	//拷贝命令到临时缓冲
	heap.set(buff, 0, sizeof(buff));
	strncpy(buff, (const char *)frame, (((sizeof(buff)-1) < frame_length)? (sizeof(buff)-1):frame_length));
    //按空格分解字符串
	p = strtok(buff, " ");
	
	for(cnt=0; cnt<(sizeof(atcmd_entry)/sizeof(struct __atcmd_entry)); cnt++)
	{
		if(strcmp(atcmd_entry[cnt].command, p))
		{
			continue;
		}
		
		//命令最后一个字符是问号说明是查询命令
		if(atcmd_entry[cnt].command[strlen(atcmd_entry[cnt].command) - 1] == '?')
		{
			result_channel = channel;
			atcmd_entry[cnt].execute(p);
			return(0);
		}
		
		//设置命令应该有后续字符串
		p = strtok(NULL, " ");
		if(!p)
		{
			return(0);
		}
		
		result_channel = channel;
		atcmd_entry[cnt].execute(p);
		return(0);
	}
    
    return(0);
}

/**
  * @brief  
  */
static uint16_t atcmd_stream_out(uint8_t channel, uint8_t *frame, uint16_t buff_length)
{
	uint16_t len;
	
	if(result && (channel == result_channel))
	{
		len = strlen(result);
		
		if(len < buff_length)
		{
			strcpy((char *)frame, result);
		}
		
		heap.free(result);
		result = (char *)0;
		
		if(len < buff_length)
		{
			return(len);
		}
	}
	
	return(0);
}


/**
  * @brief  
  */
const struct __protocol_registrable proto_atcmd = 
{
    .protocol               = 
    {
        .pf                 = PF_ATCMD,
        
        .read               = atcmd_read,
        .write              = atcmd_write,
    
        .stream             = 
        {
            .in             = atcmd_stream_in,
            .out            = atcmd_stream_out,
        },
    },
    
    .sched                  = 
    {
        .name               = "ATCMD",
        .init               = atcmd_init,
        .loop               = atcmd_loop,
        .exit               = atcmd_exit,
        .reset              = atcmd_reset,
        .status             = atcmd_status,
        .api                = (void *)0,
    },
};
