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
  * @brief  ������ȵ�ǰ״̬
  */
enum  __sched_status
{
    SCHED_NODEF = 0,//Ӧ��δ����
    SCHED_RUNNING = 0x0f,//�������ȣ�loop������ѯ��
    SCHED_HANGED = 0xf0,//���ȱ�����loop����ѯ��
};

/**
  * @brief  ������ȿ��ƽӿ�
  */
struct __task_ctrl
{
    enum  __sched_status            (*hang)(const char *name);
	enum  __sched_status            (*reset)(const char *name);
    enum  __sched_status            (*run)(const char *name);
    enum  __sched_status            (*status)(const char *name);
};

/**
  * @brief  ������Ϣ�ӿ�
  */
struct __task_trace
{
    uint8_t                         (*amount)(void);
    const struct __task_sched       *(*current)(void);
    const struct __task_sched       *(*belong)(void);
    uint8_t                         (*exist)(const char *name);
    const char                      *(*info)(uint8_t index);
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __task_ctrl task_ctrl;
extern const struct __task_trace task_trace;

#endif /* __TASKS_H__ */
