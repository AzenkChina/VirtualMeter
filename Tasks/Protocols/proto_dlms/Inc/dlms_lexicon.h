/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_LEXICON_H__
#define __DLMS_LEXICON_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "proto_dlms.h"
#include "dlms_types.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void dlms_lex_parse(const struct __cosem_request_desc *desc, const char **table, uint8_t *index, uint32_t *param, union __dlms_right *right);

#endif /* __DLSM_LEXICON_H__ */
