/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "disasm_common.h"

unsigned int get_opcode16(struct _memory *memory, int address)
{
  if (memory->endian==ENDIAN_LITTLE)
  {
    return memory_read_m(memory, address)|
    (memory_read_m(memory, address+1)<<8);
  }
    else
  {
    return (memory_read_m(memory, address)<<8)|
    (memory_read_m(memory, address+1));
  }
}

#if 0
unsigned int get_opcode24(struct _memory *memory, int address)
{
  if (memory->endian==ENDIAN_LITTLE)
  {
    return memory_read_m(memory, address)|
    (memory_read_m(memory, address+1)<<8)|
    (memory_read_m(memory, address+2)<<16);
  }
    else
  {
    return (memory_read_m(memory, address+0)<<16)|
    (memory_read_m(memory, address+1)<<8)|
    (memory_read_m(memory, address+2));
  }
}
#endif

unsigned int get_opcode32(struct _memory *memory, int address)
{
  if (memory->endian==ENDIAN_LITTLE)
  {
    return memory_read_m(memory, address)|
    (memory_read_m(memory, address+1)<<8)|
    (memory_read_m(memory, address+2)<<16)|
    (memory_read_m(memory, address+3)<<24);
  }
    else
  {
    return (memory_read_m(memory, address)<<24)|
    (memory_read_m(memory, address+1)<<16)|
    (memory_read_m(memory, address+2)<<8)|
    (memory_read_m(memory, address+3));
  }
}



