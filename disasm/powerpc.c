/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/common.h"
#include "disasm/powerpc.h"
#include "table/powerpc.h"

//#define READ_RAM(a) memory_read_m(memory, a)
//#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)
#define READ_RAM(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a)<<16)|(memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

int get_cycle_count_powerpc(unsigned short int opcode)
{
  return -1;
}

int disasm_powerpc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  *cycles_min = -1;
  *cycles_max = -1;

  return -1;
}

void list_output_powerpc(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char bytes[10];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_powerpc(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(bytes, temp);
    }

    fprintf(asm_context->list, "0x%04x: %-9s %-40s cycles: ", start, bytes, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += count;
  }
}

void disasm_range_powerpc(struct _memory *memory, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_powerpc(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", READ_RAM(start + n));
      strcat(bytes, temp);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: %-9s %-40s ?\n", start, bytes, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %-9s %-40s %d\n", start, bytes, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-9s %-40s %d-%d\n", start, bytes, instruction, cycles_min, cycles_max);
    }

    start = start + count;
  }
}


