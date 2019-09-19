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
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  xmodem task 的对外接口
  */
struct __xmodem
{
    bool                            (*init)(const char *name); //初始化
    uint8_t                         (*transfer)(const uint8_t *frame, uint16_t length); //接收报文文件
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_XMODEM_H__ */
