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
    APPL_NOMEM,
    APPL_BLOCK_MISS,
    APPL_OBJ_NODEF,
    APPL_OBJ_MISS,
    APPL_OBJ_OVERFLOW,
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
struct __cosem_request
{
    uint8_t Actived; //激活的条目数量
    uint32_t Block; //块计数
    
    struct
    {
        TypeObject Object; //数据对象
        ObjectPara Para; //对象参数
        ObjectErrs Errs; //调用结果
        
    } Entry[DLMS_REQ_LIST_MAX]; //总条目
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __cosem_request *Current = (void *)0;

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
                    if((info_length < 14) || start[12])
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
                    if((info_length < 20) || start[12])
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
    uint8_t cnt = 0;
    uint8_t index = 0;
    uint32_t param = 0;
    const char *table = (const char *)0;
    struct __cosem_request_desc desc;
    union __dlms_right right;
    
    //获取当前链接下附属的对象内存
    Current = (struct __cosem_request *)dlms_asso_storage();
    if(!Current)
    {
        //生成当前链接下附属的对象内存
        Current = (struct __cosem_request *)dlms_asso_attach_storage(sizeof(struct __cosem_request));
        
        if(!Current)
        {
            return(APPL_OTHERS);
        }
    }
    
    desc.request = (enum __dlms_request_type)request->service;
    desc.level = dlms_asso_level();
    
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
                    
                    Current->Block = 0;//块计数清零
                    Current->Actived = 1;//一个请求条目
                    
                    heap.set(&Current->Entry[0], 0, sizeof(Current->Entry[0]));
                    Current->Entry[0].Para.Input.ID = param;//赋值数据标识
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;
                    Current->Entry[0].Para.Input.Size = request->info[0].length;
                    Current->Entry[0].Object = cosem_load_object(table, index);
                    
