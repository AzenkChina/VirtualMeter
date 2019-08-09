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
#include "relay.h"

#include "jiffy.h"

/* Private define ------------------------------------------------------------*/
#define RELAY_CONF_CONTENT              ((uint8_t)(8)) //������������

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  �洢�ṹ
  */
struct __relay_param
{
    enum __relay_action     action;//��ǰ��Ҫ�ļ̵����Ķ���
    uint32_t                requests;//��Ӧ16����������ÿ����λһ��
	uint32_t                before;//��һ�ε�����״̬
    uint32_t                mask;//32��λ������16������Դ������32�ִ�������
    
    uint32_t				priority[RELAY_CONF_CONTENT][32];//���ȼ�ѡ���
    uint8_t                 selection;////��ǰ��Ч�����ȼ�
};

/**
  * @brief  ���в���
  */
struct __relay_runs
{
    enum __relay_status     status;//��ǰ�ļ̵���״̬
    enum __relay_action     action;//��ǰ��Ҫ�ļ̵����Ķ���
    uint32_t                requests;//��Ӧ16����������ÿ����λһ��
	uint32_t                before;//��һ�ε�����״̬
    uint32_t                mask;//32��λ������16������Դ������32�ִ�������
    uint32_t				priority[32];//32�����ȼ�����Ӧ16������Դ
    enum __relay_requests   reason;//��ǰ������ԭ��
    uint8_t                 flush;//״̬��ˢ��
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __task_status task_status = TASK_NOTINIT;
static struct __relay_runs relay_runs;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ����ȫ����������������ȼ������������ٲó���ǰ����
  */
static void request_arbitrate(const struct __relay_runs *runs, 
                                             enum __relay_requests *reason, 
                                             enum __relay_action *action)
{
    uint16_t loop;
    enum __relay_action request = RELAY_ACT_RELEASE;
    uint8_t priority = 0;
    
    if(!runs || !reason || !action)
    {
        return;
    }
    
    *reason = (enum __relay_requests)0xff;
    *action = RELAY_ACT_RELEASE;
    
	for(loop=0; loop<16; loop++)
	{
        request = (enum __relay_action)((runs->requests >> (loop * 2)) & 0x03);
        
        if((request == RELAY_ACT_OPEN) && !((runs->mask >> (loop * 2)) & 0x01))
        {
            if(runs->priority[loop * 2 + 0] > priority)
            {
                *action = request;
                *reason = (enum __relay_requests)loop;
            }
        }
        else if((request == RELAY_ACT_CLOSE) && !((runs->mask >> (loop * 2)) & 0x02))
        {
            if(runs->priority[loop * 2 + 1] > priority)
            {
                *action = request;
                *reason = (enum __relay_requests)loop;
            }
        }
	}
}

/**
  * @brief  ���¼̵�����ǰ״̬
  */
static void request_action(enum __relay_action action)
{
    if(action == RELAY_ACT_OPEN)
    {
        //��բ
        relay.set(SWITCH_OPEN);
    }
    else
    {
        //��բ
        relay.set(SWITCH_CLOSE);
    }
}


/**
  * @brief  
  */
static enum __relay_action relay_request(enum __relay_requests source, enum __relay_action action)
{
    if(source > 15)
    {
        return(RELAY_ACT_RELEASE);
    }
    
    relay_runs.requests &= ~(0x03 << (source * 2));
    action &= 0x03;
    relay_runs.requests |= (action << (source * 2));
    
    relay_runs.flush = 0xff;
    
