/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_LOGGER_H__
#define __TYPES_LOGGER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  �¼���ʶ
  * 		����ֻ�ṩ��ʶ���������¼�����Ч�ģ��¼�����Ч����� �¼����ñ� �ж���
  */
enum __event_id
{
    EVI_ALL				= 0, //�¼�ID��ʼ
    
    EVI_POWERUP			= 1, //�ϵ��¼�
    EVI_POWERDOWN		= 2, //�����¼�
    //...��������¼�
    
    EVI_MAX				= 256, //�¼�ID���ޣ���ֵ���޸ģ����Ʋ�Ҫ�����޸ģ�
};

/**
  * @brief  �¼�״̬
  */
enum __event_status
{
	EVS_STARTING	= 1, //�¼���Ҫ��������ʱ�У�
    EVS_ENDING		= 2, //�¼���Ҫ��������ʱ�У�
	EVS_STARTED		= 3, //�¼��ѷ���
    EVS_ENDED		= 0, //�¼��ѽ���
};


/**
  * @brief  logger task �Ķ���ӿ�
  *			void (*callback)(enum __event_id id, enum __event_status status, uint16_t mapping)
  */
struct __logger
{
	struct
	{
    	enum __event_status			(*read)(enum __event_id id); //��ȡ�¼�״̬
		bool						(*toggle)(enum __event_id id, enum __event_status status); //�޸��¼�״̬
		
	}								status;
	struct
	{
		bool						(*add)(void (*callback)(enum __event_id, enum __event_status, uint16_t)); //��ӻص�
		bool						(*remove)(void (*callback)(enum __event_id, enum __event_status, uint16_t)); //ɾ���ص�
		
	}								monitor;
	
	uint16_t						(*map)(enum __event_id id);
	enum __event_id					(*unmap)(uint16_t mapping);
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __API_LOGGER_H__ */
