/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_ASSOCIATION_H__
#define __DLMS_ASSOCIATION_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "dlms_types.h"

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void dlms_gateway(uint8_t sap,
                          const uint8_t *info,
                          uint16_t length,
                          uint8_t *buffer,
                          uint16_t buffer_length,
                          uint16_t *filled_length);
extern uint16_t dlms_asso_mtu(void);
extern uint8_t dlms_asso_systitle(uint8_t *buffer);
extern uint8_t dlms_asso_stoc(uint8_t *buffer);
extern uint8_t dlms_asso_ctos(uint8_t *buffer);
extern uint8_t dlms_asso_accept_fctos(void);
extern uint8_t dlms_asso_sc(void);
extern uint8_t dlms_asso_fc(uint8_t *buffer);
extern uint8_t dlms_asso_akey(uint8_t *buffer);
extern uint8_t dlms_asso_ekey(uint8_t Channel, uint8_t *buffer);
extern uint8_t dlms_asso_dedkey(uint8_t *buffer);
extern uint8_t dlms_asso_applname(uint8_t *buffer);
extern uint8_t dlms_asso_mechname(uint8_t *buffer);
extern uint8_t dlms_asso_contextinfo(uint8_t *buffer);
extern enum __asso_status dlms_asso_status(void);
extern enum __dlms_access_level dlms_asso_level(void);

#endif /* __DLMS_ASSOCIATION_H__ */
