/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "dlms_types.h"
#include "dlms_application.h"
#include "dlms_association.h"
#include "object_template.h"
#include "dlms_lexicon.h"
#include "cosem_objects.h"
#include "axdr.h"
#include "mbedtls/gcm.h"

/* Private define ------------------------------------------------------------*/
#define DLMS_REQ_LIST_MAX   ((uint8_t)8) //一次请求可接受的最大数据项数

/* Private typedef -----------------------------------------------------------*/
/**	
  * @brief 
  */
enum __appl_result
{
    APPL_SUCCESS = 0,
    APPL_DENIED,
    APPL_UNSUPPORT,
    APPL_ENC_FAILD,
    APPL_OTHERS,
};

/**	
  * @brief 解析报文
  */
struct __appl_request
{
    uint8_t service;//enum __dlms_request_type
    uint8_t sc;//加密方式
    uint8_t type;//enum __dlms_get_request_type || enum __dlms_set_request_type || enum __dlms_action_request_type
    uint8_t id;//Invoke-Id-And-Priority
    struct
    {
        uint8_t *classid;//类标识
        uint8_t *obis;//数据项标识
        uint8_t *index;//属性、方法标识
        uint8_t *data;//get时为访问选项，set和action 时为携带数据
        uint8_t *end;//多包传输结束
        uint32_t block;//数据包数
        uint16_t length;//数据长度
        uint8_t active;//当前请求是否激活
        
    } info[DLMS_REQ_LIST_MAX];
};

/**	
  * @brief 数据对象
  */
struct __cosem_entry
{
    uint8_t Actived;
    
