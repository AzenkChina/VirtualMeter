/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASKS_H__
#define __TASKS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported macro ------------------------------------------------------------*/
#define __TASKS_MONITOR             //�����Ƿ�����task���

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  ����ǰ״̬
  */
enum  __task_status
{
    TASK_NOTINIT = 0, //Ӧ��δ��ʼ��
    TASK_INIT, //Ӧ���ѳ�ʼ��
    TASK_RUN, //Ӧ����������
    TASK_SUSPEND, //Ӧ���Ѿ�����
    
    TASK_ERROR = 0x0f, //Ӧ�ó���
};

/**
  * @brief  ����ģ�͵��ȵ�Ԫ��ÿ��Ӧ��ģ���еĵ�һ���ṹ������ ����ģ�͵��ȵ�Ԫ
  */
struct __task_sched
{
    const char                      *name;
    
    void                            (*init)(void); //Ӧ�ó�ʼ��
    void                            (*loop)(void); //Ӧ�õĺ�̨��ѯ����
    void                            (*exit)(void); //Ӧ���˳�
    void                            (*reset)(void); //Ӧ�ûָ�Ĭ��ֵ(������ʼ��)
    enum            __task_status   (*status)(void); //Ӧ�õ�ǰ״̬
    
    void                            *api;
};

/**
  * @brief  ���������Ϣ�ӿ�
  */
struct __task_ctrl
{
    uint8_t							(*amount)(void); //Ӧ������
    const char						*(*info)(uint8_t index); //Ӧ����Ϣ
    uint8_t							(*index)(const char *name); //Ӧ������
    const struct __task_sched		*(*current)(void); //��ǰӦ��
    const struct __task_sched		*(*search)(const char *name); //�������Ʋ���Ӧ��
	uint8_t							(*reset)(const char *name); //�������Ƹ�λӦ��
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __task_ctrl task_ctrl;

#endif /* __TASKS_H__ */
