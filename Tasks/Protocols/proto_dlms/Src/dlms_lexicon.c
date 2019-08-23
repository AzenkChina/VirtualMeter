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
  * @brief  cosem 数据项简版
  * 用于描述 类 1 3 4 5 6 7 8
  * 这些类的实例中最多有三个属性可以被MID描述其物理意义
  * 具体可描述哪些属性，参考下表：
  * Data                attr 2
  * Register            attr 2 3
  * Extended register   attr 2 3 5
  * Demand register     attr 2 3 4
  * Register activation attr 
  * Profile generic     attr 4 7 8
  * Clock               attr 2 3
  */
struct __cosem_entry_low
{
    uint64_t key;//8 {classID groupA groupB groupC groupD groupE groupF suit}
    uint32_t oid;//4
    uint32_t mid[3];//3*4
    uint8_t right[15][3];//3*15
};

/**
  * @brief  cosem 数据项简版
  * struct __cosem_entry_low 的存储版，带校验，用于从文件中加载
  */
struct __cosem_entry_low_file
{
    struct __cosem_entry_low entry;
    uint32_t check;//crc32校验
};

/**
  * @brief  cosem 数据项
  * 用于描述 类号大于 8 的类
  */
struct __cosem_entry_high
{
    uint64_t key;//8 {classID groupA groupB groupC groupD groupE groupF suit}
    uint32_t oid;//4
    uint8_t right[22][3];//3*22
};

/**
  * @brief  cosem 数据项
  * struct __cosem_entry_high 的存储版，带校验，用于从文件中加载
  */
struct __cosem_entry_high_file
{
    struct __cosem_entry_high entry;
    uint32_t check;//crc32校验
};

/**
  * @brief  cosem 数据项
  * struct 用于从文件中加载
  */
union __cosem_entry_file
{
    struct __cosem_entry_low_file low;
    struct __cosem_entry_high_file high;
    uint8_t size[96];
};

/**
  * @brief  cosem 数据项文件头
  */
struct __cosem_param_header
{
    uint16_t amount; //数据项条数
	uint16_t spread[8]; //分类后数据项条数（suit 1~8）
    uint32_t check; //crc32校验
};

/**
  * @brief  cosem 数据项存储空间分布
  */
