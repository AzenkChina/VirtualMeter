/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INFO_H__
#define __INFO_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  版本位置
  */
enum  __ver_field
{
    VERSION_BIG = 1,//"2.01+git-3" 中的 "2"
    VERSION_LITTLE = 2,//"2.01+git-3" 中的 "01"
    VERSION_BIGLITTLE = 3,//"2.01+git-3" 中的 "2.01"
    VERSION_SERVER = 4,//"2.01+git-3" 中的 "git-3"
    VERSION_FULL = 5,//"2.01+git-3" 全部
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern uint8_t info_get_software_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff);
extern uint8_t info_get_hardware_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff);
extern uint8_t info_get_tasks_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff);
extern uint8_t info_get_compile_time(uint8_t buff_length, uint8_t *buff);
extern uint8_t info_get_management_passwd(uint8_t buff_length, uint8_t *buff);

extern uint64_t info_get_address(void);
extern uint64_t info_set_address(uint64_t val);
extern uint8_t info_get_serial(char *buff);
extern uint8_t info_set_serial(const char *buff);
extern uint8_t info_get_type(char *buff);
extern uint8_t info_set_type(const char *buff);
extern uint8_t info_get_phase(void);
extern uint8_t info_set_phase(uint8_t val);
extern uint8_t info_get_wire(void);
extern uint8_t info_set_wire(uint8_t val);
extern uint32_t info_get_voltage(void);
extern uint32_t info_set_voltage(uint32_t val);
extern uint32_t info_get_current_rated(void);
extern uint32_t info_set_current_rated(uint32_t val);
extern uint32_t info_get_current_max(void);
extern uint32_t info_set_current_max(uint32_t val);
extern uint32_t info_get_frequency(void);
extern uint32_t info_set_frequency(uint32_t val);
extern uint32_t info_get_accuracy_active(void);
extern uint32_t info_set_accuracy_active(uint32_t val);
extern uint32_t info_get_accuracy_reactive(void);
extern uint32_t info_set_accuracy_reactive(uint32_t val);
extern uint64_t info_get_manufacture(void);
extern uint64_t info_set_manufacture(uint64_t val);

#endif /* __INFO_H__ */
