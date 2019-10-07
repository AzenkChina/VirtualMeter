/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEYS_H__
#define __KEYS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
#define KEY_ID_00               ((uint16_t)0x0001)
#define KEY_ID_01               ((uint16_t)0x0002)
#define KEY_ID_02               ((uint16_t)0x0004)
#define KEY_ID_03               ((uint16_t)0x0008)
#define KEY_ID_04               ((uint16_t)0x0010)
#define KEY_ID_05               ((uint16_t)0x0020)
#define KEY_ID_06               ((uint16_t)0x0040)
#define KEY_ID_07               ((uint16_t)0x0080)
#define KEY_ID_08               ((uint16_t)0x0100)
#define KEY_ID_09               ((uint16_t)0x0200)

#define KEY_ID_BACK             ((uint16_t)0x0400)
#define KEY_ID_ENTER            ((uint16_t)0x0800)
#define KEY_ID_PROGRAM          ((uint16_t)0x1000)

#define KEY_ID_UP               ((uint16_t)0x2000)
#define KEY_ID_DOWN             ((uint16_t)0x4000)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __keys keys;

#endif /* __KEYS_H__ */
