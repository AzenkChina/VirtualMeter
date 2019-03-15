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
enum __relay_req_action
{
    RELAY_ACT_RELEASE = 0,//���ԣ��޲���
    RELAY_ACT_CLOSE = 1,//��բ
    RELAY_ACT_OPEN = 2,//��բ
};

/**
  * @brief �̵���״̬
  *
  */
enum __relay_req_status
{
    RELAY_STA_CLOSE = 0,//��բ
    RELAY_STA_READY = 1,//��բ����
    RELAY_STA_OPEN = 2,//��բ
};

/**
  * @brief �������
  *
  */
enum __relay_req_policy
{
    RELAY_REQ_ACCEPT = 0,//����
    RELAY_REQ_DROP = 1,//����
    RELAY_REQ_MANGLE = 2,//�޸�
};

/**
  * @brief �̵�������Դ�����8������Դ
  *
  */
enum __relay_req_source
{
    RELAY_SRC_ACCOUNT = 0,//�˻�
	RELAY_SRC_COMMAND = 1,//����
    RELAY_SRC_TEST = 2,//����
    RELAY_SRC_OVERPOWER = 3,//����
    RELAY_SRC_EVENTS = 4,//�¼�
    RELAY_SRC_TAMPER = 5,//�Ե�
    
	RELAY_SRC_RESERVE6 = 6,
	RELAY_SRC_RESERVE7 = 7,
};

/**
  * @brief �̵�������Դ���ȼ�
  *
  */
struct __relay_priority_entry
{
    uint8_t open:4;
    uint8_t close:4;
};

/**
  * @brief �̵����������
  *
  */
struct __relay_policy_entry
{
    uint16_t open2ready:2;
    uint16_t open2close:2;
    uint16_t close2open:2;
    uint16_t close2ready:2;
    uint16_t ready2open:2;
    uint16_t ready2close:2;
};

/**
  * @brief �̵�������
  *
  */
struct __relay_conf
{
    struct __relay_priority_entry   priority[8];
    struct __relay_policy_entry     policy;
};

/**
  * @brief �̵�������
  *
  */
struct __relay_db
{
    uint8_t                         conf_select;
    enum __relay_req_status         status;
    uint16_t                        requests;
};



/**
  * @brief  disconnect task �Ķ���ӿ�
  */
struct __disconnect
{
    uint8_t							(*request)(enum __relay_req_source source, enum __relay_req_action action);
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_DISCONNECT_H__ */
