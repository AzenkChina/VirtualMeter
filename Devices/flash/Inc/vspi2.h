/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VSPI2_H__
#define __VSPI2_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
extern const struct __spi vspi2;
#endif /* !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) */

#endif /* __VSPI2_H__ */
