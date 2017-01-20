/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _TABLE_TMS1000_H
#define _TABLE_TMS1000_H

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

