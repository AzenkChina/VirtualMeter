/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BATTERY_H__
#define __BATTERY_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define BAT_AMOUNT				((uint8_t)2)

#define BAT_RTC					((uint8_t)0)
#define BAT_BKP					((uint8_t)1)

/* Exported function prototypes ----------------------------------------------*/
extern const struct __battery battery[BAT_AMOUNT];

#endif /* __BATTERY_H__ */
