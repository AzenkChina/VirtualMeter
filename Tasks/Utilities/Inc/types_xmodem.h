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
  * @brief  xmodem task �Ķ���ӿ�
  */
struct __xmodem
{
    bool                            (*init)(const char *name); //��ʼ��
    uint8_t                         (*transfer)(const uint8_t *frame, uint16_t length); //���ձ����ļ�
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_XMODEM_H__ */
