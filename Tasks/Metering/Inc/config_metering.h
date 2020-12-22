/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_METERING_H__
#define __CONFIG_METERING_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "tasks.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define NAME_METERING   "task_metering"

//#define __USE_PRIMARY_ENERGY__      //选择是否使用一次侧数据而不用二次侧数据
#define __USE_64BIT_ENERGY__        //选择是否使用64位数据类型来存储电能


#define MAX_QUAD			((uint8_t)4)
#define MAX_RATE			((uint8_t)4)
#define MAX_PHASE			((uint8_t)4)

/* Exported function prototypes ----------------------------------------------*/

#endif /* __CONFIG_METERING_H__ */
