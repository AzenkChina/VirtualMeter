/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "jiffy.h"
#include "dlms_association.h"
#include "dlms_application.h"
#include "mbedtls/gcm.h"

/* Private typedef -----------------------------------------------------------*/
/**	
  * @brief SAP
  */
struct __sap
{
    uint8_t id;//client id
    uint8_t conformance[3];//conformance block
    uint8_t suit;//选择哪个object list
    enum __dlms_access_level level;//access level
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
    uint8_t sc;
    uint8_t dedkey[32+2];
    uint8_t response;
    uint8_t quality;
    uint8_t version;
    uint16_t max_pdu;
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
    uint8_t callingtitle[8+2];
    uint8_t localtitle[8+2];
    uint8_t akey[32+2];
    uint8_t ekey[32+2];
    uint8_t ctos[64+2];
    uint8_t stoc[64+2];
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

/**	
  * @brief Decode AARQ request frame
  */
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

//定义外部接口
//COSEM层请求（data, info, length, buffer, max buffer length, filled buffer length）
#define DLMS_CONFIG_COSEM_REQUEST(d,i,l,b,m,f)  dlms_appl_entrance(d,i,l,b,m,f)
//验证密码（info）
#define DLMS_CONFIG_PASSWD_VERIFY(i)            0
//加载认证密钥（info）
#define DLMS_CONFIG_LOAD_AKEY(i)                ;
//加载加密密钥（info）
#define DLMS_CONFIG_LOAD_EKEY(i)                ;
//加载本机system title（info）
#define DLMS_CONFIG_LOAD_TITLE(i)               ;

/* Private macro -------------------------------------------------------------*/
#define DLMS_SAP_AMOUNT             ((uint8_t)(sizeof(sap_support_list) / sizeof(struct __sap)))

/* Private variables ---------------------------------------------------------*/
/**	
  * @brief 已注册支持的SAP
  */
static const struct __sap sap_support_list[] = 
{
    //内容依次为
    ///客户端地址
    //支持的 conformance
    //使用的 objectlist 的标识
    //认证等级
    {0x10,{0x00, 0x10, 0x11},0x01,DLMS_ACCESS_LOWEST},
    {0x20,{0x00, 0x10, 0x11},0x01,DLMS_ACCESS_LOW},
    {0x30,{0x00, 0x30, 0x1D},0x01,DLMS_ACCESS_HIGH},
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
    uint8_t offset;
    uint8_t cnt; 
    const uint8_t *msg = info;
    uint8_t *output = (void *)0;
    uint8_t *add = (void *)0;
    uint8_t *iv = (void *)0;
    mbedtls_gcm_context ctx;
    int ret = 0;
    
    if(!asso)
    {
        return;
    }
    
    if(!info)
    {
        heap.set(asso->sap.conformance, 0, sizeof(asso->sap.conformance));
        return;
    }
    
    if((info[4] == 0x21) && ((info[5] + 4) == info[1]))
    {
        asso->info.sc = info[6];
        
        mbedtls_gcm_init(&ctx);
        ret = mbedtls_gcm_setkey(&ctx,
                                 MBEDTLS_CIPHER_ID_AES,
                                 &asso->ekey[2],
                                 asso->ekey[1]*8);
        
        switch(asso->info.sc & 0xf0)
        {
            case 0x10:
            {
                //仅认证
                output = heap.dalloc(4);
                if(!output)
                {
                    ret = -1;
                    break;
                }
                
                add = heap.dalloc(info[5] + asso->akey[1]);
                if(!add)
                {
                    ret = -1;
                    break;
                }
                add[0] = asso->info.sc;
                heap.copy(&add[1], &asso->akey[2], asso->akey[1]);
                heap.copy(&add[1+asso->akey[1]], &info[11], (info[5] - 5 - 12));
                
                iv = heap.dalloc(12);
                if(!iv)
                {
                    ret = -1;
                    break;
                }
                heap.copy(iv, &asso->callingtitle[2], 8);
                heap.copy(&iv[8], &info[7], 4);
                
                ret = mbedtls_gcm_auth_decrypt(&ctx,
                                               0,
                                               iv,
                                               12,
                                               add,
                                               (info[5] + asso->akey[1] - 5 - 12),
                                               &info[5 + (info[5] - 12)],
                                               12,
                                               &info[11],
                                               output);
                msg = &info[5];
                break;
            }
            case 0x20:
            {
                //仅加密
                output = heap.dalloc(info[5]);
                if(!output)
                {
                    ret = -1;
                    break;
                }
                
                add = heap.dalloc(4);
                if(!add)
                {
                    ret = -1;
                    break;
                }
                
                iv = heap.dalloc(12);
                if(!iv)
                {
                    ret = -1;
                    break;
                }
                heap.copy(iv, &asso->callingtitle[2], 8);
                heap.copy(&iv[8], &info[7], 4);
                
                ret = mbedtls_gcm_auth_decrypt(&ctx,
                                               (info[5] - 5),
                                               iv,
                                               12,
                                               add,
                                               0,
                                               &info[5 + info[5]],
                                               0,
                                               &info[11],
                                               output);
                msg = output;
                break;
            }
            case 0x30:
            {
                //加密加认证
                output = heap.dalloc(info[5]);
                if(!output)
                {
                    ret = -1;
                    break;
                }
                
                add = heap.dalloc(1 + asso->akey[1]);
                if(!add)
                {
                    ret = -1;
                    break;
                }
                add[0] = asso->info.sc;
                heap.copy(&add[1], &asso->akey[2], asso->akey[1]);
                
                iv = heap.dalloc(12);
                if(!iv)
                {
                    ret = -1;
                    break;
                }
                heap.copy(iv, &asso->callingtitle[2], 8);
                heap.copy(&iv[8], &info[7], 4);
                
                ret = mbedtls_gcm_auth_decrypt(&ctx,
                                               (info[5] - 5 - 12),
                                               iv,
                                               12,
                                               add,
                                               (1 + asso->akey[1]),
                                               &info[5 + (info[5] - 12)],
                                               12,
                                               &info[11],
                                               output);
                msg = output;
                break;
            }
            default:
            {
                ret = -1;
            }
        }
        
        if(add)
        {
            heap.free(add);
        }
        if(iv)
        {
            heap.free(iv);
        }
        
        mbedtls_gcm_free( &ctx );
        
        if(ret)
        {
            heap.set(asso->sap.conformance, 0, sizeof(asso->sap.conformance));
            if(output)
            {
                heap.free(output);
            }
            return;
        }
    }
    
    //dedicated_key
    if(msg[5])
    {
        if(msg[6] > 32)
        {
            if(output)
            {
                heap.free(output);
            }
            return;
        }
        else
        {
            if(msg[6] < 32)
            {
                heap.copy(asso->info.dedkey, &msg[5], (msg[6] + 2));
            }
        }
        
        offset = 7 + msg[6];
    }
    else
    {
        offset = 6;
    }
    
    //response_allowed
    if(msg[offset])
    {
        asso->info.response = msg[offset + 1];
        offset += 2;
    }
    else
    {
        offset += 1;
    }
    
    //proposed_quality
    if(msg[offset])
    {
        asso->info.quality = msg[offset + 1];
        offset += 2;
    }
    else
    {
        offset += 1;
    }
    
    //proposed_version
    asso->info.version = msg[offset];
    
    offset += 1;
    
    //conformance_block
    for(cnt=0; cnt<3; cnt++)
    {
        if((asso->sap.conformance[cnt] & msg[offset+4+cnt]) != asso->sap.conformance[cnt])
        {
            heap.set(asso->sap.conformance, 0, sizeof(asso->sap.conformance));
        }
    }
    
    //client_max_pdu
    asso->info.max_pdu = msg[offset + 7];
    asso->info.max_pdu <<= 8;
    asso->info.max_pdu += msg[offset + 8];
    
    if(asso->info.max_pdu > DLMS_CONFIG_MAX_BLOCK_SIZE)
    {
        asso->info.max_pdu = DLMS_CONFIG_MAX_BLOCK_SIZE;
    }
    
    if(output)
    {
        heap.free(output);
    }
}

/**	
  * @brief 
  */
static void dlms_asso_none(struct __dlms_association *asso,
                           const struct __aarq_request *request,
                           uint8_t *buffer,
                           uint16_t buffer_length,
                           uint16_t *filled_length)
{
    uint8_t version = 0;
    
    *(buffer + 0) = (uint8_t)AARE;
    *(buffer + 1) = 0x00;
    
    *filled_length = 2;
    
    //protocol-version
    if(request->protocol_version)
    {
        if((request->protocol_version[1] == 0x02)&&(request->protocol_version[2] == 0x02)&&(request->protocol_version[3] == 0x84))
        {
            *(buffer + *filled_length + 0) = 0x80;
            *(buffer + *filled_length + 1) = 2;
            *(buffer + *filled_length + 2) = 7;
            *(buffer + *filled_length + 3) = 0x80;
            
            *filled_length += 4;
        }
        else
        {
            asso->diagnose = FAILURE_NO_REASION;
            version = 0xff;
        }
    }
    
    //application-context-name
    //LN referencing, with no ciphering: 2, 16, 756, 5, 8, 1, 1
    //LN referencing, with ciphering: 2, 16, 756, 5, 8, 1, 3
    *(buffer + *filled_length + 0) = 0xA1;
    *(buffer + *filled_length + 1) = 0x09;
    *(buffer + *filled_length + 2) = 0x06;
    *(buffer + *filled_length + 3) = 0x07;
    heap.copy((buffer + *filled_length + 4), "\x60\x85\x74\x05\x08\x01", 6);
    *(buffer + *filled_length + 10) = 0x01;
    if(asso->appl_name.id != 1)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    if(!request->application_context_name)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else if(memcmp(&request->application_context_name[4], (buffer + *filled_length + 4), 7) != 0)
    {
        encode_object_identifier((buffer + *filled_length + 4), &asso->appl_name);
        asso->diagnose = FAILURE_NO_REASION;
    }
    else
    {
        if(asso->info.dedkey[0])
        {
            asso->diagnose = FAILURE_CONTEXT_NAME;
        }
        else
        {
            asso->status = ASSOCIATED;
        }
    }
    
    *filled_length += 11;
    
    if(asso->info.version < 6)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    if(asso->info.max_pdu <= 0x0b)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    //association-result
    *(buffer + *filled_length + 0) = 0xA2;
    *(buffer + *filled_length + 1) = 0x03;
    *(buffer + *filled_length + 2) = 0x02;
    *(buffer + *filled_length + 3) = 0x01;
    if(asso->status == ASSOCIATED)
    {
        *(buffer + *filled_length + 4) = 0;
    }
    else
    {
        *(buffer + *filled_length + 4) = 1;
    }
    
    *filled_length += 5;
    
    //associate-source-diagnostic
    *(buffer + *filled_length + 0) = 0xA3;
    *(buffer + *filled_length + 1) = 0x05;
    if(version)
    {
        asso->diagnose = FAILURE_NO_REASION;
    }
    if((asso->diagnose == FAILURE_NO_REASION) && (version))
    {
        *(buffer + *filled_length + 2) = 0xA2;
    }
    else
    {
        *(buffer + *filled_length + 2) = 0xA1;
    }
    *(buffer + *filled_length + 3) = 0x03;
    *(buffer + *filled_length + 4) = 0x02;
    *(buffer + *filled_length + 5) = 0x01;
    *(buffer + *filled_length + 6) = (uint8_t)asso->diagnose;
    
    *filled_length += 7;
    
    //user-information
    *(buffer + *filled_length + 0) = 0xBE;
    *(buffer + *filled_length + 1) = 0x06;
    *(buffer + *filled_length + 2) = 0x04;
    *(buffer + *filled_length + 3) = 0x04;
    *(buffer + *filled_length + 4) = 0x0E;
    *(buffer + *filled_length + 5) = 0x01;
    
    //negotiated-dlms-version-number
    *(buffer + *filled_length + 6) = 0x06;
    
    if(asso->info.version < 6)
    {
        *(buffer + *filled_length + 7) = 0x01;
        *filled_length += 8;
    }
    else if((asso->sap.conformance[0] == 0) && (asso->sap.conformance[1] == 0) && (asso->sap.conformance[2] == 0))
    {
        *(buffer + *filled_length + 7) = 0x02;
        *filled_length += 8;
    }
    else if(asso->info.max_pdu <= 0x0b)
    {
        *(buffer + *filled_length + 7) = 0x03;
        *filled_length += 8;
    }
    else if(asso->diagnose == FAILURE_CONTEXT_NAME)
    {
        *(buffer + *filled_length + 7) = 0x00;
        *filled_length += 8;
        return;
    }
    else if(asso->info.sc)
    {
        *(buffer + *filled_length + 7) = 0x00;
        *filled_length += 8;
    }
    else
    {
        //user-information
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = 0x10;
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = 0x0E;
        
        *(buffer + *filled_length + 4) = 0x08;
        *(buffer + *filled_length + 5) = 0x00;
        //negotiated-dlms-version-number
        *(buffer + *filled_length + 6) = 0x06;
        
        *(buffer + *filled_length + 7) = 0x5F;
        *(buffer + *filled_length + 8) = 0x1F;
        
        //negotiated-conformance
        *(buffer + *filled_length + 9) = 0x04;
        *(buffer + *filled_length + 10) = 0x00;
        //ref "Green_Book_8th_edition" page 265
        *(buffer + *filled_length + 11) = asso->sap.conformance[0];
        *(buffer + *filled_length + 12) = asso->sap.conformance[1];
        *(buffer + *filled_length + 13) = asso->sap.conformance[2];
        
        //server-max-receive-pdu-size
        *(buffer + *filled_length + 14) = (uint8_t)(asso->info.max_pdu >> 8);
        *(buffer + *filled_length + 15) = (uint8_t)(asso->info.max_pdu);
        
        //value=0x0007 for LN and 0xFA00 for SN referencing
        *(buffer + *filled_length + 16) = 0x00;
        *(buffer + *filled_length + 17) = 0x07;
        
        *filled_length += 18;
    }
    
    //添加length
    *(buffer + 1) = *filled_length - 2;
}

/**	
  * @brief 
  */
static void dlms_asso_low(struct __dlms_association *asso,
                          const struct __aarq_request *request,
                          uint8_t *buffer,
                          uint16_t buffer_length,
                          uint16_t *filled_length)
{
    mbedtls_gcm_context ctx;
    int ret;
    uint8_t iv[12];
    uint8_t *input = (void *)0;
    uint8_t input_length;
    uint8_t version = 0;
    
    *(buffer + 0) = (uint8_t)AARE;
    *(buffer + 1) = 0x00;
    
    *filled_length = 2;
    
    //protocol-version
    if(request->protocol_version)
    {
        if((request->protocol_version[1] == 0x02)&&(request->protocol_version[2] == 0x02)&&(request->protocol_version[3] == 0x84))
        {
            *(buffer + *filled_length + 0) = 0x80;
            *(buffer + *filled_length + 1) = 2;
            *(buffer + *filled_length + 2) = 7;
            *(buffer + *filled_length + 3) = 0x80;
            
            *filled_length += 4;
        }
        else
        {
            asso->diagnose = FAILURE_NO_REASION;
            version = 0xff;
        }
    }
    
    //sender_acse_requirements
    //LLS或者HLS，认证标识必须置位
    if(!request->sender_acse_requirements)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else
    {
        if(!(*(request->sender_acse_requirements + 3) & 0x80))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
        }
    }
    
