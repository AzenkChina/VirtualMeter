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
extern void dlms_asso_gateway(uint8_t sap,
                              const uint8_t *info,
                              uint16_t length,
                              uint8_t *buffer,
                              uint16_t buffer_length,
                              uint16_t *filled_length);
extern void dlms_asso_cleanup(uint8_t sap);
extern uint8_t dlms_asso_client(void);
extern enum __asso_status dlms_asso_status(void);
extern uint16_t dlms_asso_mtu(void);
extern uint8_t dlms_asso_callingtitle(uint8_t *buffer);
extern uint8_t dlms_asso_modify_callingtitle(uint8_t *buffer);
extern uint8_t dlms_asso_localtitle(uint8_t *buffer);
extern uint8_t dlms_asso_stoc(uint8_t *buffer);
extern uint8_t dlms_asso_ctos(uint8_t *buffer);
extern uint8_t dlms_asso_accept_fctos(void);
extern uint8_t dlms_asso_sc(void);
extern uint8_t dlms_asso_fc(uint8_t *buffer);
extern uint8_t dlms_asso_akey(uint8_t *buffer);
extern uint8_t dlms_asso_ekey(uint8_t *buffer);
extern uint8_t dlms_asso_dedkey(uint8_t *buffer);
extern uint8_t dlms_asso_applname(uint8_t *buffer);
extern uint8_t dlms_asso_mechname(uint8_t *buffer);
extern uint8_t dlms_asso_contextinfo(uint8_t *buffer);
extern enum __dlms_access_level dlms_asso_level(void);
extern uint8_t dlms_asso_suit(void);
extern void * dlms_asso_storage(void);
extern uint32_t dlms_asso_storage_size(void);
extern void * dlms_asso_attach_storage(uint16_t size);
extern void dlms_asso_random(uint8_t length, uint8_t *buffer);

#endif /* __DLMS_ASSOCIATION_H__ */
