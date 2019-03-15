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

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void hdlc_init(void);
extern uint8_t hdlc_matched(const uint8_t *frame, uint16_t length);
extern void hdlc_tick(uint16_t tick);
extern uint16_t hdlc_request(uint8_t channel, const uint8_t *frame, uint16_t length);
extern uint16_t hdlc_response(uint8_t channel, uint8_t *frame, uint16_t length);

#endif /* __HDLC_DATALINK_H__ */
