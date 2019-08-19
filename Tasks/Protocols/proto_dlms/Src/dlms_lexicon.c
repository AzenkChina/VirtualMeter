/**
 * @brief		
 * @details		
 * @date		2018-09-08
 **/

/* Includes ------------------------------------------------------------------*/
#include "dlms_lexicon.h"
#include "dlms_types.h"
#include "mids.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/
#define LEX_CONFIG_MAX_ATTR         (uint8_t)(24)//最多支持的属性数目
#define LEX_CONFIG_MAX_METHOD       (uint8_t)(12)//最多支持的方法数目

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  对象 键值
  */
struct __dlms_key
{
	uint8_t id;//class id
	uint8_t a;//obis a
	uint8_t b;//obis b
	uint8_t c;//obis c
	uint8_t d;//obis d
	uint8_t e;//obis e
	uint8_t f;//obis f
	uint8_t suit;//suit
};

/**
  * @brief  权限
  */
struct __dlms_authority
{
    uint8_t none;//权限一
    uint8_t low;//权限二
    uint8_t high;//权限三
};

/**
  * @brief  cosem 数据项（需要保证结构体长度不大于（256-4）字节）
  */
struct __cosem_entry
{
    uint8_t key[8];//8
    uint32_t mid[LEX_CONFIG_MAX_ATTR];//4*24
    uint8_t attr[3*LEX_CONFIG_MAX_ATTR];//3*24
    uint8_t method[3*LEX_CONFIG_MAX_METHOD];//3*12
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
  * @brief  本地数据项
  */
static const struct __cosem_entry communal[] = 
{
    /** cosem logical device name */
    {
        {0x01, 0x00, 0x00, 0x2a, 0x00, 0x00, 0xff, 0xff},\

        {M_ID2U(FMT_ASCII, 0, 0, 0, 0, 0)},\

        {ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ},\
        {METHOD_NONE, METHOD_NONE, METHOD_NONE},
    },
    
    /** clock */
    {
        {0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0xff, 0xff},\

        {0, 0, 0, 0, 0, 0, 0, 0},\

        {ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE),\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE),\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE),\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE),\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE),\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE),\
        ATTR_READ, ATTR_READ, (ATTR_READ | ATTR_WRITE)},\

        {METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE},
    },
    
    /** association ln */
    {
        {0x0f, 0x00, 0x00, 0x28, 0x00, 0x00, 0xff, 0xff},\

        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},\

        {ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_NONE, (ATTR_READ | ATTR_WRITE), (ATTR_READ | ATTR_WRITE), \
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ,\
        ATTR_READ, ATTR_READ, ATTR_READ},\

        {METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ,\
        METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE},
    },
    
    /** security setup */
    {
        {0x40, 0x00, 0x00, 0x2B, 0x00, 0x00, 0xff, 0xff},\

        {0, 0, 0, 0, 0},\

        {ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ,\
        ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ,\
        ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ,\
        ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ,\
        ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ,\
        ATTR_NONE, ATTR_NONE, METHOD_AUTHREQ},\

        {METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_AUTHREQ,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE,\
        METHOD_NONE, METHOD_NONE, METHOD_NONE},
    },
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  输入由报文解析出来的 struct __cosem_request_desc 结构，定位函数属于哪个表以及索引号
  */
void dlms_lex_parse(const struct __cosem_request_desc *desc,
                    const char **table,
                    uint8_t *index,
                    uint32_t *param)
{
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
    
    return;
}

/**
  * @brief  获取条目总数
  */
uint32_t dlms_lex_amount(enum __dlms_class val)
{
    uint32_t amount = 0;
    
    return(amount);
}
