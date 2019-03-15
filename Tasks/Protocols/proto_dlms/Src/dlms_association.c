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
//DLMS ���ò���

//DLMS ����ĳ���
#define DLMS_CONFIG_MAX_BLOCK_SIZE              ((uint16_t)(512))

//DLMS ͬʱ���֧�ֵ�ASSOCIATION����
#define DLMS_CONFIG_MAX_ASSO                    ((uint8_t)(6))

//����COSEM��ӿڣ�data, info, length, buffer, max buffer length, filled buffer length��
#define DLMS_CONFIG_COSEM_REQUEST(d,i,l,b,m,f)  dlms_appl_entrance(d,i,l,b,m,f)

/* Private macro -------------------------------------------------------------*/
#define DLMS_SAP_AMOUNT             ((uint8_t)(sizeof(sap_support_list) / sizeof(struct __sap)))

/* Private variables ---------------------------------------------------------*/
/**	
  * @brief ֧�ֵ�SAP�б�
  */
static const struct __sap sap_support_list[] = 
{
    {0x10, DLMS_ACCESS_LOWEST},
    {0x20, DLMS_ACCESS_LOW},
    {0x30, DLMS_ACCESS_HIGH},
};

/**	
  * @brief �Ѿ����������Ӷ���
  */
static struct __dlms_association *asso_list[DLMS_CONFIG_MAX_ASSO] = {0};

/**	
  * @brief ��ǰ���ڴ�������Ӷ���
  */
