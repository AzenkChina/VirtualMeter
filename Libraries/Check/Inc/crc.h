#ifndef __CRC_H__
#define __CRC_H__

#include "stdint.h"

extern uint16_t crc16(const void *cp, uint16_t length);
extern uint32_t crc32(const void *cp, uint16_t length);

#endif /* __CRC_H__ */