    //calling_authentication_value
    //LLS时为密码
    if(!request->calling_authentication_value)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else
    {
        if(request->calling_authentication_value[1] > (32+2))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
        }
        else
        {
            heap.copy(asso->akey, &request->calling_authentication_value[2], request->calling_authentication_value[1]);
            if(!DLMS_CONFIG_PASSWD_VERIFY(asso->akey))
            {
                //拒绝建立链接
                asso->diagnose = FAILURE_CONTEXT_NAME;
            }
        }
    }
    
    //application-context-name
    //LN referencing, with no ciphering: 2, 16, 756, 5, 8, 1, 1
    //LN referencing, with ciphering: 2, 16, 756, 5, 8, 1, 3
    *(buffer + *filled_length + 0) = 0xA1;
    *(buffer + *filled_length + 1) = 0x09;
    *(buffer + *filled_length + 2) = 0x06;
    *(buffer + *filled_length + 3) = 0x07;
    heap.copy((buffer + *filled_length + 4), "\x60\x85\x74\x05\x08\x01", 6);
    *(buffer + *filled_length + 10) = asso->appl_name.id;
    if((asso->appl_name.id != 1) && (asso->appl_name.id != 3))
    {
        *(buffer + *filled_length + 10) = 0x01;
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    if(!request->application_context_name)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else if(memcmp(&request->application_context_name[4], (buffer + *filled_length + 4), 6) != 0)
    {
        encode_object_identifier((buffer + *filled_length + 4), &asso->appl_name);
        asso->diagnose = FAILURE_NO_REASION;
    }
    else
    {
        if(asso->info.dedkey[0])
        {
            asso->diagnose = FAILURE_CONTEXT_NAME;
        }
        else
        {
            asso->status = ASSOCIATED;
        }
    }
    
    *filled_length += 11;
    
    if(asso->info.version < 6)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    if(asso->info.max_pdu <= 0x0b)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    //association-result
    *(buffer + *filled_length + 0) = 0xA2;
    *(buffer + *filled_length + 1) = 0x03;
    *(buffer + *filled_length + 2) = 0x02;
    *(buffer + *filled_length + 3) = 0x01;
    if(asso->status == ASSOCIATED)
    {
        *(buffer + *filled_length + 4) = 0;
    }
    else
    {
        *(buffer + *filled_length + 4) = 1;
    }
    
    *filled_length += 5;
    
    //associate-source-diagnostic
    *(buffer + *filled_length + 0) = 0xA3;
    *(buffer + *filled_length + 1) = 0x05;
    if(version)
    {
        asso->diagnose = FAILURE_NO_REASION;
    }
    if((asso->diagnose == FAILURE_NO_REASION) && (version))
    {
        *(buffer + *filled_length + 2) = 0xA2;
    }
    else
    {
        *(buffer + *filled_length + 2) = 0xA1;
    }
    *(buffer + *filled_length + 3) = 0x03;
    *(buffer + *filled_length + 4) = 0x02;
    *(buffer + *filled_length + 5) = 0x01;
    *(buffer + *filled_length + 6) = (uint8_t)asso->diagnose;
    
    *filled_length += 7;
    
    //user-information
    input = heap.dalloc(16);
    
    *(input + 0) = 0x0E;
    *(input + 1) = 0x01;
    //negotiated-dlms-version-number
    *(input + 2) = 0x06;
    
    if(asso->info.version < 6)
    {
        *(input + 3) = 0x01;
        input_length = 4;
    }
    else if((asso->sap.conformance[0] == 0) && (asso->sap.conformance[1] == 0) && (asso->sap.conformance[2] == 0))
    {
        *(input + 3) = 0x02;
        input_length = 4;
    }
    else if(asso->info.max_pdu <= 0x0b)
    {
        *(input + 3) = 0x03;
        input_length = 4;
    }
    else if(asso->diagnose == FAILURE_CONTEXT_NAME)
    {
        *(input + 3) = 0x00;
        input_length = 4;
    }
    else
    {
        //user-information
        *(input + 0) = 0x08;
        *(input + 1) = 0x00;
        //negotiated-dlms-version-number
        *(input + 2) = 0x06;
        
        *(input + 3) = 0x5F;
        *(input + 4) = 0x1F;
        
        //negotiated-conformance
        *(input + 5) = 0x04;
        *(input + 6) = 0x00;
        //ref "Green_Book_8th_edition" page 265
        *(input + 7) = asso->sap.conformance[0];
        *(input + 8) = asso->sap.conformance[1];
        *(input + 9) = asso->sap.conformance[2];
        
        //server-max-receive-pdu-size
        *(input + 10) = (uint8_t)(asso->info.max_pdu >> 8);
        *(input + 11) = (uint8_t)(asso->info.max_pdu);
        
        //value=0x0007 for LN and 0xFA00 for SN referencing
        *(input + 12) = 0x00;
        *(input + 13) = 0x07;
        input_length = 14;
    }
    
    if((asso->info.sc & 0xf0) == 0x20)
    {
        //加密 user-information
        dlms_asso_localtitle(iv);
        dlms_asso_fc(&iv[8]);
        
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = input_length + 5 + 2 + 2;
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = input_length + 5 + 2;
        *(buffer + *filled_length + 4) = 0x28;
        *(buffer + *filled_length + 5) = input_length + 5;
        *(buffer + *filled_length + 6) = asso->info.sc;
        heap.copy((buffer + *filled_length + 7), &iv[8], 4);
        
        mbedtls_gcm_init(&ctx);
        
        ret = mbedtls_gcm_setkey(&ctx,
                                 MBEDTLS_CIPHER_ID_AES,
                                 &asso->ekey[2],
                                 asso->ekey[1]*8);
        
        if(ret != 0)
                goto asso_low_enc_faild;
        
        ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                        MBEDTLS_GCM_ENCRYPT,
                                        input_length,
                                        iv,
                                        sizeof(iv),
                                        (void *)0,
                                        0,
                                        input,
                                        (buffer + *filled_length + 8),
                                        0,
                                        (void *)0);
        if(ret != 0)
                goto asso_low_enc_faild;
        
        mbedtls_gcm_free(&ctx);
        
        *filled_length += 11;
        *filled_length += input_length;
        *(buffer + 1) = *filled_length - 2;
    }
    else
    {
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = input_length + 2;
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = input_length;
        heap.copy((buffer + *filled_length + 4), input, input_length);
        *filled_length += 4;
        *filled_length += input_length;
        //添加length
        *(buffer + 1) = *filled_length - 2;
    }
    
    heap.free(input);
    return;
    
