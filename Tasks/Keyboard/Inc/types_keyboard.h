/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_KEYBOARD_H__
#define __TYPES_KEYBOARD_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "keys.h"

/* Exported types ------------------------------------------------------------*/
struct __keyboard_event
{
    uint16_t            id;
    enum __key_status   status;
};

/**
  * @brief  keyboard task �Ķ���ӿ�
  */
struct __keyboard
{
    struct
    {
        uint8_t                 (*get)(void); //��ȡ���ѳ���ʱ��
        uint8_t                 (*set)(uint8_t sec); //���û��ѳ���ʱ��
        
    }                           waketime;

};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_KEYBOARD_H__ */
