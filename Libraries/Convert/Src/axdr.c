/**
 * @brief		
 * @details		
 * @date		2018-09-06
 **/

/* Includes ------------------------------------------------------------------*/
#include "axdr.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  将 uint16_t 长度数据组包为AXDR编码格式
  */
static uint8_t axdr_encode_length(uint16_t length, uint8_t *dst)
{
    if(!dst)
    {
        return(0);
    }
    
	if(length > 255)
	{
        *(dst++) = 0x82;
        *(dst++) = (uint8_t)(length >> 8);
        *(dst++) = (uint8_t)(length & 0xff);
        
		return(3);
	}
	else
	{
		if(length < 128)
		{
            *(dst++) = (uint8_t)length;
            
			return(1);
		}
		else
		{
            *(dst++) = 0x81;
            *(dst++) = (uint8_t)length;
            
			return(2);
		}
	}
}

/**
  * @brief  将AXDR编码格式长度解析为 uint16_t 
  */
static uint8_t axdr_decode_length(const uint8_t *src, uint16_t *length)
{
    uint16_t tmp = 0;
    uint8_t len = 0;
    
    if((!src) || (!length))
    {
        return(0);
    }
    
    if(*src == 0x82)
    {
        tmp = *(src+1);
        tmp <<= 8;
        tmp += *(src+2);
        len = 3;
    }
    else if(*src == 0x81)
    {
        tmp = 128;
        tmp += (*(src+1) & 0x7f);
        len = 2;
    }
    else if(*src < 128)
    {
        tmp = *src;
        len = 1;
    }
    
    *length = tmp;
    
    return(len);
}

/**
  * @brief  计算AXDR格式编码的数据流的总字节长度（TLV结构中L+V的长度）
  * 不能计算 NULL、COMPACT ARRAY、ARRAY、STRUCTURE 的长度
  */
static uint16_t axdr_calc_length(const uint8_t *src)
{
    uint16_t len_dat = 0;
    uint8_t len_tag = 0;
    uint16_t result = 0;
    
    if(!src)
    {
        return(0);
    }
    
    switch((enum __axdr_type)*src)
    {
        case AXDR_NULL:
        case AXDR_COMPACT_ARRAY:
        case AXDR_ARRAY:
        case AXDR_STRUCTURE:
        {
            result = 0;
            break;
        }
        case AXDR_BIT_STRING:
        {
            len_tag = axdr_decode_length((src+1), &len_dat);
            result = len_tag + ((len_dat+7)/8);
            break;
        }
        case AXDR_BOOLEAN:
        case AXDR_INTEGER:
        case AXDR_UNSIGNED:
        case AXDR_ENUM:
        {
            result = 1;
            break;
        }
        case AXDR_LONG:
        case AXDR_LONG_UNSIGNED:
        {
            result = 2;
            break;
        }
        case AXDR_DOUBLE_LONG:
        case AXDR_DOUBLE_LONG_UNSIGNED:
        case AXDR_FLOAT32:
        case AXDR_TIME:
        {
            result = 4;
            break;
        }
        case AXDR_DATE:
        {
            result = 5;
            break;
        }
        case AXDR_LONG64:
        case AXDR_LONG64_UNSIGNED:
        case AXDR_FLOAT64:
        {
            result = 8;
            break;
        }
        case AXDR_DATE_TIME:
        {
            result = 12;
            break;
        }
        case AXDR_OCTET_STRING:
        case AXDR_VISIBLE_STRING:
        case AXDR_UTF8_STRING:
        case AXDR_BCD:
        {
            len_tag = axdr_decode_length((src+1), &len_dat);
            result = len_tag + len_dat;
            break;
        }
        default:
        {
            result = 0;
            break;
        }
    }
    
    return(result);
}

/**
  * @brief  组包AXDR类型
  * 
  */
static uint8_t axdr_type_encode(enum __axdr_type type, uint8_t *dst)
{
    if(!dst)
    {
        return(0);
    }
    
    switch(type)
    {
        case AXDR_NULL:
        case AXDR_ARRAY:
        case AXDR_STRUCTURE:
        case AXDR_BOOLEAN:
        case AXDR_BIT_STRING:
        case AXDR_DOUBLE_LONG:
        case AXDR_DOUBLE_LONG_UNSIGNED:
        case AXDR_VISIBLE_STRING:
        case AXDR_UTF8_STRING:
        case AXDR_BCD:
        case AXDR_INTEGER:
        case AXDR_LONG:
        case AXDR_UNSIGNED:
        case AXDR_LONG_UNSIGNED:
        case AXDR_COMPACT_ARRAY:
        case AXDR_LONG64:
        case AXDR_LONG64_UNSIGNED:
        case AXDR_ENUM:
        case AXDR_FLOAT32:
        case AXDR_FLOAT64:
        {
            *dst = (uint8_t)type;
            return(1);
        }
        case AXDR_OCTET_STRING:
        case AXDR_DATE_TIME:
        case AXDR_DATE:
        case AXDR_TIME:
        {
            *dst = (uint8_t)AXDR_OCTET_STRING;
            return(1);
        }
    }
    
    return(0);
}

