/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_LOGGER_H__
#define __TYPES_LOGGER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  �¼�״̬
  */
enum __event_status
{
    EVES_END = 0, //�¼��ѽ���
    EVES_ONGONG = 1, //�¼����ڷ���
    EVES_PENDING = 2, //�¼����ڼ�ʱ�У��ȴ��������߽�����
};

/**
  * @brief  �¼���ʶ
  */
enum __event_id
{
    EVEI_ALL = 0, //�¼�ID��ʼ
    
    EVEI_POWERUP = 1, //�ϵ��¼�
    EVEI_POWERDOWN = 2, //�����¼�
    
    //...��������¼�
    
    EVEI_MAX = 512, //�¼�ID���ޣ����Ʋ�Ҫ�����޸ģ�
};

/**
  * @brief  logger task �Ķ���ӿ�
  */
struct __logger
{
    enum __event_status             (*status)(enum __event_id id); //��ȡ�¼�״̬
    uint32_t                        (*amount)(enum __event_id id); //��־����
    uint32_t                        (*clear)(enum __event_id id); //��־���
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern void __log_trrgger(enum __event_id id);

/* Exported function prototypes ----------------------------------------------*/
#define LOG(e)       __log_trrgger(e)

#endif /* __API_LOGGER_H__ */
