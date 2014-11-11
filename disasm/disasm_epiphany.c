/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm_common.h"
#include "disasm_epiphany.h"
#include "table_epiphany.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) ((memory_read_m(memory, a)<<8)|(memory_read_m(memory, a+1)))
#define READ_RAM24(a) ((memory_read_m(memory, a)<<16)|(memory_read_m(memory, a+1)<<8)|(memory_read_m(memory, a+2)))

#define SINGLE_OPCODE(pre, op, cycles, size, instr) \
  if (opcode==op && prefix==pre) \
  { \
    strcpy(instruction, instr); \
    *cycles_min=cycles; \
    *cycles_max=cycles; \
    return size; \
  }

int get_cycle_count_epiphany(unsigned short int opcode)
{
  return -1;
}

int disasm_epiphany(struct _memory *memory, int address, char *instr, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  //int8_t offset;
  //char temp[128];
  int count = 1;
  int n;

  instr[0] = 0;

  opcode = READ_RAM16(address);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;
  while(1)
  {
    if (table_epiphany[n].opcode == (opcode & table_epiphany[n].mask))
    {
      switch(table_epiphany[n].type)
      {
        case OP_BRANCH:
        default:
          break;
      }
    }
    n++;
  }

  return count;
}

void list_output_epiphany(struct _asm_context *asm_context, int address)
{
  int cycles_min,cycles_max,count;
  char instruction[128];
  int n;

  fprintf(asm_context->list, "\n");
  count=disasm_epiphany(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  fprintf(asm_context->list, "0x%04x:", address);

  for (n = 0; n < 5; n++)
  {
    if (n < count)
    {
      fprintf(asm_context->list, " %02x", memory_read_m(&asm_context->memory, address+n));
    }
      else
    {
      fprintf(asm_context->list, "   ");
    }
  }
  fprintf(asm_context->list, " %-40s cycles: ", instruction);

  if (cycles_min == cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

}

void disasm_range_epiphany(struct _memory *memory, int start, int end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    num = READ_RAM(start) | (READ_RAM(start + 1) << 8);

    disasm_epiphany(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

#if 0
    count -= 4;
    while (count > 0)
    {
      start = start+4;
      num = READ_RAM(start) | (READ_RAM(start + 1) << 8);
      printf("0x%04x: 0x%04x\n", start, num);
      count -= 4;
    }
#endif

    start = start + 4;
  }
}