                    break;
                }
                case GET_NEXT:
                {
                    //不支持多实例
                    if((Current->Actived != 1) || (!Current->Entry[0].Object))
                    {
                        return(APPL_OBJ_OVERFLOW);
                    }
                    
                    //块计数验证
                    if((Current->Block + 1) != request->info->block)
                    {
                        return(APPL_BLOCK_MISS);
                    }
                    
                    //更新块计数
                    Current->Block = request->info->block;
                    
                    Current->Entry[0].Para.Input.Buffer = (uint8_t *)0;
                    Current->Entry[0].Para.Input.Size = 0;
                    
                    break;
                }
                case GET_WITH_LIST:
                {
                    Current->Block = 0;//块计数清零
                    Current->Actived = 0;
                    
                    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
                    {
                        if(!request->info[cnt].active)
                        {
                            break;
                        }
                        
                        desc.descriptor.classid = request->info[cnt].classid[0];
                        desc.descriptor.classid <<= 8;
                        desc.descriptor.classid += request->info[cnt].classid[1];
                        heap.copy(desc.descriptor.obis, request->info[cnt].obis, 6);
                        desc.descriptor.index = request->info[cnt].index[0];
                        desc.descriptor.selector = 0;
                        dlms_lex_parse(&desc, &table, &index, &param, &right);
                        
                        if(!table)
                        {
                            return(APPL_UNSUPPORT);
                        }
                        
                        Current->Actived += 1;//一个请求条目
                        
                        heap.set(&Current->Entry[cnt], 0, sizeof(Current->Entry[cnt]));
                        Current->Entry[cnt].Para.Input.ID = param;//赋值数据标识
                        Current->Entry[cnt].Para.Input.Buffer = request->info[cnt].data;
                        Current->Entry[cnt].Para.Input.Size = request->info[cnt].length;
                        Current->Entry[cnt].Object = cosem_load_object(table, index);
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
        case GLO_SET_REQUEST:
        case DED_SET_REQUEST:
        {
            switch(request->type)
            {
                case SET_NORMAL:
                {
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
                    
                    Current->Block = 0;//块计数清零
                    Current->Actived = 1;//一个请求条目
                    
                    heap.set(&Current->Entry[0], 0, sizeof(Current->Entry[0]));
                    Current->Entry[0].Para.Input.ID = param;//赋值数据标识
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;//赋值数据
                    Current->Entry[0].Para.Input.Size = request->info[0].length;//赋值数据长度
                    Current->Entry[0].Object = cosem_load_object(table, index);
                    break;
                }
                case SET_FIRST_BLOCK:
                {
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
                    
                    if(!request->info->block)
                    {
                        return(APPL_BLOCK_MISS);
                    }
                    
                    Current->Block = request->info->block;//块计数
                    Current->Actived = 1;//一个请求条目
                    
                    heap.set(&Current->Entry[0], 0, sizeof(Current->Entry[0]));
                    Current->Entry[0].Para.Input.ID = param;//赋值数据标识
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;//赋值数据
                    Current->Entry[0].Para.Input.Size = request->info[0].length;//赋值数据长度
                    if((request->info->end) && !(*(request->info->end)))
                    {
                        Current->Entry[0].Para.Iterator.Status = ITER_ONGOING;//赋值迭代标识
                    }
                    else
                    {
                        Current->Entry[0].Para.Iterator.Status = ITER_FINISHED;//赋值迭代标识
                    }
                    Current->Entry[0].Object = cosem_load_object(table, index);
                    break;
                }
                case SET_WITH_BLOCK:
                {
                    //不支持多实例
                    if((Current->Actived != 1) || (!Current->Entry[0].Object))
                    {
                        return(APPL_OBJ_OVERFLOW);
                    }
                    
                    //块计数验证
                    if((Current->Block + 1) != request->info->block)
                    {
                        return(APPL_BLOCK_MISS);
                    }
                    
                    Current->Block = request->info->block;//更新块计数
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;//赋值数据
                    Current->Entry[0].Para.Input.Size = request->info[0].length;//赋值数据长度
                    
                    if(!(request->info->end) || *(request->info->end))
                    {
                        Current->Entry[0].Para.Iterator.Status = ITER_FINISHED;//赋值迭代标识
                    }
                    break;
                }
                case SET_WITH_LIST:
                {
                    Current->Block = 0;//块计数清零
                    Current->Actived = 0;//一个请求条目
                    
                    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
                    {
                        if(!request->info[cnt].active)
                        {
                            break;
                        }
                        
                        desc.descriptor.classid = request->info[cnt].classid[0];
                        desc.descriptor.classid <<= 8;
                        desc.descriptor.classid += request->info[cnt].classid[1];
                        heap.copy(desc.descriptor.obis, request->info[cnt].obis, 6);
                        desc.descriptor.index = request->info[cnt].index[0];
                        desc.descriptor.selector = 0;
                        dlms_lex_parse(&desc, &table, &index, &param, &right);
                        
                        if(!table)
                        {
                            return(APPL_UNSUPPORT);
                        }
                        
                        Current->Actived += 1;//一个请求条目
                        
                        heap.set(&Current->Entry[cnt], 0, sizeof(Current->Entry[cnt]));
                        Current->Entry[cnt].Para.Input.ID = param;//赋值数据标识
                        Current->Entry[cnt].Para.Input.Buffer = request->info[cnt].data;//赋值数据
                        Current->Entry[cnt].Para.Input.Size = request->info[cnt].length;//赋值数据长度
                        Current->Entry[cnt].Object = cosem_load_object(table, index);
                    }
                    break;
                }
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
                    
                    Current->Block = 0;//块计数清零
                    Current->Actived = 1;//一个请求条目
                    
                    heap.set(&Current->Entry[0], 0, sizeof(Current->Entry[0]));
                    Current->Entry[0].Para.Input.ID = param;//赋值数据标识
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;//赋值数据
                    Current->Entry[0].Para.Input.Size = request->info[0].length;//赋值数据长度
                    Current->Entry[0].Object = cosem_load_object(table, index);
                    break;
                }
                case ACTION_NEXT_BLOCK:
                {
                    //不支持多实例
                    if((Current->Actived != 1) || (!Current->Entry[0].Object))
                    {
                        return(APPL_OBJ_OVERFLOW);
                    }
                    
                    //块计数验证
                    if((Current->Block + 1) != request->info->block)
                    {
                        return(APPL_BLOCK_MISS);
                    }
                    
                    Current->Block = request->info->block;//更新块计数
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;//赋值数据
                    Current->Entry[0].Para.Input.Size = request->info[0].length;//赋值数据长度
                    
                    if(!(request->info->end) || *(request->info->end))
                    {
                        Current->Entry[0].Para.Iterator.Status = ITER_FINISHED;//赋值迭代标识
                    }
                    break;
                }
                case ACTION_FIRST_BLOCK:
                {
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
                    
                    if(!request->info->block)
                    {
                        return(APPL_BLOCK_MISS);
                    }
                    
                    Current->Block = request->info->block;//块计数
                    Current->Actived = 1;//一个请求条目
                    
                    heap.set(&Current->Entry[0], 0, sizeof(Current->Entry[0]));
                    Current->Entry[0].Para.Input.ID = param;//赋值数据标识
                    Current->Entry[0].Para.Input.Buffer = request->info[0].data;//赋值数据
                    Current->Entry[0].Para.Input.Size = request->info[0].length;//赋值数据长度
                    if((request->info->end) && !(*(request->info->end)))
                    {
                        Current->Entry[0].Para.Iterator.Status = ITER_ONGOING;//赋值迭代标识
                    }
                    else
                    {
                        Current->Entry[0].Para.Iterator.Status = ITER_FINISHED;//赋值迭代标识
                    }
                    Current->Entry[0].Object = cosem_load_object(table, index);
                    break;
                }
                case ACTION_WITH_LIST:
                {
                    Current->Block = 0;//块计数清零
                    Current->Actived = 0;//一个请求条目
                    
                    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
                    {
                        if(!request->info[cnt].active)
                        {
                            break;
                        }
                        
                        desc.descriptor.classid = request->info[cnt].classid[0];
                        desc.descriptor.classid <<= 8;
                        desc.descriptor.classid += request->info[cnt].classid[1];
                        heap.copy(desc.descriptor.obis, request->info[cnt].obis, 6);
                        desc.descriptor.index = request->info[cnt].index[0];
                        desc.descriptor.selector = 0;
                        dlms_lex_parse(&desc, &table, &index, &param, &right);
                        
                        if(!table)
                        {
                            return(APPL_UNSUPPORT);
                        }
                        
                        Current->Actived += 1;//一个请求条目
                        
                        heap.set(&Current->Entry[cnt], 0, sizeof(Current->Entry[cnt]));
                        Current->Entry[cnt].Para.Input.ID = param;//赋值数据标识
                        Current->Entry[cnt].Para.Input.Buffer = request->info[cnt].data;//赋值数据
                        Current->Entry[cnt].Para.Input.Size = request->info[cnt].length;//赋值数据长度
                        Current->Entry[cnt].Object = cosem_load_object(table, index);
                    }
                    break;
                }
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
static enum __appl_result reply_normal(struct __appl_request *request, uint8_t *buffer, uint16_t buffer_length, uint16_t *filled_length)
{
    uint8_t cnt = 0;
    uint8_t *plain = (uint8_t *)0;
    uint16_t plain_length = 0;
    
    uint8_t akey[32] = {0};
    uint8_t akey_length = 0;
    uint8_t ekey[32] = {0};
    uint8_t ekey_length = 0;
    uint8_t iv[12] = {0};
    uint16_t cipher_length = 0;
    uint8_t tag[12] = {0};
    uint8_t *add = (uint8_t *)0;
    
    mbedtls_gcm_context ctx;
    int ret;
    
    //step 1
    //获取临时缓冲，用于组包未加密报文
    plain = heap.dalloc(dlms_asso_mtu() + 16);
    
    if(!plain)
    {
        return(APPL_NOMEM);
    }
    
    //step 2
    //组包返回的报文（明文）
    switch(request->service)
    {
        case GET_REQUEST:
        case GLO_GET_REQUEST:
        case DED_GET_REQUEST:
        {
            plain[0] = GET_RESPONSE;
            plain[2] = request->id;
            
            plain_length = 3;
            
            if(Current->Actived > 1) //Get-Response-With-List
            {
                plain[1] = GET_RESPONSE_WITH_LIST;
                
                for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
                {
                    if(!Current->Entry[cnt].Object)
                    {
                        continue;
                    }
                    
                    //Get-Response-With-List 不允许 Get-Response-With-Datablock
                    if((Current->Entry[cnt].Para.Iterator.Status != ITER_NONE) || \
                        (Current->Entry[cnt].Para.Iterator.Begin != Current->Entry[cnt].Para.Iterator.End))
                    {
                        Current->Entry[cnt].Para.Iterator.Status = ITER_NONE;
                        Current->Entry[cnt].Errs = OBJECT_ERR_MEM;
                    }
                    
                    if(Current->Entry[cnt].Errs == OBJECT_NOERR)
                    {
                        if((Current->Entry[cnt].Para.Output.Filled > Current->Entry[cnt].Para.Output.Size) || \
                            ((Current->Entry[cnt].Para.Output.Filled + plain_length) > (dlms_asso_mtu() - 20)))
                        {
                            plain[1] = GET_RESPONSE_NORMAL;
                            plain[3] = 1;
                            plain[4] = (uint8_t)OBJECT_ERR_MEM;
                            plain_length = 5;
                            break;
                        }
                        else
                        {
                            plain[plain_length + 0] = 0;
                            plain_length += 1;
                            plain_length += heap.copy(&plain[plain_length], Current->Entry[cnt].Para.Output.Buffer, Current->Entry[cnt].Para.Output.Filled);
                        }
                    }
                    else
                    {
                        plain[plain_length + 0] = 1;
                        plain[plain_length + 1] = (uint8_t)Current->Entry[cnt].Errs;
                        plain_length += 2;
                    }
                }
            }
            else
            {
                if(!Current->Entry[0].Object) //Get-Response-Normal (Error)
                {
                    plain[1] = GET_RESPONSE_NORMAL;
                    plain[3] = 1;
                    plain[4] = (uint8_t)OBJECT_ERR_MEM;
                    plain_length = 5;
                }
                else if(Current->Entry[0].Para.Iterator.Status != ITER_NONE) //Get-Response-With-Datablock
                {
                    if((Current->Entry[0].Para.Output.Filled > Current->Entry[0].Para.Output.Size) || \
                        (Current->Entry[0].Para.Output.Filled > (dlms_asso_mtu() - 20)))
                    {
                        plain[1] = GET_RESPONSE_NORMAL;
                        plain[3] = 1;
                        plain[4] = (uint8_t)OBJECT_ERR_MEM;
                        plain_length = 5;
                        
                        Current->Entry[0].Para.Iterator.Status = ITER_NONE;
                    }
                    else
                    {
                        plain[1] = GET_RESPONSE_WITH_BLOCK;
                        
                        if((Current->Entry[0].Para.Iterator.Status == ITER_FINISHED) || \
                            (Current->Entry[0].Para.Iterator.Begin == Current->Entry[0].Para.Iterator.End))
                        {
                            plain[3] = (uint8_t)IS_LAST_BLOCK;
                        }
                        else
                        {
                            plain[3] = (uint8_t)NOT_LAST_BLOCK;
                        }
                        
                        plain[4] = (uint8_t)(Current->Block >> 24);
                        plain[5] = (uint8_t)(Current->Block >> 16);
                        plain[6] = (uint8_t)(Current->Block >> 8);
                        plain[7] = (uint8_t)(Current->Block >> 0);
                        
                        plain[8] = 0;
                        
                        plain_length = 9;
                        
                        plain_length += axdr.length.encode(Current->Entry[0].Para.Output.Filled, &plain[9]);
                        plain_length += heap.copy(&plain[plain_length], Current->Entry[0].Para.Output.Buffer, Current->Entry[0].Para.Output.Filled);
                    }
                }
                else //Get-Response-Normal
                {
                    plain[1] = GET_RESPONSE_NORMAL;
                    if(Current->Entry[0].Errs == OBJECT_NOERR)
                    {
                        if((Current->Entry[0].Para.Output.Filled > Current->Entry[0].Para.Output.Size) || \
                            (Current->Entry[0].Para.Output.Filled > (dlms_asso_mtu() - 20)))
                        {
                            plain[3] = 1;
                            plain[4] = (uint8_t)OBJECT_ERR_MEM;
                            plain_length = 5;
                        }
                        else
                        {
                            plain[3] = 0;
                            plain_length = 4;
                            plain_length += heap.copy(&plain[4], Current->Entry[0].Para.Output.Buffer, Current->Entry[0].Para.Output.Filled);
                        }
                    }
                    else
                    {
                        plain[3] = 1;
                        plain[4] = (uint8_t)Current->Entry[0].Errs;
                        plain_length = 5;
                    }
                }
            }
            
            break;
        }
        case SET_REQUEST:
        case GLO_SET_REQUEST:
        case DED_SET_REQUEST:
        {
            plain[0] = SET_RESPONSE;
            plain[2] = request->id;
            
            switch(request->type)
            {
                case SET_NORMAL:
                {
                    plain[1] = SET_RESPONSE_NORMAL;
                    
                    if(!Current->Entry[0].Object || Current->Actived != 1)
                    {
                        plain[3] = (uint8_t)OBJECT_ERR_MEM;
                        plain_length = 4;
                    }
                    else
                    {
                        if((Current->Entry[0].Para.Iterator.Status != ITER_NONE) || \
                            (Current->Entry[0].Para.Iterator.Begin != Current->Entry[0].Para.Iterator.End))
                        {
                            Current->Entry[0].Para.Iterator.Status = ITER_NONE;
                            Current->Entry[0].Errs = OBJECT_ERR_MEM;
                        }
                        
                        if(Current->Entry[cnt].Errs == OBJECT_NOERR)
                        {
                            plain[3] = 0;
                            plain_length = 4;
                        }
                        else
                        {
                            plain[3] = (uint8_t)Current->Entry[cnt].Errs;
                            plain_length = 4;
                        }
                    }
                    break;
                }
                case SET_FIRST_BLOCK:
                case SET_WITH_BLOCK:
                {
                    if(!Current->Entry[0].Object || Current->Actived != 1)
                    {
                        plain[1] = SET_RESPONSE_NORMAL;
                        plain[3] = (uint8_t)OBJECT_ERR_MEM;
                        plain_length = 4;
                    }
                    else
                    {
                        if((Current->Entry[0].Para.Iterator.Status != ITER_ONGOING) || \
                            (Current->Entry[0].Errs != OBJECT_NOERR))
                        {
                            plain[1] = SET_RESPONSE_LAST_DATABLOCK;
                            plain[3] = (uint8_t)Current->Entry[0].Errs;
                            plain[4] = Current->Block >> 24;
                            plain[5] = Current->Block >> 16;
                            plain[6] = Current->Block >> 8;
                            plain[7] = Current->Block >> 0;
                            
                            plain_length = 8;
                        }
                        else
                        {
                            plain[1] = SET_RESPONSE_DATABLOCK;
                            plain[3] = Current->Block >> 24;
                            plain[4] = Current->Block >> 16;
                            plain[5] = Current->Block >> 8;
                            plain[6] = Current->Block >> 0;
                            
                            plain_length = 7;
                        }
                    }
                    break;
                }
                case SET_WITH_LIST:
                case SET_WITH_LIST_AND_FIRST_BLOCK:
                default:
                {
                    heap.free(plain);
                    return(APPL_UNSUPPORT);
                }
            }
        }
        case ACTION_REQUEST:
        case GLO_ACTION_REQUEST:
        case DED_ACTION_REQUEST:
        {
            plain[0] = ACTION_RESPONSE;
            plain[2] = request->id;
            
            switch(request->type)
            {
                case ACTION_NORMAL:
                {
                    plain[1] = ACTION_RESPONSE_NORMAL;
                    
                    if(!Current->Entry[0].Object || Current->Actived != 1)
                    {
                        plain[3] = (uint8_t)OBJECT_ERR_MEM;
                        plain[4] = 0;
                        plain_length = 5;
                    }
                    else
                    {
                        if((Current->Entry[0].Para.Iterator.Status != ITER_NONE) || \
                            (Current->Entry[0].Para.Iterator.Begin != Current->Entry[0].Para.Iterator.End))
                        {
                            Current->Entry[0].Para.Iterator.Status = ITER_NONE;
                            Current->Entry[0].Errs = OBJECT_ERR_MEM;
                        }
                        
                        if(Current->Entry[0].Errs == OBJECT_NOERR)
                        {
                            if((Current->Entry[0].Para.Output.Filled > Current->Entry[0].Para.Output.Size) || \
                                ((Current->Entry[0].Para.Output.Filled + plain_length) > (dlms_asso_mtu() - 20)))
                            {
                                plain[3] = (uint8_t)OBJECT_ERR_MEM;
                                plain[4] = 0;
                                plain_length = 5;
                            }
                            else
                            {
                                plain[3] = 0;
                                plain_length = 4;
                                plain_length += heap.copy(&plain[4], Current->Entry[0].Para.Output.Buffer, Current->Entry[0].Para.Output.Filled);
                            }
                        }
                        else
                        {
                            plain[3] = (uint8_t)OBJECT_ERR_MEM;
                            plain[4] = 0;
                            plain_length = 5;
                        }
                    }
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
                    heap.free(plain);
                    return(APPL_UNSUPPORT);
                }
            }
        }
        default:
        {
            heap.free(plain);
            return(APPL_UNSUPPORT);
        }
    }
    
    //step 3
    //判断是否需要加密报文
    switch(request->service)
    {
        case GLO_GET_REQUEST:
        case GLO_SET_REQUEST:
        case GLO_ACTION_REQUEST:
        {
            ekey_length = dlms_asso_ekey(ekey);
            akey_length = dlms_asso_akey(akey);
            dlms_asso_callingtitle(iv);
            dlms_asso_fc(&iv[8]);
            break;
        }
        case DED_GET_REQUEST:
        case DED_SET_REQUEST:
        case DED_ACTION_REQUEST:
        {
            ekey_length = dlms_asso_dedkey(ekey);
            akey_length = dlms_asso_akey(akey);
            dlms_asso_callingtitle(iv);
            dlms_asso_fc(&iv[8]);
            break;
        }
        default:
        {
            //不需要加密，明文返回
            if(plain_length > buffer_length)
            {
                plain_length = buffer_length;
            }
            
            *filled_length = heap.copy(buffer, plain, plain_length);
            
            heap.free(plain);
            return(APPL_SUCCESS);
        }
    }
    
    //step 4
    //加密
    switch(request->service)
    {
        case GLO_GET_REQUEST:
        case GLO_SET_REQUEST:
        case GLO_ACTION_REQUEST:
        case DED_GET_REQUEST:
        case DED_SET_REQUEST:
        case DED_ACTION_REQUEST:
        {
            buffer[0] = (request->service + 8);
            cipher_length = 1;
            
            if((request->sc & 0xf0) == 0x10)
            {
                cipher_length += axdr.length.encode((5+plain_length+12), &buffer[cipher_length]);
                
                if(buffer_length < (cipher_length+5+plain_length+12))
                {
                    goto enc_faild;
                }
                
                buffer[cipher_length] = request->sc;
                cipher_length += 1;
                cipher_length += heap.copy(&buffer[cipher_length], &iv[8], 4);
                
                mbedtls_gcm_init(&ctx);
                
                ret = mbedtls_gcm_setkey(&ctx,
                                         MBEDTLS_CIPHER_ID_AES,
                                         ekey,
                                         ekey_length*8);
                
                if(ret != 0)
                {
                    goto enc_faild;
                }
                
                add = heap.dalloc(1 + akey_length + plain_length);
                if(!add)
                {
                    goto enc_faild;
                }
                
                add[0] = request->sc;
                heap.copy(&add[1], akey, akey_length);
                heap.copy(&add[1+akey_length], plain, plain_length);
                
                ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                                MBEDTLS_GCM_ENCRYPT,
                                                0,
                                                iv,
                                                sizeof(iv),
                                                add,
                                                (1 + akey_length + plain_length),
                                                (void *)0,
                                                (void *)0,
                                                sizeof(tag),
                                                tag);
                heap.free(add);
                
                if(ret != 0)
                {
                    goto enc_faild;
                }
                
                mbedtls_gcm_free(&ctx);
                
                cipher_length += heap.copy(&buffer[cipher_length], plain, plain_length);
                cipher_length += heap.copy(&buffer[cipher_length], tag, sizeof(tag));
                *filled_length = cipher_length;
            }
            else if((request->sc & 0xf0) == 0x20)
            {
                cipher_length += axdr.length.encode((5+plain_length), &buffer[cipher_length]);
                
                if(buffer_length < (cipher_length+5+plain_length))
                {
                    goto enc_faild;
                }
                
                buffer[cipher_length] = request->sc;
                cipher_length += 1;
                cipher_length += heap.copy(&buffer[cipher_length], &iv[8], 4);
                
                mbedtls_gcm_init(&ctx);
                
                ret = mbedtls_gcm_setkey(&ctx,
                                         MBEDTLS_CIPHER_ID_AES,
                                         ekey,
                                         ekey_length*8);
                
                if(ret != 0)
                {
                    goto enc_faild;
                }
                
                ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                                MBEDTLS_GCM_ENCRYPT,
                                                plain_length,
                                                iv,
                                                sizeof(iv),
                                                (void *)0,
                                                0,
                                                plain,
                                                (buffer + cipher_length),
                                                0,
                                                (void *)0);
                if(ret != 0)
                {
                    goto enc_faild;
                }
                
                mbedtls_gcm_free(&ctx);
                
                *filled_length = cipher_length + plain_length;
            }
            else if((request->sc & 0xf0) == 0x30)
            {
                cipher_length += axdr.length.encode((5+plain_length+12), &buffer[cipher_length]);
                
                if(buffer_length < (cipher_length+5+plain_length+12))
                {
                    goto enc_faild;
                }
                
                buffer[cipher_length] = request->sc;
                cipher_length += 1;
                cipher_length += heap.copy(&buffer[cipher_length], &iv[8], 4);
                
                mbedtls_gcm_init(&ctx);
                
                ret = mbedtls_gcm_setkey(&ctx,
                                         MBEDTLS_CIPHER_ID_AES,
                                         ekey,
                                         ekey_length*8);
                
                if(ret != 0)
                {
                    goto enc_faild;
                }
                
                add = heap.dalloc(1 + akey_length);
                if(!add)
                {
                    goto enc_faild;
                }
                
                add[0] = request->sc;
                heap.copy(&add[1], akey, akey_length);
                
                ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                                MBEDTLS_GCM_ENCRYPT,
                                                plain_length,
                                                iv,
                                                sizeof(iv),
                                                add,
                                                (1 + akey_length),
                                                plain,
                                                (buffer + cipher_length),
                                                sizeof(tag),
                                                tag);
                heap.free(add);
                
                if(ret != 0)
                {
                    goto enc_faild;
                }
                
                mbedtls_gcm_free(&ctx);
                
                cipher_length += plain_length;
                cipher_length += heap.copy((buffer + cipher_length), tag, sizeof(tag));
                
                *filled_length = cipher_length;
            }
            else
            {
                goto enc_faild;
            }
            
            break;
        }
        default:
        {
            goto enc_faild;
            break;
        }
    }
    
    heap.free(plain);
    return(APPL_SUCCESS);
enc_faild:
    heap.free(plain);
    return(APPL_ENC_FAILD);
}

