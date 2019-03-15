/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_LOGGER_H__
#define __TYPES_LOGGER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  logger task 的对外接口
  */
struct __logger
{
    uint16_t                        (*record)(const char *task_name, const char *details); //记录一条日志
    uint16_t                        (*get)(uint16_t index, uint16_t length, char *buff); //读取日志
    uint16_t                        (*amount)(void); //日志条数
    uint16_t                        (*clear)(void); //日志清空
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __API_LOGGER_H__ */
