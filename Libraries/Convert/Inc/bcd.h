/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BCD_H__
#define __BCD_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern uint8_t B2U8(uint8_t val);
extern uint8_t U2B8(uint8_t val);
extern uint16_t B2U16(uint16_t val);
extern uint16_t U2B16(uint16_t val);
extern uint32_t B2U32(uint32_t val);
extern uint32_t U2B32(uint32_t val);

#endif /* __BCD_H__ */
