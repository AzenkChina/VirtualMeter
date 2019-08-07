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
#define LEX_CONFIG_MAX_ATTR         (uint8_t)(24)//���֧�ֵ�������Ŀ
#define LEX_CONFIG_MAX_METHOD       (uint8_t)(12)//���֧�ֵķ�����Ŀ

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  ���� ������
  */
struct __dlms_attr
{
    uint8_t         table[2];//����������
    uint8_t         index[2];//��������
    
    struct
    {
        uint16_t    lowest:4;
        uint16_t    low:4;
        uint16_t    high:4;
        uint16_t    reserve:4;
        
    }               right;
    
    uint32_t        param;//�������
};

/**
  * @brief  ���� ������
  */
struct __dlms_method
{
    uint8_t         table;//����������
    uint8_t         index;//��������
    
    struct
    {
        uint16_t    lowest:4;
        uint16_t    low:4;
        uint16_t    high:4;
        uint16_t    reserve:4;
        
    }               right;
};

/**
  * @brief  cosem ������
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
  * @brief  �����ɱ��Ľ��������� struct __cosem_request_desc �ṹ����λ���������ĸ����Լ�������
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
  * @brief  ��ȡ��Ŀ����
  */
uint32_t dlms_lex_amount(enum __dlms_class val)
{
    uint32_t amount = 0;
    
    return(amount);
}
