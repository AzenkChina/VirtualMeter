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
    M_P_ENERGY = 1,//�й����ܣ�mWh��
    M_Q_ENERGY = 2,//�޹����ܣ�mVarh��
    M_S_ENERGY = 3,//���ڵ��ܣ�mVAh��
    M_P_POWER = 4,//�й����ʣ�mW��
    M_Q_POWER = 5,//�޹����ʣ�mVar��
    M_S_POWER = 6,//���ڹ��ʣ�mVA��
    M_VOLTAGE = 7,//��ѹ��mV��
    M_CURRENT = 8,//������mA��
    M_POWER_FACTOR = 9,//����������1/1000��
    M_ANGLE = 10,//��ǣ�1/1000�ȣ�
    M_FREQUENCY = 11,//Ƶ�ʣ�1/1000Hz��
    /** �������ݵĸ�ʽ����֪�ģ�����Ҫָ�� */
    
/**
  * @brief  ����ʾ�б�����ӷǼ�������ʱ����ʾ���������ȡ��������
  *         ��Ҫ�������ĸ�ʽ����ʾ��Ĭ����ʾΪʮ���ƣ�����Ҫ��ʾΪ��
  *         ʮ���Ƹ�ʽ������Ҫ��������ʽ��ָ��
  */
    FMT_BIN = 24,//�����Ƹ�ʽ
    FMT_HEX = 25,//ʮ�����Ƹ�ʽ
    FMT_DATE = 26,//���ڸ�ʽ
    FMT_TIME = 27,//ʱ���ʽ
    FMT_DTIME = 28,//һ����ʱ���ʽ
    FMT_BCD = 29,//BCD��ʽ
    FMT_ASCII = 30,//ASCII��ʽ
    FMT_STR = 31,//�ַ�����ʽ
};

/**
  * @brief  �������� ���ޱ�ʶ
  */
enum __metering_quad
{
    M_QUAD_N = 0,//��
	M_QUAD_I = 1,//1����
	M_QUAD_II = 2,//2����
	M_QUAD_III = 3,//3����
    M_QUAD_V = 4,//4����
    
    M_PACT = 5,//�����й�
    M_NACT = 6,//�����й�
    M_PREACT = 7,//�����޹�
    M_NREACT = 8,//�����޹�
    
    M_DPACT = 9,//�����й�-�����й�
    M_DNACT = 10,//�����й�-�����й�
    M_DPREACT = 11,//�����޹�-�����޹�
    M_DNREACT = 12,//�����޹�-�����޹�
    
    M_TACT = 13,//���й�
    M_TREACT = 14,//���޹�
};

/**
  * @brief  �������� �����ʶ
  */
enum __metering_phase
{
    M_PHASE_N = 0,//N��
	M_PHASE_A = 1,//A��
	M_PHASE_B = 2,//B��
	M_PHASE_C = 4,//C��
	M_PHASE_AB = 3,//AB
	M_PHASE_AC = 5,//AC
	M_PHASE_BC = 6,//BC
    M_PHASE_T = 7,//��
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


//�ж�һ��U32 ID�Ƿ�Ϊ��ʾ��ʽ
#define M_UINTISFMT(val)            ((((val>>27)&0x1f) >= FMT_BIN) && (((val>>27)&0x1f) <= FMT_STR))

//�ж�һ��U32 ID�Ƿ�Ϊ�������ݱ�ʶ
#define M_UINTISID(val)             ((((val>>27)&0x1f) >= M_P_ENERGY) && (((val>>27)&0x1f) <= M_FREQUENCY))

//�ж�һ��U32 ID�Ƿ�Ϊ����
#define M_UINTISENERGY(val)         ((((val>>27)&0x1f) >= M_P_ENERGY) && (((val>>27)&0x1f) <= M_S_ENERGY))
//�ж�һ��U32 ID�Ƿ�Ϊ����
#define M_UINTISPOWER(val)          ((((val>>27)&0x1f) >= M_P_POWER) && (((val>>27)&0x1f) <= M_S_POWER))
//�ж�һ��U32 ID�Ƿ�Ϊ��ѹ
#define M_UINTISVOLTAGE(val)        (((val>>27)&0x1f) == M_VOLTAGE)
//�ж�һ��U32 ID�Ƿ�Ϊ����
#define M_UINTISCURRENT(val)        (((val>>27)&0x1f) == M_CURRENT)


/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_METERING_H__ */
