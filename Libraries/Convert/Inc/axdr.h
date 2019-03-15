/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AXDR_H__
#define __AXDR_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief   AXDR 数据类型
  */
enum __axdr_type
{
    AXDR_NULL = 0,              // 0 
    AXDR_ARRAY,                 // 1 
    AXDR_STRUCTURE,             // 2 
    AXDR_BOOLEAN,               // 3 boolean
    AXDR_BIT_STRING,            // 4 An ordered sequence of boolean values
    AXDR_DOUBLE_LONG,           // 5 Integer32
    AXDR_DOUBLE_LONG_UNSIGNED,  // 6 Unsigned32
    
    AXDR_OCTET_STRING = 9,      // 9 An ordered sequence of octets (8 bit bytes)
    AXDR_VISIBLE_STRING,        // 10 An ordered sequence of ASCII characters
    
    AXDR_UTF8_STRING = 12,      // 11 An ordered sequence of characters encoded as UTF-8
    AXDR_BCD,                   // 12 binary coded decimal
    
    AXDR_INTEGER = 15,          // 15 Integer8
    AXDR_LONG,                  // 16 Integer16
    AXDR_UNSIGNED,              // 17 Unsigned8
    AXDR_LONG_UNSIGNED,         // 18 Unsigned16
    AXDR_COMPACT_ARRAY,         // 19 
    
    AXDR_LONG64 = 20,           // 20 Integer64
    AXDR_LONG64_UNSIGNED,       // 21 Unsigned64
    AXDR_ENUM,                  // 22 enum
    AXDR_FLOAT32,               // 23 OCTET STRING (SIZE(4))
    AXDR_FLOAT64,               // 24 OCTET STRING (SIZE(8))
    AXDR_DATE_TIME,             // 25 OCTET STRING SIZE(12))
    AXDR_DATE,                  // 26 OCTET STRING (SIZE(5))
    AXDR_TIME,                  // 27 OCTET STRING (SIZE(4))
};

/**
  * @brief   AXDR 数据容器
  */
union __axdr_container
{
    uint8_t     u8_t;
    int8_t      i8_t;
    uint16_t    u16_t;
    int16_t     i16_t;
    uint32_t    u32_t;
    int32_t     i32_t;
    uint64_t    u64_t;
    int64_t     i64_t;
    float       float_t;
    double      double_t;
};

/**
  * @brief   AXDR 编码接口
  */
struct __axdr_conv
{
    uint16_t                (*encode)(const void *src, uint16_t length, enum __axdr_type type, uint8_t *dst);
    uint16_t                (*decode)(const uint8_t *src, enum __axdr_type *type, void *dst);
    
    struct
    {
        uint8_t             (*encode)(enum __axdr_type type, uint8_t *dst);
        enum __axdr_type    (*decode)(const uint8_t *src);
        
    }                       type;
    
    struct
    {
        uint8_t             (*encode)(uint16_t length, uint8_t *dst);
        uint8_t             (*decode)(const uint8_t *src, uint16_t *length);
        uint16_t            (*calc)(const uint8_t *src);
        
    }                       length;
    
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define AXDR_CONTAINED(type)        ((type >= 5 && type <= 6)   || \
                                    (type >= 15 && type <= 18)  || \
                                    (type >= 20 && type <= 24))

/* Exported function prototypes ----------------------------------------------*/
extern const struct __axdr_conv axdr;

#endif /* __AXDR_H__ */