asso_low_enc_faild:
    mbedtls_gcm_free(&ctx);
    heap.free(input);
    *(buffer + *filled_length + 0) = 0xBE;
    *(buffer + *filled_length + 1) = 0x06;
    *(buffer + *filled_length + 2) = 0x04;
    *(buffer + *filled_length + 3) = 0x04;
    *(buffer + *filled_length + 4) = 0x0E;
    *(buffer + *filled_length + 5) = 0x01;
    *(buffer + *filled_length + 6) = 0x06;
    *(buffer + *filled_length + 7) = 0x00;
    *filled_length += 8;
    *(buffer + 1) = *filled_length - 2;
}

/**	
  * @brief 
  */
static void dlms_asso_high(struct __dlms_association *asso,
                           const struct __aarq_request *request,
                           uint8_t *buffer,
                           uint16_t buffer_length,
                           uint16_t *filled_length)
{
    mbedtls_gcm_context ctx;
    int ret;
    uint8_t iv[12];
    uint8_t tag[12];
    uint8_t *input = (void *)0;
    uint8_t *add = (void *)0;
    uint8_t input_length;
    uint8_t version = 0;
    
    *(buffer + 0) = (uint8_t)AARE;
    *(buffer + 1) = 0x00;
    
    *filled_length = 2;
    
    //protocol-version
    if(request->protocol_version)
    {
        if((request->protocol_version[1] == 0x02)&&(request->protocol_version[2] == 0x02)&&(request->protocol_version[3] == 0x84))
        {
            *(buffer + *filled_length + 0) = 0x80;
            *(buffer + *filled_length + 1) = 2;
            *(buffer + *filled_length + 2) = 7;
            *(buffer + *filled_length + 3) = 0x80;
            
            *filled_length += 4;
        }
        else
        {
            asso->diagnose = FAILURE_NO_REASION;
            version = 0xff;
        }
    }
    
    //sender_acse_requirements
    //LLS或者HLS，认证标识必须置位
    if(!request->sender_acse_requirements)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else
    {
        if(!(*(request->sender_acse_requirements + 3) & 0x80))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
        }
    }
    
    //calling_authentication_value
    //HLS时为随机数
    if(!request->calling_authentication_value)
    {
        //拒绝建立链接
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else
    {
        if(request->calling_authentication_value[1] > (64+2))
        {
            //拒绝建立链接
            asso->diagnose = FAILURE_CONTEXT_NAME;
        }
        else
        {
            heap.copy(asso->ctos, &request->calling_authentication_value[2], request->calling_authentication_value[1]);
        }
    }
    
    //application-context-name
    //LN referencing, with no ciphering: 2, 16, 756, 5, 8, 1, 1
    //LN referencing, with ciphering: 2, 16, 756, 5, 8, 1, 3
    *(buffer + *filled_length + 0) = 0xA1;
    *(buffer + *filled_length + 1) = 0x09;
    *(buffer + *filled_length + 2) = 0x06;
    *(buffer + *filled_length + 3) = 0x07;
    heap.copy((buffer + *filled_length + 4), "\x60\x85\x74\x05\x08\x01", 6);
    *(buffer + *filled_length + 10) = 0x03;
    if(asso->appl_name.id != 3)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    if(!request->application_context_name)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    else if(memcmp(&request->application_context_name[4], (buffer + *filled_length + 4), 6) != 0)
    {
        encode_object_identifier((buffer + *filled_length + 4), &asso->appl_name);
        asso->diagnose = FAILURE_NO_REASION;
    }
    else
    {
        asso->status = ASSOCIATION_PENDING;
    }
    
    *filled_length += 11;
    
    if(asso->info.version < 6)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    if(asso->info.max_pdu <= 0x0b)
    {
        asso->diagnose = FAILURE_CONTEXT_NAME;
    }
    
    //association-result
    *(buffer + *filled_length + 0) = 0xA2;
    *(buffer + *filled_length + 1) = 0x03;
    *(buffer + *filled_length + 2) = 0x02;
    *(buffer + *filled_length + 3) = 0x01;
    if(asso->status != NON_ASSOCIATED)
    {
        *(buffer + *filled_length + 4) = 0;
    }
    else
    {
        *(buffer + *filled_length + 4) = 1;
    }
    
    *filled_length += 5;
    
    //associate-source-diagnostic
    *(buffer + *filled_length + 0) = 0xA3;
    *(buffer + *filled_length + 1) = 0x05;
    if(version)
    {
        asso->diagnose = FAILURE_NO_REASION;
    }
    if((asso->diagnose == FAILURE_NO_REASION) && (version))
    {
        *(buffer + *filled_length + 2) = 0xA2;
    }
    else
    {
        *(buffer + *filled_length + 2) = 0xA1;
    }
    *(buffer + *filled_length + 3) = 0x03;
    *(buffer + *filled_length + 4) = 0x02;
    *(buffer + *filled_length + 5) = 0x01;
    *(buffer + *filled_length + 6) = (uint8_t)asso->diagnose;
    
    *filled_length += 7;
    
    
    
    //user-information
    input = heap.dalloc(16);
    
    *(input + 0) = 0x0E;
    *(input + 1) = 0x01;
    //negotiated-dlms-version-number
    *(input + 2) = 0x06;
    
    if(asso->info.version < 6)
    {
        *(input + 3) = 0x01;
        input_length = 4;
    }
    else if((asso->sap.conformance[0] == 0) && (asso->sap.conformance[1] == 0) && (asso->sap.conformance[2] == 0))
    {
        *(input + 3) = 0x02;
        input_length = 4;
    }
    else if(asso->info.max_pdu <= 0x0b)
    {
        *(input + 3) = 0x03;
        input_length = 4;
    }
    else if(asso->diagnose == FAILURE_CONTEXT_NAME)
    {
        *(input + 3) = 0x00;
        input_length = 4;
    }
    else
    {
        //user-information
        *(input + 0) = 0x08;
        *(input + 1) = 0x00;
        //negotiated-dlms-version-number
        *(input + 2) = 0x06;
        
        *(input + 3) = 0x5F;
        *(input + 4) = 0x1F;
        
        //negotiated-conformance
        *(input + 5) = 0x04;
        *(input + 6) = 0x00;
        //ref "Green_Book_8th_edition" page 265
        *(input + 7) = asso->sap.conformance[0];
        *(input + 8) = asso->sap.conformance[1];
        *(input + 9) = asso->sap.conformance[2];
        
        //server-max-receive-pdu-size
        *(input + 10) = (uint8_t)(asso->info.max_pdu >> 8);
        *(input + 11) = (uint8_t)(asso->info.max_pdu);
        
        //value=0x0007 for LN and 0xFA00 for SN referencing
        *(input + 12) = 0x00;
        *(input + 13) = 0x07;
        input_length = 14;
    }
    
    if((asso->info.sc & 0xf0) == 0x10)
    {
        //仅认证 user-information
        dlms_asso_localtitle(iv);
        dlms_asso_fc(&iv[8]);
        
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = input_length + 5 + 2 + 2 + sizeof(tag);
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = input_length + 5 + 2 + sizeof(tag);
        *(buffer + *filled_length + 4) = 0x28;
        *(buffer + *filled_length + 5) = input_length + 5 + sizeof(tag);
        *(buffer + *filled_length + 6) = asso->info.sc;
        heap.copy((buffer + *filled_length + 7), &iv[8], 4);
        heap.copy((buffer + *filled_length + 11), input, input_length);
        
        mbedtls_gcm_init(&ctx);
        
        ret = mbedtls_gcm_setkey(&ctx,
                                 MBEDTLS_CIPHER_ID_AES,
                                 &asso->ekey[2],
                                 asso->ekey[1]*8);
        
        if(ret != 0)
                goto asso_high_enc_faild;
        
        add = heap.dalloc(1 + asso->akey[1] + input_length);
        add[0] = asso->info.sc;
        heap.copy(&add[1], &asso->akey[2], asso->akey[1]);
        heap.copy(&add[1+asso->akey[1]], input, input_length);
        
        ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                        MBEDTLS_GCM_ENCRYPT,
                                        0,
                                        iv,
                                        sizeof(iv),
                                        add,
                                        (1 + asso->akey[1] + input_length),
                                        (void *)0,
                                        (void *)0,
                                        sizeof(tag),
                                        tag);
        heap.free(add);
        
        if(ret != 0)
                goto asso_high_enc_faild;
        
        mbedtls_gcm_free(&ctx);
        
        heap.copy((buffer + *filled_length + 11 + input_length), tag, sizeof(tag));
        *filled_length += 11;
        *filled_length += input_length;
        *filled_length += sizeof(tag);
        *(buffer + 1) = *filled_length - 2;
    }
    else if((asso->info.sc & 0xf0) == 0x20)
    {
        //仅加密 user-information
        dlms_asso_localtitle(iv);
        dlms_asso_fc(&iv[8]);
        
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = input_length + 5 + 2 + 2;
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = input_length + 5 + 2;
        *(buffer + *filled_length + 4) = 0x28;
        *(buffer + *filled_length + 5) = input_length + 5;
        *(buffer + *filled_length + 6) = asso->info.sc;
        heap.copy((buffer + *filled_length + 7), &iv[8], 4);
        
        mbedtls_gcm_init(&ctx);
        
        ret = mbedtls_gcm_setkey(&ctx,
                                 MBEDTLS_CIPHER_ID_AES,
                                 &asso->ekey[2],
                                 asso->ekey[1]*8);
        
        if(ret != 0)
                goto asso_high_enc_faild;
        
        ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                        MBEDTLS_GCM_ENCRYPT,
                                        input_length,
                                        iv,
                                        sizeof(iv),
                                        (void *)0,
                                        0,
                                        input,
                                        (buffer + *filled_length + 11),
                                        0,
                                        (void *)0);
        if(ret != 0)
                goto asso_high_enc_faild;
        
        mbedtls_gcm_free(&ctx);
        
        *filled_length += 11;
        *filled_length += input_length;
        *(buffer + 1) = *filled_length - 2;
    }
    else if((asso->info.sc & 0xf0) == 0x30)
    {
        //加密和认证 user-information
        dlms_asso_localtitle(iv);
        dlms_asso_fc(&iv[8]);
        
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = input_length + 5 + 2 + 2 + sizeof(tag);
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = input_length + 5 + 2 + sizeof(tag);
        *(buffer + *filled_length + 4) = 0x28;
        *(buffer + *filled_length + 5) = input_length + 5 + sizeof(tag);
        *(buffer + *filled_length + 6) = asso->info.sc;
        heap.copy((buffer + *filled_length + 7), &iv[8], 4);
        
        mbedtls_gcm_init(&ctx);
        
        ret = mbedtls_gcm_setkey(&ctx,
                                 MBEDTLS_CIPHER_ID_AES,
                                 &asso->ekey[2],
                                 asso->ekey[1]*8);
        
        if(ret != 0)
                goto asso_high_enc_faild;
        
        add = heap.dalloc(1 + asso->akey[1]);
        add[0] = asso->info.sc;
        heap.copy(&add[1], &asso->akey[2], asso->akey[1]);
        
        ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                        MBEDTLS_GCM_ENCRYPT,
                                        input_length,
                                        iv,
                                        sizeof(iv),
                                        add,
                                        (1 + asso->akey[1]),
                                        input,
                                        (buffer + *filled_length + 11),
                                        sizeof(tag),
                                        tag);
        heap.free(add);
        
        if(ret != 0)
                goto asso_high_enc_faild;
        
        mbedtls_gcm_free(&ctx);
        
        heap.copy((buffer + *filled_length + 11 + input_length), tag, sizeof(tag));
        *filled_length += 11;
        *filled_length += input_length;
        *filled_length += sizeof(tag);
        *(buffer + 1) = *filled_length - 2;
    }
    else
    {
        *(buffer + *filled_length + 0) = 0xBE;
        *(buffer + *filled_length + 1) = input_length + 2;
        *(buffer + *filled_length + 2) = 0x04;
        *(buffer + *filled_length + 3) = input_length;
        heap.copy((buffer + *filled_length + 4), input, input_length);
        *filled_length += 4;
        *filled_length += input_length;
        //添加length
        *(buffer + 1) = *filled_length - 2;
    }
    
    heap.free(input);
    return;
    
