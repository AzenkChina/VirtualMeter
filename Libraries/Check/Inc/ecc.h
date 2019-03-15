/*
 *  drivers/mtd/nand_ecc.h
 *
 *  Copyright (C) 2000-2010 Steven J. Hill <sjhill@realitydiluted.com>
 *			    David Woodhouse <dwmw2@infradead.org>
 *			    Thomas Gleixner <tglx@linutronix.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This file is the header for the ECC algorithm.
 */

#ifndef __ECC_H__
#define __ECC_H__

#include "stdint.h"

/*
 * Calculate 3 byte ECC code for eccsize byte block
 */
void __nand_calculate_ecc(const uint8_t *dat, uint32_t eccsize, uint8_t *ecc_code);

/*
 * Detect and correct a 1 bit error for eccsize byte block
 */
int8_t __nand_correct_data(uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc, uint32_t eccsize);



#endif /* __ECC_H__ */
