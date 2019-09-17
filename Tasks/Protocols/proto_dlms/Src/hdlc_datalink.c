/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdbool.h"

#include "system.h"
#include "config_protocol.h"
#include "hdlc_datalink.h"
#include "dlms_association.h"

/* Private define ------------------------------------------------------------*/
//HDLC配置参数

//服务端地址长度，取值：1, 2, 4
#define HDLC_CONFIG_ADDR_LENGTH         2

//有效荷载长度
#define HDLC_CONFIG_INFO_LEN_MIN        ((uint16_t)(64)) // >=32
#define HDLC_CONFIG_INFO_LEN_DEFAULT    ((uint16_t)(128))
#define HDLC_CONFIG_INFO_LEN_MAX        ((uint16_t)(220)) // >=128

//支持的通道数
#define HDLC_CONFIG_MAX_CHANNEL         ((uint8_t)(4))

//定义外部接口
//应用层请求（session, info, length, buffer, max buffer length, filled buffer length）
#define HDLC_CONFIG_APPL_REQUEST(s,i,l,b,m,f)   dlms_asso_gateway(s,i,l,b,m,f)
//应用层断开
#define HDLC_CONFIG_APPL_RELEASE(s)             dlms_asso_cleanup(s)
//应用层最大报文长度
#define HDLC_CONFIG_APPL_MTU()                  dlms_asso_mtu()

/* Private typedef -----------------------------------------------------------*/
struct __hdlc_configs
{
    uint16_t local_addr;//本地地址
    uint16_t inact_time;//链路超时时间
};

enum __hdlc_link_status
{
    LINK_DISCONNECTED = 0,
    LINK_CONNECTED,
    LINK_MARK,
};

enum __hdlc_errors
{
    HDLC_NO_ERR = 0,
    HDLC_ERR_FRAME_NODEF,
    HDLC_ERR_ADDRESS,
    HDLC_ERR_HCS,
    HDLC_ERR_FCS,
    HDLC_ERR_LENGTH,
    HDLC_ERR_LLC,
    HDLC_ERR_NOMEM,
    HDLC_ERR_NODEF,
};

enum __hdlc_frmr_state
{
    FRMR_OPCODE = 0,    //0--控制字不支持
    FRMR_HAVEINFO,      //1--这个控制字不应有Information，但有了
    FRMR_INFOLENGTH,    //2--接收Information长度超出约定的最大长度
    FRMR_NR,            //3--接收的N(R)无效
};

struct __hdlc_frame_desc
{
    uint8_t format; //帧类型
    uint8_t segment; //分片标记
    uint16_t length_total; //帧长度
    uint8_t * dst; //目的地址
    uint8_t length_dst; //目的地址长度
    uint8_t * src; //源地址
    uint8_t length_src; //原地址长度
    uint8_t * ctrl; //控制域
    uint8_t rrr; //接收帧计数，在控制域中
    uint8_t sss; //发送帧计数，在控制域中
    uint8_t poll; //Server应答允许标识，在控制域中
    uint8_t * hck; //帧头校验
    uint8_t * llc; //LLC层
    uint8_t * info; //数据域
    uint16_t length_info; //数据域长度
    uint8_t * fck; //帧校验
};

struct __hdlc_info_recv
{
    uint16_t length; //动态分配的内存大小
    uint16_t filled; //内存写指针
    uint8_t *data; //将动态分配的内存首地址记录到这里
};

struct __hdlc_info_send
{
    uint16_t length; //动态分配的内存大小
    uint16_t filled; //内存写指针
    uint8_t *data; //将动态分配的内存首地址记录到这里
    uint16_t sent; //内存读指针
    
    /** 链路层将待发送的数据进行分片，分片后暂存在segment结构体中 */
    struct
    {
        uint16_t  length; //内存中数据发送长度
        uint16_t  confirming; //内存中数据发送长度备份，真正发送完成后清零否则用来实现重发
        uint8_t data[HDLC_CONFIG_INFO_LEN_MAX + 32];
    } segment;
};

struct __hdlc_link
{
    uint8_t sss; //发送帧计数
    uint8_t rrr; //接收帧计数
    uint8_t rrr_confirming; //接收帧计数备份
    uint8_t csss; //客户端发送帧计数
    
    uint8_t crrr; //客户端接收帧计数
    uint8_t ws_trans; //发送窗口大小
    uint8_t ws_recv; //接收窗口大小
    
    uint16_t max_len_trans; //最大发送帧长度
    uint16_t max_len_recv; //最大接收帧长度
    