asso_high_enc_faild:
    mbedtls_gcm_free(&ctx);
    heap.free(input);
    *(buffer + *filled_length + 0) = 0xBE;
    *(buffer + *filled_length + 1) = 0x06;
    *(buffer + *filled_length + 2) = 0x04;
    *(buffer + *filled_length + 3) = 0x04;
    *(buffer + *filled_length + 4) = 0x0E;
    *(buffer + *filled_length + 5) = 0x01;
    *(buffer + *filled_length + 6) = 0x06;
    *(buffer + *filled_length + 7) = 0x00;
    *filled_length += 8;
    *(buffer + 1) = *filled_length - 2;
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
    
    //保存 application-context-name
    //2 16 756 5 8 1 x
    //context_id = 1 : Logical_Name_Referencing_No_Ciphering 只响应不加密报文
    //context_id = 3 : Logical_Name_Referencing_With_Ciphering 响应加密报文和不加密报文
    if(request.application_context_name)
    {
        encode_object_identifier((request.application_context_name + 4), &asso->appl_name);
    }
    
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
        heap.copy(asso->callingtitle, &request.calling_AP_title[2], request.calling_AP_title[1]);
		//加载 local_AP_title
        DLMS_CONFIG_LOAD_TITLE(asso->localtitle);
        //加载 ekay
        DLMS_CONFIG_LOAD_EKEY(asso->ekey);
    }
    
    //保存 user_information
    if(request.user_information)
    {
        parse_user_information(request.user_information, asso);
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
                dlms_asso_none(asso, &request, buffer, buffer_length, filled_length);
            }
            break;
        case 1:
            if(asso->sap.level == DLMS_ACCESS_LOW)
            {
                //低级别认证
                dlms_asso_low(asso, &request, buffer, buffer_length, filled_length);
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            if(asso->sap.level == DLMS_ACCESS_HIGH)
            {
                //高级别认证
                //加载 akey
                DLMS_CONFIG_LOAD_AKEY(asso->akey);
                dlms_asso_high(asso, &request, buffer, buffer_length, filled_length);
            }
            break;
        default:
            break;
    }
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
    DLMS_CONFIG_COSEM_REQUEST(&asso->appl, info, length, buffer, buffer_length, filled_length);
}









