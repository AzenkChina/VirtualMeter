/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_LEXICON_H__
#define __DLMS_LEXICON_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "proto_dlms.h"
#include "dlms_types.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  cosem КэѕЭПо
  */
struct __cosem_object
{
    uint8_t classid;
    uint8_t obis[6];
    uint8_t suit;
    
    uint8_t amount_of_attr;
    uint8_t amount_of_method;
    
    uint8_t right_attr[24][3];
    uint8_t right_method[16][3];
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void dlms_lex_parse(const struct __cosem_request_desc *desc,
                           union __dlms_right *right,
                           uint32_t *oid,
                           uint32_t *mid);
extern uint16_t dlms_lex_amount(uint8_t suit);
extern uint16_t dlms_lex_entry(uint16_t index, struct __cosem_object *entry);

#endif /* __DLSM_LEXICON_H__ */
