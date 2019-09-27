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
#include "cpu.h"
#include "crc.h"
#include "mbedtls/md5.h"

/* Private define ------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(4)

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
    uint8_t right[16][3];//3*16
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
    uint8_t right[24][3];//3*24
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
    uint64_t key;
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
	uint16_t reserve;
    uint32_t check; //crc32校验
};

/**
  * @brief  cosem 数据项文件信息
  */
struct __cosem_param_info
{
    uint64_t version; //数据发布版本
    uint64_t date; //数据发布时间（时间戳）
    uint8_t md5[16];//amount 个 entry 的md5校验
    uint32_t check; //crc32校验
};

/**
  * @brief  cosem 数据项存储空间分布
  */
struct __cosem_param
{
    struct __cosem_param_header header;
    struct __cosem_param_info info;
	uint8_t reserve[sizeof(union __cosem_entry_file) - sizeof(struct __cosem_param_header) - sizeof(struct __cosem_param_info)];
    union __cosem_entry_file entry[(64*1024)/sizeof(union __cosem_entry_file) - 1];
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
  * @brief  本地数据项表
  */
static const struct __cosem_entry_high communal[] = 
{
    /** image transfer */
    /** 用于下载lexicon参数文件到系统 */
    {
        0x1200002C0080FF80,/** suit 8 only */

        0xffffff00, //oid

        {
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 1
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 2
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 3
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 4
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 5
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 6
            {ATTR_NONE, ATTR_READ, ATTR_READ}, //attribute 7

            {METHOD_NONE, METHOD_ACCESS, METHOD_ACCESS}, //method 1
            {METHOD_NONE, METHOD_ACCESS, METHOD_ACCESS}, //method 2
            {METHOD_NONE, METHOD_ACCESS, METHOD_ACCESS}, //method 3
            {METHOD_NONE, METHOD_ACCESS, METHOD_ACCESS}, //method 4
        },
    },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  计算 log2(n)
  */
static uint16_t fastlog2(uint16_t x)
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
    
    if((attr + method) > 16)
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
    
    if((attr + method) > 24)
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
    uint16_t step;
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
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, header), sizeof(struct __cosem_param_header), &header) != \
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
    step = position;
    
    for(cnt=0; cnt<fastlog2(header.amount); cnt++)
    {
        cpu.watchdog.feed();
        
        //读取
        if(file.read("lexicon", \
                     STRUCT_OFFSET(struct __cosem_param, entry[position]), \
                     sizeof(entry), \
                     &entry) != sizeof(entry))
        {
            break;
        }
        
        step = step / 2;
        
        if(step == 0)
        {
            step = 1;
        }
        
        //键值比对
        if(key < (entry.key & 0xffffffffffffff00))
        {
            if(position > step)
            {
                position -= step;
            }
            else
            {
                position = 0;
            }
            
            continue;
        }
        else if(key > (entry.key & 0xffffffffffffff00))
        {
            if((position + step) < header.amount)
            {
                position += step;
            }
            else
            {
                position = header.amount - 1;
            }
            
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
    uint16_t amount = 0;
    struct __cosem_param_header header;
	
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
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, header), sizeof(struct __cosem_param_header), &header) != \
        sizeof(struct __cosem_param_header))
    {
        return(amount);
    }
    else if(crc32(&header, (sizeof(struct __cosem_param_header) - sizeof(uint32_t))) != \
            header.check)
    {
        return(amount);
    }
	
	if(suit == 0xff)
	{
		amount = header.amount;
	}
	else
	{
		for(cnt=0; cnt<8; cnt++)
		{
			if((suit >> cnt) & 0x01)
			{
				amount = header.spread[cnt];
				break;
			}
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
    if(((fil.key >> 56) & 0xff) <= 8)
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

/**
  * @brief  获取条目信息文件的版本
  */
uint64_t dlms_lex_version(void)
{
    struct __cosem_param_info info;
    
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, info), sizeof(struct __cosem_param_info), &info) != \
        sizeof(struct __cosem_param_info))
    {
        return(0);
    }
    if(crc32(&info, (sizeof(struct __cosem_param_info) - sizeof(uint32_t))) != \
            info.check)
    {
        return(0);
    }
    
    return(info.version);
}

