/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI1_H__
#define __SPI1_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
extern const struct __spi spi1;
#endif /* !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) */

#endif /* __SPI1_H__ */
