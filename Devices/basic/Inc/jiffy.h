/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __JIFFY_H__
#define __JIFFY_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __jiffy jiffy;
extern void jitter_update(uint16_t val);

#endif /* __JIFFY_H__ */
