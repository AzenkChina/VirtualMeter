/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_comm.h"
#include "types_comm.h"
#include "config_comm.h"

#include "types_protocol.h"

/** ���������ͨ�Ŷ˿�ͷ�ļ� */
#include "rs485_1.h"
#include "rs485_2.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  �˿ڼ�¼
  */
struct __port_entry
{
	enum __port_type					port_type;
	const struct __serial				*serial;
};


/* Private define ------------------------------------------------------------*/
#define BUFF_SIZE			((uint16_t)(256))

/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;

/**
  * @brief  �����еĶ˿��Լ��˿����ͼ�¼���ṹ���� 
  */
static const struct __port_entry port_table[] = 
{
	/** �˿����� �˿ڽṹ��ָ�� */
	{PORT_485, &rs485_1},
	{PORT_485, &rs485_2},
};

/* Private macro -------------------------------------------------------------*/
#define PORT_AMOUNT			((uint8_t)(sizeof(port_table) / sizeof(struct __port_entry)))

/* Private variables ---------------------------------------------------------*/
static uint8_t *buff = (uint8_t *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ��ȡ�˿����� 
  */
static enum __port_type serial_porttype(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __port_type)0xff);
    }
    
    return(port_table[channel].port_type);
}

/**
  * @brief  ��ȡ�˿�״̬ 
  */
static enum __port_status serial_portstatus(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __port_status)0xff);
    }
    
    if(port_table[channel].serial->status() != BUS_IDLE)
    {
    	return(PORT_BUSY);
    }
    
    return(PORT_IDLE);
}

/**
  * @brief  ����֡��ʱʱ�� 
  */
static uint16_t serial_timeout_set(uint8_t channel, uint16_t msecond)
{
    if(channel >= PORT_AMOUNT)
    {
        return(0);
    }
    
    return(port_table[channel].serial->timeout.set(msecond));
}

/**
  * @brief  ��ȡ֡��ʱʱ�� 
  */
static uint16_t serial_timeout_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return(0);
    }
    
    return(port_table[channel].serial->timeout.get());
}

/**
  * @brief  ���ò����� 
  */
static enum __baud serial_baudrate_set(uint8_t channel, enum __baud baudrate)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __baud)0xff);
    }
    
    return(port_table[channel].serial->uart->baudrate.set(baudrate));
}

/**
  * @brief  ��ȡ������ 
  */
static enum __baud serial_baudrate_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __baud)0xffff);
    }
    
    return(port_table[channel].serial->uart->baudrate.get());
}

/**
  * @brief  ����У��λ 
  */
static enum __parity serial_parity_set(uint8_t channel, enum __parity parity)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __parity)0xff);
    }
    
    return(port_table[channel].serial->uart->parity.set(parity));
}

/**
  * @brief  ��ȡУ��λ 
  */
static enum __parity serial_parity_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __parity)0xff);
    }
    
    return(port_table[channel].serial->uart->parity.get());
}

/**
  * @brief  ����ֹͣλ 
  */
static enum __stop serial_stop_set(uint8_t channel, enum __stop stop)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __stop)0xff);
    }
    
    return(port_table[channel].serial->uart->stop.set(stop));
}

/**
  * @brief  ��ȡֹͣλ 
  */
static enum __stop serial_stop_get(uint8_t channel)
{
    if(channel >= PORT_AMOUNT)
    {
        return((enum __stop)0xff);
    }
    
    return(port_table[channel].serial->uart->stop.get());
}

/**
  * @brief  
  */
static const struct __comm comm = 
{
    .attrib				= 
    {
		.type			= serial_porttype,
		.status			= serial_portstatus,
    },
    
    .timeout            = 
    {
        .set            = serial_timeout_set,
        .get            = serial_timeout_get,
    },
    
    .baudrate           =
    {
        .set            = serial_baudrate_set,
        .get            = serial_baudrate_get,
    },
    
    .parity             =
    {
        .set            = serial_parity_set,
        .get            = serial_parity_get,
    },
    
    .stop               =
    {
        .set            = serial_stop_set,
        .get            = serial_stop_get,
    },
};








/**
  * @brief  task ��ʼ�� 
  */
