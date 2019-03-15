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
  * @brief  keyboard task 的对外接口
  */
struct __keyboard
{
    struct
    {
        uint8_t                 (*get)(void); //获取唤醒持续时间
        uint8_t                 (*set)(uint8_t sec); //设置唤醒持续时间
        
    }                           waketime;

};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_KEYBOARD_H__ */
