/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INFO_H__
#define __INFO_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  �汾λ��
  */
enum  __ver_field
{
    VERSION_BIG = 1,//"2.01+git-3" �е� "2"
    VERSION_LITTLE = 2,//"2.01+git-3" �е� "01"
    VERSION_BIGLITTLE = 3,//"2.01+git-3" �е� "2.01"
    VERSION_SERVER = 4,//"2.01+git-3" �е� "git-3"
    VERSION_FULL = 5,//"2.01+git-3" ȫ��
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern uint8_t get_software_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff);
extern uint8_t get_hardware_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff);
extern uint8_t get_tasks_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff);
extern uint8_t get_compile_time(uint8_t buff_length, uint8_t *buff);
extern uint8_t get_management_passwd(uint8_t buff_length, uint8_t *buff);
#endif /* __INFO_H__ */
