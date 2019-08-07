/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_disconnect.h"
#include "types_disconnect.h"
#include "config_disconnect.h"

#include "jiffy.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  
  */
struct __relay_runs
{
    enum __relay_status     status;
    uint32_t                requests;
	uint32_t                requests;
	uint32_t				priority[32];//32�����ȼ�����Ӧ16������Դ
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __relay_runs relay_runs;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static uint8_t relay_request(enum __relay_requests source, enum __relay_action action)
{
    if(source > 15)
    {
        return(0);
    }
    
    relay_runs.requests &= ~(0x03 << (source * 2));
    action &= 0x03;
    relay_runs.requests |= (action << (source * 2));
    
    return((uint8_t)action);
}

/**
  * @brief  
  */
static enum __relay_status relay_status(void)
{
	
}

/**
  * @brief  
  */
static enum __relay_requests relay_reason(void)
{
	
}

/**
  * @brief  
  */
static uint8_t relay_priority_set(enum __relay_requests source, enum __relay_action action, uint8_t priority)
{
	
}

/**
  * @brief  
  */
static uint8_t relay_priority_get(enum __relay_requests source, enum __relay_action action)
{
	
}


/**
  * @brief  
  */
static const struct __relay relay = 
{
    .request            = relay_request,
	.status				= relay_status,
	.reason				= relay_reason,
	.priority			= 
	{
		.set			= relay_priority_set,
		.get			= relay_priority_get,
	},
};





/**
  * @brief  ����ȫ�����������������ȼ��ٲó���ǰ����
  */
static enum __relay_action req_arbitrate(void)
{
    return(RELAY_ACT_RELEASE);
}

/**
  * @brief  ���ݼ̵�����ǰ״̬�͵�ǰ����Ķ���ȷ����ǰ����
  */
static enum __relay_policy req_policy(enum __relay_status status, enum __relay_action * action)
{
    return(RELAY_REQ_ACCEPT);
}

/**
  * @brief  ���¼̵�����ǰ״̬
  */
static void req_update(enum __relay_action action)
{
    return;
}






/**
  * @brief  
  */
static void disc_init(void)
{
    
}

/**
  * @brief  
  */
static void disc_loop(void)
{
    static uint32_t timing = 0;
    
    enum __relay_action action;
    enum __relay_policy policy;
    
    if(jiffy.after(timing) < 1000)
    {
        return;
    }
    
    timing = jiffy.value();
    
    
    //��ȡ��ǰ�̵�������
    action = req_arbitrate();
    
    //��ȡ��ǰ�̵�������
    policy = req_policy(relay_runs.status, &action);
    
    if(policy != RELAY_REQ_DROP)
    {
        //���µ�ǰ�̵���״̬
        req_update(action);
    }
    
}

/**
  * @brief  
  */
static void disc_exit(void)
{
    
}

/**
  * @brief  
  */
static void disc_reset(void)
{
    
}

/**
  * @brief  
  */
static enum __task_status disc_status(void)
{
    return(TASK_INIT);
}


/**
  * @brief  
  */
const struct __task_sched task_disconnect = 
{
    .name               = NAME_DISCONNECT,
    .init               = disc_init,
    .loop               = disc_loop,
    .exit               = disc_exit,
    .reset              = disc_reset,
    .status             = disc_status,
    .api                = (void *)&relay,
};
