/**
 * @brief		
 * @details		
 * @date		2018-09-08
 **/

/* Includes ------------------------------------------------------------------*/
#include "dlms_lexicon.h"
#include "types_metering.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/

/**
  * @brief  属性 描述符
  */
struct __dlms_attr_desc
{
    const char      *table_r;//get函数表
    uint8_t         index_r;//get函数索引
    
    const char      *table_w;//set函数表
    uint8_t         index_w;//set函数索引
    
    uint32_t        param;//输入参数
    
    struct
    {
        uint16_t    lowest:3;
        uint16_t    low:3;
        uint16_t    high:3;
        uint16_t    reserve1:3;
		uint16_t    reserve2:3;
    }               right;
};


/**
  * @brief  方法 描述符
  */
struct __dlms_method_desc
{
    const char      *table;//函数表
    uint8_t         index;//函数索引
    
    struct
    {
        uint16_t    lowest:3;
        uint16_t    low:3;
        uint16_t    high:3;
        uint16_t    reserve1:3;
		uint16_t    reserve2:3;
    }               right;
};



/**
  * @brief  cosem 数据项 data
  */
struct __cosem_entry_data
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[1];
};

/**
  * @brief  cosem 数据项 register
  */
struct __cosem_entry_register
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[2];
    struct __dlms_method_desc method[1];
};

/**
  * @brief  cosem 数据项 extended register
  */
struct __cosem_entry_ext_register
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[4];
    struct __dlms_method_desc method[1];
};

/**
  * @brief  cosem 数据项 demand register
  */
struct __cosem_entry_demand
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[8];
    struct __dlms_method_desc method[2];
};

/**
  * @brief  cosem 数据项 profile generic
  */
struct __cosem_entry_profile
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[7];
    struct __dlms_method_desc method[2];
};

/**
  * @brief  cosem 数据项 association ln
  */
struct __cosem_entry_association_ln
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[10];
    struct __dlms_method_desc method[6];
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Association LN 类数据项列表
  */
