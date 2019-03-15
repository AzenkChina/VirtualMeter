/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEYS_H__
#define __KEYS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
#define KEY_ID_00               ((uint16_t)0x00)
#define KEY_ID_01               ((uint16_t)0x01)
#define KEY_ID_02               ((uint16_t)0x02)
#define KEY_ID_03               ((uint16_t)0x03)
#define KEY_ID_04               ((uint16_t)0x04)
#define KEY_ID_05               ((uint16_t)0x05)
#define KEY_ID_06               ((uint16_t)0x06)
#define KEY_ID_07               ((uint16_t)0x07)
#define KEY_ID_08               ((uint16_t)0x08)
#define KEY_ID_09               ((uint16_t)0x09)

#define KEY_ID_BACK             ((uint16_t)0x0A)
#define KEY_ID_ENTER            ((uint16_t)0x0B)
#define KEY_ID_PROGRAM          ((uint16_t)0x0C)

#define KEY_ID_UP               ((uint16_t)0x10)
#define KEY_ID_DOWN             ((uint16_t)0x20)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __keys keys;

#endif /* __KEYS_H__ */
