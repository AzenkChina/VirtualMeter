/**
 * @brief       Xmodem Frame form:    <SOH><blk><255-blk><--128 data bytes--><CRC hi><CRC lo>
 * @details     
 * @date        2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_xmodem.h"
#include "types_xmodem.h"

#include "jiffy.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  
  */
enum  __xmodem_task
{
    XMODEM_TASK_IDLE = 0, //ø’œ–
    XMODEM_TASK_SEND, //∑¢ÀÕ
    XMODEM_TASK_REC, //Ω” ’
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum  __xmodem_task xmodem_task = XMODEM_TASK_IDLE;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


static enum  __xmodem_status xfr_status(void)
{
    
}

static uint32_t xfr_receive(const char *name)
{
    
}

static uint32_t xfr_send(const char *name)
{
    
}

/**
  * @brief  
  */
static const struct __xmodem xmodem = 
{
    .status             = xfr_status,
    .receive            = xfr_receive,
    .send               = xfr_send,
};




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
    return(TASK_RUN);
}


/**
  * @brief  
  */
const struct __task_sched task_xmodem = 
{
    .name               = "task_xmodem",
    .init               = xmodem_init,
    .loop               = xmodem_loop,
    .exit               = xmodem_exit,
    .reset              = xmodem_reset,
    .status             = xmodem_status,
    .api                = (void *)0,
};

