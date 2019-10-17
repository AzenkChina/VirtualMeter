/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_COMM_H__
#define __TYPES_COMM_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief	�˿�����
  */
enum __port_type
{
	PORT_TCP = 0,
	PORT_UDP = 1,
	PORT_FTP = 2,
	PORT_SMTP = 3,
	PORT_SMS = 4,
	PORT_HDLC = 5,
	PORT_MBUS = 6,
	PORT_ZIGBEE = 7,

	PORT_GPRS = 200,
	PORT_PLC = 201,
	PORT_GPRS_PLC = 202,
};

/**
  * @brief	�˿�״̬ 
  */
enum __port_status
{
    PORT_IDLE = 0,
    PORT_BUSY,
};


/**
  * @brief  protocol task �Ķ���ӿ�
  */
struct __comm
{
    struct
    {
        enum __port_type			(*type)(uint8_t channel); //��ȡ�˿����� 
        enum __port_status			(*status)(uint8_t channel); //��ȡ�˿�״̬ 
        
    }                               attrib;
    
    struct
    {
        uint16_t                    (*set)(uint8_t channel, uint16_t msecond); //����֡��ʱʱ��
        uint16_t                    (*get)(uint8_t channel); //��ȡ��ǰ֡��ʱʱ��
        
    }                               timeout;
    
    struct
    {
        enum __baud                 (*set)(uint8_t channel, enum __baud baudrate); //���ò�����
        enum __baud                 (*get)(uint8_t channel); //��ȡ��ǰ������
        
    }                               baudrate;
    
    struct
    {
        enum __parity				(*set)(uint8_t channel, enum __parity parity); //����У��λ
        enum __parity				(*get)(uint8_t channel); //��ȡ��ǰУ��λ
        
    }                                parity;
    
    struct
    {
        enum __stop					(*set)(uint8_t channel, enum __stop stop); //����ֹͣλ
        enum __stop					(*get)(uint8_t channel); //��ȡ��ǰֹͣλ
        
    }                                stop;
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_COMM_H__ */
