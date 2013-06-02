/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 * 65XX file by Joe Davisson
 *
 */

#ifndef _TABLE_65XX_H
#define _TABLE_65XX_H

struct _opcodes_65xx
{
  char *name;
  int opcode[12];
};

struct _cycles_65xx
{
  int opcode;
  int cycles;
  int inc;
  int mode;
};

#endif

