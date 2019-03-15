/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/
/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "dlms_association.h"
#include "dlms_application.h"

/* Private typedef -----------------------------------------------------------*/
/**	
  * @brief SAP
  */
struct __sap
{
    uint8_t id;
    enum __dlms_access_level level;
};

/**	
  * @brief Object identifier
  */
struct __object_identifier
{
    uint8_t joint_iso_ctt;
    uint8_t country;
    uint16_t name;
    uint8_t organization;
    uint8_t ua;
    uint8_t context;
    uint8_t id;
};

/**	
  * @brief Association Source Diagnostics
  */
enum __asso_diagnose
{
    SUCCESS_NOSEC_LLS = 0,
    FAILURE_CONTEXT_NAME,
    FAILURE_NO_REASION,
    SUCCESS_HLS = 0x0E,
};

/**	
  * @brief User information
  */
struct __user_info
{
    uint8_t dedkey[16+2];
    uint8_t response_allowed;
    uint8_t proposed_quality;
    uint8_t proposed_version;
    uint8_t conformance[3];
    uint16_t client_max_pdu;
};

/**	
  * @brief Association object
  */
struct __dlms_association
{
    enum __asso_status status;
    enum __asso_diagnose diagnose;
    struct __sap sap;
    struct __object_identifier appl_name;
    struct __object_identifier mech_name;
    uint8_t systitle[8+2];
    uint8_t akey[16+2];
    uint8_t ekey[16+2];
    uint8_t ctos[32+2];
    uint8_t stoc[32+2];
    uint8_t sc;
    uint32_t fc;
    struct __user_info info;
    void *appl;
};

/**	
  * @brief Association request type
  */
enum __asso_reqs
{
    AARQ = 0x60,
    AARE,
    RLRQ,
    RLRE,
};

struct __aarq_request
{
    const uint8_t *protocol_version;
    const uint8_t *application_context_name;
    const uint8_t *calling_AP_title;
    const uint8_t *calling_AE_qualifier;
    const uint8_t *calling_AP_invocation_id;
    const uint8_t *calling_AE_invocation_id;
    const uint8_t *sender_acse_requirements;
    const uint8_t *mechanism_name;
    const uint8_t *calling_authentication_value;
    const uint8_t *implementation_information;
    const uint8_t *user_information;
};

/* Private define ------------------------------------------------------------*/
//DLMS 配置参数

//DLMS 最大报文长度
#define DLMS_CONFIG_MAX_BLOCK_SIZE              ((uint16_t)(512))

//DLMS 同时最多支持的ASSOCIATION数量
#define DLMS_CONFIG_MAX_ASSO                    ((uint8_t)(6))

//定义COSEM层接口（data, info, length, buffer, max buffer length, filled buffer length）
#define DLMS_CONFIG_COSEM_REQUEST(d,i,l,b,m,f)  dlms_appl_entrance(d,i,l,b,m,f)

/* Private macro -------------------------------------------------------------*/
#define DLMS_SAP_AMOUNT             ((uint8_t)(sizeof(sap_support_list) / sizeof(struct __sap)))

/* Private variables ---------------------------------------------------------*/
/**	
  * @brief 支持的SAP列表
  */
static const struct __sap sap_support_list[] = 
{
    {0x10, DLMS_ACCESS_LOWEST},
    {0x20, DLMS_ACCESS_LOW},
    {0x30, DLMS_ACCESS_HIGH},
};

/**	
  * @brief 已经建立的连接对象
  */
static struct __dlms_association *asso_list[DLMS_CONFIG_MAX_ASSO] = {0};

/**	
  * @brief 当前正在处理的连接对象
  */
