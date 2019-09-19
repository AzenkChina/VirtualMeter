/**
 * @brief		
 * @details		
 * @date		
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "proto_xmodem.h"
#include "types_xmodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t result = 0;
static uint8_t result_channel = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void xmodem_init(void)
{
	
}

static void xmodem_loop(void)
{
	
}

static void xmodem_exit(void)
{
	
}

static void xmodem_reset(void)
{
	
}

static enum __task_status xmodem_status(void)
{
	return(TASK_INIT);
}



static uint16_t xmodem_read(uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *id)
{
	return(0);
}

static uint16_t xmodem_write(uint8_t *descriptor, uint8_t *buff, uint16_t size)
{
    return(0);
}

static uint16_t xmodem_stream_in(uint8_t channel, const uint8_t *frame, uint16_t frame_length)
{
    struct __xmodem *api_xmodem = api("task_xmodem");
    
    if(!api_xmodem)
    {
        return(0);
    }
    
    result_channel = channel;
    result = api_xmodem->transfer(frame, frame_length);
    
    return(result);
}

static uint16_t xmodem_stream_out(uint8_t channel, uint8_t *frame, uint16_t buff_length)
{
    if((result) && (result_channel == channel))
    {
        frame[0] = result;
        result = 0;
        return(1);
    }
    
	return(0);
}


const struct __protocol_registrable proto_xmodem = 
{
    .protocol               = 
    {
        .pf                 = PF_XMODEM,
        
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
        .name               = "XMODEM",
        .init               = xmodem_init,
        .loop               = xmodem_loop,
        .exit               = xmodem_exit,
        .reset              = xmodem_reset,
        .status             = xmodem_status,
        .api                = (void *)0,
    },
};
