/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "string.h"
#include "task_logger.h"
#include "types_logger.h"
#include "config_logger.h"
#include "crc.h"

/* Private define ------------------------------------------------------------*/
#define MAX_MONITORS	((uint8_t)16)

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  �¼���Ϊ
  */
enum __event_behaviors
{
	EVB_PU		= 0x0001, //��������ʱ�ɴ���
	EVB_PD		= 0x0002, //�͹���ʱ�ɴ���
	EVB_STR		= 0x0004, //�¼���ʼ�ɴ���
	EVB_END		= 0x0008, //�¼������ɴ���
	EVB_STRPU	= 0x0010, //�ϵ�ʱǿ�ƴ�����ʼ
	EVB_ENDPU	= 0x0020, //�ϵ�ʱǿ�ƴ�������
	EVB_STRPD	= 0x0040, //����ʱǿ�ƴ�����ʼ������ʱ�¼�״̬ǿ������λ EVS_STARTING���´��ϵ紦��
	EVB_ENDPD	= 0x0080, //����ʱǿ�ƴ�������������ʱ�¼�״̬ǿ������λ EVS_ENDING���´��ϵ紦��
};

/**
  * @brief  �¼����ñ�
  */
struct __logger_table
{
	enum __event_id id; //�¼�ID
	uint16_t mapping; //ӳ����ID
	uint16_t behaviors; //�¼�����
};

/**
  * @brief  �¼���ǰ״̬
  */
struct __logger_status
{
	uint8_t value[(EVI_MAX*2+7)/8];
	uint32_t check;
};

/**
  * @brief  �¼�������
  */
struct __logger_monitor
{
	void (*callback[MAX_MONITORS])(enum __event_id, enum __event_status, uint16_t);
	uint32_t check;
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __task_status task_status = TASK_NOTINIT;

/**
  * @brief  �¼����ñ�
  */
static const struct __logger_table table[] = 
{
	/** �¼���ʶ		ӳ���ʶ	�¼���Ϊ */
	{EVI_POWERUP,		11,			(EVB_PU | EVB_STR | EVB_ENDPD | EVB_STRPU)},
	{EVI_POWERDOWN,		12,			(EVB_PU | EVB_STR | EVB_ENDPU | EVB_STRPD)},
};

/**
  * @brief  ������¼�״̬�֣�ÿ���¼�ռ��2��bit���� enum __event_id ��˳��һ��
  */
static struct __logger_status logger_status;

/**
  * @brief  �¼�������
  */
static struct __logger_monitor monitors;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ��ȡָ�����¼���״̬
  */
static enum __event_status logger_event_status_read(enum __event_id id)
{
	enum __event_id val = EVI_ALL;
	
	if(logger_status.check != crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check))))
	{
		return(EVS_ENDED);
	}
	
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].id == id)
		{
			val = id;
		}
	}
	
    return((enum __event_status)((logger_status.value[val/4] >> ((val%4)*2)) & 0x03));
}

/**
  * @brief  ����ָ�����¼���״̬
  */
static bool logger_event_status_toggle(enum __event_id id, enum __event_status status)
{
	uint16_t behaviors = 0;
	enum __event_status status_current;
	
	if((id > EVI_MAX) || (id <= EVI_ALL))
	{
		return(false);
	}
	
	if(logger_status.check != crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check))))
	{
		return(false);
	}
	
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].id == id)
		{
			behaviors = table[n].behaviors;
			break;
		}
	}
	
	if(!behaviors)
	{
		return(false);
	}
	
	status_current = (enum __event_status)((logger_status.value[id/4] >> ((id%4)*2)) & 0x03);
	
	//����״̬�£�EVB_PU δ��λ ������
    if((system_status() == SYSTEM_RUN) && !(behaviors & EVB_PU))
    {
		return(false);
	}
	
	//�͹���״̬�£�EVB_PD δ��λ ������
    if(((system_status() == SYSTEM_SLEEP) || (system_status() == SYSTEM_WAKEUP)) && !(behaviors & EVB_PD))
    {
		return(false);
	}
	
	//����״̬
	logger_status.value[id/4] &= ~(0x03 << ((id%4)*2));
	logger_status.value[id/4] |= (((uint8_t)status) << ((id%4)*2));
	logger_status.check = crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check)));
	
	//�¼���ʼ�ɴ���
	if(behaviors & EVB_STR)
	{
		
	}
	
	//�¼������ɴ���
	if(behaviors & EVB_END)
	{
		
	}
	
    return(true);
}

