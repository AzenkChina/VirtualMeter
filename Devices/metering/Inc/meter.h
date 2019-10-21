/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __METER_H__
#define __METER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __meter meter;
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
uint8_t is_powered(void);
#endif

#endif /* __METER_H__ */
