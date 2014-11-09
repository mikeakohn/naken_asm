/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#ifndef _TABLE_65816_H
#define _TABLE_65816_H

enum
{
  // a
  ABSOLUTE,

  // (a,x)
  ABSOLUTE_INDIRECT_X,

  // (a)
  ABSOLUTE_INDIRECT,

  // al
  ABSOLUTE_LONG,

  // al,x
  ABSOLUTE_LONG_X,

  // a,x
  ABSOLUTE_X,

  // a,y
  ABSOLUTE_Y,

  // A
  ACCUMULATOR,

  // xyc
  BLOCK_MOVE,

  // d
  DIRECT,

  // (d,x)
  DIRECT_INDIRECT_X,

  // (d)
  DIRECT_INDIRECT,

  // (d),y
  DIRECT_INDIRECT_Y,

  // [d],y
  DIRECT_INDIRECT_LONG_Y,

  // [d]
  DIRECT_INDIRECT_LONG,

  // d,x
  DIRECT_X,

  // d,y
  DIRECT_Y,

  // #
  IMMEDIATE,

  // i
  IMPLIED,

  // r
  RELATIVE,

  // rl
  RELATIVE_LONG,

  // s
  STACK,

  // d,s
  STACK_RELATIVE,

  // (d,s),y
  STACK_RELATIVE_INDIRECT_Y
};

struct _table_65816
{
  int opcode;
  char *instruction;
  int mode;
  int cycles_min;
  int cycles_max;
};

#endif

