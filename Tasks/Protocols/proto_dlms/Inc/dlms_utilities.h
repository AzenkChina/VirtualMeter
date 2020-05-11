/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_UTILITIES_H__
#define __DLMS_UTILITIES_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern uint16_t dlms_util_load_hdlc_address(void);
extern uint16_t dlms_util_write_hdlc_address(uint16_t val);
extern uint16_t dlms_util_load_hdlc_interval(void);
extern uint16_t dlms_util_write_hdlc_interval(uint16_t val);

extern uint8_t dlms_util_load_title(uint8_t *buffer);
extern uint8_t dlms_util_write_title(uint8_t *buffer);

extern uint8_t dlms_util_load_management_passwd(uint8_t *buffer);

extern uint8_t dlms_util_load_passwd(uint8_t *buffer);
extern uint8_t dlms_util_write_passwd(uint8_t *buffer);

extern uint8_t dlms_util_load_akey(uint8_t *buffer);
extern uint8_t dlms_util_write_akey(uint8_t *buffer);
extern uint8_t dlms_util_load_bekey(uint8_t *buffer);
extern uint8_t dlms_util_write_bekey(uint8_t *buffer);
extern uint8_t dlms_util_load_uekey(uint8_t *buffer);
extern uint8_t dlms_util_write_uekey(uint8_t *buffer);
extern uint8_t dlms_util_load_mkey(uint8_t *buffer);
extern uint8_t dlms_util_write_mkey(uint8_t *buffer);

extern uint8_t dlms_util_load_saprikey(uint8_t *buffer);
extern uint8_t dlms_util_write_saprikey(uint8_t *buffer);
extern uint8_t dlms_util_load_sapubkey(uint8_t *buffer);
extern uint8_t dlms_util_write_sapubkey(uint8_t *buffer);
extern uint8_t dlms_util_load_ssprikey(uint8_t *buffer);
extern uint8_t dlms_util_write_ssprikey(uint8_t *buffer);
extern uint8_t dlms_util_load_sspubkey(uint8_t *buffer);
extern uint8_t dlms_util_write_sspubkey(uint8_t *buffer);
extern uint8_t dlms_util_load_capubkey(uint8_t *buffer);
extern uint8_t dlms_util_write_capubkey(uint8_t *buffer);
extern uint8_t dlms_util_load_cspubkey(uint8_t *buffer);
extern uint8_t dlms_util_write_cspubkey(uint8_t *buffer);

#endif /* __DLMS_UTILITIES_H__ */
