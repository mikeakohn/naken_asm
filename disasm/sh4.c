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

#include "disasm/sh4.h"
#include "table/sh4.h"

#define READ_RAM16(a) (memory_read_m(memory, a + 0) << 8) | \
                       memory_read_m(memory, a + 1)

int get_cycle_count_sh4(unsigned short int opcode)
{
  return -1;
}

int disasm_sh4(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint16_t opcode;
  int n;

  opcode = READ_RAM16(address);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;
  while (table_sh4[n].instr != NULL)
  {
    if ((opcode & table_sh4[n].mask) == table_sh4[n].opcode)
    {
      switch (table_sh4[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_sh4[n].instr);
          return 2;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }

    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_sh4(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_sh4(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = (memory_read_m(&asm_context->memory, start + 0) << 8) |
              memory_read_m(&asm_context->memory, start + 1);

    fprintf(asm_context->list, "0x%04x: %04x %-40s\n", start / 2, opcode, instruction);

    start += count;
  }
}

void disasm_range_sh4(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_sh4(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = READ_RAM16(start);

    printf("0x%04x: %04x %-40s\n", start / 2, opcode, instruction);

    start = start + count;
  }
}