/**
  * @brief  获取条目信息文件的发布日期
  */
uint64_t dlms_lex_date(void)
{
    struct __cosem_param_info info;
    
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, info), sizeof(struct __cosem_param_info), &info) != \
        sizeof(struct __cosem_param_info))
    {
        return(0);
    }
    if(crc32(&info, (sizeof(struct __cosem_param_info) - sizeof(uint32_t))) != \
            info.check)
    {
        return(0);
    }
    
    return(info.date);
}

/**
  * @brief  获取条目信息文件的md5签名
  */
uint8_t dlms_lex_signature(uint8_t *signature)
{
    struct __cosem_param_info info;
    
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, info), sizeof(struct __cosem_param_info), &info) != \
        sizeof(struct __cosem_param_info))
    {
        return(0);
    }
    if(crc32(&info, (sizeof(struct __cosem_param_info) - sizeof(uint32_t))) != \
            info.check)
    {
        return(0);
    }
    
    heap.copy(signature, info.md5, 16);
    
    return(16);
}

/**
  * @brief  验证条目信息文件是否有效
  */
bool dlms_lex_check(void)
{
    uint16_t cnt;
    uint64_t accumulate = 0;
    struct __cosem_param_info info;
    struct __cosem_param_header header;
    union __cosem_entry_file entry;
    mbedtls_md5_context ctx;
    uint8_t md5_output[16] = {0};
    
    //读取参数文件中的信息头，并检查是否正确
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, header), sizeof(struct __cosem_param_header), &header) != \
        sizeof(struct __cosem_param_header))
    {
        return(false);
    }
    
    if(crc32(&header, (sizeof(struct __cosem_param_header) - sizeof(uint32_t))) != \
            header.check)
    {
        return(false);
    }
    
	for(cnt=0; cnt<8; cnt++)
	{
		if(header.spread[cnt] > header.amount)
		{
			return(false);
		}
	}
    
    //读取参数文件中的信息，并检查是否正确
    if(file.read("lexicon", STRUCT_OFFSET(struct __cosem_param, info), sizeof(struct __cosem_param_info), &info) != \
        sizeof(struct __cosem_param_info))
    {
        return(false);
    }
    
    if(crc32(&info, (sizeof(struct __cosem_param_info) - sizeof(uint32_t))) != \
            info.check)
    {
        return(false);
    }
    
    mbedtls_md5_init(&ctx);
    if(mbedtls_md5_starts_ret(&ctx) != 0 )
    {
        goto exit;
    }
    
	for(cnt=0; cnt<header.amount; cnt++)
	{
        cpu.watchdog.feed();
        
        //读取
        if(file.read("lexicon", \
                     STRUCT_OFFSET(struct __cosem_param, entry[cnt]), \
                     sizeof(entry), \
                     &entry) != sizeof(entry))
        {
            goto exit;
        }
        
        if(mbedtls_md5_update_ret(&ctx, (const unsigned char *)&entry, sizeof(entry) ) != 0)
        {
            goto exit;
        }
        
        if((entry.key & 0xffffffffffffff00) < accumulate)
        {
            goto exit;
        }
        
        accumulate = (entry.key & 0xffffffffffffff00);
        
        if(((entry.key >> 56) & 0xff) <= 8)
        {
            //校验
            if(crc32(&entry.low.entry, sizeof(entry.low.entry)) != entry.low.check)
            {
                goto exit;
            }
        }
        else
        {
            //校验
            if(crc32(&entry.high.entry, sizeof(entry.high.entry)) != entry.high.check)
            {
                goto exit;
            }
        }
	}

    if(mbedtls_md5_finish_ret(&ctx, md5_output) != 0)
    {
        goto exit;
    }
    
    if(memcmp(md5_output, info.md5, 16) != 0)
    {
        goto exit;
    }
    
    
    mbedtls_md5_free(&ctx);
    return(true);
    
exit:
    mbedtls_md5_free(&ctx);
    return( false );
}

#pragma pack(pop)
