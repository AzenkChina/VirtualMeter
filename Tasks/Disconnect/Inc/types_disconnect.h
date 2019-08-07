/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_DISCONNECT_H__
#define __TYPES_DISCONNECT_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
#define RELAY_CONF_CONTENT              (8) //������������


/**
  * @brief �̵�������
  *
  */
enum __relay_action
{
    RELAY_ACT_RELEASE = 0,//���ԣ��޲���
    RELAY_ACT_CLOSE = 1,//��բ
    RELAY_ACT_OPEN = 2,//��բ
};

/**
  * @brief �̵���״̬
  *
  */
enum __relay_status
{
    RELAY_STA_CLOSE = 0,//��բ
    RELAY_STA_OPEN = 1,//��բ
};

/**
  * @brief �������
  *
  */
enum __relay_policy
{
    RELAY_POL_ACCEPT = 0,//����
    RELAY_POL_DROP = 1,//����
    RELAY_POL_MANGLE = 2,//�޸�
};

/**
  * @brief �̵�������Դ����16��
  *
  */
enum __relay_requests
{
    RELAY_REQ_SLOT_01 = 0,
	RELAY_REQ_SLOT_02,
    RELAY_REQ_SLOT_03,
    RELAY_REQ_SLOT_04,
    RELAY_REQ_SLOT_05,
    RELAY_REQ_SLOT_06,
	RELAY_REQ_SLOT_07,
	RELAY_REQ_SLOT_08,
    RELAY_REQ_SLOT_09,
	RELAY_REQ_SLOT_10,
    RELAY_REQ_SLOT_11,
    RELAY_REQ_SLOT_12,
    RELAY_REQ_SLOT_13,
    RELAY_REQ_SLOT_14,
	RELAY_REQ_SLOT_15,
	RELAY_REQ_SLOT_16,
};

/**
  * @brief  disconnect task �Ķ���ӿ�
  */
struct __relay
{
    uint8_t							(*request)(enum __relay_requests source, enum __relay_action action);
    enum __relay_status             (*status)(void);
    enum __relay_requests           (*reason)(void);
	struct
	{
		uint8_t						(*set)(enum __relay_requests source, enum __relay_action action, uint8_t priority);
		uint8_t						(*get)(enum __relay_requests source, enum __relay_action action);
									
	}								priority;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define RELAY_ACCOUNT       RELAY_REQ_SLOT_01
#define RELAY_COMMAND       RELAY_REQ_SLOT_02
#define RELAY_TEST          RELAY_REQ_SLOT_03
#define RELAY_OVERPOWER     RELAY_REQ_SLOT_04
#define RELAY_EVENTS        RELAY_REQ_SLOT_05
#define RELAY_TAMPER        RELAY_REQ_SLOT_06

/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_DISCONNECT_H__ */
