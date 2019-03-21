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
/**	
  * @brief 应用层多包分包器
  */
struct __dlms_sequencer
{
    struct
    {
        uint32_t                    (*get)(void);
        uint32_t                    (*set)(uint32_t val);
        void                        (*end)(void);
        uint8_t                     (*is_end)(void);
        
    }                               block;
    
    struct
    {
        struct
        {
            uint32_t                (*get)(void);
            uint32_t                (*set)(uint32_t val);
            
        }                           amount;
        
        struct
        {
            uint32_t                (*get)(void);
            void                    (*increase)(void);
            void                    (*clear)(void);
            
        }                           counter;
        
    }                               rows;
};

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void dlms_appl_entrance(void **data,
                               const uint8_t *info,
                               uint16_t length,
                               uint8_t *buffer,
                               uint16_t buffer_length,
                               uint16_t *filled_length);

#endif /* __DLMS_APPLICATION_H__ */
