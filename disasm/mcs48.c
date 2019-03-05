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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/mcs48.h"
#include "table/mcs48.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_mcs48(unsigned short int opcode)
{
  return -1;
}

int disasm_mcs48(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  //char temp[32];
  //int value;
  int n;

  opcode = memory_read_m(memory, address);

  n = 0;

  while(table_mcs48[n].name != NULL)
  { 
    if (table_mcs48[n].opcode == (opcode & table_mcs48[n].mask))
    {
      sprintf(instruction, "%s", table_mcs48[n].name);
    }

    n++;
  }

  return 0;
}

void list_output_mcs48(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min = -1, cycles_max = -1, count;
  char instruction[128];
  char temp[32];
  char temp2[4];
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_mcs48(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;
    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-10s %-40s cycles: ", start, temp, instruction);

#if 0
    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
#endif

    start += count;
  }
}

void disasm_range_mcs48(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char temp[32];
  char temp2[4];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    //num=READ_RAM(start)|(READ_RAM(start+1)<<8);

    count = disasm_mcs48(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;
    for (n = 0; n < count; n++)
    {
      //sprintf(temp2, "%02x ", READ_RAM(start+n));
      sprintf(temp2, "%02x ", memory_read_m(memory, start+n));
      strcat(temp, temp2);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: %-10s %-40s ?\n", start, temp, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %-10s %-40s %d\n", start, temp, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-10s %-40s %d-%d\n", start, temp, instruction, cycles_min, cycles_max);
    }

    start += count;
  }
}

