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
#define LEX_CONFIG_MAX_ATTR         (uint8_t)(24)//���֧�ֵ�������Ŀ
#define LEX_CONFIG_MAX_METHOD       (uint8_t)(12)//���֧�ֵķ�����Ŀ

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  ���� ��ֵ
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
  * @brief  Ȩ��
  */
struct __dlms_authority
{
    uint8_t none;//Ȩ��һ
    uint8_t low;//Ȩ�޶�
    uint8_t high;//Ȩ����
};

/**
  * @brief  cosem �������Ҫ��֤�ṹ�峤�Ȳ����ڣ�256-4���ֽڣ�
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
  * @brief  ����������
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