    uint32_t link_inactive_timer; //链路层超时断开时间计数器（毫秒）
    enum __hdlc_link_status link_status; //连接状态
    uint8_t client_address; //客户端地址
    uint16_t device_address; //设备地址
    uint16_t logic_address; //逻辑地址
    struct __hdlc_info_send send; //information帧发送数据结构
    struct __hdlc_info_recv recv; //information帧接收数据结构（多包需要在链路层组包）
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//CRC16表
static const uint16_t fcstab[256] =
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

//通道连接信息
static struct __hdlc_link hdlc_links[HDLC_CONFIG_MAX_CHANNEL];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief 计算字节数据流的 CRC16
  * @param  cp 数据流地址
  * @param  length 数据流长度
  * @retval CRC结果
  */
static uint16_t hdlc_check(const uint8_t *cp, uint16_t length)
{
    uint16_t fcs = 0xffff;
    
    while(length--)
    {
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    }

    fcs ^= 0xffff;
    
    return fcs;
}

/**
  * @brief 添加CRC校验
  * @param  
  * @param  
  * @param  
  * @retval 输出长度
  */
static uint8_t add_check(uint8_t *in, uint16_t length, uint8_t *out)
{
    uint16_t crc;
    
    if(!in || !length || !out)
    {
        return(0);
    }
    
    crc = hdlc_check(in, length);
    *(out + 0) = (uint8_t)(crc & 0x00ff);
    *(out + 1) = (uint8_t)(crc >> 8);
    
    return(2);
}

/**
  * @brief 组包客户端地址域
  * @param  
  * @param  
  * @param  
  * @retval 
  */
static uint16_t makeup_client_address(const uint8_t *s)
{
    if(!s)
    {
        return(0);
    }
    
    return(s[0] >> 1);
}

/**
  * @brief 组包物理地址域
  * @param  
  * @param  
  * @param  
  * @retval 
  */
static uint16_t makeup_device_address(const uint8_t *s)
{
    uint16_t address = 0;
    uint8_t length = 0;
    uint8_t cnt = 0;
    
    if(!s)
    {
        return(0);
    }
    
    for(cnt=0; cnt<4; cnt++)
    {
        if(s[cnt] & 0x01)
        {
            length = cnt + 1;
            break;
        }
    }
    
    if(length == 1)
    {
        return(0xffff);
    }
    else if(length == 2)
    {
        address = s[1] >> 1;
        return(address);
    }
    else if(length == 4)
    {
        address = s[2] >> 1;
        address <<= 7;
        address += s[3] >> 1;
        return(address);
    }
    else
    {
        return(0);
    }
}

/**
  * @brief 组包逻辑地址域
  * @param  
  * @param  
  * @param  
  * @retval 
  */
static uint16_t makeup_logic_address(const uint8_t *s)
{
    uint16_t address = 0;
    uint8_t length = 0;
    uint8_t cnt = 0;
    
    if(!s)
    {
        return(0);
    }
    
    for(cnt=0; cnt<4; cnt++)
    {
        if(s[cnt] & 0x01)
        {
            length = cnt + 1;
            break;
        }
    }
    
    if(length == 1)
    {
        address = s[0] >> 1;
        return(address);
    }
    else if(length == 2)
    {
        address = s[0] >> 1;
        return(address);
    }
    else if(length == 4)
    {
        address = s[0] >> 1;
        address <<= 7;
        address += s[1] >> 1;
        return(address);
    }
    else
    {
        return(0);
    }
}

/**
  * @brief 索引HDLC报文
  * @param  FrameReceived 数据流地址
  * @param  FrameLength 数据流长度
  * @param  HdlcDesc 解析后的HDLC报文数据结构
  * @retval 错误状态
  */
static enum __hdlc_errors decode_hdlc_frame(const uint8_t *frame, \
                                            uint16_t length, \
                                            struct __hdlc_frame_desc *hdlc_desc)
{
    uint8_t cnt;
    uint16_t frame_read = 0;
    //uint16_t crc = 0;
    
    if(!hdlc_desc)
    {
        return(HDLC_ERR_NOMEM);
    }
    
    heap.set((void*)hdlc_desc, 0, sizeof(struct __hdlc_frame_desc));
    
    frame_read += 1;
    
    //帧类型
    hdlc_desc->format = *(frame + frame_read) & 0xf0;
    
    //链路层分片标识
    hdlc_desc->segment = *(frame + frame_read) & 0x08;
    
    //帧长度（除HDLC帧头帧尾的flag之外的数据的长度）
    hdlc_desc->length_total = *(frame + frame_read) & 0x07;
    hdlc_desc->length_total <<= 8;
    frame_read += 1;
    hdlc_desc->length_total += *(frame + frame_read);
    frame_read += 1;
    
    //目的地址
    hdlc_desc->dst = (uint8_t *)(frame + frame_read);
    for(cnt=0; cnt<4; cnt++)
    {
        if(*(frame + frame_read) & 0x01)
        {
            hdlc_desc->length_dst = cnt + 1;
            break;
        }
        
        frame_read += 1;
    }
    
    //地址长度不一致时需要处理
    if((cnt>=4) || (hdlc_desc->length_dst == 3))
    {
        return(HDLC_ERR_ADDRESS);
    }
    
    frame_read += 1;
    
    //源地址
    hdlc_desc->src = (uint8_t *)(frame + frame_read);
    for(cnt=0; cnt<4; cnt++)
    {
        if(*(frame + frame_read) & 0x01)
        {
            hdlc_desc->length_src = cnt + 1;
            break;
        }
        
        frame_read += 1;
    }
    
    if((cnt>=4) || (hdlc_desc->length_src != 1))
    {
        return(HDLC_ERR_ADDRESS);
    }
    
    frame_read += 1;
    
    //控制域
    hdlc_desc->ctrl = (uint8_t *)(frame + frame_read);
    hdlc_desc->poll = (*(uint8_t *)(frame + frame_read) & 0x10) >> 4;
    hdlc_desc->rrr = (*(uint8_t *)(frame + frame_read) & 0xE0) >> 5;
    hdlc_desc->sss = (*(uint8_t *)(frame + frame_read) & 0x0E) >> 1;
    frame_read += 1;
    
    //HDLC帧头校验
    hdlc_desc->hck = (uint8_t *)(frame + frame_read);
    frame_read += 2;
    
    if((*(hdlc_desc->hck) + *(hdlc_desc->hck + 1) * 256) != hdlc_check((frame + 1), (frame_read - 3)))
    {
        return(HDLC_ERR_HCS);
    }
    
    if((*(frame + frame_read) != 0xE6) || (*(frame + frame_read + 1) != 0xE6) || (*(frame + frame_read + 2) != 0x00))
    {
        hdlc_desc->length_info = length - 3 - frame_read;
    }
    else
    {
        //应用层数据长度
        if(length < frame_read + 3)
        {
            return(HDLC_NO_ERR);
        }
        
        frame_read += 3;
        hdlc_desc->length_info = length - 3 - frame_read;
    }
    
