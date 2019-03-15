/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASK_CONSOLE_H__
#define __TASK_CONSOLE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "tasks.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
extern const struct __task_sched task_console;
#endif

#endif /* __TASK_CONSOLE_H__ */

