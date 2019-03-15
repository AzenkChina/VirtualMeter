/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
extern const struct __console console;
#endif

#endif /* __CONSOLE_H__ */
