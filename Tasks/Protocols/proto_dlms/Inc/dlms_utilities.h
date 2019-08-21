/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_UTILITIES_H__
#define __DLMS_UTILITIES_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern uint8_t dlms_util_load_akey(uint8_t *buffer);
extern uint8_t dlms_util_write_akey(uint8_t *buffer);
extern uint8_t dlms_util_load_ekey(uint8_t *buffer);
extern uint8_t dlms_util_write_ekey(uint8_t *buffer);
extern uint8_t dlms_util_load_passwd(uint8_t *buffer);
extern uint8_t dlms_util_write_passwd(uint8_t *buffer);
extern uint8_t dlms_util_load_title(uint8_t *buffer);
extern uint8_t dlms_util_write_title(uint8_t *buffer);

#endif /* __DLMS_UTILITIES_H__ */
