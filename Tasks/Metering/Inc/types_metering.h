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

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  �������� ����
  */
enum __metering_item
{
    M_NULL = 0,//δ����
    M_P_POWER = 1,//�й����ʣ�mW��
    M_Q_POWER = 2,//�޹����ʣ�mVar��
    M_S_POWER = 3,//���ڹ��ʣ�mVA��
    M_P_ENERGY = 4,//�й����ܣ�mWh��
    M_Q_ENERGY = 5,//�޹����ܣ�mVarh��
    M_S_ENERGY = 6,//���ڵ��ܣ�mVAh��
	M_P_CURRENT_DEMAND = 7,//�й���ǰ������mW��
    M_Q_CURRENT_DEMAND = 8,//�޹���ǰ������mVar��
    M_S_CURRENT_DEMAND = 9,//���ڵ�ǰ������mVA��
	M_P_MAX_DEMAND = 10,//�й����������mW��
    M_Q_MAX_DEMAND = 11,//�޹����������mVar��
    M_S_MAX_DEMAND = 12,//�������������mVA��
    M_P_MAX_DEMAND_TIM = 13,//�й��������ʱ�䣨UNIXʱ�����
    M_Q_MAX_DEMAND_TIM = 14,//�޹��������ʱ�䣨UNIXʱ�����
	M_S_MAX_DEMAND_TIM = 15,//�����������ʱ�䣨UNIXʱ�����
    M_VOLTAGE = 16,//��ѹ��mV��
    M_CURRENT = 17,//������mA��
    M_POWER_FACTOR = 18,//����������1/1000��
    M_ANGLE = 19,//��ǣ�1/1000�ȣ�
    M_FREQUENCY = 20,//Ƶ�ʣ�1/1000Hz��
    /** �������ݵĸ�ʽ����֪�ģ�����Ҫָ�� */
    
/**
  * @brief  ����ʾ�б�����ӷǼ�������ʱ����ʾ���������ȡ��������
  *         ��Ҫ�������ĸ�ʽ����ʾ��Ĭ����ʾΪʮ���ƣ�����Ҫ��ʾΪ��
  *         ʮ���Ƹ�ʽ������Ҫ��������ʽ��ָ��
  */
    FMT_BIN = 25,//�����Ƹ�ʽ
    FMT_HEX = 26,//ʮ�����Ƹ�ʽ
    FMT_DATE = 27,//���ڸ�ʽ
    FMT_TIME = 28,//ʱ���ʽ
    FMT_DTIME = 29,//һ����ʱ���ʽ
    FMT_BCD = 30,//BCD��ʽ
    FMT_STR = 31,//�ַ�����ʽ
};

/**
  * @brief  �������� ���ޱ�ʶ
  */
enum __metering_quad
{
    M_QUAD_T = 0,//�ܣ�1+2+3+4��
	M_QUAD_1 = 1,//1����
	M_QUAD_2 = 2,//2����
	M_QUAD_3 = 3,//3����
    M_QUAD_4 = 4,//4����
    M_QUAD_14 = 5,//14���ޣ������й�������޹�1��
    M_QUAD_23 = 6,//23���ޣ������й�������޹�2��
    M_QUAD_12 = 7,//12���ޣ������޹�������й�1��
    M_QUAD_34 = 8,//34���ޣ������޹�������й�2��
};

/**
  * @brief  �������� �����ʶ
  */
enum __metering_phase
{
    M_PHASE_T = 0,//��
	M_PHASE_A = 1,//A��
	M_PHASE_B = 2,//B��
	M_PHASE_C = 3,//C��
    M_PHASE_N = 4,//����
	M_PHASE_AB = 5,//AB
	M_PHASE_AC = 6,//AC
	M_PHASE_BC = 7,//BC
};

/**
  * @brief  �������� ��ʶ��
  */