struct __cosem_param
{
    struct __cosem_param_header header;
    union __cosem_entry_file entry[(63*1024+512)/sizeof(union __cosem_entry_file)];
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
  * @brief  本地数据项表
  */
static const struct __cosem_entry_high communal[] = 
{
    /** association ln */
    {
        0x0F0000280000FF80,/** suit 8 only */

        0x80000000, //uid

        {
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 1
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 2
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 3
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 4
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 5
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 6
            {ATTR_NONE, ATTR_NONE, (ATTR_READ | ATTR_WRITE)}, //attribute 7
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 8
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 9
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 10
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 11
            
            {METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ}, //method 1
            {METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ}, //method 2
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 3
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 4
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 5
            {METHOD_NONE, METHOD_NONE, METHOD_NONE}, //method 6
        },
    },

    /** image transfer */
    /** 用于下载lexicon参数文件到系统 */
    {
        0x1200002C0080FF80,/** suit 8 only */

        0x80000000, //uid

        {
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 1
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 2
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 3
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 4
            {ATTR_NONE, ATTR_NONE, (ATTR_READ | ATTR_WRITE)}, //attribute 5
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 6
            {ATTR_NONE, ATTR_NONE, ATTR_READ}, //attribute 7
            
            {METHOD_NONE, METHOD_NONE, METHOD_ACCESS}, //method 1
            {METHOD_NONE, METHOD_NONE, METHOD_ACCESS}, //method 2
            {METHOD_NONE, METHOD_NONE, METHOD_ACCESS}, //method 3
            {METHOD_NONE, METHOD_NONE, METHOD_ACCESS}, //method 4
        },
    },

    /** security setup */
    {
        0x4000002B0000FF80,/** suit 8 only */

        0x80000000, //uid

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
  * @brief  计算 log2(n)
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
  * @brief   获取 类的 属性 和 方法 个数
  */
static void get_class_map(uint16_t id, uint8_t *attr, uint8_t *method)
{
    if((!id) || (!attr) || (!method))
    {
        return;
    }
    
    switch(id)
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
  * @brief   获取 类 属性 的内部数据标识
  */
static void get_class_mid(const struct __cosem_request_desc *desc,
                          const struct __cosem_entry_low *entry,
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
  * @brief   解析 struct __cosem_entry_low 数据
  */
static void prase_cosem_entry_low(const struct __cosem_request_desc *desc,
                                   const struct __cosem_entry_low *entry,
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
    
    get_class_map(desc->descriptor.classid, &attr, &method);
    
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
  * @brief  解析 struct __cosem_entry_high 数据
  */
static void prase_cosem_entry_high(const struct __cosem_request_desc *desc,
                                     const struct __cosem_entry_high *entry,
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
    
    get_class_map(desc->descriptor.classid, &attr, &method);
    
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
  * @brief  根据 struct __cosem_request_desc 中的内容，获取对象的配置信息
  * @param  desc 输入参数，由请求报文以及当前的连接状态合成
  * @param  right 当前对象的访问权限
  * @param  oid 对象索引
  * @param  mid 内部数据标识
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
    struct __cosem_param_header header;
    union __cosem_entry_file entry;
    
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
    
    //生成比对键值
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
    
    //查找本地数据项表
    if(desc->descriptor.classid > 8)
    {
        for(cnt=0; cnt<(sizeof(communal)/sizeof(struct __cosem_entry_high)); cnt++)
        {
            if(key == ((communal + cnt)->key & 0xffffffffffffff00))
            {
                prase_cosem_entry_high(desc, (communal + cnt), right, oid);
                return;
            }
        }
    }
    
    //读取参数文件中的信息头，并检查是否正确
    if(file.read("lexicon", 0, sizeof(struct __cosem_param_header), &header) != \
        sizeof(struct __cosem_param_header))
    {
        return;
    }
    if(crc32(&header, (sizeof(struct __cosem_param_header) - sizeof(uint32_t))) != \
            header.check)
    {
        return;
    }
    
    //开始二分法查找
    
    //从中间开始查找
    position = header.amount / 2;
    
    for(cnt=0; cnt<clog2(header.amount); cnt++)
    {
        //读取
        if(file.read("lexicon", \
                     STRUCT_OFFSET(struct __cosem_param, entry[position]), \
                     sizeof(entry), \
                     &entry) != sizeof(entry))
        {
            break;
        }
        
        //键值比对
        if(key < (entry.low.entry.key & 0xffffffffffffff00))
        {
            position -= (position / 2);
            continue;
        }
        else if(key > (entry.low.entry.key & 0xffffffffffffff00))
        {
            position += (position / 2);
            continue;
        }
        
        //键值匹配
        if(desc->descriptor.classid <= 8)
        {
            //校验
            if(crc32(&entry.low.entry, sizeof(entry.low.entry)) != entry.low.check)
            {
                //校验失败
                break;
            }
            
            prase_cosem_entry_low(desc, &entry.low.entry, right, oid, mid);
            break;
        }
        else
        {
            //校验
            if(crc32(&entry.high.entry, sizeof(entry.high.entry)) != entry.high.check)
            {
                //校验失败
                break;
            }
            
            prase_cosem_entry_high(desc, &entry.high.entry, right, oid);
            break;
        }
    }
    
    return;
}

/**
  * @brief  获取指定suit的配置信息条数
  */
uint16_t dlms_lex_amount(uint8_t suit)
{
    uint16_t cnt;
    struct __cosem_param_header header;
	uint16_t amount = 0;
	
	if(!suit)
	{
		return(0);
	}
    
    //查找本地数据项表
    for(cnt=0; cnt<(sizeof(communal)/sizeof(struct __cosem_entry_high)); cnt++)
    {
        if((communal + cnt)->key & suit)
        {
            amount += 1;
        }
    }
    
    //读取参数文件中的信息头，并检查是否正确
    if(file.read("lexicon", 0, sizeof(struct __cosem_param_header), &header) != \
        sizeof(struct __cosem_param_header))
    {
        return(amount);
    }
    else if(crc32(&header, (sizeof(struct __cosem_param_header) - sizeof(uint32_t))) != \
            header.check)
    {
        return(amount);
    }
	
	for(cnt=0; cnt<8; cnt++)
	{
		if((suit >> cnt) & 0x01)
		{
			amount += header.spread[cnt];
		}
	}
	
	return(amount);
}

/**
  * @brief  获取指定条目信息
  */
uint16_t dlms_lex_entry(uint16_t index, struct __cosem_object *entry)
{
    union __cosem_entry_file fil;
    
    if(!entry)
    {
        return(0);
    }
    
    heap.set(entry, 0, sizeof(struct __cosem_object));
    
    if((sizeof(fil) * index) >= sizeof((struct __cosem_param *)0)->entry)
    {
        return(0);
    }
    
    //读取一条数据
    if(file.read("lexicon", \
                 STRUCT_OFFSET(struct __cosem_param, entry[index]), \
                 sizeof(fil), \
                 &fil) != sizeof(fil))
    {
        return(0);
    }
    
    //判断类号
    if(((fil.low.entry.key >> 56) & 0xff) <= 8)
    {
        //校验数据是否有效
        if(crc32(&fil.low.entry, sizeof(fil.low.entry)) != fil.low.check)
        {
            return(0);
        }
        
        //获取类的属性和方法数
        get_class_map(((fil.low.entry.key >> 56) & 0xff), &entry->amount_of_attr, &entry->amount_of_method);
        
        if(!(entry->amount_of_attr) && !(entry->amount_of_method))
        {
            return(0);
        }
        
        //组包返回数据
        entry->classid = ((fil.low.entry.key >> 56) & 0xff);
        entry->obis[0] = ((fil.low.entry.key >> 48) & 0xff);
        entry->obis[1] = ((fil.low.entry.key >> 40) & 0xff);
        entry->obis[2] = ((fil.low.entry.key >> 32) & 0xff);
        entry->obis[3] = ((fil.low.entry.key >> 24) & 0xff);
        entry->obis[4] = ((fil.low.entry.key >> 16) & 0xff);
        entry->obis[5] = ((fil.low.entry.key >> 8) & 0xff);
        entry->suit = ((fil.low.entry.key >> 0) & 0xff);
        
        heap.copy(&entry->right_attr[0][0], \
                  &fil.low.entry.right[0][0], \
                  sizeof(fil.low.entry.right[0])*entry->amount_of_attr);
        
        heap.copy(&entry->right_method[0][0], \
                  &fil.low.entry.right[entry->amount_of_attr][0], \
                  sizeof(fil.low.entry.right[0])*entry->amount_of_method);
    }
    else
    {
        //校验数据是否有效
        if(crc32(&fil.high.entry, sizeof(fil.high.entry)) != fil.high.check)
        {
            return(0);
        }
        
        //获取类的属性和方法数
        get_class_map(((fil.high.entry.key >> 56) & 0xff), &entry->amount_of_attr, &entry->amount_of_method);
        
        if(!(entry->amount_of_attr) && !(entry->amount_of_method))
        {
            return(0);
        }
        
        //组包返回数据
        entry->classid = ((fil.high.entry.key >> 56) & 0xff);
        entry->obis[0] = ((fil.high.entry.key >> 48) & 0xff);
        entry->obis[1] = ((fil.high.entry.key >> 40) & 0xff);
        entry->obis[2] = ((fil.high.entry.key >> 32) & 0xff);
        entry->obis[3] = ((fil.high.entry.key >> 24) & 0xff);
        entry->obis[4] = ((fil.high.entry.key >> 16) & 0xff);
        entry->obis[5] = ((fil.high.entry.key >> 8) & 0xff);
        entry->suit = ((fil.high.entry.key >> 0) & 0xff);
        
        heap.copy(&entry->right_attr[0][0], \
                  &fil.high.entry.right[0][0], \
                  sizeof(fil.high.entry.right[0])*entry->amount_of_attr);
        
        heap.copy(&entry->right_method[0][0], \
                  &fil.high.entry.right[entry->amount_of_attr][0], \
                  sizeof(fil.high.entry.right[0])*entry->amount_of_method);
    }
    
    return(sizeof(fil));
}
