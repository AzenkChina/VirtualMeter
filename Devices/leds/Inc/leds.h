/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDS_H__
#define __LEDS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define LED_AMOUNT			((uint8_t)2)

#define LED_RELAY			((uint8_t)0)
#define LED_WARNING			((uint8_t)1)

/* Exported function prototypes ----------------------------------------------*/
extern const struct __led led[LED_AMOUNT];

#endif /* __LEDS_H__ */
