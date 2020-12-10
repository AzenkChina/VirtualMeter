/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H__
#define __LCD_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported define -----------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )

#define LCD_MAX_WINDOWS             (2)//windows����

#define LCD_WINDOW_MAIN             (0)//��windows
#define LCD_WINDOW_SUB              (1)//��windows



#define LCD_MAX_LABELS              (18)//label����

#define LCD_LABEL_U1                (0)//���� U1
#define LCD_LABEL_U2                (1)//���� U2
#define LCD_LABEL_U3                (2)//���� U3
#define LCD_LABEL_I1                (3)//���� I1
#define LCD_LABEL_I2                (4)//���� I2
#define LCD_LABEL_I3                (5)//���� I3
#define LCD_LABEL_L1                (6)//���� L1
#define LCD_LABEL_L2                (7)//���� L2
#define LCD_LABEL_L3                (8)//���� L3
#define LCD_LABEL_LN                (9)//���� LN
#define LCD_LABEL_PF                (10)//���� PF
#define LCD_LABEL_RELAY             (11)//�̵���״̬
#define LCD_LABEL_COMM              (12)//ͨѶ����
#define LCD_LABEL_WARN              (13)//��������
#define LCD_LABEL_QUAD              (14)//��������
#define LCD_LABEL_RATE              (15)//����
#define LCD_LABEL_DATE              (16)//����
#define LCD_LABEL_TIM               (17)//ʱ��

#else

#if defined (BUILD_REAL_WORLD)
#define LCD_MAX_WINDOWS             (2)//windows����

#define LCD_WINDOW_MAIN             (0)//��windows
#define LCD_WINDOW_SUB              (1)//��windows



#define LCD_MAX_LABELS              (37)//label����

#define LCD_LABEL_U1                (0)//���� U1
#define LCD_LABEL_U2                (1)//���� U2
#define LCD_LABEL_U3                (2)//���� U3
#define LCD_LABEL_I1                (3)//���� I1
#define LCD_LABEL_I2                (4)//���� I2
#define LCD_LABEL_I3                (5)//���� I3
#define LCD_LABEL_L1                (6)//���� L1
#define LCD_LABEL_L2                (7)//���� L2
#define LCD_LABEL_L3                (8)//���� L3
#define LCD_LABEL_LN                (9)//���� LN
#define LCD_LABEL_PF                (10)//���� PF
#define LCD_LABEL_RELAY             (11)//�̵���״̬
#define LCD_LABEL_COMM              (12)//ͨѶ����
#define LCD_LABEL_WARN              (13)//��������
#define LCD_LABEL_QUAD              (14)//��������
#define LCD_LABEL_RATE              (15)//����
#define LCD_LABEL_DATE              (16)//����
#define LCD_LABEL_TIM               (17)//ʱ��

//���� LABEL Ϊ���� LABEL

#define LCD_LABEL_SIGNAL            (18)//�����ź�ǿ��
#define LCD_LABEL_BATRTC            (19)//RTC���״̬
#define LCD_LABEL_BATBAK            (20)//������״̬
#define LCD_LABEL_SECOND            (21)//���β��ʶ
#define LCD_LABEL_PRIM				(22)//һ�β��ʶ
#define LCD_LABEL_NET				(23)//������Դ����
#define LCD_LABEL_AUX				(24)//������Դ����
#define LCD_LABEL_PULSE             (25)//��������
#define LCD_LABEL_HOUSE             (26)//���ڷ���
#define LCD_LABEL_MON               (27)//�·ݷ���
#define LCD_LABEL_NOW               (28)//���·���
#define LCD_LABEL_LAST              (29)//���·���
#define LCD_LABEL_TOTAL             (30)//��Ϸ���
#define LCD_LABEL_NEG               (31)//�޹�����
#define LCD_LABEL_POS               (32)//�й�����
#define LCD_LABEL_TARIF             (33)//TARIF����
#define LCD_LABEL_CREDIT            (34)//CREDIT����
#define LCD_LABEL_ENERGY            (35)//ENERGY����
#define LCD_LABEL_NUMBER            (36)//NUMBER����
#endif

#endif




/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __lcd lcd;

#endif /* __LCD_H__ */