    struct
    {
        TypeObject Object;
        ObjectPara Para;
        
    } Entry[DLMS_REQ_LIST_MAX];
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __cosem_entry *current_entry = (void *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief 索引数据报文中有效数据
  * 
  */
static enum __appl_result parse_dlms_frame(const uint8_t *info, uint16_t length, struct __appl_request *request)
{
    uint16_t frame_length = 0;
    uint16_t frame_decoded = 0;
    const uint8_t *start = (const uint8_t *)0;
    uint16_t info_length = 0;
    uint16_t ninfo;
    uint8_t nloop;
    
    if(!request || !info || !length)
    {
        return(APPL_OTHERS);
    }
    
    request->service = info[0];
    frame_decoded = 1;
    
    //第一遍解析，去除加密
    switch(request->service)
    {
        case GLO_GET_REQUEST:
        case GLO_SET_REQUEST:
        case GLO_ACTION_REQUEST:
        case DED_GET_REQUEST:
        case DED_SET_REQUEST:
        case DED_ACTION_REQUEST:
        {
            uint8_t *add = (void *)0;
            uint8_t *input = (void *)0;
            uint8_t iv[12];
            uint8_t ekey[32];
            uint8_t akey[32];
            uint8_t len_ekey;
            uint8_t len_akey;
            mbedtls_gcm_context ctx;
            int ret = 0;
            
            frame_decoded += axdr.length.decode((info + frame_decoded), &frame_length);
            request->sc = info[frame_decoded];
            
            if(length != (frame_length + frame_decoded))
            {
                return(APPL_OTHERS);
            }
            
            if((request->service == DED_GET_REQUEST) || \
                (request->service == DED_SET_REQUEST) || \
                (request->service == DED_ACTION_REQUEST))
            {
                len_ekey = dlms_asso_dedkey(ekey);
            }
            else
            {
                len_ekey = dlms_asso_ekey(ekey);
            }
            
            len_akey = dlms_asso_akey(akey);
            
            dlms_asso_callingtitle(iv);
            heap.copy(&iv[8], &info[3], 4);
            
            mbedtls_gcm_init(&ctx);
            ret = mbedtls_gcm_setkey(&ctx,
                                     MBEDTLS_CIPHER_ID_AES,
                                     ekey,
                                     len_ekey*8);
            
            switch(request->sc & 0xf0)
            {
                case 0x10:
                {
                    //仅认证
                    if(ret)
                    {
                        break;
                    }
                    
                    if(frame_length < (5 + 12))
                    {
                        ret = -1;
                        break;
                    }
                    
                    add = heap.dalloc(frame_length + 1 + len_akey);
                    if(!add)
                    {
                        ret = -1;
                        break;
                    }
                    
                    
                    add[0] = request->sc;
                    heap.copy(&add[1], akey, len_akey);
                    heap.copy(&add[1+len_akey], &info[7], (frame_length - 5 - 12));
                    
                    if(ret)
                    {
                        break;
                    }
                    
                    ret = mbedtls_gcm_auth_decrypt(&ctx,
                                                   0,
                                                   iv,
                                                   12,
                                                   add,
                                                   (1 + len_akey + frame_length - 5 - 12),
                                                   &info[frame_decoded + (frame_length - 12 + 1)],
                                                   12,
                                                   (unsigned char *)0,
                                                   (unsigned char *)0);
                    info_length = frame_length - 5 - 12;
                    
                    break;
                }
                case 0x20:
                {
                    //仅加密
                    if(ret)
                    {
                        break;
                    }
                    
                    if(frame_length < 5)
                    {
                        ret = -1;
                        break;
                    }
                    
                    input = heap.dalloc(frame_length - 5);
                    if(!input)
                    {
                        ret = -1;
                        break;
                    }
                    
                    heap.copy(input, &info[7], (frame_length - 5));
                    
                    ret = mbedtls_gcm_auth_decrypt(&ctx,
                                                   (frame_length - 5),
                                                   iv,
                                                   12,
                                                   (unsigned char *)0,
                                                   0,
                                                   (unsigned char *)0,
                                                   0,
                                                   input,
                                                   (unsigned char *)&info[7]);
                    info_length = frame_length - 5;
                    
                    break;
                }
                case 0x30:
                {
                    //加密加认证
                    if(ret)
                    {
                        break;
                    }
                    
                    if(frame_length < (5 + 12))
                    {
                        ret = -1;
                        break;
                    }
                    
                    add = heap.dalloc(1 + len_akey);
                    if(!add)
                    {
                        ret = -1;
                        break;
                    }
                    
                    add[0] = request->sc;
                    heap.copy(&add[1], akey, len_akey);
                    
                    input = heap.dalloc(frame_length - 5 - 12);
                    if(!input)
                    {
                        ret = -1;
                        break;
                    }
                    
                    heap.copy(input, &info[7], (frame_length - 5 - 12));
                    
                    ret = mbedtls_gcm_auth_decrypt(&ctx,
                                                   (frame_length - 5 - 12),
                                                   iv,
                                                   12,
                                                   add,
                                                   (1 + len_akey),
                                                   &info[frame_length - 12 + 1],
                                                   12,
                                                   input,
                                                   (unsigned char *)&info[7]);
                    info_length = frame_length - 5 - 12;
                    
                    break;
                }
                default:
                {
                    ret = -1;
                }
            }
            
            mbedtls_gcm_free( &ctx );
            
            if(add)
            {
                heap.free(add);
            }
            
            if(input)
            {
                heap.free(input);
            }
            
            if(!ret)
            {
                start = &info[frame_decoded + 5];
            }
            else
            {
                start = (uint8_t *)0;
                info_length = 0;
            }
            
            break;
        }
        case GET_REQUEST:
        case SET_REQUEST:
        case ACTION_REQUEST:
        {
            start = info;
            info_length = length;
            break;
        }
        default:
        {
            start = (const uint8_t *)0;
            info_length = 0;
        }
    }
    
    if(!start || !info_length)
    {
        return(APPL_ENC_FAILD);
    }
    
    request->type = start[1];
    request->id = start[2];
    
    //第二遍解析，索引报文
    switch(start[0])
    {
        case GET_REQUEST:
        {
            switch(request->type)
            {
                case GET_NORMAL:
                {
                    request->info[0].active = 0xff;
                    request->info[0].classid = (uint8_t *)&start[4];
                    request->info[0].obis = (uint8_t *)&start[5];
                    request->info[0].index = (uint8_t *)&start[11];
                    request->info[0].data = (uint8_t *)&start[12];
                    if(info_length < 13)
                    {
                        return(APPL_OTHERS);
                    }
                    else
                    {
                        request->info[0].length = info_length - 12;
                    }
                    break;
                }
                case GET_NEXT:
                {
                    request->info[0].active = 0xff;
                    request->info[0].block = start[3];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[4];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[5];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[6];
                    if(info_length < 7)
                    {
                        return(APPL_OTHERS);
                    }
                    else
                    {
                        request->info[0].length = 0;
                    }
                    break;
                }
                case GET_WITH_LIST:
                {
                    if((!start[3]) || (start[3] > DLMS_REQ_LIST_MAX))
                    {
                        return(APPL_UNSUPPORT);
                    }
                    
                    ninfo = 4;
                    
                    for(nloop=0; nloop<start[3]; nloop ++)
                    {
                        request->info[nloop].active = 0xff;
                        request->info[nloop].classid = (uint8_t *)&start[ninfo + 1];
                        request->info[nloop].obis = (uint8_t *)&start[ninfo + 2];
                        request->info[nloop].index = (uint8_t *)&start[ninfo + 8];
                        request->info[nloop].data = (uint8_t *)&start[ninfo + 9];
                        
                        if(info_length < (ninfo + 9))
                        {
                            return(APPL_OTHERS);
                        }
                        
                        if(*(request->info[nloop].data) != 0)
                        {
                            return(APPL_UNSUPPORT);
                        }
                        
                        request->info[nloop].length = 0;
                        
                        ninfo += 10;
                    }
                    
                    break;
                }
                default:
                {
                    return(APPL_UNSUPPORT);
                }
            }
            break;
        }
        case SET_REQUEST:
        {
            switch(request->type)
            {
                case SET_NORMAL:
                {
                    request->info[0].active = 0xff;
                    request->info[0].classid = (uint8_t *)&start[4];
                    request->info[0].obis = (uint8_t *)&start[5];
                    request->info[0].index = (uint8_t *)&start[11];
                    request->info[0].data = (uint8_t *)&start[13];
                    request->info[0].length = info_length - 13;
                    if(info_length < 14)
                    {
                        return(APPL_OTHERS);
                    }
                    break;
                }
                case SET_FIRST_BLOCK:
                {
                    request->info[0].active = 0xff;
                    request->info[0].classid = (uint8_t *)&start[4];
                    request->info[0].obis = (uint8_t *)&start[5];
                    request->info[0].index = (uint8_t *)&start[11];
                    request->info[0].end = (uint8_t *)&start[13];
                    request->info[0].block = start[14];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[15];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[16];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[17];
                    request->info[0].data = (uint8_t *)&start[18 + axdr.length.decode(&start[18], &request->info[0].length)];
                    if(info_length < 20)
                    {
                        return(APPL_OTHERS);
                    }
                    break;
                }
                case SET_WITH_BLOCK:
                {
                    request->info[0].active = 0xff;
                    request->info[0].end = (uint8_t *)&start[3];
                    request->info[0].block = start[4];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[5];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[6];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[7];
                    request->info[0].data = (uint8_t *)&start[8 + axdr.length.decode(&start[8], &request->info[0].length)];
                    if(info_length < 10)
                    {
                        return(APPL_OTHERS);
                    }
                    break;
                }
                case SET_WITH_LIST:
                case SET_WITH_LIST_AND_FIRST_BLOCK:
                default:
                {
                    return(APPL_UNSUPPORT);
                }
            }
            break;
        }
        case ACTION_REQUEST:
        {
            switch(request->type)
            {
                case ACTION_NORMAL:
                {
                    request->info[0].active = 0xff;
                    request->info[0].classid = (uint8_t *)&start[4];
                    request->info[0].obis = (uint8_t *)&start[5];
                    request->info[0].index = (uint8_t *)&start[11];
                    
                    if(start[12])
                    {
                        request->info[0].data = (uint8_t *)&start[13];
                        request->info[0].length = info_length - 13;
                        if(info_length < 14)
                        {
                            return(APPL_OTHERS);
                        }
                    }
                    else
                    {
                        request->info[0].data = (uint8_t *)0;
                        request->info[0].length = 0;
                        if(info_length < 12)
                        {
                            return(APPL_OTHERS);
                        }
                    }
                    break;
                }
                case ACTION_NEXT_BLOCK:
                {
                    request->info[0].active = 0xff;
                    request->info[0].end = (uint8_t *)&start[3];
                    request->info[0].block = start[4];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[5];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[6];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[7];
                    request->info[0].data = (uint8_t *)&start[8 + axdr.length.decode(&start[8], &request->info[0].length)];
                    if(info_length < 10)
                    {
                        return(APPL_OTHERS);
                    }
                    break;
                }
                case ACTION_FIRST_BLOCK:
                {
                    request->info[0].active = 0xff;
                    request->info[0].classid = (uint8_t *)&start[4];
                    request->info[0].obis = (uint8_t *)&start[5];
                    request->info[0].index = (uint8_t *)&start[11];
                    request->info[0].end = (uint8_t *)&start[12];
                    request->info[0].block = start[13];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[14];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[15];
                    request->info[0].block <<= 8;
                    request->info[0].block += start[16];
                    request->info[0].data = (uint8_t *)&start[17 + axdr.length.decode(&start[17], &request->info[0].length)];
                    if(info_length < 19)
                    {
                        return(APPL_OTHERS);
                    }
                    break;
                }
                case ACTION_WITH_LIST:
                case ACTION_WITH_LIST_AND_FIRST_BLOCK:
                case ACTION_WITH_BLOCK:
                default:
                {
                    return(APPL_UNSUPPORT);
                }
            }
            break;
        }
        default:
        {
            return(APPL_UNSUPPORT);
        }
    }
    
    return(APPL_SUCCESS);
}

/**
  * @brief 生成访问实例
  * 
  */
static enum __appl_result make_cosem_instance(const struct __appl_request *request)
{
    struct __cosem_request_desc desc;
    const char *table;
    uint8_t index;
    uint32_t param;
    union __dlms_right right;
    
    current_entry = (struct __cosem_entry *)dlms_asso_storage();
    if(!current_entry)
    {
        current_entry = (struct __cosem_entry *)dlms_asso_attach_storage(sizeof(struct __cosem_entry));
    }
    
    if(!current_entry)
    {
        return(APPL_OTHERS);
    }
    
    switch(request->service)
    {
        case GET_REQUEST:
        case GLO_GET_REQUEST:
        case DED_GET_REQUEST:
        {
            switch(request->type)
            {
                case GET_NORMAL:
                {
                    desc.request = GET_REQUEST;
                    desc.level = dlms_asso_level();
                    desc.descriptor.classid = request->info[0].classid[0];
                    desc.descriptor.classid <<= 8;
                    desc.descriptor.classid += request->info[0].classid[1];
                    heap.copy(desc.descriptor.obis, request->info[0].obis, 6);
                    desc.descriptor.index = request->info[0].index[0];
                    desc.descriptor.selector = 0;
                    dlms_lex_parse(&desc, &table, &index, &param, &right);
                    
                    if(!table)
                    {
                        return(APPL_UNSUPPORT);
                    }
                    
                    current_entry->Actived = 1;
                    current_entry->Entry[0].Object = cosem_load_object(table, index);
                    
                    break;
                }
                case GET_NEXT:
                {
                    break;
                }
                case GET_WITH_LIST:
                {
                    break;
                }
                default:
                {
                    return(APPL_UNSUPPORT);
                }
            }
            break;
        }
        case SET_REQUEST:
        case GLO_SET_REQUEST:
        case DED_SET_REQUEST:
        {
            switch(request->type)
            {
                case SET_NORMAL:
                {
                    break;
                }
                case SET_FIRST_BLOCK:
                {
                    break;
                }
                case SET_WITH_BLOCK:
                {
                    break;
                }
                case SET_WITH_LIST:
                case SET_WITH_LIST_AND_FIRST_BLOCK:
                default:
                {
                    return(APPL_UNSUPPORT);
                }
            }
            break;
        }
        case ACTION_REQUEST:
        case GLO_ACTION_REQUEST:
        case DED_ACTION_REQUEST:
        {
            switch(request->type)
            {
                case ACTION_NORMAL:
                {
                    break;
                }
                case ACTION_NEXT_BLOCK:
                {
                    break;
                }
                case ACTION_FIRST_BLOCK:
                {
                    break;
                }
                case ACTION_WITH_LIST:
                case ACTION_WITH_LIST_AND_FIRST_BLOCK:
                case ACTION_WITH_BLOCK:
                default:
                {
                    return(APPL_UNSUPPORT);
                }
            }
            break;
        }
        default:
        {
            return(APPL_UNSUPPORT);
        }
    }
    
    return(APPL_SUCCESS);
}

/**	
  * @brief 
  */
static void reply_exception(enum __appl_result result, uint16_t buffer_length, uint16_t *filled_length)
{
    
}


/**	
  * @brief 
  */
void dlms_appl_entrance(const uint8_t *info,
                        uint16_t length,
                        uint8_t *buffer,
                        uint16_t buffer_length,
                        uint16_t *filled_length)
{
    enum __appl_result result;
    struct __appl_request request;
    
    current_entry = (void *)0;
    
    heap.set(&request, 0, sizeof(request));
    
    //解析报文
    result = parse_dlms_frame(info, length, &request);
    
    if(result != APPL_SUCCESS)
    {
        reply_exception(result, buffer_length, filled_length);
        return;
    }
    
    //生成访问对象
    result = make_cosem_instance(&request);
    
    if(!current_entry)
    {
        return;
    }
    
    if(result != APPL_SUCCESS)
    {
        current_entry = (void *)0;
        reply_exception(result, buffer_length, filled_length);
        return;
    }
    
//    //遍历访问所有生成的访问对象
//    for(cnt=0; cnt<entry_amount; cnt++)
//    {
//        current_entry = ((struct __cosem_entry *)(*data)) + cnt;
//        
//        //...调用
//        
//        current_entry = (void *)0;
//    }
    
    current_entry = (void *)0;
    
    
    //...组包返回数据
    
    //...释放 struct __appl_request request;
    
    //...判断对象生命周期是否已经结束
    
}
