/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_GET_INT_H
#define NAKEN_ASM_GET_INT_H

int get_int32_be(const uint8_t *buffer);
int get_int16_be(const uint8_t *buffer);
int get_int32_le(const uint8_t *buffer);
int get_int16_le(const uint8_t *buffer);

#endif

