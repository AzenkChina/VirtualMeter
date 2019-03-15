/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_XMODEM_H__
#define __TYPES_XMODEM_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  当前状态
  */
enum  __xmodem_status
{
    XMODEM_IDLE = 0, //当前空闲
    XMODEM_BUSY, //当前忙
    XMODEM_ERROR, //发生错误
};

/**
  * @brief  xmodem task 的对外接口
  */
struct __xmodem
{
    enum  __xmodem_status           (*status)(void); //当前状态
    uint32_t                        (*receive)(const char *name); //接收文件
    uint32_t                        (*send)(const char *name); //发送文件
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_XMODEM_H__ */
