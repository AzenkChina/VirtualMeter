/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HDLC_DATALINK_H__
#define __HDLC_DATALINK_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void hdlc_init(void);
extern uint8_t hdlc_matched(const uint8_t *frame, uint16_t length);
extern void hdlc_tick(uint16_t tick);
extern uint16_t hdlc_get_address(void);
extern uint16_t hdlc_set_address(uint16_t addr);
extern uint16_t hdlc_get_timeout(void);
extern uint16_t hdlc_set_timeout(uint16_t sec);
extern uint8_t hdlc_get_window_size(void);
extern uint16_t hdlc_get_max_info_length(void);
extern uint16_t hdlc_request(uint8_t channel, const uint8_t *frame, uint16_t length);
extern uint16_t hdlc_response(uint8_t channel, uint8_t *frame, uint16_t length);
extern uint8_t hdlc_get_linked_channel(void);
extern uint16_t hdlc_send_info(uint8_t channel, const uint8_t *frame, uint16_t length);
extern bool hdlc_send_result(uint8_t channel);

#endif /* __HDLC_DATALINK_H__ */