/**	
  * @brief 
  */
static void reply_exception(enum __appl_result result, uint8_t *buffer, uint16_t buffer_length, uint16_t *filled_length)
{
    buffer[0] = EXCEPTION_RESPONSE;
    
    switch(result)
    {
        case APPL_DENIED:
        {
            buffer[1] = 1;
            buffer[2] = 1;
            break;
        }
        case APPL_NOMEM:
        {
            buffer[1] = 1;
            buffer[2] = 3;
            break;
        }
        case APPL_BLOCK_MISS:
        {
            buffer[1] = 1;
            buffer[2] = 1;
            break;
        }
        case APPL_OBJ_NODEF:
        {
            buffer[1] = 1;
            buffer[2] = 1;
            break;
        }
        case APPL_OBJ_MISS:
        {
            buffer[1] = 1;
            buffer[2] = 3;
            break;
        }
        case APPL_OBJ_OVERFLOW:
        {
            buffer[1] = 1;
            buffer[2] = 3;
            break;
        }
        case APPL_UNSUPPORT:
        {
            buffer[1] = 2;
            buffer[2] = 2;
            break;
        }
        case APPL_ENC_FAILD:
        {
            buffer[1] = 1;
            buffer[2] = 3;
            break;
        }
        case APPL_OTHERS:
        {
            buffer[1] = 2;
            buffer[2] = 3;
            break;
        }
        default:
        {
            buffer[1] = 2;
            buffer[2] = 3;
            break;
        }
    }
    
    *filled_length = 3;
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
    uint8_t cnt = 0;
    uint8_t alive = 0;
    uint8_t *cosem_data = (uint8_t *)0;
    enum __appl_result result;
    struct __appl_request request;
    
    Current = (void *)0;
    
    heap.set(&request, 0, sizeof(request));
    
    //step 1
    //解析报文
    //去除加密，并且索引报文中有效字段
    result = parse_dlms_frame(info, length, &request);
    
    //解析报文失败
    if(result != APPL_SUCCESS)
    {
        reply_exception(result, buffer, buffer_length, filled_length);
        return;
    }
    
    //step 2
    //生成访问对象
    result = make_cosem_instance(&request);
    
    //生成访问对象失败
    if((result != APPL_SUCCESS) || (!Current))
    {
        //清零访问对象
        if(Current)
        {
            heap.set(Current, 0, sizeof(struct __cosem_request));
            Current = (void *)0;
        }
        
        reply_exception(result, buffer, buffer_length, filled_length);
        
        return;
    }
    
    //检查生成的访问对象是否合法
    if((Current->Actived == 0) || (Current->Actived > DLMS_REQ_LIST_MAX))
    {
        reply_exception(APPL_OBJ_OVERFLOW, buffer, buffer_length, filled_length);
        Current = (void *)0;
        return;
    }
    else
    {
        for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
        {
            if(Current->Entry[cnt].Object)
            {
                alive += 1;
            }
        }
        
        if(Current->Actived != alive)
        {
            reply_exception(APPL_OBJ_OVERFLOW, buffer, buffer_length, filled_length);
            Current = (void *)0;
            return;
        }
    }
    
    //step 3
    //获取访问对象对应的输出缓冲
    cosem_data = heap.dalloc((dlms_asso_mtu() + Current->Actived * 16));
    if(!cosem_data)
    {
        //清零链接内存
        heap.set(Current, 0, sizeof(struct __cosem_request));
        reply_exception(APPL_NOMEM, buffer, buffer_length, filled_length);
        Current = (void *)0;
        return;
    }
    
    //均分内存给访问对象内的有效条目
    alive = 0;
    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
    {
        if(Current->Entry[cnt].Object)
        {
            Current->Entry[cnt].Para.Output.Buffer = cosem_data + (dlms_asso_mtu() / Current->Actived + 16) * alive;
            Current->Entry[cnt].Para.Output.Size = (dlms_asso_mtu() - 20) / Current->Actived;
            alive += 1;
        }
    }
    
    //step 4
    //遍历访问每个条目
    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
    {
        if(Current->Entry[cnt].Object)
        {
            Current->Entry[cnt].Errs = Current->Entry[cnt].Object(&Current->Entry[cnt].Para);
        }
    }
    
    //step 5
    //组包返回数据
    result = reply_normal(&request, buffer, buffer_length, filled_length);
    
    if(result != APPL_SUCCESS)
    {
        reply_exception(result, buffer, buffer_length, filled_length);
    }
    
    //step 6
    //判断条目生命周期是否结束
    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
    {
        if(Current->Entry[cnt].Object)
        {
            //迭代器状态不在运行中，或者迭代器起始等于终止，或者调用结果为异常，生命周期结束
            if((Current->Entry[cnt].Para.Iterator.Status != ITER_ONGOING) || \
                (Current->Entry[cnt].Para.Iterator.Begin == Current->Entry[cnt].Para.Iterator.End) || \
                Current->Entry[cnt].Errs != OBJECT_NOERR)
            {
                //清零条目
                heap.set((void *)&Current->Entry[cnt], 0, sizeof(Current->Entry[cnt]));
            }
        }
    }
    
    //重新计算激活的条目数
    Current->Actived = 0;
    for(cnt=0; cnt<DLMS_REQ_LIST_MAX; cnt++)
    {
        if(Current->Entry[cnt].Object)
        {
            Current->Actived += 1;
        }
    }
    
    //释放内存
    heap.free(cosem_data);
    
    Current = (void *)0;
}
