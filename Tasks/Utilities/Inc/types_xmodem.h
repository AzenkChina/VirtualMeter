/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_XMODEM_H__
#define __TYPES_XMODEM_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  ��ǰ״̬
  */
enum  __xmodem_status
{
    XMODEM_IDLE = 0, //��ǰ����
    XMODEM_BUSY, //��ǰæ
    XMODEM_ERROR, //��������
};

/**
  * @brief  xmodem task �Ķ���ӿ�
  */
struct __xmodem
{
    enum  __xmodem_status           (*status)(void); //��ǰ״̬
    uint32_t                        (*receive)(const char *name); //�����ļ�
    uint32_t                        (*send)(const char *name); //�����ļ�
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_XMODEM_H__ */