/**
  * @brief  ���һ�����ص�
  */
static bool logger_event_add_monitor(void (*callback)(enum __event_id, enum __event_status, uint16_t))
{
	uint16_t blank = 0xffff;
	
	if(!callback)
	{
		return(false);
	}
	
	if(monitors.check != crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check))))
	{
		return(false);
	}
	
	for(uint8_t n=0; n<MAX_MONITORS; n++)
	{
		if((void *)(monitors.callback[n]) == (void *)0)
		{
			if(blank == 0xffff)
			{
				blank = n;
			}
			continue;
		}
		
		if(monitors.callback[n] == callback)
		{
			return(false);
		}
	}
	
	if(blank >= MAX_MONITORS)
	{
		return(false);
	}
	
	monitors.callback[blank] = callback;
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	return(true);
}

/**
  * @brief  ɾ��һ�����ص�
  */
static bool logger_event_remove_monitor(void (*callback)(enum __event_id, enum __event_status, uint16_t))
{
	if(!callback)
	{
		return(false);
	}
	
	if(monitors.check != crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check))))
	{
		return(false);
	}
	
	for(uint8_t n=0; n<MAX_MONITORS; n++)
	{
		if((void *)(monitors.callback[n]) == (void *)0)
		{
			continue;
		}
		
		if(monitors.callback[n] == callback)
		{
			monitors.callback[n] = (void *)0;
		}
	}
	
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	return(true);
}

/**
  * @brief  ��ȡӳ����ID
  */
static uint16_t logger_event_map(enum __event_id id)
{
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].id == id)
		{
			return(table[n].mapping);
		}
	}
	
	return(0);
}

/**
  * @brief  ��ȡӳ��ǰ��ID
  */
static enum __event_id logger_event_unmap(uint16_t mapping)
{
	for(uint8_t n=0; n<(sizeof(table)/sizeof(struct __logger_table)); n++)
	{
		if(table[n].mapping == mapping)
		{
			return(table[n].id);
		}
	}
	
	return(EVI_ALL);
}

/**
  * @brief  
  */
static const struct __logger logger = 
{
	.status			= 
	{
    	.read		= logger_event_status_read,
		.toggle		= logger_event_status_toggle,
	},
	
	.monitor		= 
	{
		.add		= logger_event_add_monitor,
		.remove		= logger_event_remove_monitor,
	},
	
	.map			= logger_event_map,
	.unmap			= logger_event_unmap,
};








/**
  * @brief  
  */
static void task_logger_init(void)
{
	//ֻ�������ϵ�״̬�²����У�����״̬�²�����
    if(system_status() == SYSTEM_RUN)
    {
	    task_status = TASK_INIT;
    }
	
	//��ʼ�������б�
    memset(&monitors, 0, sizeof(monitors));
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	
	//...�����¼�״̬
	memset(&logger_status, 0, sizeof(logger_status));
	
	//...��¼�ϵ�ʱ��Ҫ������¼����ӳٴ���
}

/**
  * @brief  
  */
static void task_logger_loop(void)
{
	//ֻ�������ϵ�״̬�²����У�����״̬�²�����
    if(system_status() == SYSTEM_RUN)
    {
	    task_status = TASK_INIT;
    }
}

/**
  * @brief  
  */
static void task_logger_exit(void)
{
	//...�����¼�״̬
	memset(&logger_status, 0, sizeof(logger_status));
	
	task_status = TASK_NOTINIT;
}

/**
  * @brief  
  */
static void task_logger_reset(void)
{
	//��ʼ�������б�
    memset(&monitors, 0, sizeof(monitors));
	monitors.check = crc32(&monitors, (sizeof(monitors) - sizeof(monitors.check)));
	
	//...��ʼ���¼�״̬
	memset(&logger_status, 0, sizeof(logger_status));
	logger_status.check = crc32(&logger_status, (sizeof(logger_status) - sizeof(logger_status.check)));
	//...д��
	
	task_status = TASK_NOTINIT;
}

/**
  * @brief  
  */
static enum __task_status task_logger_status(void)
{
    return(task_status);
}


/**
  * @brief  
  */
const struct __task_sched task_logger = 
{
    .name               = NAME_LOGGER,
    .init               = task_logger_init,
    .loop               = task_logger_loop,
    .exit               = task_logger_exit,
    .reset              = task_logger_reset,
    .status             = task_logger_status,
    .api                = (void *)&logger,
};