/**	
  * @brief 
  */
void dlms_asso_gateway(uint8_t sap,
                       const uint8_t *info,
                       uint16_t length,
                       uint8_t *buffer,
                       uint16_t buffer_length,
                       uint16_t *filled_length)
{
    uint8_t cnt;
    const struct __sap *sap_support = (void *)0;
    uint8_t id = (sap >> 1);
    struct __dlms_association *asso = (void *)0;
    
    //有效性判断
    if((!info) || (!length) || (!buffer) || (!buffer_length) || (!filled_length))
    {
        return;
    }
    
    *filled_length = 0;
    
    //查询SAP是否在支持的SAP列表中
    for(cnt=0; cnt<DLMS_SAP_AMOUNT; cnt++)
    {
        if(sap_support_list[cnt].id == id)
        {
            sap_support = &sap_support_list[cnt];
            break;
        }
    }
    
    //没有查询到支持的SAP
    if(!sap_support)
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
                asso->sap = *sap_support;
                break;
            }
        }
        
        //SAP对象生成失败
        if(!asso)
        {
            return;
        }
        
        //初始化FC
        srand((unsigned int)jiffy.value());
        asso->fc = (uint32_t)rand();
    }
    
    asso_current = asso;
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
    asso_current = (void *)0;
}

/**
  * @brief 清理 Association
  */
