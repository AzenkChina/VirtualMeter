/**
 * @brief		
 * @details		
 * @date		2018-09-08
 **/

/* Includes ------------------------------------------------------------------*/
#include "dlms_lexicon.h"
#include "types_metering.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/
#define LEX_CONFIG_MAX_ATTR         (uint8_t)(24)//最多支持的属性数目
#define LEX_CONFIG_MAX_METHOD       (uint8_t)(12)//最多支持的方法数目

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  属性 描述符
  */
struct __dlms_attr
{
    uint8_t         table[2];//函数表索引
    uint8_t         index[2];//函数索引
    
    struct
    {
        uint16_t    lowest:4;
        uint16_t    low:4;
        uint16_t    high:4;
        uint16_t    reserve:4;
        
    }               right;
    
    uint32_t        param;//输入参数
};

/**
  * @brief  方法 描述符
  */
struct __dlms_method
{
    uint8_t         table;//函数表索引
    uint8_t         index;//函数索引
    
    struct
    {
        uint16_t    lowest:4;
        uint16_t    low:4;
        uint16_t    high:4;
        uint16_t    reserve:4;
        
    }               right;
};

/**
  * @brief  cosem 数据项
  */
struct __cosem_entry
{
    uint8_t suit;
    uint8_t classid;
    uint8_t obis[6];
    
    struct __dlms_attr attr[LEX_CONFIG_MAX_ATTR];
    struct __dlms_method method[LEX_CONFIG_MAX_METHOD];
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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
