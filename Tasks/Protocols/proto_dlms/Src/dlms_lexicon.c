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
  * @brief  ���� ������
  */
struct __dlms_attr_desc
{
    const char      *table_r;//get������
    uint8_t         index_r;//get��������
    
    const char      *table_w;//set������
    uint8_t         index_w;//set��������
    
    uint32_t        param;//�������
    
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
  * @brief  ���� ������
  */
struct __dlms_method_desc
{
    const char      *table;//������
    uint8_t         index;//��������
    
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
  * @brief  cosem ������ data
  */
struct __cosem_entry_data
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[1];
};

/**
  * @brief  cosem ������ register
  */
struct __cosem_entry_register
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[2];
    struct __dlms_method_desc method[1];
};

/**
  * @brief  cosem ������ extended register
  */
struct __cosem_entry_ext_register
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[4];
    struct __dlms_method_desc method[1];
};

/**
  * @brief  cosem ������ demand register
  */
struct __cosem_entry_demand
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[8];
    struct __dlms_method_desc method[2];
};

/**
  * @brief  cosem ������ profile generic
  */
struct __cosem_entry_profile
{
    uint8_t instance[6];
    
    struct __dlms_attr_desc attr[7];
    struct __dlms_method_desc method[2];
};

/**
  * @brief  cosem ������ association ln
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
  * @brief  Association LN ���������б�
  */
struct __cosem_entry_association_ln cosem_class_association_ln[] =
{
    {   /** Association LN */ \
        {0, 0, 40, 0, 0, 255},\

        /** ���� */
        {\
            /** object_list */
            {"Association", 0, "", 0,    /** ������������� */ \
                0,  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** associated_partners_id */
            {"Association", 1, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** application_context_name */
            {"Association", 2, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** xDLMS_context_info */
            {"Association", 3, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** authentication_mechanism_name */
            {"Association", 4, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** secret */
            {"", 0, "Association", 5,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_WRITE_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_WRITE_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_WRITE_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** association_status */
            {"Association", 6, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** security_setup_reference */
            {"Association", 7, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** user_list */
            {"Association", 8, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            /** current_user */
            {"Association", 9, "", 0,    /** ������������� */
                0,  /** Я������ */
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** reply_to_HLS_authentication */
            {"Association", 10,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_AUTH_ACCESS,   /** Ȩ���� */ \
                },\
            },\
            /** change_HLS_secret */
            {"Association", 11,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
            /** add_object */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
            /** remove_object */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
            /** add_user */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
            /** remove_user */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
};

/**
  * @brief  Register ���������б�
  */
static const struct __cosem_entry_register cosem_class_register[] =
{
    {   /** A���ѹ */ \
        {1, 0, 32, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_VOLTAGE, M_PHASE_A, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_VOLTAGE, M_PHASE_A, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** B���ѹ */ \
        {1, 0, 52, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_VOLTAGE, M_PHASE_B, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_VOLTAGE, M_PHASE_B, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** C���ѹ */ \
        {1, 0, 72, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_VOLTAGE, M_PHASE_C, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_VOLTAGE, M_PHASE_C, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** A����� */ \
        {1, 0, 31, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_CURRENT, M_PHASE_A, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_CURRENT, M_PHASE_A, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** B����� */ \
        {1, 0, 51, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_CURRENT, M_PHASE_B, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_CURRENT, M_PHASE_B, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** C����� */ \
        {1, 0, 71, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_CURRENT, M_PHASE_C, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_CURRENT, M_PHASE_C, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** ���ߵ��� */ \
        {1, 0, 91, 7, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_CURRENT, M_PHASE_N, M_QUAD_N, 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_READ_ONLY,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_CURRENT, M_PHASE_N, M_QUAD_N, 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ�޶� */ \
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** �����й��ܵ��� */ \
        {1, 0, 1, 8, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_I|M_QUAD_V), 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_I|M_QUAD_V), 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
    
    {   /** �����й��ܵ��� */ \
        {1, 0, 2, 8, 0, 255},\

        /** ���� */
        {\
            /** ���Զ� */
            {"Register", 0, "", 0,    /** ������������� */ \
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_II|M_QUAD_III), 0, 0),  /** Я������ */ \
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
            
            /** ������ */
            {"", 0, "", 0,    /** ������������� */
                M_ID2UINT(M_P_ENERGY, M_PHASE_T, (M_QUAD_II|M_QUAD_III), 0, 0),  /** Я������ */
                {\
                    ATTRIBUTE_NO_ACCESS,    /** Ȩ��һ */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ�޶� */ \
                    ATTRIBUTE_READ_ONLY,    /** Ȩ���� */ \
                },\
            },\
        },
        
        /** ���� */
        {\
            /** ����һ */
            {"", 0,  /** ������������� */
                {\
                    METHOD_NO_ACCESS,   /** Ȩ��һ */ \
                    METHOD_NO_ACCESS,   /** Ȩ�޶� */ \
                    METHOD_NO_ACCESS,   /** Ȩ���� */ \
                },\
            },\
        },\
    },
};



/**
  * @brief  �����ɱ��Ľ��������� struct __cosem_request_desc �ṹ����λ���������ĸ����Լ�������
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
    
    //�ҵ���Ӧ�ı���ȡ�������������
    switch(desc->descriptor.classid)
    {
        case CLASS_REGISTER:
        {
            Table = (void *)cosem_class_register;
            step = sizeof(struct __cosem_entry_register);
            amount = sizeof(cosem_class_register) / step;
            
            for(count=0; count<amount; count++)
            {
                //�ȶ�OBIS�Ƿ�ƥ��
                if(memcmp(desc->descriptor.obis, ((struct __cosem_entry_register *)Table)->instance, 6) != 0)
                {
                    Table += sizeof(struct __cosem_entry_register);
                    
                    continue;
                }
                
                //�ж����Ի��߷����������Ƿ���Ч
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
                //�ȶ�OBIS�Ƿ�ƥ��
                if(memcmp(desc->descriptor.obis, ((struct __cosem_entry_association_ln *)Table)->instance, 6) != 0)
                {
                    Table += sizeof(struct __cosem_entry_association_ln);
                    
                    continue;
                }
                
                //�ж����Ի��߷����������Ƿ���Ч
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
  * @brief  ��ȡ��Ŀ����
  */
uint32_t dlms_lex_amount(void)
{
    uint32_t amount = 0;
    
    amount += sizeof(cosem_class_register) / sizeof(struct __cosem_entry_register);
    amount += sizeof(cosem_class_association_ln) / sizeof(struct __cosem_entry_association_ln);
    
    return(amount);
}
