/**
 * @brief		
 * @details		
 * @date		azenk@2016-11-15
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KERNEL_H__
#define __KERNEL_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  �ں����еȼ�
  */
enum __klevel
{
    SYSTEM_BOOT = 0,//ϵͳ����ʱ�ĳ�ʼ״̬������ʱ��Ӧ���д�״̬
    
    SYSTEM_SLEEP = 0x30,//ϵͳ�������״̬
    SYSTEM_WAKEUP = 0x3f,//�������״̬����power�������ƽ����״̬
    
    SYSTEM_RUN = 0xc0,//ϵͳ��������״̬
    SYSTEM_REBOOT = 0xcf,//�������е���״̬����power�������ƽ����״̬
    
    SYSTEM_RESET = 0xf0,//ϵͳ����
};

/* Exported define ------------------------------------------------------------*/
//app��ѯƵ�ʣ�ȡֵ�ռ�Ϊ5~1000���������ܱ�1000����
#define KERNEL_LOOP_FREQ                ((uint16_t)10)
//���߻��Ѽ��ʱ�䣬��λ����
#define KERNEL_LOOP_SLEEPED             ((uint32_t)1000)
//������״̬��task�� loop ��������ѯ����
#define PERIOD_RUNNING                  ((uint32_t)((KERNEL_LOOP_FREQ >= 10 && KERNEL_LOOP_FREQ <= 1000)? (1000 / KERNEL_LOOP_FREQ) : 10))

/* Exported macro -------------------------------------------------------------*/
//task�ﵱǰ loop ��������ѯ����
//task�п���ʹ�øú������򵥵ļ�ʱ��
#define KERNEL_PERIOD                   ((system_status() == SYSTEM_SLEEP)? KERNEL_LOOP_SLEEPED:PERIOD_RUNNING)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern enum __klevel system_status(void);
extern uint16_t system_usage(void);

#endif /* __KERNEL_H__ */
