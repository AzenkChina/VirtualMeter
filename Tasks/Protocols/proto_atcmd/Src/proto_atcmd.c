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
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static void xmodem_init(void)
{
	
}

/**
  * @brief  
  */
static void xmodem_loop(void)
{
	
}

/**
  * @brief  
  */
static void xmodem_exit(void)
{
	
}

/**
  * @brief  
  */
static void xmodem_reset(void)
{
	
}

/**
  * @brief  
  */
static enum __task_status xmodem_status(void)
{
	return(TASK_INIT);
}


/**
  * @brief  
  */
static uint16_t xmodem_read(uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *id)
{
	return(0);
}

/**
  * @brief  
  */
static uint16_t xmodem_write(uint8_t *descriptor, uint8_t *buff, uint16_t size)
{
    return(0);
}

/**
  * @brief  
  */
static uint16_t xmodem_stream_in(uint8_t channel, const uint8_t *frame, uint16_t frame_length)
{
	struct __comm *comm = api("task_comm");
	
	if(!comm)
	{
		return(0);
	}
	
	if(!(comm->attrib.protocol(channel) | PF_ATCMD))
	{
		return(0);
	}
    
    return(0);
}

/**
  * @brief  
  */
static uint16_t xmodem_stream_out(uint8_t channel, uint8_t *frame, uint16_t buff_length)
{
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
        
        .read               = xmodem_read,
        .write              = xmodem_write,
    
        .stream             = 
        {
            .in             = xmodem_stream_in,
            .out            = xmodem_stream_out,
        },
    },
    
    .sched                  = 
    {
        .name               = "ATCMD",
        .init               = xmodem_init,
        .loop               = xmodem_loop,
        .exit               = xmodem_exit,
        .reset              = xmodem_reset,
        .status             = xmodem_status,
        .api                = (void *)0,
    },
};
