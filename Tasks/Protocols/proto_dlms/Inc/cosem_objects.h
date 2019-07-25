/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COSEM_OBJECTS_H__
#define __COSEM_OBJECTS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "object_template.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern TypeObject cosem_load_object(const char *table, uint8_t index);

#endif /* __COSEM_OBJECTS_H__ */
