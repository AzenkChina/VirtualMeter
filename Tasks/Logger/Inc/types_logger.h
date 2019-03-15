/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_LOGGER_H__
#define __TYPES_LOGGER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  logger task �Ķ���ӿ�
  */
struct __logger
{
    uint16_t                        (*record)(const char *task_name, const char *details); //��¼һ����־
    uint16_t                        (*get)(uint16_t index, uint16_t length, char *buff); //��ȡ��־
    uint16_t                        (*amount)(void); //��־����
    uint16_t                        (*clear)(void); //��־���
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __API_LOGGER_H__ */
