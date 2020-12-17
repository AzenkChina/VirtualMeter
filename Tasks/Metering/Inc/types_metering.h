/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_METERING_H__
#define __TYPES_METERING_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "mids.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  ���ܼ���״̬
  */
enum __metering_status
{
    M_ENERGY_STOP = 0,//���ܲ�����
    M_ENERGY_START = 0xff,//���ܼ���
};

/**
  * @brief  metering task �Ķ���ӿ�
  */
struct __metering
{
    //��ȡһ����ǰ��������
    enum __meta_item                        (*instant)(struct __meta_identifier id, int64_t *val);
    
    //��ȡ��������Ӷ����������
    enum __meta_item                        (*recent)(struct __meta_identifier id, int64_t *val);
    
    //��һ��������ת��Ϊһ�β�ֵ
    //���궨�� __USE_PRIMARY_ENERGY__ ��ʱ����Щ����������������������κδ���
    int64_t                                 (*primary)(struct __meta_identifier id, int64_t val);
	
    struct
    {
        struct
        {
            uint8_t                         (*read)(void); //��ȡ��ǰ���з���
            uint8_t                         (*change)(uint8_t rate); //���õ�ǰ���з���
            uint8_t                         (*max)(void); //���֧�ֵķ����� 
            
        }                                   rate;
        
        struct
        {
	        struct
	        {
	            uint32_t					(*get)(void); //��ȡ������ȣ�����ĸ֮��ģ�
	            uint32_t					(*set)(uint32_t val); //���õ�����ȣ���ĸ��1��
	            uint32_t					(*get_num)(void); //��ȡ������ȷ���
	            uint32_t					(*set_num)(uint32_t val); //���õ�����ȷ���
	            uint32_t					(*get_denum)(void); //��ȡ������ȷ�ĸ
	            uint32_t					(*set_denum)(uint32_t val); //���õ�����ȷ�ĸ
	            
	        }								current;
	        
	        struct
	        {
	            uint32_t					(*get)(void); //��ȡ��ѹ��ȣ�����ĸ֮��ģ�
	            uint32_t					(*set)(uint32_t val); //���õ�ѹ��ȣ���ĸ��1��
	            uint32_t					(*get_num)(void); //��ȡ��ѹ��ȷ���
	            uint32_t					(*set_num)(uint32_t val); //���õ�ѹ��ȷ���
	            uint32_t					(*get_denum)(void); //��ȡ��ѹ��ȷ�ĸ
	            uint32_t					(*set_denum)(uint32_t val); //���õ�ѹ��ȷ�ĸ
	            
	        }								voltage;
	        
        }                                   ratio;
        
        struct
        {
            uint8_t                         (*get_period)(void); //��ȡ����ʱ�䣨���ӣ�
            uint8_t                         (*set_period)(uint8_t minute); //���û���ʱ�䣨���ӣ�
            uint8_t                         (*get_multiple)(void); //��ȡ����ʱ�䱶����Ϊ1ʱΪblock demand������Ϊsliding demand��
            uint8_t                         (*set_multiple)(uint8_t val); //���û���ʱ�䱶��
            
			void                            (*clear_current)(void); //��յ�ǰ����
            void                            (*clear_max)(void); //����������
            
        }                                   demand;
        
        struct
        {
            enum __metering_status          (*start)(void); //��ʼ���ܼ���
            enum __metering_status          (*stop)(void); //ֹͣ���ܼ���
            enum __metering_status          (*status)(void); //���ܼ���״̬
            void                            (*clear)(void); //��������
            
        }                                   energy;
        
        struct
        {
            struct
            {
                uint32_t                    (*get)(void); //��ȡ�й����峣��
                uint32_t                    (*set)(uint32_t val); //�����й����峣��
                
            }                               active;
            
            struct
            {
                uint32_t                    (*get)(void); //��ȡ�޹����峣��
                uint32_t                    (*set)(uint32_t val); //�����޹����峣��
                
            }                               reactive;
            
        }                                   div;
        
        struct
        {
            struct
            {
                uint32_t                    (*get)(void); //��ȡУ׼��ѹ
                uint32_t                    (*set)(uint32_t val); //����У׼��ѹ
                
            }                               voltage;
            
            struct
            {
                uint32_t                    (*get)(void); //��ȡУ׼����
                uint32_t                    (*set)(uint32_t val); //����У׼����
                
            }                               current;
			
            void                            (*enter)(void *args); //��ʼУ׼
            void                            (*exit)(void); //�˳�У׼
            
        }                                   calibration;
        
    }                                       config;
    
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_METERING_H__ */
