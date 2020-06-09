/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_TMS1000_H
#define NAKEN_ASM_TABLE_TMS1000_H

#include "common/assembler.h"

struct _table_tms1000
{
  const char *instr;
  unsigned short int op1000;
  unsigned short int op1100;
};

extern struct _table_tms1000 table_tms1000[];
extern int tms1000_reverse_constant[];
extern int tms1000_reverse_bit_address[];
extern uint8_t tms1000_address_to_lsfr[];
extern uint8_t tms1000_lsfr_to_address[];

#endif