struct __cosem_entry_association_ln cosem_class_association_ln[] =
{
    {   /** Association LN */ \
        {0, 0, 40, 0, 0, 255},\

        /** 属性 */
        {\
            /** object_list */
            {"Association", 0, "", 0,    /** 函数表，函数序号 */ \
                0,  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** associated_partners_id */
            {"Association", 1, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** application_context_name */
            {"Association", 2, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** xDLMS_context_info */
            {"Association", 3, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** authentication_mechanism_name */
            {"Association", 4, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** secret */
            {"", 0, "Association", 5,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_WRITE_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_WRITE_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_WRITE_ONLY,    /** 权限三 */ \
                },\
            },\
            /** association_status */
            {"Association", 6, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** security_setup_reference */
            {"Association", 7, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** user_list */
            {"Association", 8, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            /** current_user */
            {"Association", 9, "", 0,    /** 函数表，函数序号 */
                0,  /** 携带参数 */
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** reply_to_HLS_authentication */
            {"Association", 10,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_AUTH_ACCESS,   /** 权限三 */ \
                },\
            },\
            /** change_HLS_secret */
            {"Association", 11,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
            /** add_object */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
            /** remove_object */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
            /** add_user */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
            /** remove_user */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
};

/**
  * @brief  Register 类数据项列表
  */
static const struct __cosem_entry_register cosem_class_register[] =
{
    {   /** A相电压 */ \
        {1, 0, 32, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_VOLTAGE, M_PHASE_A, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_VOLTAGE, M_PHASE_A, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** B相电压 */ \
        {1, 0, 52, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_VOLTAGE, M_PHASE_B, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_VOLTAGE, M_PHASE_B, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** C相电压 */ \
        {1, 0, 72, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_VOLTAGE, M_PHASE_C, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_VOLTAGE, M_PHASE_C, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** A相电流 */ \
        {1, 0, 31, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_CURRENT, M_PHASE_A, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_CURRENT, M_PHASE_A, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** B相电流 */ \
        {1, 0, 51, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_CURRENT, M_PHASE_B, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_CURRENT, M_PHASE_B, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** C相电流 */ \
        {1, 0, 71, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_CURRENT, M_PHASE_C, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_CURRENT, M_PHASE_C, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** 零线电流 */ \
        {1, 0, 91, 7, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_CURRENT, M_PHASE_N, M_QUAD_N, 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_CURRENT, M_PHASE_N, M_QUAD_N, 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限二 */ \
                    ATTRIBUTE_NO_ACCESS,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** 正向有功总电能 */ \
        {1, 0, 1, 8, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_I|M_QUAD_V), 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_I|M_QUAD_V), 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
    
    {   /** 反向有功总电能 */ \
        {1, 0, 2, 8, 0, 255},\

        /** 属性 */
        {\
            /** 属性二 */
            {"Register", 0, "", 0,    /** 函数表，函数序号 */ \
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_II|M_QUAD_III), 0, 0),  /** 携带参数 */ \
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
            
            /** 属性三 */
            {"", 0, "", 0,    /** 函数表，函数序号 */
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_II|M_QUAD_III), 0, 0),  /** 携带参数 */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** 权限一 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限二 */ \
                    ATTRIBUTE_READ_ONLY,    /** 权限三 */ \
                },\
            },\
        },
        
        /** 方法 */
        {\
            /** 方法一 */
            {"", 0,  /** 函数表，函数序号 */
                {\
                    METHOD_NO_ACCESS,   /** 权限一 */ \
                    METHOD_NO_ACCESS,   /** 权限二 */ \
                    METHOD_NO_ACCESS,   /** 权限三 */ \
                },\
            },\
        },\
    },
};



/**
  * @brief  输入由报文解析出来的 struct __cosem_request_desc 结构，定位函数属于哪个表以及索引号
  */
void dlms_lex_parse(const struct __cosem_request_desc *desc,
                    const char **table,
                    uint8_t *index,
                    uint32_t *param)
{
    uint8_t *Table = (uint8_t *)0;
    union __dlms_right right;
    uint16_t step = 0;
    uint16_t amount = 0;
    uint16_t count = 0;
    
    
    if((!table) || (!index) || (!param))
    {
        return;
    }
    
    *table = 0;
    *index = 0;
    *param = 0;
    
    if(!desc)
    {
        return;
    }
    
    //找到对应的表并获取表内数据项个数
    switch(desc->descriptor.classid)
    {
        case CLASS_REGISTER:
        {
            Table = (void *)cosem_class_register;
            step = sizeof(struct __cosem_entry_register);
            amount = sizeof(cosem_class_register) / step;
            
            for(count=0; count<amount; count++)
            {
                //比对OBIS是否匹配
                if(memcmp(desc->descriptor.obis, ((struct __cosem_entry_register *)Table)->instance, 6) != 0)
                {
                    Table += sizeof(struct __cosem_entry_register);
                    
                    continue;
                }
                
                //判断属性或者方法的索引是否有效
                if((desc->request == GET_REQUEST) || (desc->request == SET_REQUEST))
                {
                    if(desc->descriptor.index < 2)
                    {
                        break;
                    }
                    
                    if(desc->descriptor.index > (sizeof((((struct __cosem_entry_register *)Table)->attr)) / sizeof(struct __dlms_attr_desc) + 1))
                    {
                        break;
                    }
                    
                    if(desc->request == GET_REQUEST)
                    {
                        *table = ((struct __cosem_entry_register *)Table)->attr[desc->descriptor.index - 2].table_r;
                        *index = ((struct __cosem_entry_register *)Table)->attr[desc->descriptor.index - 2].index_r;
                    }
                    else
                    {
                        *table = ((struct __cosem_entry_register *)Table)->attr[desc->descriptor.index - 2].table_w;
                        *index = ((struct __cosem_entry_register *)Table)->attr[desc->descriptor.index - 2].index_w;
                    }
                    
                    *param = ((struct __cosem_entry_register *)Table)->attr[desc->descriptor.index - 2].param;
                    
                    switch(desc->level)
                    {
                        case DLMS_ACCESS_LOWEST:
                        {
                            right.attr = (enum __dlms_attr_right)((struct __cosem_entry_register *)Table)->attr->right.lowest;
                            break;
                        }
                        case DLMS_ACCESS_LOW:
                        {
                            right.attr = (enum __dlms_attr_right)((struct __cosem_entry_register *)Table)->attr->right.low;
                            break;
                        }
                        case DLMS_ACCESS_HIGH:
                        {
                            right.attr = (enum __dlms_attr_right)((struct __cosem_entry_register *)Table)->attr->right.high;
                            break;
                        }
                        default:
                        {
                            right.attr = ATTRIBUTE_NO_ACCESS;
                            break;
                        }
                    }
                    
                    break;
                }
                else if(desc->request == ACTION_REQUEST)
                {
                    if(desc->descriptor.index < 1)
                    {
                        break;
                    }
                    
                    if(desc->descriptor.index > (sizeof((((struct __cosem_entry_register *)Table)->method)) / sizeof(struct __dlms_method_desc)))
                    {
                        break;
                    }
                    
                    *table = ((struct __cosem_entry_register *)Table)->method[desc->descriptor.index - 1].table;
                    *index = ((struct __cosem_entry_register *)Table)->method[desc->descriptor.index - 1].index;
                    
                    switch(desc->level)
                    {
                        case DLMS_ACCESS_LOWEST:
                        {
                            right.method = (enum __dlms_method_right)((struct __cosem_entry_register *)Table)->method->right.lowest;
                            break;
                        }
                        case DLMS_ACCESS_LOW:
                        {
                            right.method = (enum __dlms_method_right)((struct __cosem_entry_register *)Table)->method->right.low;
                            break;
                        }
                        case DLMS_ACCESS_HIGH:
                        {
                            right.method = (enum __dlms_method_right)((struct __cosem_entry_register *)Table)->method->right.high;
                            break;
                        }
                        default:
                        {
                            right.method = METHOD_NO_ACCESS;
                            break;
                        }
                    }
                    
                    break;
                }
                else
                {
                    break;
                }
            }
            
            break;
        }
        case CLASS_ASSOCIATION_LN:
        {
            Table = (void *)cosem_class_association_ln;
            step = sizeof(struct __cosem_entry_association_ln);
            amount = sizeof(cosem_class_association_ln) / step;
            
            for(count=0; count<amount; count++)
            {
                //比对OBIS是否匹配
                if(memcmp(desc->descriptor.obis, ((struct __cosem_entry_association_ln *)Table)->instance, 6) != 0)
                {
                    Table += sizeof(struct __cosem_entry_association_ln);
                    
                    continue;
                }
                
                //判断属性或者方法的索引是否有效
                if((desc->request == GET_REQUEST) || (desc->request == SET_REQUEST))
                {
                    if(desc->descriptor.index < 2)
                    {
                        break;
                    }
                    
                    if(desc->descriptor.index > (sizeof((((struct __cosem_entry_association_ln *)Table)->attr)) / sizeof(struct __dlms_attr_desc) + 1))
                    {
                        break;
                    }
                    
                    if(desc->request == GET_REQUEST)
                    {
                        *table = ((struct __cosem_entry_association_ln *)Table)->attr[desc->descriptor.index - 2].table_r;
                        *index = ((struct __cosem_entry_association_ln *)Table)->attr[desc->descriptor.index - 2].index_r;
                    }
                    else
                    {
                        *table = ((struct __cosem_entry_association_ln *)Table)->attr[desc->descriptor.index - 2].table_w;
                        *index = ((struct __cosem_entry_association_ln *)Table)->attr[desc->descriptor.index - 2].index_w;
                    }
                    
                    *param = ((struct __cosem_entry_association_ln *)Table)->attr[desc->descriptor.index - 2].param;
                    
                    switch(desc->level)
                    {
                        case DLMS_ACCESS_LOWEST:
                        {
                            right.attr = (enum __dlms_attr_right)((struct __cosem_entry_association_ln *)Table)->attr->right.lowest;
                            break;
                        }
                        case DLMS_ACCESS_LOW:
                        {
                            right.attr = (enum __dlms_attr_right)((struct __cosem_entry_association_ln *)Table)->attr->right.low;
                            break;
                        }
                        case DLMS_ACCESS_HIGH:
                        {
                            right.attr = (enum __dlms_attr_right)((struct __cosem_entry_association_ln *)Table)->attr->right.high;
                            break;
                        }
                        default:
                        {
                            right.attr = ATTRIBUTE_NO_ACCESS;
                            break;
                        }
                    }
                    
                    break;
                }
                else if(desc->request == ACTION_REQUEST)
                {
                    if(desc->descriptor.index < 1)
                    {
                        break;
                    }
                    
                    if(desc->descriptor.index > (sizeof((((struct __cosem_entry_association_ln *)Table)->method)) / sizeof(struct __dlms_method_desc)))
                    {
                        break;
                    }
                    
                    *table = ((struct __cosem_entry_association_ln *)Table)->method[desc->descriptor.index - 1].table;
                    *index = ((struct __cosem_entry_association_ln *)Table)->method[desc->descriptor.index - 1].index;
                    
                    switch(desc->level)
                    {
                        case DLMS_ACCESS_LOWEST:
                        {
                            right.method = (enum __dlms_method_right)((struct __cosem_entry_association_ln *)Table)->method->right.lowest;
                            break;
                        }
                        case DLMS_ACCESS_LOW:
                        {
                            right.method = (enum __dlms_method_right)((struct __cosem_entry_association_ln *)Table)->method->right.low;
                            break;
                        }
                        case DLMS_ACCESS_HIGH:
                        {
                            right.method = (enum __dlms_method_right)((struct __cosem_entry_association_ln *)Table)->method->right.high;
                            break;
                        }
                        default:
                        {
                            right.method = METHOD_NO_ACCESS;
                            break;
                        }
                    }
                    
                    break;
                }
                else
                {
                    break;
                }
            }
            
            break;
        }
        default:
        	break;
    }
    
    return;
}

/**
  * @brief  获取条目总数
  */
uint32_t dlms_lex_amount(void)
{
    uint32_t amount = 0;
    
    amount += sizeof(cosem_class_register) / sizeof(struct __cosem_entry_register);
    amount += sizeof(cosem_class_association_ln) / sizeof(struct __cosem_entry_association_ln);
    
    return(amount);
}
