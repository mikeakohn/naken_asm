/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/arm64.h"
#include "table/arm64.h"

static char reg_size[] = { 'w', 'x' };
static char *vec_size[] = { "8b", "16b", "4h", "8h", "2s", "4s", "1d", "2d" };

int get_cycle_count_arm64(unsigned short int opcode)
{
  return -1;
}

int disasm_arm64(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int n;
  int rm, rn, rd;
  int size;

  opcode = memory_read32_m(memory, address);

  *cycles_min = -1;
  *cycles_max = -1;

  rm = (opcode >> 16) & 0x1f;
  rn = (opcode >> 5) & 0x1f;
  rd = opcode & 0x1f;
  size = (opcode >> 22) & 0x3;

  for (n = 0; table_arm64[n].instr != NULL; n++)
  {
    if ((opcode & table_arm64[n].mask) == table_arm64[n].opcode)
    {
      switch (table_arm64[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_arm64[n].instr);
          return 4;
        }
        case OP_MATH_R32_R32_R32:
        {
          sprintf(instruction, "%s w%d, w%d, w%d",
            table_arm64[n].instr, rd, rn, rm);
          return 4;
        }
        case OP_MATH_R64_R64_R64:
        {
          sprintf(instruction, "%s w%d, w%d, w%d",
            table_arm64[n].instr, rd, rn, rm);
          return 4;
        }
        case OP_SCALAR_R_R:
        {
          if (size > 1) { continue; }

          sprintf(instruction, "%s %c%d, %c%d",
            table_arm64[n].instr, reg_size[size], rd, reg_size[size], rn);

          return 4;
        }
        case OP_VECTOR_V_V:
        {
          size = (size << 1) | ((opcode >> 30) & 1);

          sprintf(instruction, "%s v%d.%s, v%d.%s",
            table_arm64[n].instr, rd, vec_size[size], rn, vec_size[size]);

          return 4;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  strcpy(instruction, "???");

  return 4;
}

void list_output_arm64(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint32_t opcode;
  int count;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_arm64(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read32_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: %04x %-40s\n", start / 2, opcode, instruction);

    start += count;
  }
}

void disasm_range_arm64(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
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
    count = disasm_arm64(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read32_m(memory, start);

    printf("0x%04x: %04x %-40s\n", start / 2, opcode, instruction);

    start = start + count;
  }
}