struct __metering_identifier
{
    uint32_t item       :5;// enum __metering_item
    uint32_t quad       :4;// enum __metering_quad
    uint32_t phase      :3;// enum __metering_phase
    uint32_t rate       :4;//����0 ~ 15
	uint32_t offset		:16;//�Զ����ֶ�
};




/**
  * @brief  ���ܼ���״̬
  */
enum __metering_status
{
    M_ENERGY_STOP = 0,//���ܲ�����
    M_ENERGY_START,//���ܼ���
};

/**
  * @brief  metering task �Ķ���ӿ�
  */
struct __metering
{
    //��ȡһ����ǰ��������
    enum __metering_item                    (*instant)(struct __metering_identifier id, int64_t *val);
    
    //��ȡ��������Ӷ����������
    enum __metering_item                    (*recent)(struct __metering_identifier id, int64_t *val);
    
    //��һ��������ת��Ϊһ�β�ֵ
    //���궨�� __USE_PRIMARY_ENERGY__ ��ʱ����Щ����������������������κδ���
    int64_t                                 (*primary)(struct __metering_identifier id, int64_t val);
	
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
            void                            (*enter)(void *args); //��ʼУ׼
            void                            (*exit)(void); //�˳�У׼
            
        }                                   calibration;
        
    }                                       config;
    
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//��һ���������ݱ�ʶת����U32 ID
#define M_ID2UINT(i,q,p,r,o)		((uint32_t)(((i&0x1f)<<27)|((q&0x0f)<<23)|((p&0x07)<<20)|((r&0x0f)<<16))|((o&0xffff)<<0))

//��һ��U32 IDת���ɼ������ݱ�ʶ
#define M_UINT2ID(val, i)           i.item = ((val>>27)&0x1f);\
                                    i.quad = ((val>>23)&0x0f);\
                                    i.phase = ((val>>20)&0x07);\
                                    i.rate = ((val>>16)&0x0f);\
									i.offset = ((val>>0)&0xffff)

//��һ��U32 IDת���ɷǼ������ݱ�ʶ
#define M_UINT2NMB(src, dst)        (dst = (src&0x07ffffff))

//�ж�һ��U32 ID�Ƿ�Ϊ�������ݱ�ʶ
#define M_UINTISID(val)             ((((val>>27)&0x1f) >= M_P_POWER) && (((val>>27)&0x1f) <= M_FREQUENCY))

//�ж�һ��U32 ID�Ƿ��ǵ�ѹ
#define M_UINTISVOLTAGE(val)        (((val>>27)&0x1f) == M_VOLTAGE)
//�ж�һ��U32 ID�Ƿ��ǵ���
#define M_UINTISCURRENT(val)        (((val>>27)&0x1f) == M_CURRENT)
//�ж�һ��U32 ID�Ƿ��ǹ���
#define M_UINTISPOWER(val)          ((((val>>27)&0x1f) >= M_P_POWER) && (((val>>27)&0x1f) <= M_S_POWER))
//�ж�һ��U32 ID�Ƿ��ǵ���
#define M_UINTISENERGY(val)         ((((val>>27)&0x1f) >= M_P_ENERGY) && (((val>>27)&0x1f) <= M_S_ENERGY))
//�ж�һ��U32 ID�Ƿ�������
#define M_UINTISDEMAND(val)         ((((val>>27)&0x1f) >= M_P_CURRENT_DEMAND) && (((val>>27)&0x1f) <= M_S_MAX_DEMAND))
//�ж�һ��U32 ID�Ƿ�����������ʱ��
#define M_UINTISDEMANDTIM(val)      ((((val>>27)&0x1f) >= M_P_MAX_DEMAND_TIM) && (((val>>27)&0x1f) <= M_S_MAX_DEMAND_TIM))
//�ж�һ��U32 ID�Ƿ�ָ�������ݵ���ʾ��ʽ
#define M_UINTISFMT(val)            ((((val>>27)&0x1f) >= FMT_BIN) && (((val>>27)&0x1f) <= FMT_STR))


/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_METERING_H__ */