static void comm_init(void)
{
	uint8_t cnt;
	
    buff = (uint8_t *)0;
    
	//ֻ�������ϵ�״̬�²����У�����״̬�²�����
    if(system_status() == SYSTEM_RUN)
    {
        buff = heap.salloc(NAME_COMM, (PORT_AMOUNT*2 + 1)*BUFF_SIZE);
        
        if(!buff)
        {
            status = TASK_ERROR;
            return;
        }
        
    	//��ʼ�����д�������
    	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
    	{
    		port_table[cnt].serial->control.init(DEVICE_NORMAL);
    		port_table[cnt].serial->timeout.set(50);
    		port_table[cnt].serial->rxbuff.set((buff+BUFF_SIZE*(1+cnt*2+0)), BUFF_SIZE);
    		port_table[cnt].serial->txbuff.set((buff+BUFF_SIZE*(1+cnt*2+1)), BUFF_SIZE);
    	}
	    
	    TRACE(TRACE_INFO, "Task comm initialized.");
	    
	    status = TASK_INIT;
    }
}

/**
  * @brief  task ��ѯ
  */
static void comm_loop(void)
{
	uint8_t cnt_port;
	uint16_t length;
	
	uint8_t *pbuff;
	uint16_t pbuff_length;
    
    struct __protocol *api_stream;
	
	//ֻ�������ϵ�״̬�²����У�����״̬�²�����
    if(system_status() != SYSTEM_RUN)
    {
    	return;
    }
    
    //�������ṩʱ��
	for(cnt_port=0; cnt_port<PORT_AMOUNT; cnt_port++)
	{
		port_table[cnt_port].serial->runner(KERNEL_PERIOD);
	}
    
    //�����߶�ȡ����֡
	for(cnt_port=0; cnt_port<PORT_AMOUNT; cnt_port++)
	{
		//�����߶�ȡ
		length = port_table[cnt_port].serial->read(buff, BUFF_SIZE);
        
	    if(length)
	    {
            api_stream = (struct __protocol *)api("task_protocol");
            
            if(api_stream)
            {
                api_stream->stream.in(cnt_port, buff, length);
            }
		}
	}
    
    //������д������֡
	for(cnt_port=0; cnt_port<PORT_AMOUNT; cnt_port++)
	{
		//����������æ�������� 
        if(port_table[cnt_port].serial->status() != BUS_IDLE)
        {
            continue;
        }
        
        //��ȡ������ߵķ��ͻ���ͻ��峤�� 
        pbuff_length = port_table[cnt_port].serial->txbuff.get(&pbuff);
        
        if((!pbuff_length) || (!pbuff))
        {
            continue;
        }
        
        //��ѯע���Э��ջ������Ƿ�������Ҫ�Ӵ˶˿ڷ���
        api_stream = (struct __protocol *)api("task_protocol");
        
        if(!api_stream)
        {
            continue;
        }
        
        length = api_stream->stream.out(cnt_port, pbuff, pbuff_length);
        
        if(length)
        {
            port_table[cnt_port].serial->write(length);
        }
	}
	    
    status = TASK_RUN;
}

/**
  * @brief  task �˳� 
  */
static void comm_exit(void)
{
	uint8_t cnt;
	
	//�������д�������
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		port_table[cnt].serial->control.suspend();
	}
    
    TRACE(TRACE_INFO, "Task comm exited.");
    status = TASK_SUSPEND;
}

/**
  * @brief  task ��λ 
  */
static void comm_reset(void)
{
	uint8_t cnt;
	
	//�������д�������
	for(cnt=0; cnt<PORT_AMOUNT; cnt++)
	{
		port_table[cnt].serial->control.suspend();
	}
    
    TRACE(TRACE_INFO, "Task comm reset.");
    status = TASK_NOTINIT;
}

/**
  * @brief  task ״̬
  */
static enum __task_status comm_status(void)
{
    return(status);
}


/**
  * @brief  
  */
const struct __task_sched task_comm = 
{
    .name               = NAME_COMM,
    .init               = comm_init,
    .loop               = comm_loop,
    .exit               = comm_exit,
    .reset              = comm_reset,
    .status             = comm_status,
    .api                = (void *)&comm,
};