void dlms_asso_cleanup(uint8_t sap)
{
    uint8_t cnt;
    uint8_t id = (sap >> 1);
    
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
  * @brief 应用层最大报文传输长度
  */
uint16_t dlms_asso_mtu(void)
{
    return(DLMS_CONFIG_MAX_BLOCK_SIZE);
}

/**
  * @brief 获取 Calling AP Title 8字节
  */
uint8_t dlms_asso_callingtitle(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if(asso_current->callingtitle[1] != 8)
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->callingtitle[2], 8);
    
    return(8);
}

/**
  * @brief 修改 Calling AP Title 8字节
  */
uint8_t dlms_asso_modify_callingtitle(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    asso_current->callingtitle[1] = 8;
    heap.copy(&asso_current->callingtitle[2], buffer, 8);
    
    return(8);
}

/**
  * @brief 获取 Local AP Title 8字节
  */
uint8_t dlms_asso_localtitle(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if(asso_current->localtitle[1] != 8)
    {
        return(0);
    }
    
    heap.copy(buffer, &asso_current->localtitle[2], 8);
    
    return(8);
}

/**
  * @brief 获取 stoc <64字节
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
  * @brief 获取 ctos <64字节
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
  * @brief 获取 akey <32字节
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
  * @brief 获取 ekey <32字节
  */
uint8_t dlms_asso_ekey(uint8_t *buffer)
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
  * @brief 获取 dedkey <32字节
  */
uint8_t dlms_asso_dedkey(uint8_t *buffer)
{
    if(!asso_current)
    {
        return(0);
    }
    
    if((asso_current->info.dedkey[1] > 32) || (asso_current->info.dedkey[1] == 0))
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

/**
  * @brief 获取 object list suit
  */
uint8_t dlms_asso_suit(void)
{
    if(!asso_current)
    {
        return(0);
    }
    
    return(asso_current->sap.suit);
}
