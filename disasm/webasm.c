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

#include "disasm/webasm.h"
#include "table/webasm.h"

int get_cycle_count_webasm(unsigned short int opcode)
{
  return -1;
}

int disasm_webasm(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint8_t opcode;
  int n;

  instruction[0] = 0;

  *cycles_min = 0;
  *cycles_max = 0;

  opcode = memory_read_m(memory, address);

  n = 0;
  while(table_webasm[n].instr != NULL)
  {
    if (opcode != table_webasm[n].opcode)
    {
      n++;
      continue;
    }

    switch(table_webasm[n].type)
    {
      case WEBASM_OP_NONE:
        sprintf(instruction, "%s", table_webasm[n].instr);
        return 1;
      default:
        return 1;
    }
  }

  return 1;
}

void list_output_webasm(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  count = disasm_webasm(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  hex[0] = 0;

  for (n = 0; n < count; n++)
  {
    opcode = memory_read_m(&asm_context->memory, start + n);

    sprintf(temp, "%02x ", opcode);
    strcat(hex, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-20s %-40s\n", start, hex, instruction);
}

void disasm_range_webasm(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_webasm(memory, start, instruction, &cycles_min, &cycles_max);

     hex[0] = 0;

     for (n = 0; n < count; n++)
     {
       opcode = memory_read_m(memory, start + n);

       sprintf(temp, "%02x ", opcode);
       strcat(hex, temp);
     }

    printf("0x%04x: %-20s %-40s\n", start, hex, instruction);

    start += count;
  }
}