static struct __dlms_association *asso_current = (void *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief 密码比对
  */
static uint8_t password_identified(const uint8_t *info)
{
    //...
    return(0);
}

/**
  * @brief 索引 AARQ 报文
  */
static void parse_aarq_frame(const uint8_t *info, uint16_t length, struct __aarq_request *request)
{
    uint16_t frame_length = *(info + 1);
    uint16_t frame_decoded = 2;
    
    if(!request)
    {
        return;
    }
    
    heap.set(request, 0, sizeof(struct __aarq_request));
    
    if(length != (frame_length + 2))
    {
        return;
    }
    
    while(1)
    {
        if((frame_decoded >= length) || (frame_decoded >= frame_length))
        {
            break;
        }
        
        switch(*(info + frame_decoded))
        {
            case 0x80: //protocol-version,implicit bit-string
			{
                request->protocol_version = (info + frame_decoded);
                frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0xA1: //APPLICATION-CONTEXT-NAME
			{
                 request->application_context_name = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0xA6: //AP-title(calling)
			{
                 request->calling_AP_title = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0xA7: //AE-qualifier-identifier(calling)
			{
                 request->calling_AE_qualifier = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0xA8: //AP-invocation-identifier(calling)
			{
                 request->calling_AP_invocation_id = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0xA9: //AE-invocation-identifier(calling)
			{
                 request->calling_AE_invocation_id = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0x8A: //ACSE-requirement
			{
                 request->sender_acse_requirements = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0x8B: //Mechanism-name
			{
                 request->mechanism_name = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0x9D: //implementation-information
            case 0xBD: //按绿皮书应是0x9D,而CTI软件里是0xBD
			{
                 request->implementation_information = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            case 0xAC: //Authentication-value
			{
                 request->calling_authentication_value = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
            }
            case 0xBE: //Association-information
			{
                 request->user_information = (info + frame_decoded);
                 frame_decoded += *(info + frame_decoded + 1) + 2;
				break;
			}
            default: //error
			{
                return;
			}
        }
    }
}

/**
  * @brief 组包 ObjectIdentifier
  */
static void encode_object_identifier(const uint8_t *info, struct __object_identifier *object_identifier)
{
    uint16_t temp;
    
    if(!info)
    {
        return;
    }
    
    if(!object_identifier)
    {
        return;
    }
    
    heap.set(object_identifier, 0, sizeof(struct __object_identifier));
    
    object_identifier->joint_iso_ctt = *(info + 0) / 40;
    object_identifier->country = *(info + 0) % 40;
    temp = *(info + 1);
    temp <<= 8;
    temp += *(info + 2);
    
    if(temp & 0x0100)
    {
        object_identifier->name = (*(info + 2) | 0x80);
        temp &= 0x7fff;
        temp >>= 1;
        object_identifier->name += (temp & 0xff00);
    }
    else
    {
        object_identifier->name = (temp & 0x7fff);
    }
    
    object_identifier->organization = *(info + 3);
    object_identifier->ua = *(info + 4);
    object_identifier->context = *(info + 5);
    object_identifier->id = *(info + 6);
}

/**
  * @brief 组包 UserInformation
  */
static void parse_user_information(const uint8_t *info, struct __dlms_association *asso)
{
    //...
}

/**	
  * @brief 
  */
static void dlms_asso_lowest(struct __dlms_association *asso, const struct __aarq_request *request)
{
	//user_information
    parse_user_information(request->user_information, asso);
}

/**	
  * @brief 
  */
static void dlms_asso_low(struct __dlms_association *asso, const struct __aarq_request *request)
{
    //sender_acse_requirements
    //LLS或者HLS，认证标识必须置位
    if(!request->sender_acse_requirements)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(!(*(request->sender_acse_requirements + 3) & 0x80))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
    }
    
    //calling_authentication_value
    //LLS时为密码
    if(!request->calling_authentication_value)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(request->calling_authentication_value[1] > (16+2))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
        else
        {
            heap.copy(asso->akey, &request->calling_authentication_value[2], request->calling_authentication_value[1]);
            if(!password_identified(asso->akey))
            {
                //拒绝建立链接
                asso->diagnose = FAILURE_CONTEXT_NAME;
                return;
            }
        }
    }
    
	//user_information
    parse_user_information(request->user_information, asso);
}

/**	
  * @brief 
  */
static void dlms_asso_high(struct __dlms_association *asso, const struct __aarq_request *request)
{
    //sender_acse_requirements
    //LLS或者HLS，认证标识必须置位
    if(!request->sender_acse_requirements)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(!(*(request->sender_acse_requirements + 3) & 0x80))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
    }
    
    //calling_authentication_value
    //LLS时为密码
    if(!request->calling_authentication_value)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(request->calling_authentication_value[1] > (32+2))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
        else
        {
            heap.copy(asso->ctos, &request->calling_authentication_value[2], request->calling_authentication_value[1]);
        }
    }
    
	//user_information
    parse_user_information(request->user_information, asso);
}

/**
  * @brief 解析 ObjectIdentifier, 输出 7字节
  */
static void decode_object_identifier(const struct __object_identifier *object_identifier, uint8_t *info)
{
    if(!info)
    {
        return;
    }
    
    if(!object_identifier)
    {
        return;
    }
    
    heap.set(info, 0, 7);
    
    *(info + 0) = object_identifier->joint_iso_ctt * 40 + object_identifier->country;

    if(object_identifier->name & 0x0080)
    {
        *(info + 2) = (uint8_t)(object_identifier->name & 0x007f);
        *(info + 1) = (uint8_t)(object_identifier->name >> 8);
        *(info + 1) <<= 1;
        *(info + 1) |= 0x81;
    }
    else
    {
        *(info + 2) = (uint8_t)(object_identifier->name & 0x007f);
        *(info + 1) = (uint8_t)(object_identifier->name >> 8);
        *(info + 1) |= 0x80;
    }

    *(info + 3) = object_identifier->organization;
    *(info + 4) = object_identifier->ua;
    *(info + 5) = object_identifier->context;
    *(info + 6) = object_identifier->id;
}

/**
  * @brief 解析 UserInformation
  */
static void generate_user_information(struct __dlms_association *asso, uint8_t *info)
{
    
}

/**	
  * @brief 
  */
static void dlms_generate_reply(struct __dlms_association *asso,
                                uint8_t *buffer,
                                uint16_t buffer_length,
                                uint16_t *filled_length)
{
    //...
}

/**	
  * @brief 
  */
static void dlms_asso_aarq(struct __dlms_association *asso,
                           const uint8_t *info,
                           uint16_t length,
                           uint8_t *buffer,
                           uint16_t buffer_length,
                           uint16_t *filled_length)
{
    struct __aarq_request request;
    
    parse_aarq_frame(info, length, &request);
    
    //application-context-name 和 user_information 是建立连接所必须的
    //在只存在 application-context-name 和 user_information 两个数据且匹配时，建立最低级别连接
    if(!request.application_context_name || !request.user_information)
    {
        return;
    }
    
    //保存 application-context-name
    //2 16 756 5 8 1 x
    //context_id = 1 : Logical_Name_Referencing_No_Ciphering 只响应不加密报文
    //context_id = 3 : Logical_Name_Referencing_With_Ciphering 响应加密报文和不加密报文
    encode_object_identifier((request.application_context_name + 4), &asso->appl_name);

    if(asso->appl_name.id == 3)
    {
        //有效性判断，context_id 为 3 时，必须提供 calling_AP_title
        if(!request.calling_AP_title)
        {
            return;
        }
        
        if(request.calling_AP_title[1] != 10)
        {
            return;
        }
        
        //保存 calling_AP_title
        heap.copy(asso->systitle, &request.calling_AP_title[2], request.calling_AP_title[1]);
    }
    
    //保存 mechanism_name
    //2 16 756 5 8 2 x
    if(request.mechanism_name)
    {
        encode_object_identifier((request.mechanism_name + 4), &asso->mech_name);
    }
    
    //通过 mechanism_name 来判别要建立什么级别的连接
    //x=0 最低等级连接，不需要认证，此时 mechanism_name 可以没有
    //x=1 低等级连接，使用密码来认证
    //x=2 高等级连接，使用自定义加密算法来认证
    //x=3 高等级连接，使用MD5加密算法来认证
    //x=4 高等级连接，使用SHA-1加密算法来认证
    //x=5 高等级连接，使用GMAC加密算法来认证
    //x=6 高等级连接，使用SHA-256加密算法来认证
    //x=7 高等级连接，使用_ECDSA加密算法来认证
    //目前支持 0  1  5
    switch(asso->mech_name.id)
    {
        case 0:
            if(asso->sap.level == DLMS_ACCESS_LOWEST)
            {
                //无认证
                dlms_asso_lowest(asso, &request);
            }
            break;
        case 1:
            if(asso->sap.level == DLMS_ACCESS_LOW)
            {
                //低级别认证
                dlms_asso_low(asso, &request);
            }
            break;
        case 2:
        case 3:
        case 4:
            break;
        case 5:
            if(asso->sap.level == DLMS_ACCESS_HIGH)
            {
                //高级别认证
                dlms_asso_high(asso, &request);
            }
            break;
        case 6:
        case 7:
        default:
            break;
    }
    
    //生成回复报文
    dlms_generate_reply(asso, buffer, buffer_length, filled_length);
}

/**	
  * @brief 
  */
static void dlms_asso_rlrq(struct __dlms_association *asso,
                           const uint8_t *info,
                           uint16_t length,
                           uint8_t *buffer,
                           uint16_t buffer_length,
                           uint16_t *filled_length)
{
    uint8_t cnt;
    
    for(cnt=0; cnt<DLMS_CONFIG_MAX_ASSO; cnt++)
    {
        if(!asso_list[cnt])
        {
            continue;
        }
        
        if(asso_list[cnt] == asso)
        {
            if(asso_list[cnt]->appl)
            {
                heap.free(asso_list[cnt]->appl);
            }
            heap.free(asso_list[cnt]);
            asso_list[cnt] = (void *)0;
        }
    }
    
    if(buffer_length < 3)
    {
        return;
    }
    
    buffer[0] = RLRE;
    buffer[1] = 0x00;
    buffer[2] = 0x00;
    *filled_length = 3;
}

/**	
  * @brief 
  */
static void dlms_appl_request(struct __dlms_association *asso,
                              const uint8_t *info,
                              uint16_t length,
                              uint8_t *buffer,
                              uint16_t buffer_length,
                              uint16_t *filled_length)
{
    asso_current = asso;
    DLMS_CONFIG_COSEM_REQUEST(&asso->appl, info, length, buffer, buffer_length, filled_length);
    asso_current = (void *)0;
}









/**	
  * @brief 
  */
void dlms_entrance(uint8_t sap,
                        const uint8_t *info,
                        uint16_t length,
                        uint8_t *buffer,
                        uint16_t buffer_length,
                        uint16_t *filled_length)
{
    uint8_t cnt;
    enum __dlms_access_level level = DLMS_ACCESS_NO;
    uint8_t id = (sap >> 1);
    struct __dlms_association *asso = (void *)0;
    
    if(filled_length)
    {
        *filled_length = 0;
    }
    
    //有效性判断
    if((!info) || (!length) || (!buffer) || (!buffer_length) || (!filled_length))
    {
        if(!sap)
        {
            return;
        }
        
        //查询SAP是否已经在协商列表中
        for(cnt=0; cnt<DLMS_CONFIG_MAX_ASSO; cnt++)
        {
            if(!asso_list[cnt])
            {
                continue;
            }
            
            if(asso_list[cnt]->sap.id == id)
            {
                if(asso_list[cnt]->appl)
                {
                    heap.free(asso_list[cnt]->appl);
                }
                heap.free(asso_list[cnt]);
                asso_list[cnt] = (void *)0;
            }
        }
        
        return;
    }
    
    //查询SAP是否在支持的SAP列表中
    for(cnt=0; cnt<DLMS_SAP_AMOUNT; cnt++)
    {
        if(sap_support_list[cnt].id == id)
        {
            level = sap_support_list[cnt].level;
            break;
        }
    }
    
    //没有查询到支持的SAP
    if(level == DLMS_ACCESS_NO)
    {
        return;
    }
    
    //查询SAP是否已完成协商
    for(cnt=0; cnt<DLMS_CONFIG_MAX_ASSO; cnt++)
    {
        if(!asso_list[cnt])
        {
            continue;
        }
        
        if(asso_list[cnt]->sap.id == id)
        {
            asso = asso_list[cnt];
        }
    }
    
    //SAP不在已完成的协商列表中，生成一个新的SAP对象
    if(!asso)
    {
        for(cnt=0; cnt<DLMS_CONFIG_MAX_ASSO; cnt++)
        {
            if(!asso_list[cnt])
            {
                asso_list[cnt] = heap.salloc(sizeof(struct __dlms_association));
                if(!asso_list[cnt])
                {
                    return;
                }
                
                asso = asso_list[cnt];
                heap.set(asso, 0, sizeof(struct __dlms_association));
                asso->sap.id = 0xff;
                break;
            }
        }
    }
    
    //SAP对象生成失败
    if(!asso)
    {
        return;
    }
    
    //解析报文
    switch(*info)
    {
        case AARQ:
        {
            dlms_asso_aarq(asso, info, length, buffer, buffer_length, filled_length);
            break;
        }
        case RLRQ:
        {
            dlms_asso_rlrq(asso, info, length, buffer, buffer_length, filled_length);
            break;
        }
        default:
        {
            if(asso->status != NON_ASSOCIATED)
            {
                dlms_appl_request(asso, info, length, buffer, buffer_length, filled_length);
            }
            break;
        }
    }
}

/**	
  * @brief 应用层最大报文传输长度
  */
uint16_t dlms_asso_mtu(void)
{
    return(DLMS_CONFIG_MAX_BLOCK_SIZE);
}

/**
  * @brief 获取 Calling AP Title 8字节
  */
uint8_t dlms_asso_systitle(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if(asso_current->systitle[1] != 8)
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->systitle[2], 8);
    
    return(8);
}

/**
  * @brief 获取 stoc <32字节
  */
uint8_t dlms_asso_stoc(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if((asso_current->stoc[1] > 32) || (asso_current->stoc[1] == 0))
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->stoc[2], asso_current->stoc[1]);
    
    return(asso_current->stoc[1]);
}

/**
  * @brief 获取 ctos <32字节
  */
uint8_t dlms_asso_ctos(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if((asso_current->ctos[1] > 32) || (asso_current->ctos[1] == 0))
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->ctos[2], asso_current->ctos[1]);
    
    return(asso_current->ctos[1]);
}

/**
  * @brief 接受 F(CtoS)
  */
uint8_t dlms_asso_accept_fctos(void)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if(asso_current->status == ASSOCIATION_PENDING)
    {
        asso_current->status = ASSOCIATED;
        asso_current->sap.level = DLMS_ACCESS_HIGH;
        return(0xff);
    }
    
    return(0);
}

/**
  * @brief 获取 Security Control Byte 1字节
  */
uint8_t dlms_asso_sc(void)
{
    if(!asso_current)
    {
        return(0);
    }
    
    return(asso_current->sc);
}

/**
  * @brief 获取 Frame Counter 4 字节
  */
uint8_t dlms_asso_fc(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    asso_current->fc += 1;
    
    buffer[0] = (uint8_t)(asso_current->fc >> 24);
    buffer[1] = (uint8_t)(asso_current->fc >> 16);
    buffer[2] = (uint8_t)(asso_current->fc >> 8);
    buffer[3] = (uint8_t)(asso_current->fc >> 0);
    
    return(4);
}

/**
  * @brief 获取 akey
  */
uint8_t dlms_asso_akey(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if((asso_current->akey[1] > 16) || (asso_current->akey[1] == 0))
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->akey[2], asso_current->akey[1]);
    
    return(asso_current->akey[1]);
}

/**
  * @brief 获取 ekey
  */
uint8_t dlms_asso_ekey(uint8_t Channel, uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if((asso_current->ekey[1] > 16) || (asso_current->ekey[1] == 0))
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->ekey[2], asso_current->ekey[1]);
    
    return(asso_current->ekey[1]);
}

