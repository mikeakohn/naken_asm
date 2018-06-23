/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2018 by Michael Kohn
 *
 */

#ifndef _GET_INT_H
#define _GET_INT_H

int get_int32_be(uint8_t *buffer);
int get_int16_be(uint8_t *buffer);
int get_int32_le(uint8_t *buffer);
int get_int16_le(uint8_t *buffer);

#endif

