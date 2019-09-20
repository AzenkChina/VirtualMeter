/**
 * @brief    x-modem frame 
 *            ---------------------------------------------------------------------------
 *            |     Byte1     |    Byte2    |     Byte3      |Byte4~Byte131|  Byte132   |
 *            |-------------------------------------------------------------------------|
 *            |Start Of Header|Packet Number|~(Packet Number)| Packet Data |  Check Sum |
 *            ---------------------------------------------------------------------------
 * @details     
 * @date        2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "config_protocol.h"
#include "proto_xmodem.h"
#include "info.h"
#include "stdbool.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  
  */
struct __xfr_info
{
    char                    file[32];
    uint32_t                counter;
    uint16_t                timeout;
};

/* Private define ------------------------------------------------------------*/
#define SOH		0x01 //Xmodem����ͷ
#define EOT		0x04 //���ͽ���
#define ACK		0x06 //�Ͽ���Ӧ
#define NAK		0x15 //���Ͽ���Ӧ
#define CAN		0x18 //��������
#define CTRLZ   0x1A //����ַ�

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __xfr_info *xfr_info = (struct __xfr_info *)0;
static uint8_t result = 0;
static uint8_t result_channel = 0xff;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static bool xfr_init(const char *name)
{
    //�ļ��������ж�
    if(strlen(name) > 32)
    {
        if(xfr_info)
        {
            heap.free(xfr_info);
            xfr_info = (struct __xfr_info *)0;
        }
        
        return(false);
    }
    
    //��ѯ�ļ��Ƿ����
    if(file.size(name) == 0)
    {
        if(xfr_info)
        {
            heap.free(xfr_info);
            xfr_info = (struct __xfr_info *)0;
        }
        
        return(false);
    }
    else
    {
        //��ʼ��������Ϣ
        if(!xfr_info)
        {
            xfr_info = heap.salloc(NAME_PROTOCOL, sizeof(struct __xfr_info));
            if(!xfr_info)
            {
                return(false);
            }
        }
        
        heap.set(xfr_info, 0, sizeof(struct __xfr_info));
        
        strcpy(xfr_info->file, name);
        
        return(true);
    }
}

/**
  * @brief  
  */
static uint8_t xfr_receive(const uint8_t *frame, uint16_t length)
{
    uint8_t cnt = 0;
    uint8_t sum = 0;
    uint8_t passwd[16];
    
    //����״̬��Ϣ
    if(!xfr_info)
    {
        //�����ж�
        if(length != 132)
        {
            return(0);
        }
        
        //֡ͷ��ʶ
        if(frame[0] != SOH)
        {
            return(0);
        }
        
        //����У��
        for(cnt=0; cnt<128; cnt++)
        {
            sum += frame[3+cnt];
        }
        
        if(sum != frame[131])
        {
            return(0);
        }
        
        //������
        if(frame[1] != ~frame[2])
        {
            return(0);
        }
        
        if(frame[1] != 0)
        {
            return(0);
        }
        
        //������֤
        get_management_passwd(sizeof(passwd), passwd);
        if(memcmp(passwd, &frame[3], 16) != 0)
        {
            return(0);
        }
        
        //�ļ�������֤
        if(xfr_init((const char *)&frame[19]) == false)
        {
            return(0);
        }
        
        return(NAK);
    }
    
    //�������
    if(frame[0] == EOT)
    {
        heap.free(xfr_info);
        xfr_info = (struct __xfr_info *)0;
        return(ACK);
    }
    
    //�����ж�
    if(length != 132)
    {
        return(0);
    }
    
    //֡ͷ��ʶ
    if(frame[0] != SOH)
    {
        return(0);
    }
    
    //����У��
    for(cnt=0; cnt<128; cnt++)
    {
        sum += frame[3+cnt];
    }
    
    if(sum != frame[131])
    {
        return(NAK);
    }
    
    //������
    if(frame[1] != ~frame[2])
    {
        return(NAK);
    }
    
    //������
    if(frame[1] == (xfr_info->counter & 0xff))
    {
        return(ACK);
    }
    else if(((xfr_info->counter & 0xff) + 1) != frame[1])
    {
        heap.free(xfr_info);
        xfr_info = (struct __xfr_info *)0;
        return(CAN);
    }
    
    //д�ļ�
    if(file.write(xfr_info->file, (xfr_info->counter * 128), 128, &frame[3]) == 0)
    {
        heap.free(xfr_info);
        xfr_info = (struct __xfr_info *)0;
        return(CAN);
    }
    
    xfr_info->counter += 1;
    xfr_info->timeout = 0;
    
    return(ACK);
}


/**
  * @brief  
  */
static void xmodem_init(void)
{
    if(xfr_info)
    {
        heap.free(xfr_info);
        xfr_info = (struct __xfr_info *)0;
    }
}

/**
  * @brief  
  */
static void xmodem_loop(void)
{
    if(system_status() == SYSTEM_RUN)
    {
        if(!xfr_info)
        {
            return;
        }
        
        //��ʱ����������
        if(xfr_info->timeout > (60*1000))
        {
            heap.free(xfr_info);
            xfr_info = (struct __xfr_info *)0;
        }
        else
        {
            xfr_info->timeout += KERNEL_PERIOD;
        }
    }
}

/**
  * @brief  
  */
static void xmodem_exit(void)
{
    if(xfr_info)
    {
        heap.free(xfr_info);
        xfr_info = (struct __xfr_info *)0;
    }
}

/**
  * @brief  
  */
static void xmodem_reset(void)
{
    if(xfr_info)
    {
        heap.free(xfr_info);
        xfr_info = (struct __xfr_info *)0;
    }
}

/**
  * @brief  
  */
static enum __task_status xmodem_status(void)
{
	return(TASK_INIT);
}


/**
  * @brief  
  */
static uint16_t xmodem_read(uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *id)
{
	return(0);
}

/**
  * @brief  
  */
static uint16_t xmodem_write(uint8_t *descriptor, uint8_t *buff, uint16_t size)
{
    return(0);
}

/**
  * @brief  
  */
static uint16_t xmodem_stream_in(uint8_t channel, const uint8_t *frame, uint16_t frame_length)
{
    result = xfr_receive(frame, frame_length);
    
    if(result)
    {
        result_channel = channel;
    }
    
    return(result);
}

/**
  * @brief  
  */
static uint16_t xmodem_stream_out(uint8_t channel, uint8_t *frame, uint16_t buff_length)
{
    if(result)
    {
        if(result_channel == channel)
        {
            frame[0] = result;
            result = 0;
            result_channel = 0xff;
            return(1);
        }
    }
    
	return(0);
}


/**
  * @brief  
  */
const struct __protocol_registrable proto_xmodem = 
{
    .protocol               = 
    {
        .pf                 = PF_XMODEM,
        
        .read               = xmodem_read,
        .write              = xmodem_write,
    
        .stream             = 
        {
            .in             = xmodem_stream_in,
            .out            = xmodem_stream_out,
        },
    },
    
    .sched                  = 
    {
        .name               = "XMODEM",
        .init               = xmodem_init,
        .loop               = xmodem_loop,
        .exit               = xmodem_exit,
        .reset              = xmodem_reset,
        .status             = xmodem_status,
        .api                = (void *)0,
    },
};
