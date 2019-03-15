/**
 * @brief		
 * @details		
 * @date		azenk@2018-12-14
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __API_H__
#define __API_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
struct __task_api
{
	void *          (*query)(char *name);
	void			(*release)(void);
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __task_api api;

#endif /* __API_H__ */