/**
  * @brief  解析AXDR类型
  * 
  */
static enum __axdr_type axdr_type_decode(const uint8_t *src)
{
    if(!src)
    {
        return((enum __axdr_type)0xff);
    }
    
    switch(*src)
    {
        case AXDR_NULL:
        case AXDR_ARRAY:
        case AXDR_STRUCTURE:
        case AXDR_BOOLEAN:
        case AXDR_BIT_STRING:
        case AXDR_DOUBLE_LONG:
        case AXDR_DOUBLE_LONG_UNSIGNED:
        case AXDR_OCTET_STRING:
        case AXDR_VISIBLE_STRING:
        case AXDR_UTF8_STRING:
        case AXDR_BCD:
        case AXDR_INTEGER:
        case AXDR_LONG:
        case AXDR_UNSIGNED:
        case AXDR_LONG_UNSIGNED:
        case AXDR_COMPACT_ARRAY:
        case AXDR_LONG64:
        case AXDR_LONG64_UNSIGNED:
        case AXDR_ENUM:
        case AXDR_FLOAT32:
        case AXDR_FLOAT64:
        case AXDR_DATE_TIME:
        case AXDR_DATE:
        case AXDR_TIME:
        {
            return((enum __axdr_type)(*src));
        }
    }
    
    return((enum __axdr_type)0xff);
}

/**
  * @brief  组包AXDR格式数据
  * ARRAY、COMPACT ARRAY、STRUCTURE不可组包
  */
static uint16_t axdr_encode(const void *src, uint16_t length, enum __axdr_type type, uint8_t *dst)
{
    uint16_t length_encode = 0;
    
    if((!src) || (!dst))
    {
        return(0);
    }
    
    *(dst++) = type;
    length_encode = 1;
    
    switch(type)
    {
		case AXDR_NULL:
		{
			break;
		}
		case AXDR_BOOLEAN:
		case AXDR_INTEGER:
        case AXDR_UNSIGNED:
        case AXDR_ENUM:
		{
            *dst = *((uint8_t *)(src));
            length_encode += 1;
			break;
		}
		case AXDR_BIT_STRING:
		{
            uint8_t len_tag = axdr_encode_length(length, dst);
            length_encode += len_tag;
            memcpy((dst+len_tag), src, ((length+7)/8));
            length_encode += ((length+7)/8);
			break;
		}
		case AXDR_DOUBLE_LONG:
        case AXDR_DOUBLE_LONG_UNSIGNED:
        case AXDR_FLOAT32:
		{
            const uint32_t *val = (const uint32_t *)src;
            *(dst++) = ((*val >> 24) & 0xff);
            *(dst++) = ((*val >> 16) & 0xff);
            *(dst++) = ((*val >> 8) & 0xff);
            *(dst++) = ((*val >> 0) & 0xff);
            length_encode += 4;
			break;
		}
		case AXDR_OCTET_STRING:
		case AXDR_VISIBLE_STRING:
		case AXDR_UTF8_STRING:
		case AXDR_BCD:
		{
            uint8_t len_tag = axdr_encode_length(length, dst);
            length_encode += len_tag;
            memcpy((dst+len_tag), src, length);
            length_encode += length;
			break;
		}
		case AXDR_LONG:
		case AXDR_LONG_UNSIGNED:
		{
            const uint16_t *val = (const uint16_t *)src;
            *(dst++) = ((*val >> 8) & 0xff);
            *(dst++) = ((*val >> 0) & 0xff);
            length_encode += 2;
			break;
		}
		case AXDR_LONG64:
		case AXDR_LONG64_UNSIGNED:
        case AXDR_FLOAT64:
		{
            const uint64_t *val = (const uint64_t *)src;
            *(dst++) = ((*val >> 56) & 0xff);
            *(dst++) = ((*val >> 48) & 0xff);
            *(dst++) = ((*val >> 40) & 0xff);
            *(dst++) = ((*val >> 32) & 0xff);
            *(dst++) = ((*val >> 24) & 0xff);
            *(dst++) = ((*val >> 16) & 0xff);
            *(dst++) = ((*val >> 8) & 0xff);
            *(dst++) = ((*val >> 0) & 0xff);
            length_encode += 8;
			break;
		}
		case AXDR_DATE_TIME:
		{
            uint8_t len_tag = axdr_encode_length(12, dst);
            length_encode += len_tag;
            //...将时间戳转换为DATE_TIME，暂未实现
            length_encode += 12;
			break;
		}
		case AXDR_DATE:
		{
            uint8_t len_tag = axdr_encode_length(5, dst);
            length_encode += len_tag;
            //...将时间戳转换为DATE，暂未实现
            length_encode += 5;
			break;
		}
		case AXDR_TIME:
		{
            uint8_t len_tag = axdr_encode_length(4, dst);
            length_encode += len_tag;
            //...将时间戳转换为TIME，暂未实现
            length_encode += 4;
			break;
		}
		case AXDR_ARRAY:
        case AXDR_COMPACT_ARRAY:
		case AXDR_STRUCTURE:
        default:
        {
            return(0);
        }
    }
    
    return(length_encode);
}

