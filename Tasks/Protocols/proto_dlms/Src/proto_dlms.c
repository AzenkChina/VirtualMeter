/**
 * @brief		
 * @details		
 * @date		
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "proto_dlms.h"
#include "dlms_types.h"
#include "cosem_objects.h"
#include "allocator.h"
#include "axdr.h"
#include "dlms_lexicon.h"
#include "dlms_utilities.h"
#include "hdlc_datalink.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void dlms_init(void)
{
	hdlc_init();
}

static void dlms_loop(void)
{
	hdlc_tick(KERNEL_PERIOD);
}

static void dlms_exit(void)
{
	hdlc_init();
}

static void dlms_reset(void)
{
    uint8_t buff[34];
    
	hdlc_init();
    
    hdlc_set_address(1);//本机地址
    hdlc_set_timeout(30);//链路超时时间
    
    heap.set(buff, 0x30, sizeof(buff));
    buff[1] = 16;
    
    buff[0] = 'P';
    dlms_util_write_passwd(buff);//密码
    
    buff[0] = 'A';
    dlms_util_write_akey(buff);//认证密钥
    
    buff[0] = 'B';
    dlms_util_write_bekey(buff);//加密密钥
    
    buff[0] = 'U';
    dlms_util_write_uekey(buff);//加密密钥
    
    buff[1] = 8;
    buff[0] = 'S';
    dlms_util_write_title(buff);//system title
}

static enum __task_status dlms_status(void)
{
	return(TASK_INIT);
}



static uint16_t dlms_read(uint8_t *descriptor, uint8_t *buff, uint16_t size, uint32_t *id)
{
    struct __cosem_request_desc desc;
    union __dlms_right right;
    TypeObject Func;
    uint8_t input[8];
    ObjectPara P;
    struct __cosem_descriptor cosem_descriptor;
    
    cosem_descriptor.classid = descriptor[0];
    cosem_descriptor.classid <<= 8;
    cosem_descriptor.classid += descriptor[1];
    heap.copy(cosem_descriptor.obis, &descriptor[2], 6);
    cosem_descriptor.index = descriptor[8];
    cosem_descriptor.selector = descriptor[9];
    cosem_descriptor.selector <<= 8;
    cosem_descriptor.selector += descriptor[10];
    
    heap.set(&P, 0, sizeof(P));
    P.Input.Buffer = input;
    P.Input.Size = sizeof(input);
    P.Output.Buffer = buff;
    P.Output.Size = size;
    
    desc.suit = 0xff;
    desc.level = DLMS_ACCESS_HIGH;
    desc.request = GET_REQUEST;
    desc.descriptor = cosem_descriptor;
	
    dlms_lex_parse(&desc, &right, &P.Input.OID, &P.Input.MID);
	Func = CosemLoadAttribute(cosem_descriptor.classid, cosem_descriptor.index, MOTIV_GET);
    
    if(Func)
    {
    	Func(&P);
	}
	
	*id = P.Input.MID;
	
	return(P.Output.Filled);
}

static uint16_t dlms_write(uint8_t *descriptor, uint8_t *buff, uint16_t size)
{
    struct __cosem_request_desc desc;
    union __dlms_right right;
    TypeObject Func;
    uint8_t output[8];
    ObjectPara P;
    struct __cosem_descriptor cosem_descriptor;
    
    cosem_descriptor.classid = descriptor[0];
    cosem_descriptor.classid <<= 8;
    cosem_descriptor.classid += descriptor[1];
    heap.copy(cosem_descriptor.obis, &descriptor[2], 6);
    cosem_descriptor.index = descriptor[8];
    cosem_descriptor.selector = descriptor[9];
    cosem_descriptor.selector <<= 8;
    cosem_descriptor.selector += descriptor[10];
    
    heap.set(&P, 0, sizeof(P));
    P.Input.Buffer = buff;
    P.Input.Size = size;
    P.Output.Buffer = output;
    P.Output.Size = sizeof(output);
    
    desc.suit = 0xff;
    desc.level = DLMS_ACCESS_HIGH;
    desc.request = SET_REQUEST;
    desc.descriptor = cosem_descriptor;
	
    dlms_lex_parse(&desc, &right, &P.Input.OID, &P.Input.MID);
	Func = CosemLoadAttribute(cosem_descriptor.classid, cosem_descriptor.index, MOTIV_SET);
    
    if(Func)
    {
    	if(Func(&P) == OBJECT_NOERR)
        {
            return(size);
        }
	}
    
    return(0);
}

static uint16_t dlms_stream_in(uint8_t channel, const uint8_t *frame, uint16_t frame_length)
{
    if(hdlc_matched(frame, frame_length))
    {
        return(hdlc_request(channel, frame, frame_length));
    }
    
	return(0);
}

static uint16_t dlms_stream_out(uint8_t channel, uint8_t *frame, uint16_t buff_length)
{
	return(hdlc_response(channel, frame, buff_length));
}


const struct __protocol_registrable proto_dlms = 
{
    .protocol               = 
    {
        .pf                 = PF_DLMS,
        
        .read               = dlms_read,
        .write              = dlms_write,
    
        .stream             = 
        {
            .in             = dlms_stream_in,
            .out            = dlms_stream_out,
        },
    },
    
    .sched                  = 
    {
        .name               = "DLMS",
        .init               = dlms_init,
        .loop               = dlms_loop,
        .exit               = dlms_exit,
        .reset              = dlms_reset,
        .status             = dlms_status,
        .api                = (void *)0,
    },
};
