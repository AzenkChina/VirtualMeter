/**
 * @brief		
 * @details		
 * @date		2019-12-14
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TWIGS_H__
#define __TWIGS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  
  */
struct __twigs
{
    uint32_t						(*start)(const char *name, uint32_t stacks, void (*proc)(void *args));
	bool							(*stop)(uint32_t id);
	struct
	{
		bool						(*byid)(uint32_t id);
		bool						(*byfun)(void (*proc)(void *args));
		
	}								status;
	
	uint32_t						(*sleep)(uint32_t msecond);
};

/* Exported function prototypes ----------------------------------------------*/

#endif /* __TWIGS_H__ */
