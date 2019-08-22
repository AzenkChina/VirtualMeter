/**
 * @brief		
 * @details		
 * @date		2018-09-08
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "dlms_lexicon.h"
#include "dlms_types.h"
#include "mids.h"
#include "string.h"
#include "crc.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/**
  * @brief  cosem ��������
  * �������� �� 1 3 4 5 6 7 8
  * ��Щ���ʵ����������������Կ��Ա�MID��������������
  * �����������Щ���ԣ��ο��±�
  * Data                attr 2
  * Register            attr 2 3
  * Extended register   attr 2 3 5
  * Demand register     attr 2 3 4
  * Register activation attr 
  * Profile generic     attr 4 7 8
  * Clock               attr 2 3
  */
struct __cosem_entry_lite
{
    uint64_t key;//8 {classID groupA groupB groupC groupD groupE groupF suit}
    uint32_t oid;//4
    uint32_t mid[3];//3*4
    uint8_t right[15][3];//3*15
};

/**
  * @brief  cosem ������
  * �������� ��Ŵ��� 8 ����
  */
struct __cosem_entry_normal
{
    uint64_t key;//8 {classID groupA groupB groupC groupD groupE groupF suit}
    uint32_t oid;//4
    uint8_t right[22][3];//3*22
};

/**
  * @brief  cosem ��������
  * struct __cosem_entry_lite �Ĵ洢�棬��У�飬���ڴ��ļ��м���
  */
struct __cosem_entry_lite_file
{
    struct __cosem_entry_lite entry;
    uint32_t check;//crc32У��
};

/**
  * @brief  cosem ������
  * struct __cosem_entry_normal �Ĵ洢�棬��У�飬���ڴ��ļ��м���
  */
struct __cosem_entry_normal_file
{
    struct __cosem_entry_normal entry;
    uint32_t check;//crc32У��
};

/**
  * @brief  cosem �������ļ�ͷ
  */
struct __cosem_param_header
{
    struct
    {
        uint32_t    from; //���������ļ��е���ʼλ��
        uint16_t    amount; //����������
        
    }           lite;
    
    struct
    {
        uint32_t    from; //���������ļ��е���ʼλ��
        uint16_t    amount; //����������
        
    }           normal;
    
    uint32_t    check; //crc32У��
};

/**
  * @brief  cosem ������洢�ռ�ֲ�
  */
struct __cosem_param
{
    struct __cosem_param_header header;
    uint8_t         space[250*1024];
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
  * @brief  ���������� �� 1
  */
static const struct __cosem_entry_lite communal_lite[] = 
{
    /** cosem logical device name */
    {
        0x0100002A0000FFFF,

        0, //uid

        {M_ID2U(FMT_ASCII, 0, 0, 0, 0, 0)}, //mid attr 2

        {
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 1
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 2
        },
    },

    /** clock */
    {
        0x080000010000FFFF,

        0, //uid

        {M_ID2U(FMT_DTIME, 0, 0, 0, 0, 0), 0}, //mid attr 2 3

        {
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 1
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 2
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 3
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 4
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 5
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 6
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 7
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 8
            {ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)}, //attribute 9

            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 1
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 2
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 3
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 4
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 5
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 6
        },
    },
};

/**
  * @brief  ���������� �� 2
  */