static struct __dlms_association *asso_current = (void *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief ����ȶ�
  */
static uint8_t password_identified(const uint8_t *info)
{
    //...
    return(0);
}

/**
  * @brief ���� AARQ ����
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
            case 0xBD: //����Ƥ��Ӧ��0x9D,��CTI�������0xBD
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
  * @brief ��� ObjectIdentifier
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
  * @brief ��� UserInformation
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
    //LLS����HLS����֤��ʶ������λ
    if(!request->sender_acse_requirements)
    {
        //�ܾ���������
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(!(*(request->sender_acse_requirements + 3) & 0x80))
        {
            //�ܾ���������
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
    }
    
    //calling_authentication_value
    //LLSʱΪ����
    if(!request->calling_authentication_value)
    {
        //�ܾ���������
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(request->calling_authentication_value[1] > (16+2))
        {
            //�ܾ���������
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
        else
        {
            heap.copy(asso->akey, &request->calling_authentication_value[2], request->calling_authentication_value[1]);
            if(!password_identified(asso->akey))
            {
                //�ܾ���������
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
    //LLS����HLS����֤��ʶ������λ
    if(!request->sender_acse_requirements)
    {
        //�ܾ���������
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(!(*(request->sender_acse_requirements + 3) & 0x80))
        {
            //�ܾ���������
            asso->diagnose = FAILURE_CONTEXT_NAME;
            return;
        }
    }
    
    //calling_authentication_value
    //LLSʱΪ����
    if(!request->calling_authentication_value)
    {
        //�ܾ���������
        asso->diagnose = FAILURE_CONTEXT_NAME;
        return;
    }
    else
    {
        if(request->calling_authentication_value[1] > (32+2))
        {
            //�ܾ���������
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
  * @brief ���� ObjectIdentifier, ��� 7�ֽ�
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
  * @brief ���� UserInformation
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
    
    //application-context-name �� user_information �ǽ��������������
    //��ֻ���� application-context-name �� user_information ����������ƥ��ʱ��������ͼ�������
    if(!request.application_context_name || !request.user_information)
    {
        return;
    }
    
    //���� application-context-name
    //2 16 756 5 8 1 x
    //context_id = 1 : Logical_Name_Referencing_No_Ciphering ֻ��Ӧ�����ܱ���
    //context_id = 3 : Logical_Name_Referencing_With_Ciphering ��Ӧ���ܱ��ĺͲ����ܱ���
    encode_object_identifier((request.application_context_name + 4), &asso->appl_name);

    if(asso->appl_name.id == 3)
    {
        //��Ч���жϣ�context_id Ϊ 3 ʱ�������ṩ calling_AP_title
        if(!request.calling_AP_title)
        {
            return;
        }
        
        if(request.calling_AP_title[1] != 10)
        {
            return;
        }
        
        //���� calling_AP_title
        heap.copy(asso->systitle, &request.calling_AP_title[2], request.calling_AP_title[1]);
    }
    
    //���� mechanism_name
    //2 16 756 5 8 2 x
    if(request.mechanism_name)
    {
        encode_object_identifier((request.mechanism_name + 4), &asso->mech_name);
    }
    
    //ͨ�� mechanism_name ���б�Ҫ����ʲô���������
    //x=0 ��͵ȼ����ӣ�����Ҫ��֤����ʱ mechanism_name ����û��
    //x=1 �͵ȼ����ӣ�ʹ����������֤
    //x=2 �ߵȼ����ӣ�ʹ���Զ�������㷨����֤
    //x=3 �ߵȼ����ӣ�ʹ��MD5�����㷨����֤
    //x=4 �ߵȼ����ӣ�ʹ��SHA-1�����㷨����֤
    //x=5 �ߵȼ����ӣ�ʹ��GMAC�����㷨����֤
    //x=6 �ߵȼ����ӣ�ʹ��SHA-256�����㷨����֤
    //x=7 �ߵȼ����ӣ�ʹ��_ECDSA�����㷨����֤
    //Ŀǰ֧�� 0  1  5
    switch(asso->mech_name.id)
    {
        case 0:
            if(asso->sap.level == DLMS_ACCESS_LOWEST)
            {
                //����֤
                dlms_asso_lowest(asso, &request);
            }
            break;
        case 1:
            if(asso->sap.level == DLMS_ACCESS_LOW)
            {
                //�ͼ�����֤
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
                //�߼�����֤
                dlms_asso_high(asso, &request);
            }
            break;
        case 6:
        case 7:
        default:
            break;
    }
    
    //���ɻظ�����
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
    
    //��Ч���ж�
    if((!info) || (!length) || (!buffer) || (!buffer_length) || (!filled_length))
    {
        if(!sap)
        {
            return;
        }
        
        //��ѯSAP�Ƿ��Ѿ���Э���б���
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
    
    //��ѯSAP�Ƿ���֧�ֵ�SAP�б���
    for(cnt=0; cnt<DLMS_SAP_AMOUNT; cnt++)
    {
        if(sap_support_list[cnt].id == id)
        {
            level = sap_support_list[cnt].level;
            break;
        }
    }
    
    //û�в�ѯ��֧�ֵ�SAP
    if(level == DLMS_ACCESS_NO)
    {
        return;
    }
    
    //��ѯSAP�Ƿ������Э��
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
    
    //SAP��������ɵ�Э���б��У�����һ���µ�SAP����
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
    
    //SAP��������ʧ��
    if(!asso)
    {
        return;
    }
    
    //��������
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
  * @brief Ӧ�ò�����Ĵ��䳤��
  */
uint16_t dlms_asso_mtu(void)
{
    return(DLMS_CONFIG_MAX_BLOCK_SIZE);
}

/**
  * @brief ��ȡ Calling AP Title 8�ֽ�
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
  * @brief ��ȡ stoc <32�ֽ�
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
  * @brief ��ȡ ctos <32�ֽ�
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
  * @brief ���� F(CtoS)
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
  * @brief ��ȡ Security Control Byte 1�ֽ�
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
  * @brief ��ȡ Frame Counter 4 �ֽ�
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
  * @brief ��ȡ akey
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
  * @brief ��ȡ ekey
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
  * @brief ��ȡ dedkey
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
  * @brief ��ȡ Application Context Name
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
  * @brief ��ȡ Authentication Mechanism Name
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
  * @brief ��ȡ xDLMS Context Info
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
  * @brief ��ȡ Association Status
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
  * @brief ��ȡ Access Level
  */
enum __dlms_access_level dlms_asso_level(void)
{
    if(!asso_current)
    {
        return(DLMS_ACCESS_NO);
    }
    
    return(asso_current->sap.level);
}
