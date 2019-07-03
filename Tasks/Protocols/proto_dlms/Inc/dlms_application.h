/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_APPLICATION_H__
#define __DLMS_APPLICATION_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "dlms_types.h"

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void dlms_appl_entrance(const uint8_t *info,
                               uint16_t length,
                               uint8_t *buffer,
                               uint16_t buffer_length,
                               uint16_t *filled_length);

#endif /* __DLMS_APPLICATION_H__ */
