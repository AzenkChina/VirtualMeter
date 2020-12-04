#ifndef __CRC_H__
#define __CRC_H__

#include "stdint.h"

extern uint16_t crc16(const void *cp, uint16_t length, uint16_t init);
extern uint32_t crc32(const void *cp, uint16_t length, uint32_t init);

#endif /* __CRC_H__ */
