/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H__
#define __BUTTON_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __switch main_cover;
extern const struct __switch sub_cover;
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
extern enum __switch_status mailslot_magnetic(void);
#endif

#endif /* __BUTTON_H__ */