    return(action);
}

/**
  * @brief  
  */
static enum __relay_status relay_status(void)
{
	return(relay_runs.status);
}

/**
  * @brief  
  */
static enum __relay_requests relay_reason(void)
{
	return(relay_runs.reason);
}

/**
  * @brief  
  */
static uint8_t relay_priority_write(uint8_t selection, const uint8_t *priority)
{
    struct __relay_param param;
    
    if(!priority)
    {
        return(0);
    }
    
    if(selection >= RELAY_CONF_CONTENT)
    {
        return(0);
    }
    
    heap.set(&param, 0, sizeof(param));
    
    file.read("disconnect", 0, sizeof(param), &param);
        
    if(param.selection >= RELAY_CONF_CONTENT)
    {
        param.selection = 0;
    }
    
    heap.copy(param.priority[param.selection], priority, sizeof(param.priority[param.selection]));
    
    file.write("disconnect", 0, sizeof(param), &param);
    
    if(param.selection == selection)
    {
        heap.copy(relay_runs.priority, priority, sizeof(relay_runs.priority));
        relay_runs.flush = 0xff;
    }
    
	return(sizeof(param.priority[param.selection]));
}

/**
  * @brief  
  */
static uint8_t relay_priority_read(uint8_t selection, uint8_t *priority)
{
    struct __relay_param param;
    
    if(!priority)
    {
        return(0);
    }
    
    if(selection >= RELAY_CONF_CONTENT)
    {
        return(0);
    }
    
    heap.set(&param, 0, sizeof(param));
    
    file.read("disconnect", 0, sizeof(param), &param);
    
    return(heap.copy(priority, param.priority[selection], sizeof(param.priority[selection])));
}

/**
  * @brief  
  */
static uint8_t relay_selection_set(uint8_t selection)
{
    struct __relay_param param;
    
    if(selection >= RELAY_CONF_CONTENT)
    {
        return(0);
    }
    
    heap.set(&param, 0, sizeof(param));
    
    file.read("disconnect", 0, sizeof(param), &param);
    
    param.selection = selection;
    
    file.write("disconnect", 0, sizeof(param), &param);
    
    heap.copy(relay_runs.priority, param.priority[selection], sizeof(relay_runs.priority));
    relay_runs.flush = 0xff;
    
	return(selection);
}

/**
  * @brief  
  */
static uint8_t relay_selection_get(void)
{
    struct __relay_param param;
    
    heap.set(&param, 0, sizeof(param));
    
    file.read("disconnect", 0, sizeof(param), &param);
    
	return(param.selection);
}

/**
  * @brief  
  */
static uint32_t relay_mask_set(uint32_t mask)
{
    struct __relay_param param;
    
    heap.set(&param, 0, sizeof(param));
    
    file.read("disconnect", 0, sizeof(param), &param);
    
    param.mask = mask;
    relay_runs.mask = mask;
    
    file.write("disconnect", 0, sizeof(param), &param);
    
    relay_runs.flush = 0xff;
    
	return(mask);
}

/**
  * @brief  
  */
static uint32_t relay_mask_get(void)
{
    return(relay_runs.mask);
}


/**
  * @brief  
  */
static const struct __disconnect disconnect = 
{
    .request            = relay_request,
	.status				= relay_status,
	.reason				= relay_reason,
	.priority			= 
	{
		.write			= relay_priority_write,
		.read			= relay_priority_read,
	},
	.selection          = 
	{
        .set            = relay_selection_set,
        .get            = relay_selection_get,
	},
	.mask               = 
	{
        .set            = relay_mask_set,
        .get            = relay_mask_get,
	},
};






/**
  * @brief  
  */
static void disc_init(void)
{
    struct __relay_param param;
    enum __relay_action action;
    enum __relay_requests reason;
    
    if(system_status() == SYSTEM_RUN)
    {
        relay.control.init(DEVICE_NORMAL);
        
        heap.set(&param, 0, sizeof(param));
        
        file.read("disconnect", 0, sizeof(param), &param);
        
        if(param.selection >= RELAY_CONF_CONTENT)
        {
            param.selection = 0;
        }
        
        relay_runs.status = RELAY_STA_CLOSE;
        relay_runs.action = param.action;
        relay_runs.requests = param.requests;
        relay_runs.before = 0;
        relay_runs.mask = param.mask;
        relay_runs.reason = (enum __relay_requests)0xff;
        relay_runs.flush = 0;
        heap.copy(relay_runs.priority, param.priority[param.selection], sizeof(relay_runs.priority));
        
        request_arbitrate(&relay_runs, &reason, &action);
        relay_runs.reason = reason;
        relay_runs.action = action;
        request_action(action);
        
        task_status = TASK_INIT;
    }
}

/**
  * @brief  
  */
static void disc_loop(void)
{
    static uint32_t timing = 0;
    enum __relay_action action;
    enum __relay_requests reason;
    
	//ֻ�������ϵ�״̬�²����У�����״̬�²�����
    if(system_status() == SYSTEM_RUN)
    {
        task_status = TASK_RUN;
        
        relay.runner(KERNEL_PERIOD);
        
        //ˢ�¼̵���״̬
        if(relay_runs.flush)
        {
            relay_runs.flush = 0;
            
            request_arbitrate(&relay_runs, &reason, &action);
            
            if(action != relay_runs.action)
            {
                relay_runs.reason = reason;
                relay_runs.action = action;
                request_action(action);
            }
        }
        
        if(jiffy.after(timing) < 1000)
        {
            return;
        }
        
        timing = jiffy.value();
        
        if(relay.get() == SWITCH_OPEN)
        {
            relay_runs.status = RELAY_STA_OPEN;
        }
        else if(relay.get() == SWITCH_CLOSE)
        {
            relay_runs.status = RELAY_STA_CLOSE;
        }
        else
        {
            relay_runs.status = (enum __relay_status)0xff;
        }
    }
}

/**
  * @brief  
  */
static void disc_exit(void)
{
    relay.control.suspend();
    task_status = TASK_NOTINIT;
}

/**
  * @brief  
  */
static void disc_reset(void)
{
    relay.control.suspend();
    task_status = TASK_NOTINIT;
    
    {
//...Just for test
//��ʼ��EEPROM����
        struct __relay_param param;
        uint8_t content, cnt;
        
        heap.set(&param, 0, sizeof(param));
        
        param.action = RELAY_ACT_CLOSE;
        param.requests = 0;
        param.before = 0;
        param.mask = 0;
        param.selection = 0;
        
        for(content=0; content<RELAY_CONF_CONTENT; content++)
        {
            for(cnt=0; cnt<16; cnt++)
            {
                param.priority[content][cnt] = cnt;
            }
        }
        
        file.write("disconnect", 0, sizeof(param), &param);
    }
}

/**
  * @brief  
  */
static enum __task_status disc_status(void)
{
    return(task_status);
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
    .api                = (void *)&disconnect,
};
