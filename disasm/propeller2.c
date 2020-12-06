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

#include "disasm/propeller2.h"
#include "table/propeller2.h"

#define READ_RAM32(a) (memory_read_m(memory, a + 3) << 24) | \
                      (memory_read_m(memory, a + 2) << 16) | \
                      (memory_read_m(memory, a + 1) << 8) | \
                       memory_read_m(memory, a + 0)

static const char *conditions[] =
{
  "_ret_ ",
  "if_nc_and_nz ",
  "if_nc_and_z ",
  "if_nc ",
  "if_c_and_nz ",
  "if_nz ",
  "if_c_ne_z ",
  "if_nc_or_nz ",
  "if_c_and_z ",
  "if_c_eq_z ",
  "if_z ",
  "if_nc_or_z ",
  "if_c ",
  "if_c_or_nz ",
  "if_c_or_z ",
  "",
};

int get_cycle_count_propeller2(uint32_t opcode)
{
  return -1;
}

int disasm_propeller2(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int n;
  int i, d, s;
  int cond;
  int wz, wc, wcz;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM32(address);

  i = (opcode >> 18) & 1;
  s = opcode & 0x1ff;
  d = (opcode >> 9) & 0x1ff;
  wc = (opcode >> 20) & 1;
  wz = (opcode >> 19) & 1;
  char operands[64];
  cond = (opcode >> 28) & 0xf;
  const char *condition = conditions[cond];
  const char *instr;

  if (wc == 1 && wz == 1)
  {
    wc = 0;
    wz = 0;
    wcz = 1;
  }

  operands[0] = 0;

  for (n = 0; table_propeller2[n].instr != NULL; n++)
  {
    if ((opcode & table_propeller2[n].mask) == table_propeller2[n].opcode)
    {
      *cycles_min = table_propeller2[n].cycles8_min;
      *cycles_max = table_propeller2[n].cycles8_min;
    }

    instr = table_propeller2[n].instr;

    int t;

    for (t = 0; t < table_propeller2[n].operand_count; t++)
    {
      char temp[16];

      if (t != 0) { strcat(operands, ", "); }

      switch (table_propeller2[n].operands[t])
      {
        case OP_D:
          sprintf(temp, "0x%x", d);
          strcat(operands, temp);
          break;
        case OP_NUM_D:
          if (i == 1) { strcat(operands, "#"); }
          sprintf(temp, "0x%x", d);
          strcat(operands, temp);
          break;
        case OP_NUM_S:
          if (i == 1) { strcat(operands, "#"); }
          sprintf(temp, "0x%x", s);
          strcat(operands, temp);
          break;
        case OP_NUM_SP:
        case OP_N_1:
        case OP_N_2:
        case OP_N_3:
        case OP_N_23:
        case OP_A:
        case OP_P:
          break;
        case OP_C:
          strcat(operands, "c");
          break;
        case OP_Z:
          strcat(operands, "z");
          break;
        default:
          printf("Internal Error: %s:%d\n", __FILE__, __LINE__);
          exit(1);
      }
    }

    n++;
  }

  if (wc == 1) { strcat(operands, ", wc"); }
  if (wz == 1) { strcat(operands, ", wz"); }
  if (wcz == 1) { strcat(operands, ", wcz"); }

  sprintf(instruction, "%s%s %s", condition, instr, operands);

  return 4;
}

void list_output_propeller2(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  uint32_t opcode;
  char instruction[128];
  char bytes[16];
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_propeller2(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read32_m(&asm_context->memory, start);
    sprintf(bytes, "0x%08x", opcode);

    fprintf(asm_context->list, "0x%04x: %-16s %-40s cycles: ", start / 4, bytes, instruction);

    if (cycles_min == 0)
    {
      fprintf(asm_context->list, "?\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      fprintf(asm_context->list, "%d\n", cycles_min);
    }
      else
    if (cycles_max < 1)
    {
      fprintf(asm_context->list, "%d or %d\n", cycles_min, -cycles_max);
    }
      else
    {
      fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
    }

    start += count;
  }
}

void disasm_range_propeller2(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char bytes[16];
  int cycles_min = 0,cycles_max = 0;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    disasm_propeller2(memory, start, instruction, &cycles_min, &cycles_max);

    sprintf(bytes, "0x%08x", READ_RAM32(start));
    printf("0x%04x: %-16s %-40s ", start / 4, bytes, instruction);

    if (cycles_min == 0)
    {
      printf("?\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("%d\n", cycles_min);
    }
      else
    if (cycles_max < 1)
    {
      printf("%d or %d\n", cycles_min, -cycles_max);
    }
      else
    {
      printf("%d-%d\n", cycles_min, cycles_max);
    }

    start = start + 4;
  }
}

