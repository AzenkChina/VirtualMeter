/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_DISPLAY_H__
#define __TYPES_DISPLAY_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported macro ------------------------------------------------------------*/
//���� config_display.h �е���ʾ�б�����
#define DISP_SCROLL				((uint8_t)0) //ѭ���б�
#define DISP_ALTERNATE			((uint8_t)1) //�����б�
#define DISP_DEBUG				((uint8_t)2) //�����б�

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  display task �Ķ���ӿ�
  */
struct __display
{
    uint8_t                         (*change)(uint8_t channel); //�л���ǰ��ʾ�б�
    uint8_t                         (*channel)(void); //��ȡ��ǰ��ʾ�б�
    
    struct
    {
        struct
        {
			uint8_t					(*next)(void); //��ʾ��һ����ʾ��
			uint8_t					(*last)(void); //��ʾ��һ����ʾ��
			uint8_t					(*index)(uint8_t val); //��ת����������ʾ��
            
        }                           show;
        
		uint8_t						(*current)(void); //��ȡ��ǰ��ʾ��������
        
    }                               list;
    
    struct
    {
        struct
        {
            struct
            {
                uint8_t             (*get)(void); //��ȡ����ȫ��ʱ��
                uint8_t             (*set)(uint8_t second); //���ÿ���ȫ��ʱ��
                
            }                       start;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ����ʱ��
                uint8_t             (*set)(uint8_t second); //���ù���ʱ��
                
            }                       scroll;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ����ʱ��
                uint8_t             (*set)(uint8_t second); //���ñ���ʱ��
                
            }                       backlight;
            
        }                           time;
        
        struct
        {
            struct
            {
                uint8_t             (*get)(void); //��ȡ����С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //���ù���С������ʾλ��
                
            }                       power;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ��ѹС������ʾλ��
                uint8_t             (*set)(uint8_t dot); //���õ�ѹС������ʾλ��
                
            }                       voltage;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ����С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //���õ���С������ʾλ��
                
            }                       current;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ����С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //���õ���С������ʾλ��
                
            }                       energy;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ����С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //��������С������ʾλ��
                
            }                       demand;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ��Ǽ�������С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //�����������С������ʾλ��
                
            }                       angle;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡƵ�ʼ�������С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //����Ƶ������С������ʾλ��
                
            }                       freq;
            
            struct
            {
                uint8_t             (*get)(void); //��ȡ����������������С������ʾλ��
                uint8_t             (*set)(uint8_t dot); //���ù�����������С������ʾλ��
                
            }                       pf;
            
        }                           dot;
        
        struct
        {
            uint16_t                (*write)(uint8_t channel, void *id); //��һ����������ӵ�channel��ʾ�б���
            uint16_t                (*read)(uint8_t channel, uint16_t index, void *id); //��channel��ʾ�б���index��ʾ��ID
			uint8_t					(*amount)(uint8_t channel); //��ȡchannel��ʾ�б�����ʾ����Ŀ
			uint8_t					(*clean)(uint8_t channel); //���channel��ʾ�б�����ʾ��
            
        }                           list;
        
    }                               config;
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_DISPLAY_H__ */