    //通过应用层数据长度判断下面的数据是否存在
    if(hdlc_desc->length_info)
    {
        hdlc_desc->info = (uint8_t *)(frame + frame_read);
        hdlc_desc->fck = (uint8_t *)(frame + length - 3);
        
        if((*(hdlc_desc->fck) + (*(hdlc_desc->fck + 1) * 256)) != hdlc_check((frame + 1), (length - 4)))
        {
            hdlc_desc->length_info = 0;
            return(HDLC_ERR_FCS);
        }
    }
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 检查报文目的地址是否为广播地址
  * @param  
  * @retval 
  */
static bool broadcast_matched(const struct __hdlc_frame_desc *hdlc_desc)
{
    if(hdlc_desc->length_dst == 2)
    {
        if(hdlc_desc->dst && (hdlc_desc->dst[1] == 0xFF))
        {
            return(true);
        }
    }
    else if(hdlc_desc->length_dst == 4)
    {
        if(hdlc_desc->dst && \
           ((hdlc_desc->dst[3] == 0xFF && hdlc_desc->dst[2] == 0xFE)))
        {
            return(true);
        }
    }
    
    return(false);
}

/**
  * @brief 检查报文目的地址是否为本机地址
  * @param  
  * @retval 
  */
static bool address_matched(uint8_t channel, const struct __hdlc_frame_desc *hdlc_desc)
{
    if(channel > HDLC_CONFIG_MAX_CHANNEL)
    {
        return(false);
    }
    
    if(hdlc_links[channel].link_status == LINK_CONNECTED)
    {
        //匹配连接对象存储的地址和报文中的地址是否一致
        if(hdlc_links[channel].device_address != makeup_device_address(hdlc_desc->dst))
        {
            return(false);
        }
    }
    else
    {
        //先获取本机地址，然后和报文中的地址是否一致
        if(hdlc_get_address() != makeup_device_address(hdlc_desc->dst))
        {
            return(false);
        }
    }
	
    return(true);
}

/**
  * @brief 填充客户端地址
  * @param  
  * @retval 
  */
static uint8_t fill_client_address(uint16_t address, uint8_t *out)
{
    if(!out)
    {
        return(0);
    }
    
    out[0] = address;
    out[0] <<= 1;
    out[0] |= 0x01;
    
    return(1);
}

/**
  * @brief 填充服务端地址
  * @param  
  * @retval 
  */
static uint8_t fill_server_address(uint16_t device, uint16_t logic, uint8_t length, uint8_t *out)
{
    if(length == 1)
    {
        out[0] = (logic << 1) & 0xfe;
        out[0] |= 0x01;
        return(1);
    }
    else if(length == 2)
    {
        out[0] = (logic << 1) & 0xfe;
        out[1] = (device << 1) & 0xfe;
        out[1] |= 0x01;
        return(2);
    }
    else if(length == 4)
    {
        out[0] = (logic >> 6) & 0xfe;
        out[1] = (logic << 1) & 0xfe;
        out[2] = (device >> 6) & 0xfe;
        out[3] = (device << 1) & 0xfe;
        out[3] |= 0x01;
        return(4);
    }
    else
    {
        return(0);
    }
}

/**
  * @brief 初始化连接对象
  * @param  
  * @retval 
  */
static enum __hdlc_errors link_setup(struct __hdlc_link *link)
{
    if(!link)
    {
        return(HDLC_ERR_NOMEM);
    }
    
    if(link->recv.data)
    {
        heap.free(link->recv.data);
    }
    
    if(link->send.data)
    {
        heap.free(link->send.data);
    }
    
    heap.set((void*)link, 0, sizeof(struct __hdlc_link));
    
    //获取 发送数据缓冲区
    link->send.data = (uint8_t *)heap.salloc(NAME_PROTOCOL, HDLC_CONFIG_APPL_MTU() + 32);
    if(!link->send.data)
    {
        return(HDLC_ERR_NOMEM);
    }
    
    link->send.length = HDLC_CONFIG_APPL_MTU();
    
    //获取 接收数据缓冲区
    link->recv.data = (uint8_t *)heap.salloc(NAME_PROTOCOL, HDLC_CONFIG_APPL_MTU() + 32);
    if(!link->recv.data)
    {
        heap.free(link->send.data);
        return(HDLC_ERR_NOMEM);
    }
    
    link->recv.length = HDLC_CONFIG_APPL_MTU();
    
    //初始化默认窗口大小
    link->ws_recv = 1;
    link->ws_trans = 1;
    
    //初始化默认收发帧长度
    link->max_len_recv = HDLC_CONFIG_INFO_LEN_DEFAULT;
    link->max_len_trans = HDLC_CONFIG_INFO_LEN_DEFAULT;
    
    //初始化超时时间
    link->link_inactive_timer = hdlc_get_timeout();
    
    //初始化客户端的帧计数
    link->crrr = 0xff;
    link->csss = 0xff;
    
    //初始化通信地址
    link->device_address = hdlc_get_address();
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 释放连接对象
  * @param  
  * @retval 
  */
static enum __hdlc_errors link_cleanup(struct __hdlc_link *link)
{
    struct __dlms_session id;
    
    if(!link)
    {
        return(HDLC_ERR_NOMEM);
    }
    
    if(link->recv.data)
    {
        heap.free(link->recv.data);
    }
    
    if(link->send.data)
    {
        heap.free(link->send.data);
    }
    
    if(link->link_status != LINK_DISCONNECTED)
    {
        id.session = link->client_address;
        id.sap = link->logic_address;
        //取消应用层连接
        HDLC_CONFIG_APPL_RELEASE(id);
    }
    