static const struct __cosem_entry_normal communal[] = 
{
    /** association ln */
    {
        0x0F0000280000FFFF,

        0, //uid

        {
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 1
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 2
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 3
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 4
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 5
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 6
            {ATTR_NONE, (ATTR_READ | ATTR_WRITE), (ATTR_READ | ATTR_WRITE)}, //attribute 7
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 8
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 9
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 10
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 11
            
            {METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ}, //method 1
            {METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ}, //method 2
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 3
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 4
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 5
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 6
        },
    },

    /** image transfer */
    /** �����������в����ļ���ϵͳ */
    {
        0x1200002C0080FFFF,

        0x80, //uid

        {
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 1
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 2
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 3
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 4
            {(ATTR_READ | ATTR_WRITE), (ATTR_READ | ATTR_WRITE), (ATTR_READ | ATTR_WRITE)}, //attribute 5
            {ATTR_READ, ATTR_READ, ATTR_READ}, //attribute 6
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 7
            
            {METHOD_ACCESS, METHOD_ACCESS, METHOD_ACCESS}, //method 1
            {METHOD_ACCESS, METHOD_ACCESS, METHOD_ACCESS}, //method 2
            {METHOD_ACCESS, METHOD_ACCESS, METHOD_ACCESS}, //method 3
            {METHOD_ACCESS, METHOD_ACCESS, METHOD_ACCESS}, //method 4
        },
    },

    /** security setup */
    {
        0x4000002B0000FFFF,

        0, //uid

        {
            {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ}, //attribute 1
            {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ}, //attribute 2
            {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ}, //attribute 3
            {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ}, //attribute 4
            {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ}, //attribute 5
            {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ}, //attribute 6
            
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 1
            {METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ}, //method 2
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 3
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 4
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 5
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 6
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 7
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 8
        },
    },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ���� log2(n)
  */
static uint16_t clog2(uint16_t x)
{
    static const uint8_t log_2[256] = 
    {
        0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
    };
    
    int16_t l = -1;
    
    while (x >= 256)
    {
        l += 8;
        x >>= 8;
    }
    
    if((l + log_2[x]) < 0)
    {
        return(0);
    }
    
    return l + log_2[x];
}

/**
  * @brief   ��ȡ ��� ���� �� ���� ����
  */
static void get_class_map(const struct __cosem_request_desc *desc,
                          uint8_t *attr,
                          uint8_t *method)
{
    if((!desc) || (!attr) || (!method))
    {
        return;
    }
    
    switch(desc->descriptor.classid)
    {
        case CLASS_DATA: *attr = 2; *method = 0; return;
        case CLASS_REGISTER: *attr = 3; *method = 1; return;
        case CLASS_EXTREGISTER: *attr = 5; *method = 1; return;
        case CLASS_DEMANDREGISTER: *attr = 9; *method = 2; return;
        case CLASS_PROFILE: *attr = 8; *method = 2; return;
        case CLASS_CLOCK: *attr = 9; *method = 6; return;
        case CLASS_SCRIPT: *attr = 2; *method = 1; return;
        case CLASS_SCHEDULE: *attr = 2; *method = 3; return;
        case CLASS_SPECIALDAY: *attr = 2; *method = 2; return;
        case CLASS_ASSOCIATION_LN: *attr = 11; *method = 6; return;
        case CLASS_SAP: *attr = 2; *method = 1; return;
        case CLASS_IMAGE_TRANSFER: *attr = 7; *method = 4; return;
        case CLASS_ACTIVITYCALENDER: *attr = 10; *method = 1; return;
        case CLASS_REGISTER_MONITOR: *attr = 4; *method = 0; return;
        case CLASS_SINGLE_ACTION: *attr = 4; *method = 0; return;
        case CLASS_HDLC_SETUP: *attr = 9; *method = 0; return;
        case CLASS_MAC_ADDRESS_SETUP: *attr = 2; *method = 0; return;
        case CLASS_RELAY: *attr = 4; *method = 2; return;
        case CLASS_LIMITER: *attr = 11; *method = 0; return;
    }
    
    *attr = 0;
    *method = 0;
}

/**
  * @brief   ��ȡ �� ���� ���ڲ����ݱ�ʶ
  */
static void get_class_mid(const struct __cosem_request_desc *desc,
                          const struct __cosem_entry_lite *entry,
                          uint32_t *mid)
{
    if((!desc) || (!entry) || (!mid))
    {
        return;
    }
    
    *mid = 0;
    
    switch(desc->descriptor.classid)
    {
        case CLASS_DATA:
        {
            if(desc->descriptor.index == 2)
            {
                *mid = entry->mid[0];
            }
            
            break;
        }
        case CLASS_REGISTER:
        {
            if(desc->descriptor.index == 2)
            {
                *mid = entry->mid[0];
            }
            else if(desc->descriptor.index == 3)
            {
                *mid = entry->mid[1];
            }
            
            break;
        }
        case CLASS_EXTREGISTER:
        {
            if(desc->descriptor.index == 2)
            {
                *mid = entry->mid[0];
            }
            else if(desc->descriptor.index == 3)
            {
                *mid = entry->mid[1];
            }
            else if(desc->descriptor.index == 5)
            {
                *mid = entry->mid[2];
            }
            
            break;
        }
        case CLASS_DEMANDREGISTER:
        {
            if(desc->descriptor.index == 2)
            {
                *mid = entry->mid[0];
            }
            else if(desc->descriptor.index == 3)
            {
                *mid = entry->mid[1];
            }
            else if(desc->descriptor.index == 4)
            {
                *mid = entry->mid[2];
            }
            
            break;
        }
        case CLASS_PROFILE:
        {
            if(desc->descriptor.index == 4)
            {
                *mid = entry->mid[0];
            }
            else if(desc->descriptor.index == 7)
            {
                *mid = entry->mid[1];
            }
            else if(desc->descriptor.index == 8)
            {
                *mid = entry->mid[2];
            }
            
            break;
        }
        case CLASS_CLOCK:
        {
            if(desc->descriptor.index == 2)
            {
                *mid = entry->mid[0];
            }
            else if(desc->descriptor.index == 3)
            {
                *mid = entry->mid[1];
            }
            
            break;
        }
    }
}

/**
  * @brief   ���� struct __cosem_entry_lite ����
  */
static void prase_cosem_entry_lite(const struct __cosem_request_desc *desc,
                                   const struct __cosem_entry_lite *entry,
                                   union __dlms_right *right,
                                   uint32_t *oid,
                                   uint32_t *mid)
{
    uint8_t attr = 0;
    uint8_t method = 0;
    
    if((!desc) || (!entry) || (!right) || (!oid) || (!mid))
    {
        return;
    }
    
    if(desc->descriptor.classid > 8)
    {
        return;
    }
    
    if(!(desc->suit & (entry->key & 0xff)))
    {
        return;
    }
    
    get_class_map(desc, &attr, &method);
    
    if((attr + method) > 15)
    {
        return;
    }
    
    *oid = entry->oid;
    
    if( desc->request == GET_REQUEST || \
        desc->request == GLO_GET_REQUEST || \
        desc->request == DED_GET_REQUEST || \
        desc->request == SET_REQUEST || \
        desc->request == GLO_SET_REQUEST || \
        desc->request == DED_SET_REQUEST)
    {
        if(desc->descriptor.index > attr)
        {
            return;
        }
        
        get_class_mid(desc, entry, mid);
        
        if(desc->level == DLMS_ACCESS_LOWEST)
        {
            right->attr = (enum __dlms_attr_right)entry->right[desc->descriptor.index][0];
        }
        else if(desc->level == DLMS_ACCESS_LOW)
        {
            right->attr = (enum __dlms_attr_right)entry->right[desc->descriptor.index][1];
        }
        else if(desc->level == DLMS_ACCESS_HIGH)
        {
            right->attr = (enum __dlms_attr_right)entry->right[desc->descriptor.index][2];
        }
        else
        {
            right->attr = ATTR_NONE;
        }
    }
    else if(desc->request == ACTION_REQUEST || \
            desc->request == GLO_ACTION_REQUEST || \
            desc->request == DED_ACTION_REQUEST)
    {
        if(desc->descriptor.index > method)
        {
            return;
        }
        
        if(desc->level == DLMS_ACCESS_LOWEST)
        {
            right->method = (enum __dlms_method_right)entry->right[attr + desc->descriptor.index][0];
        }
        else if(desc->level == DLMS_ACCESS_LOW)
        {
            right->method = (enum __dlms_method_right)entry->right[attr + desc->descriptor.index][1];
        }
        else if(desc->level == DLMS_ACCESS_HIGH)
        {
            right->method = (enum __dlms_method_right)entry->right[attr + desc->descriptor.index][2];
        }
        else
        {
            right->method = METHOD_NONE;
        }
    }
}

/**
  * @brief  ���� struct __cosem_entry_normal ����
  */
static void prase_cosem_entry_normal(const struct __cosem_request_desc *desc,
                                     const struct __cosem_entry_normal *entry,
                                     union __dlms_right *right,
                                     uint32_t *oid)
{
    uint8_t attr = 0;
    uint8_t method = 0;
    
    if((!desc) || (!entry) || (!right) || (!oid))
    {
        return;
    }
    
    if(desc->descriptor.classid <= 8)
    {
        return;
    }
    
    if(!(desc->suit & (entry->key & 0xff)))
    {
        return;
    }
    
    get_class_map(desc, &attr, &method);
    
    if((attr + method) > 22)
    {
        return;
    }
    
    *oid = entry->oid;
    
    if( desc->request == GET_REQUEST || \
        desc->request == GLO_GET_REQUEST || \
        desc->request == DED_GET_REQUEST || \
        desc->request == SET_REQUEST || \
        desc->request == GLO_SET_REQUEST || \
        desc->request == DED_SET_REQUEST)
    {
        if(desc->descriptor.index > attr)
        {
            return;
        }
        
        if(desc->level == DLMS_ACCESS_LOWEST)
        {
            right->attr = (enum __dlms_attr_right)entry->right[desc->descriptor.index][0];
        }
        else if(desc->level == DLMS_ACCESS_LOW)
        {
            right->attr = (enum __dlms_attr_right)entry->right[desc->descriptor.index][1];
        }
        else if(desc->level == DLMS_ACCESS_HIGH)
        {
            right->attr = (enum __dlms_attr_right)entry->right[desc->descriptor.index][2];
        }
        else
        {
            right->attr = ATTR_NONE;
        }
    }
    else if(desc->request == ACTION_REQUEST || \
            desc->request == GLO_ACTION_REQUEST || \
            desc->request == DED_ACTION_REQUEST)
    {
        if(desc->descriptor.index > method)
        {
            return;
        }
        
        if(desc->level == DLMS_ACCESS_LOWEST)
        {
            right->method = (enum __dlms_method_right)entry->right[attr + desc->descriptor.index][0];
        }
        else if(desc->level == DLMS_ACCESS_LOW)
        {
            right->method = (enum __dlms_method_right)entry->right[attr + desc->descriptor.index][1];
        }
        else if(desc->level == DLMS_ACCESS_HIGH)
        {
            right->method = (enum __dlms_method_right)entry->right[attr + desc->descriptor.index][2];
        }
        else
        {
            right->method = METHOD_NONE;
        }
    }
}

/**
  * @brief  ���� struct __cosem_request_desc �е����ݣ���ȡ�����������Ϣ
  * @param  desc ������������������Լ���ǰ������״̬�ϳ�
  * @param  right ��ǰ����ķ���Ȩ��
  * @param  oid ��������
  * @param  mid �ڲ����ݱ�ʶ
  * @retval None
  */
void dlms_lex_parse(const struct __cosem_request_desc *desc,
                    union __dlms_right *right,
                    uint32_t *oid,
                    uint32_t *mid)
{
    uint64_t key;
    uint16_t cnt;
    uint16_t position;
    void *data;
    struct __cosem_param_header header;
    
    if((!right) || (!oid) || (!mid))
    {
        return;
    }
    
    right->attr = ATTR_NONE;
    *oid = 0xffffffff;
    *mid = 0;
    
    if(!desc)
    {
        return;
    }
    
    //���ɱȶԼ�ֵ
    key = (desc->descriptor.classid & 0xff);
    key <<= 8;
    key += (desc->descriptor.obis[0] & 0xff);
    key <<= 8;
    key += (desc->descriptor.obis[1] & 0xff);
    key <<= 8;
    key += (desc->descriptor.obis[2] & 0xff);
    key <<= 8;
    key += (desc->descriptor.obis[3] & 0xff);
    key <<= 8;
    key += (desc->descriptor.obis[4] & 0xff);
    key <<= 8;
    key += (desc->descriptor.obis[5] & 0xff);
    key <<= 8;
    
    //���ұ����������
    if(desc->descriptor.classid <= 8)
    {
        data = (void *)communal_lite;
        
        for(cnt=0; cnt<(sizeof(communal_lite)/sizeof(struct __cosem_entry_lite)); cnt++)
        {
            if(key == ((((struct __cosem_entry_lite *)data) + cnt)->key & 0xffffffffffffff00))
            {
                prase_cosem_entry_lite(desc, (((struct __cosem_entry_lite *)data) + cnt), right, oid, mid);
                return;
            }
        }
    }
    else
    {
        data = (void *)communal;
        
        for(cnt=0; cnt<(sizeof(communal)/sizeof(struct __cosem_entry_normal)); cnt++)
        {
            if(key == ((((struct __cosem_entry_normal *)data) + cnt)->key & 0xffffffffffffff00))
            {
                prase_cosem_entry_normal(desc, (((struct __cosem_entry_normal *)data) + cnt), right, oid);
                return;
            }
        }
    }
    
    //��ȡ�����ļ��е���Ϣͷ��������Ƿ���ȷ
    if(file.read("lexicon", 0, sizeof(struct __cosem_param_header), &header) != \
        sizeof(struct __cosem_param_header))
    {
        return;
    }
    else if(crc32(&header, (sizeof(struct __cosem_param_header) - sizeof(uint32_t))) != \
            header.check)
    {
        return;
    }
    
    //���Ҳ����ļ����������
    if(desc->descriptor.classid <= 8)
    {
        //�����ڴ����ڶ�ȡ��Ŀ
        data = heap.dalloc(sizeof(struct __cosem_entry_lite_file));
        if(!data)
        {
            return;
        }
        
        //���м俪ʼ����
        position = header.lite.amount / 2;
        
        //��ʼ���ַ�����
        for(cnt=0; cnt<clog2(header.lite.amount); cnt++)
        {
            //��ȡ
            if(file.read("lexicon", \
                         STRUCT_OFFSET(struct __cosem_param, space) + header.lite.from + position * sizeof(struct __cosem_entry_lite_file), 
                         sizeof(struct __cosem_entry_lite_file), \
                         data) != \
               sizeof(struct __cosem_entry_lite_file))
            {
                heap.free(data);
                return;
            }
            
            //У��
            if(crc32(&(((struct __cosem_entry_lite_file *)data)->entry), sizeof(struct __cosem_entry_lite)) != \
               ((struct __cosem_entry_lite_file *)data)->check)
            {
                //У��ʧ�ܣ�����λ��΢��
                if(position)
                {
                    position -= 1;
                    continue;
                }
                else
                {
                    heap.free(data);
                    return;
                }
            }
            
            //��ֵ�ȶ�
            if(key == (((struct __cosem_entry_lite_file *)data)->entry.key & 0xffffffffffffff00))
            {
                prase_cosem_entry_lite(desc, &(((struct __cosem_entry_lite_file *)data)->entry), right, oid, mid);
                heap.free(data);
                return;
            }
            else if(key < (((struct __cosem_entry_lite_file *)data)->entry.key & 0xffffffffffffff00))
            {
                position -= (position / 2);
            }
            else
            {
                position += (position / 2);
            }
        }
        
        heap.free(data);
    }
    else
    {
        //�����ڴ����ڶ�ȡ��Ŀ
        data = heap.dalloc(sizeof(struct __cosem_entry_normal_file));
        if(!data)
        {
            return;
        }
        
        //���м俪ʼ����
        position = header.normal.amount / 2;
        
        //��ʼ���ַ�����
        for(cnt=0; cnt<clog2(header.normal.amount); cnt++)
        {
            //��ȡ
            if(file.read("lexicon", \
                         STRUCT_OFFSET(struct __cosem_param, space) + header.normal.from + position * sizeof(struct __cosem_entry_normal_file), 
                         sizeof(struct __cosem_entry_normal_file), \
                         data) != \
               sizeof(struct __cosem_entry_normal_file))
            {
                heap.free(data);
                return;
            }
            
            //У��
            if(crc32(&(((struct __cosem_entry_normal_file *)data)->entry), sizeof(struct __cosem_entry_normal)) != \
               ((struct __cosem_entry_normal_file *)data)->check)
            {
                //У��ʧ�ܣ�����λ��΢��
                if(position)
                {
                    position -= 1;
                    continue;
                }
                else
                {
                    heap.free(data);
                    return;
                }
            }
            
            //��ֵ�ȶ�
            if(key == (((struct __cosem_entry_normal_file *)data)->entry.key & 0xffffffffffffff00))
            {
                prase_cosem_entry_normal(desc, &(((struct __cosem_entry_normal_file *)data)->entry), right, oid);
                heap.free(data);
                return;
            }
            else if(key < (((struct __cosem_entry_normal_file *)data)->entry.key & 0xffffffffffffff00))
            {
                position -= (position / 2);
            }
            else
            {
                position += (position / 2);
            }
        }
        
        heap.free(data);
    }
    
    return;
}
