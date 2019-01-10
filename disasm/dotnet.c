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
#include "table/dotnet.h"

int get_cycle_count_dotnet(unsigned short int opcode)
{
  return -1;
}

int disasm_dotnet(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint8_t opcode;
  int n;

  instruction[0] = 0;

  *cycles_min = 0;
  *cycles_max = 0;

  opcode = memory_read_m(memory, address);

  if (opcode == 0xfe)
  {
    opcode = memory_read_m(memory, address + 1);

    n = 0;
    while(table_dotnet_fe[n].instr != NULL)
    {
      if (opcode != table_dotnet_fe[n].opcode)
      {
        n++;
        continue;
      }

      switch(table_dotnet_fe[n].type)
      {
        case DOTNET_OP_NONE:
          sprintf(instruction, "%s", table_dotnet_fe[n].instr);
          return 2;
        default:
          return 2;
      }
    }
  }

  n = 0;
  while(table_dotnet[n].instr != NULL)
  {
    if (opcode != table_dotnet[n].opcode)
    {
      n++;
      continue;
    }

    switch(table_dotnet[n].type)
    {
      case DOTNET_OP_NONE:
        sprintf(instruction, "%s", table_dotnet[n].instr);
        return 1;
      default:
        return 1;
    }
  }

  return 1;
}

void list_output_dotnet(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  count = disasm_dotnet(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  hex[0] = 0;

  for (n = 0; n < count; n++)
  {
    opcode = memory_read_m(&asm_context->memory, start + n);

    sprintf(temp, "%02x ", opcode);
    strcat(hex, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-20s %-40s\n", start, hex, instruction);
}

void disasm_range_dotnet(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
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
    count = disasm_dotnet(memory, start, instruction, &cycles_min, &cycles_max);

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

