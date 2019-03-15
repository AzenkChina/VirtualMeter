/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CPU_H__
#define __CPU_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
extern char *proc_self;
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __cpu cpu;

#endif /* __CPU_H__ */
