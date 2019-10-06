/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VIIC1_H__
#define __VIIC1_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
extern const struct __iic viic1;
#endif /* !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) */

#endif /* __VIIC1_H__ */