/**
  * @brief 获取 dedkey
  */
uint8_t dlms_asso_dedkey(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if((asso_current->info.dedkey[1] > 16) || (asso_current->info.dedkey[1] == 0))
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->info.dedkey[2], asso_current->info.dedkey[1]);
    
    return(asso_current->info.dedkey[1]);
}

/**
  * @brief 获取 Application Context Name
  */
uint8_t dlms_asso_applname(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    heap.copy(buffer, (uint8_t *)&(asso_current->appl_name), sizeof(asso_current->appl_name));
    
    return(sizeof(asso_current->appl_name));
}

/**
  * @brief 获取 Authentication Mechanism Name
  */
uint8_t dlms_asso_mechname(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    heap.copy(buffer, (uint8_t *)&(asso_current->mech_name), sizeof(asso_current->mech_name));
    
    return(sizeof(asso_current->mech_name));
}

/**
  * @brief 获取 xDLMS Context Info
  */
uint8_t dlms_asso_contextinfo(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    heap.copy(buffer, (uint8_t *)&(asso_current->info), sizeof(asso_current->info));
    
    return(sizeof(asso_current->info));
}

/**
  * @brief 获取 Association Status
  */
enum __asso_status dlms_asso_status(void)
{
    if(!asso_current)
    {
        return(NON_ASSOCIATED);
    }
    
    return(asso_current->status);
}

/**
  * @brief 获取 Access Level
  */
enum __dlms_access_level dlms_asso_level(void)
{
    if(!asso_current)
    {
        return(DLMS_ACCESS_NO);
    }
    
    return(asso_current->sap.level);
}
