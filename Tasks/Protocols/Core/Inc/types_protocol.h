/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_PROTOCOL_H__
#define __TYPES_PROTOCOL_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "tasks.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  定义协议族
  */
enum  __protocol_family 
{
    PF_NONE = 0,
    PF_IEC1107,
    PF_DLMS,
    PF_DLT645,
    PF_DLT698,
    PF_HDLC,
    PF_MODBUS,
    
    
    PF_ALL = 0xff,
};

/**
  * @brief  协议栈操作接口
  */
struct __protocol
{
    enum  __protocol_family         pf;
    
    uint16_t                        (*read)(uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *param);
    uint16_t                        (*write)(uint8_t *descriptor, uint8_t *buff, uint16_t size);
    
    struct
    {
        uint16_t                    (*in)(uint8_t channel, const uint8_t *frame, uint16_t length);
        uint16_t                    (*out)(uint8_t channel, uint8_t *buff, uint16_t length);
        
    }                               stream;
};

/**
  * @brief  协议栈接口，要注册一个协议栈到系统，需要实现此接口
  */
struct __protocol_registrable
{
    struct __protocol               protocol;
    struct __task_sched             sched;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_PROTOCOL_H__ */
