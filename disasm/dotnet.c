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

#include "disasm/dotnet.h"
//#include "table/dotnet.h"

int get_cycle_count_dotnet(unsigned short int opcode)
{
  return -1;
}

int disasm_dotnet(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  *cycles_min = 0;
  *cycles_max = 0;

  return 1;
}

void list_output_dotnet(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode;

  fprintf(asm_context->list, "\n");

  disasm_dotnet(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);
  opcode = memory_read_m(&asm_context->memory, start);

  fprintf(asm_context->list, "0x%04x: %04x %-40s", start, opcode, instruction);
}

void disasm_range_dotnet(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int opcode;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_dotnet(memory, start, instruction, &cycles_min, &cycles_max);
    opcode = memory_read_m(memory, start);

    printf("0x%04x: %04x %-40s\n", start, opcode, instruction);

    start += count;
  }
}