    heap.set((void*)link, 0, sizeof(struct __hdlc_link));
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 组包FRMR帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors encode_frmr(const struct __hdlc_frame_desc *hdlc_desc, struct __hdlc_link *link, enum __hdlc_frmr_state state)
{
    uint16_t frame_encode = 0;
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    //添加帧类型域以及 Segment 标记位
    *(link->send.segment.data + frame_encode) = 0xA0;
    frame_encode += 1;
    
    //保留帧长度域
    frame_encode += 1;
    
    //控制域
    if(link->link_status == LINK_CONNECTED)
    {
        //添加目的地址
        frame_encode += fill_client_address(link->client_address, \
                                            (link->send.segment.data+frame_encode));
        
        //添加源地址
        frame_encode += fill_server_address(link->device_address, \
                                            link->logic_address, \
                                            hdlc_desc->length_dst, \
                                            (link->send.segment.data+frame_encode));
    }
    else
    {
        //添加目的地址
        frame_encode += fill_client_address(makeup_client_address(hdlc_desc->src), \
                                            (link->send.segment.data+frame_encode));
        
        //添加源地址
        frame_encode += fill_server_address(makeup_device_address(hdlc_desc->dst), \
                                            makeup_logic_address(hdlc_desc->dst), \
                                            hdlc_desc->length_dst, \
                                            (link->send.segment.data+frame_encode));
    }
    
    //控制域
    *(link->send.segment.data + frame_encode) = 0x97;
    frame_encode += 1;
    
    //添加帧长度域
    *(link->send.segment.data + 2) = (frame_encode + 5);
    
    //帧头校验 HCS
    frame_encode += add_check((link->send.segment.data + 1), (frame_encode - 1), (link->send.segment.data + frame_encode + 0));
    
    //错误代码信息  ref IEC_13239  5.5.3.4.2
    *(link->send.segment.data + frame_encode) = *(uint8_t*)(hdlc_desc->ctrl);
    if(link->crrr == 0xff || link->csss == 0xff)
    {
        *(link->send.segment.data + frame_encode) = 0;
    }
    else
    {
        *(link->send.segment.data + frame_encode) = ((link->crrr << 5) & 0xE0) + ((link->csss << 1) & 0x0E);
    }
    *(link->send.segment.data + frame_encode + 1) = (uint8_t)state;
    frame_encode += 2;
    
    //帧头校验 FCS
    frame_encode += add_check((link->send.segment.data + 1), (frame_encode - 1), (link->send.segment.data + frame_encode + 0));
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    link->send.segment.length = frame_encode;
    link->send.segment.confirming = 0;
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 组包RR帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors encode_rr(const struct __hdlc_frame_desc *hdlc_desc, struct __hdlc_link *link)
{
    uint16_t frame_encode = 0;
    
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    //添加帧类型域以及 Segment 标记位
    *(link->send.segment.data + frame_encode) = 0xA0;
    frame_encode += 1;
    
    //保留帧长度域
    frame_encode += 1;
    
    //控制域
    if(link->link_status == LINK_CONNECTED)
    {
        //添加目的地址
        frame_encode += fill_client_address(link->client_address, \
                                            (link->send.segment.data+frame_encode));
        
        //添加源地址
        frame_encode += fill_server_address(link->device_address, \
                                            link->logic_address, \
                                            hdlc_desc->length_dst, \
                                            (link->send.segment.data+frame_encode));
    }
    else
    {
        //添加目的地址
        frame_encode += fill_client_address(makeup_client_address(hdlc_desc->src), \
                                            (link->send.segment.data+frame_encode));
        
        //添加源地址
        frame_encode += fill_server_address(makeup_device_address(hdlc_desc->dst), \
                                            makeup_logic_address(hdlc_desc->dst), \
                                            hdlc_desc->length_dst, \
                                            (link->send.segment.data+frame_encode));
    }
    
    //控制域
    *(link->send.segment.data + frame_encode) = (((link->rrr << 5) + 0x01) | 0x10);
    frame_encode += 1;
    
    //添加帧长度域
    *(link->send.segment.data + 2) = (frame_encode + 1);
    
    //帧头校验 HCS
    frame_encode += add_check((link->send.segment.data + 1), (frame_encode - 1), (link->send.segment.data + frame_encode + 0));
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    link->send.segment.length = frame_encode;
    link->send.segment.confirming = 0;
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 HDLC SNRM 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_snrm(struct __hdlc_link *link, \
                            const struct __hdlc_frame_desc *hdlc_desc)
{
    enum __hdlc_errors errors;
    uint16_t frame_encode = 0;
    uint16_t index_fcs;
    uint16_t max_info_length_recv = 0;
    uint16_t max_info_length_trans = 0;
    uint8_t * index_info;
    
    //初始化链路层通信信道
    errors = link_setup(link);
    if(errors != HDLC_NO_ERR)
    {
        return(errors);
    }
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    //添加帧类型域以及 Segment 标记位
    *(link->send.segment.data+frame_encode) = 0xA0;
    frame_encode += 1;
    
    //保留帧长度域
    frame_encode += 1;
    
    //保存客户端地址
    link->client_address = makeup_client_address(hdlc_desc->src);
    if(!link->client_address)
    {
        link_cleanup(link);
        return (HDLC_ERR_ADDRESS);
    }
    
    //添加目的地址
    frame_encode += fill_client_address(link->client_address, (link->send.segment.data+frame_encode));
    
    
    //保存逻辑地址
    link->logic_address = makeup_logic_address(hdlc_desc->dst);
    if(link->logic_address)
    {
        link_cleanup(link);
        return (HDLC_ERR_ADDRESS);
    }
    
    frame_encode += fill_server_address(link->device_address, link->logic_address, hdlc_desc->length_dst, (link->send.segment.data+frame_encode));
    
    //控制域
    *(link->send.segment.data+frame_encode) = 0x73;
    frame_encode += 1;
    
    //帧头校验 HCS
    index_fcs = frame_encode;
    frame_encode += 2;
    
    //初始化变量
    //SNRM中的可选数据域
    //SNRM中的最大链路数据长度
    if((hdlc_desc->info) && (hdlc_desc->length_info >= 8))
    {
        index_info = &(hdlc_desc->info[3]);
        
        if(index_info[0] == 0x05)
        {
            if(index_info[1] == 1)
            {
                max_info_length_recv = index_info[2];
                index_info += 3;
            }
            else if(index_info[1] == 2)
            {
                if(hdlc_desc->length_info < 10)
                {
                    max_info_length_recv = 0;
                }
                else
                {
                    max_info_length_recv = index_info[2];
                    max_info_length_recv <<= 8;
                    max_info_length_recv += index_info[3];
                    index_info += 4;
                }
            }
        }
        
        if(index_info[0] == 0x06)
        {
            if(index_info[1] == 1)
            {
                max_info_length_trans = index_info[2];
            }
            else if(index_info[1] == 2)
            {
                if(hdlc_desc->length_info < 11)
                {
                    max_info_length_trans = 0;
                }
                else
                {
                    max_info_length_trans = index_info[2];
                    max_info_length_trans <<= 8;
                    max_info_length_trans += index_info[3];
                }
            }
        }
    }
    
    //判断SNRM中的最大链路数据长度有效性并保存
    if(max_info_length_recv && max_info_length_trans)
    {
        if(max_info_length_recv < HDLC_CONFIG_INFO_LEN_MIN)
        {
            link->max_len_recv = HDLC_CONFIG_INFO_LEN_MIN;
        }
        else if(max_info_length_recv > HDLC_CONFIG_INFO_LEN_MAX)
        {
            link->max_len_recv = HDLC_CONFIG_INFO_LEN_MAX;
        }
        else
        {
            link->max_len_recv = max_info_length_recv;
        }
        
        
        if(max_info_length_trans < HDLC_CONFIG_INFO_LEN_MIN)
        {
            link->max_len_trans = HDLC_CONFIG_INFO_LEN_MIN;
        }
        else if(max_info_length_trans > HDLC_CONFIG_INFO_LEN_MAX)
        {
            link->max_len_trans = HDLC_CONFIG_INFO_LEN_MAX;
        }
        else
        {
            link->max_len_trans = max_info_length_trans;
        }
    }
    
    *(link->send.segment.data + frame_encode + 0) = 0x81;
    *(link->send.segment.data + frame_encode + 1) = 0x80;
    *(link->send.segment.data + frame_encode + 2) = 0x12;
    frame_encode += 3;
    
    *(link->send.segment.data + frame_encode + 0) = 0x05;
    *(link->send.segment.data + frame_encode + 1) = 0x01;
    *(link->send.segment.data + frame_encode + 2) = link->max_len_recv;
    frame_encode += 3;
    
    *(link->send.segment.data + frame_encode + 0) = 0x06;
    *(link->send.segment.data + frame_encode + 1) = 0x01;
    *(link->send.segment.data + frame_encode + 2) = link->max_len_trans;
    frame_encode += 3;
    
    *(link->send.segment.data + frame_encode + 0) = 0x07;
    *(link->send.segment.data + frame_encode + 1) = 0x04;
    *(link->send.segment.data + frame_encode + 2) = 0x00;
    *(link->send.segment.data + frame_encode + 3) = 0x00;
    *(link->send.segment.data + frame_encode + 4) = 0x00;
    *(link->send.segment.data + frame_encode + 5) = link->ws_recv;
    frame_encode += 6;
    
    *(link->send.segment.data + frame_encode + 0) = 0x08;
    *(link->send.segment.data + frame_encode + 1) = 0x04;
    *(link->send.segment.data + frame_encode + 2) = 0x00;
    *(link->send.segment.data + frame_encode + 3) = 0x00;
    *(link->send.segment.data + frame_encode + 4) = 0x00;
    *(link->send.segment.data + frame_encode + 5) = link->ws_trans;
    frame_encode += 6;
    
    //添加帧长度域
    *(link->send.segment.data+1) += (((frame_encode-1+2) >> 8) & 0x1f);
    *(link->send.segment.data+2) = ((frame_encode-1+2) & 0x00ff);
    
    //帧头校验 HCS
    add_check((link->send.segment.data+1), (index_fcs-1), (link->send.segment.data+index_fcs+0));
    
    //帧校验 FCS
    frame_encode += add_check((link->send.segment.data+1), (frame_encode-1), (link->send.segment.data+frame_encode+0));
    
    //组包返回的数据
    *(link->send.segment.data+frame_encode) = 0x7e;
    
    frame_encode += 1;
    
    //数据长度
    link->send.segment.length = frame_encode;
    link->send.segment.confirming = frame_encode;
    
    link->link_status = LINK_CONNECTED;
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 HDLC I 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_info(struct __hdlc_link *link, \
                          const struct __hdlc_frame_desc *hdlc_desc)
{
    uint16_t frame_encode = 0;
    uint16_t info_length;
    uint16_t index_hcs;
    struct __dlms_session id;
    
    //判断 information 长度是否超过协商最大长度
    if(hdlc_desc->llc)
    {
        if((hdlc_desc->length_info + 3) > link->max_len_recv)
        {
            //FRMR_INFOLENGTH
            return(encode_frmr(hdlc_desc, link, FRMR_INFOLENGTH));
        }
    }
    else
    {
        if(hdlc_desc->length_info > link->max_len_recv)
        {
            //FRMR_INFOLENGTH
            return(encode_frmr(hdlc_desc, link, FRMR_INFOLENGTH));
        }
    }
    
    //帧序号有效性判断
    //当请求报文中的SSS与上次请求中的一致时则认为是重发的请求
    if(hdlc_desc->sss == link->csss)
    {
        if(link->send.sent >= link->send.segment.confirming)
        {
            link->send.sent -= link->send.segment.confirming;
        }
        else
        {
            link->send.sent = 0;
        }
        link->send.segment.confirming = 0;
        
        link->sss -= 1;
        link->sss &= 0x07;
        link->rrr = link->rrr_confirming;
    }
    else if(hdlc_desc->sss != ((link->csss + 1) & 0x07))
    {
        //RR
        return(encode_rr(hdlc_desc, link));
    }
    
    link->crrr = hdlc_desc->rrr;
    link->csss = hdlc_desc->sss;
    link->rrr_confirming = link->rrr;
    
    //多包接收处理
    if((link->recv.filled + hdlc_desc->length_info) > HDLC_CONFIG_APPL_MTU())
    {
        //多包接收数据已经溢出
        link->recv.filled = 0;
        //FRMR_INFOLENGTH
        return(encode_frmr(hdlc_desc, link, FRMR_INFOLENGTH));
    }
    
    //将数据保存到接收缓冲中
    memcpy((link->recv.data + link->recv.filled), hdlc_desc->info, hdlc_desc->length_info);
    link->recv.filled += hdlc_desc->length_info;
    
    if(hdlc_desc->segment)
    {
        link->csss = hdlc_desc->sss;
        link->rrr_confirming = link->rrr;
        link->rrr += 1;
        link->rrr &= 0x07;
        //RR
        return(encode_rr(hdlc_desc, link));
    }
    
    //使用 HDLC 报文中的 info 来访问 application 层
    id.session = link->client_address;
    id.sap = link->logic_address;
    HDLC_CONFIG_APPL_REQUEST(id,
                             link->recv.data,
                             link->recv.filled,
                             link->send.data,
                             link->send.length,
                             &link->send.filled);
    
    //清空接收缓冲区
    link->recv.filled = 0;
    
    if(!link->send.filled)
    {
        return(HDLC_ERR_LENGTH);
    }
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    //判断是否需要分片发送，添加帧类型域以及 Segment 标记位
    if((link->send.filled + 3) > link->max_len_trans)
    {
        *(link->send.segment.data + frame_encode) = 0xA8;
        frame_encode += 1;
        info_length = link->max_len_trans - 3;
        link->send.sent = link->max_len_trans - 3;
    }
    else
    {
        *(link->send.segment.data + frame_encode) = 0xA0;
        frame_encode += 1;
        info_length = link->send.filled;
        link->send.sent = 0;
        link->send.filled = 0;
    }
    
    //保留帧长度域
    frame_encode += 1;
    
    //添加目的地址
    frame_encode += fill_client_address(link->client_address, (link->send.segment.data+frame_encode));
    
    //添加源地址
    frame_encode += fill_server_address(link->device_address, link->logic_address, hdlc_desc->length_dst, (link->send.segment.data+frame_encode));
    
    //控制域
    link->rrr += 1;
    link->rrr &= 0x07;
    *(link->send.segment.data + frame_encode) = (((link->rrr << 5) + (link->sss << 1)) | 0x10);
    link->sss += 1;
    link->sss &= 0x07;
    
    frame_encode += 1;
    
    index_hcs = frame_encode;
    //帧头校验 HCS
    frame_encode += 2;
    
    //LLC 层标识
    *(link->send.segment.data + frame_encode + 0) = 0xE6;
    *(link->send.segment.data + frame_encode + 1) = 0xE7;
    *(link->send.segment.data + frame_encode + 2) = 0x00;
    frame_encode += 3;
    
    //添加应用层数据
    memcpy((link->send.segment.data + frame_encode), link->send.data, info_length);
    frame_encode += info_length;
    
    //添加帧长度域
    *(link->send.segment.data + 1) += (((frame_encode - 1 + 2) >> 8) & 0x1f);
    *(link->send.segment.data + 2) = ((frame_encode - 1 + 2) & 0x00ff);
    
    //帧头校验 HCS
    add_check((link->send.segment.data + 1), (index_hcs - 1), (link->send.segment.data + index_hcs + 0));
    //帧校验 FCS
    frame_encode += add_check((link->send.segment.data + 1), (frame_encode - 1), (link->send.segment.data + frame_encode + 0));
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    link->send.segment.length = frame_encode;
    link->send.segment.confirming = frame_encode;
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 HDLC RR 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_rr(struct __hdlc_link *link, \
                           const struct __hdlc_frame_desc *hdlc_desc)
{
    uint16_t frame_encode = 0;
    uint16_t index_hcs;
    uint16_t info_length;
    
    //帧序号有效性判断
    //当请求报文中的RRR与上次请求中的一致时则认为是重发的请求
    if(hdlc_desc->rrr == link->crrr)
    {
        if(link->send.sent >= link->send.segment.confirming)
        {
            link->send.sent -= link->send.segment.confirming;
        }
        else
        {
            link->send.sent = 0;
        }
        link->send.segment.confirming = 0;
        link->sss -= 1;
        link->sss &= 0x07;
    }
    else if(hdlc_desc->rrr != ((link->crrr + 1) & 0x07))
    {
        //FRMR_NR
        return(encode_frmr(hdlc_desc, link, FRMR_NR));
    }
    
    link->crrr = hdlc_desc->rrr;
    
    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    
    //判断是否有未发送完成的数据
    if(link->send.filled <= link->send.sent)
    {
        //RR
        return(encode_rr(hdlc_desc, link));
    }
    //判断是否需要分片发送，添加帧类型域以及 Segment 标记位
    else if((link->send.filled - link->send.sent) > link->max_len_trans)
    {
        *(link->send.segment.data + frame_encode) = 0xA8;
        frame_encode += 1;
        info_length = link->max_len_trans;
    }
    else
    {
        *(link->send.segment.data + frame_encode) = 0xA0;
        frame_encode += 1;
        info_length = link->send.filled - link->send.sent;
    }

    //保留帧长度域
    frame_encode += 1;

    //添加目的地址
    frame_encode += fill_client_address(link->client_address, (link->send.segment.data+frame_encode));
    
    //添加源地址
    frame_encode += fill_server_address(link->device_address, link->logic_address, hdlc_desc->length_dst, (link->send.segment.data+frame_encode));

    //控制域
    *(link->send.segment.data + frame_encode) = (((link->rrr << 5) + (link->sss << 1)) | 0x10);
    link->sss += 1;
    link->sss &= 0x07;
    frame_encode += 1;

    index_hcs = frame_encode;
    //帧头校验 HCS
    frame_encode += 2;

    //添加应用层数据
    memcpy((link->send.segment.data + frame_encode), (link->send.data + link->send.sent), info_length);
    link->send.sent += info_length;
    frame_encode += info_length;

    //添加帧长度域
    *(link->send.segment.data + 1) += (((frame_encode - 1 + 2) >> 8) & 0x1f);
    *(link->send.segment.data + 2) = ((frame_encode - 1 + 2) & 0x00ff);

    //帧头校验 HCS
    add_check((link->send.segment.data + 1), (index_hcs - 1), (link->send.segment.data + index_hcs + 0));
    //帧校验 FCS
    frame_encode += add_check((link->send.segment.data + 1), (frame_encode - 1), (link->send.segment.data + frame_encode + 0));

    //组包返回的数据
    *(link->send.segment.data + frame_encode) = 0x7e;
    frame_encode += 1;
    link->send.segment.length = frame_encode;
    link->send.segment.confirming = frame_encode;
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 HDLC DISC 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_disc(struct __hdlc_link *link, \
                             const struct __hdlc_frame_desc *hdlc_desc)
{
    uint16_t frame_encode = 0;
    
    //组包返回的数据
    *(link->send.segment.data+frame_encode) = 0x7e;
    frame_encode += 1;
    
    //添加帧类型域以及 Segment 标记位
    *(link->send.segment.data+frame_encode) = 0xA0;
    frame_encode += 1;
    
    //保留帧长度域
    frame_encode += 1;
    
    //控制域
    if(link->link_status == LINK_CONNECTED)
    {
        //添加目的地址
        frame_encode += fill_client_address(link->client_address, \
                                            (link->send.segment.data+frame_encode));
        
        //添加源地址
        frame_encode += fill_server_address(link->device_address, \
                                            link->logic_address, \
                                            hdlc_desc->length_dst, \
                                            (link->send.segment.data+frame_encode));
        
        *(link->send.segment.data+frame_encode) = 0x73;
        link->link_status = LINK_MARK;
    }
    else
    {
        //添加目的地址
        frame_encode += fill_client_address(makeup_client_address(hdlc_desc->src), \
                                            (link->send.segment.data+frame_encode));
        
        //添加源地址
        frame_encode += fill_server_address(makeup_device_address(hdlc_desc->dst), \
                                            makeup_logic_address(hdlc_desc->dst), \
                                            hdlc_desc->length_dst, \
                                            (link->send.segment.data+frame_encode));
    
        *(link->send.segment.data+frame_encode) = 0x1F;
    }
    frame_encode += 1;
    
    //添加帧长度域
    *(link->send.segment.data + 2) = (frame_encode + 1);
    
    //帧头校验 HCS
    frame_encode += add_check((link->send.segment.data + 1), (frame_encode - 1), (link->send.segment.data + frame_encode + 0));
    
    //组包返回的数据
    *(link->send.segment.data+frame_encode) = 0x7e;
    frame_encode += 1;
    
    //赋值组包长度
    link->send.segment.length = frame_encode;
    link->send.segment.confirming = frame_encode;
    
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 HDLC RNR 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_rnr(struct __hdlc_link *link, \
                            const struct __hdlc_frame_desc *hdlc_desc)
{
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 HDLC UI 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_ui(struct __hdlc_link *link, \
                           const struct __hdlc_frame_desc *hdlc_desc)
{
    return(HDLC_NO_ERR);
}

/**
  * @brief 处理 未知 帧
  * @param  
  * @retval 
  */
static enum __hdlc_errors request_unknown(struct __hdlc_link *link, \
                           const struct __hdlc_frame_desc *hdlc_desc)
{
    return(encode_frmr(hdlc_desc, link, FRMR_OPCODE));
}








/**
  * @brief 链路初始化
  * @param  
  * @retval 
  */
void hdlc_init(void)
{
    uint8_t cnt;
    
    for(cnt=0; cnt<HDLC_CONFIG_MAX_CHANNEL; cnt++)
    {
        link_cleanup(&hdlc_links[cnt]);
    }
    
    heap.set((void *)hdlc_links, 0, sizeof(hdlc_links));
}

/**
  * @brief 链路数据报匹配
  * @param  
  * @retval 
  */
uint8_t hdlc_matched(const uint8_t *frame, uint16_t length)
{
    if(!frame)
    {
        return(0);
    }
    
    if(length < 9)
    {
        return(0);
    }
    
    if((frame[0] == 0x7e) && (frame[length - 1] == 0x7e))
    {
        return(0xff);
    }
    
    return(0);
}

/**
  * @brief 链路计时器
  * @param  
  * @retval 
  */
void hdlc_tick(uint16_t tick)
{
    uint8_t cnt;
    
    for(cnt=0; cnt<HDLC_CONFIG_MAX_CHANNEL; cnt++)
    {
        if(hdlc_links[cnt].link_status == LINK_DISCONNECTED)
        {
            continue;
        }
        
        if(hdlc_links[cnt].link_status == LINK_MARK)
        {
            //断开连接
            link_cleanup(&hdlc_links[cnt]);
            continue;
        }
        
        if(hdlc_links[cnt].link_inactive_timer > tick)
        {
            //计时递减
            hdlc_links[cnt].link_inactive_timer -= tick;
        }
        else
        {
            //断开连接
            link_cleanup(&hdlc_links[cnt]);
        }
    }
}

/**
  * @brief 获取HDLC地址
  * @param  
  * @retval 
  */
uint16_t hdlc_get_address(void)
{
    uint16_t address = 0;
    
    file.read("hdlc", \
              STRUCT_OFFSET(struct __hdlc_configs, local_addr), \
              STRUCT_SIZE(struct __hdlc_configs, local_addr), \
              &address);
    
    if((address < 0x10) || (address > 0x3FFD))
    {
        address = 0x10;
    }
    
    return(address);
}

/**
  * @brief 设置HDLC地址
  * @param  
  * @retval 
  */
uint16_t hdlc_set_address(uint16_t addr)
{
    if(addr < 0x10)
    {
        addr = 0x10;
    }
    else if(addr > 0x3FFD)
    {
        addr = 0x3FFD;
    }
    
    file.write("hdlc", \
              STRUCT_OFFSET(struct __hdlc_configs, local_addr), \
              STRUCT_SIZE(struct __hdlc_configs, local_addr), \
              &addr);
    
    return(addr);
}

/**
  * @brief 获取HDLC超时时间
  * @param  
  * @retval 
  */
uint16_t hdlc_get_timeout(void)
{
    uint16_t timeout = 0;
    
    file.read("hdlc", \
              STRUCT_OFFSET(struct __hdlc_configs, inact_time), \
              STRUCT_SIZE(struct __hdlc_configs, inact_time), \
              &timeout);
    
    if(timeout < 10)
    {
        timeout = 10;
    }
    
    return(timeout);
}

/**
  * @brief 设置HDLC超时时间
  * @param  
  * @retval 
  */
uint16_t hdlc_set_timeout(uint16_t sec)
{
    if(sec < 10)
    {
        sec = 10;
    }
    
    file.write("hdlc", \
              STRUCT_OFFSET(struct __hdlc_configs, inact_time), \
              STRUCT_SIZE(struct __hdlc_configs, inact_time), \
              &sec);
    
    return(sec);
}

/**
  * @brief HDLC窗口大小
  * @param  
  * @retval 
  */
uint8_t hdlc_get_window_size(void)
{
    return(1);
}

/**
  * @brief HDLC最大帧长度
  * @param  
  * @retval 
  */
uint16_t hdlc_get_max_info_length(void)
{
    return(HDLC_CONFIG_INFO_LEN_MAX);
}

/**
  * @brief 链路数据请求
  * @param  
  * @retval 
  */
uint16_t hdlc_request(uint8_t channel, const uint8_t *frame, uint16_t length)
{
    struct __hdlc_frame_desc frame_desc;
    enum __hdlc_errors hdlc_errors;
    
    if(channel > HDLC_CONFIG_MAX_CHANNEL)
    {
        return(0);
    }
    
    //解析HDLC报文
    if(decode_hdlc_frame(frame, length, &frame_desc) != HDLC_NO_ERR)
    {
        return(0);
    }
    
    //接收到广播地址
    if(broadcast_matched(&frame_desc))
    {
        if((*(frame_desc.ctrl) & 0xef) == 0x03)
        {
            //UI Frame Received
            hdlc_errors = request_ui(&hdlc_links[channel], &frame_desc);
        }
        
        return(0);
    }
    //接收到单播地址
    else
    {
        //检查报文目的地址是否为本机地址
        if(!address_matched(channel, &frame_desc))
        {
            return(0);
        }
        
        if((*(frame_desc.ctrl) & 0x01) == 0)
        {
            //I Frame Received
            if(hdlc_links[channel].link_status == LINK_CONNECTED)
            {
                hdlc_errors = request_info(&hdlc_links[channel], &frame_desc);
            }
        }
        else
        {
            if((*(frame_desc.ctrl) & 0xef) == 0x83)
            {
                //SNRM Frame Received
                hdlc_errors = request_snrm(&hdlc_links[channel], &frame_desc);
            }
            else if((*(frame_desc.ctrl) & 0x0f) == 0x01)
            {
                //RR Frame Received
                if(hdlc_links[channel].link_status == LINK_CONNECTED)
                {
                    hdlc_errors = request_rr(&hdlc_links[channel], &frame_desc);
                }
            }
            else if((*(frame_desc.ctrl) & 0xef) == 0x03)
            {
                //UI Frame Received
                hdlc_errors = request_ui(&hdlc_links[channel], &frame_desc);
            }
            else if((*(frame_desc.ctrl) & 0xef) == 0x43)
            {
                //DISC Frame Received
                hdlc_errors = request_disc(&hdlc_links[channel], &frame_desc);
            }
            else if((*(frame_desc.ctrl) & 0x0f) == 0x06)
            {
                //RNR Frame Received
                if(hdlc_links[channel].link_status == LINK_CONNECTED)
                {
                    hdlc_errors = request_rnr(&hdlc_links[channel], &frame_desc);
                }
            }
            else
            {
                //error
                hdlc_errors = request_unknown(&hdlc_links[channel], &frame_desc);
            }
        }
        
        if(hdlc_errors == HDLC_NO_ERR)
        {
            //重新初始化定时器
            hdlc_links[channel].link_inactive_timer = hdlc_get_timeout() * 1000;
            
            return(0xff);
        }
        
        return(0);
    }
}

/**
  * @brief 链路数据响应
  * @param  
  * @retval 
  */
uint16_t hdlc_response(uint8_t channel, uint8_t *frame, uint16_t length)
{
    uint16_t segment_length;
    
    if(channel > HDLC_CONFIG_MAX_CHANNEL)
    {
        return(0);
    }
    
    if(!hdlc_links[channel].send.segment.length)
    {
        return(0);
    }
    
    segment_length = hdlc_links[channel].send.segment.length;
    hdlc_links[channel].send.segment.length = 0;
    
    if(segment_length <= length)
    {
        memcpy(frame, hdlc_links[channel].send.segment.data, segment_length);
        return(segment_length);
    }
    else
    {
        memcpy(frame, hdlc_links[channel].send.segment.data, length);
        return(length);
    }
}