/**
  * @brief  解包AXDR格式数据
  * NULL、COMPACT ARRAY、ARRAY、STRUCTURE 不可解包
  * 不定长数据类型，返回数据域首地址
  */
static uint16_t axdr_decode(const uint8_t *src, enum __axdr_type *type, void *dst)
{
    uint16_t length_decode = 0;
    
    if((!src) || (!dst))
    {
        return(0);
    }
    
    if(type)
    {
        *type = (enum __axdr_type)(*src);
    }
    
    switch((enum __axdr_type)(*src))
    {
        case AXDR_NULL:
		{
			break;
		}
		case AXDR_BOOLEAN:
		case AXDR_INTEGER:
        case AXDR_UNSIGNED:
        case AXDR_ENUM:
		{
            *((uint8_t *)(dst)) = *(src+1);
            length_decode += 1;
			break;
		}
		case AXDR_DOUBLE_LONG:
        case AXDR_DOUBLE_LONG_UNSIGNED:
        case AXDR_FLOAT32:
		{
            uint32_t *val = (uint32_t *)dst;
            *val = *(src+1);
            *val <<= 8;
            *val += *(src+2);
            *val <<= 8;
            *val += *(src+3);
            *val <<= 8;
            *val += *(src+4);
            length_decode += 4;
			break;
		}
		case AXDR_BIT_STRING:
        case AXDR_OCTET_STRING:
		case AXDR_VISIBLE_STRING:
		case AXDR_UTF8_STRING:
		case AXDR_BCD:
		{
            uint16_t len_data = 0;
            uint8_t len_tag = axdr_decode_length((src+1), &len_data);
            length_decode += len_tag;
            dst = (void *)(src+1+len_tag);
            length_decode += len_data;
			break;
		}
		case AXDR_LONG:
		case AXDR_LONG_UNSIGNED:
		{
            uint16_t *val = (uint16_t *)dst;
            *val = *(src+1);
            *val <<= 8;
            *val += *(src+2);
            length_decode += 2;
			break;
		}
		case AXDR_LONG64:
		case AXDR_LONG64_UNSIGNED:
        case AXDR_FLOAT64:
		{
            uint64_t *val = (uint64_t *)dst;
            *val = *(src+1);
            *val <<= 8;
            *val += *(src+2);
            *val <<= 8;
            *val += *(src+3);
            *val <<= 8;
            *val += *(src+4);
            *val <<= 8;
            *val += *(src+5);
            *val <<= 8;
            *val += *(src+6);
            *val <<= 8;
            *val += *(src+7);
            *val <<= 8;
            *val += *(src+8);
            length_decode += 8;
			break;
		}
		case AXDR_DATE_TIME:
		case AXDR_DATE:
		case AXDR_TIME:
		{
            //...转换为时间戳，暂未实现
			break;
		}
		case AXDR_ARRAY:
        case AXDR_COMPACT_ARRAY:
		case AXDR_STRUCTURE:
        default:
        {
            return(0);
        }
    }
    
    return(length_decode);
}











const struct __axdr_conv axdr = 
{
    .encode             = axdr_encode,
    .decode             = axdr_decode,
    
    .type               = 
    {
        .encode         = axdr_type_encode,
        .decode         = axdr_type_decode,
    },
    
    .length             = 
    {
        .encode         = axdr_encode_length,
        .decode         = axdr_decode_length,
        .calc           = axdr_calc_length,
        
    },
};



