/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/pic14.h"
#include "table/pic14.h"

int get_cycle_count_pic14(unsigned short int opcode)
{
  return -1;
}

int disasm_pic14(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int n;
  int f, d, b, k;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read16_m(memory, address);

  for (n = 0; table_pic14[n].instr != NULL; n++)
  {
    if ((opcode & table_pic14[n].mask) == table_pic14[n].opcode)
    {
      *cycles_min = table_pic14[n].cycles_min;
      *cycles_max = table_pic14[n].cycles_min;

      switch (table_pic14[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_pic14[n].instr);
          return 2;
        }
        case OP_F_D:
        {
          f = opcode & 0x7f;
          d = (opcode >> 7) & 1;
          sprintf(instruction, "%s 0x%02x, %c", table_pic14[n].instr, f, d == 1 ? 'f':'w');
          return 2;
        }
        case OP_F:
        {
          f = opcode & 0x7f;
          sprintf(instruction, "%s 0x%02x", table_pic14[n].instr, f);
          return 2;
        }
        case OP_F_B:
        {
          f = opcode & 0x7f;
          b = (opcode >> 7) & 7;
          sprintf(instruction, "%s 0x%02x, %d", table_pic14[n].instr, f, b);
          return 2;
        }
        case OP_K8:
        {
          k = opcode & 0xff;
          sprintf(instruction, "%s 0x%02x", table_pic14[n].instr, k);
          return 2;
        }
        case OP_K11:
        {
          k = opcode & 0x7ff;
          sprintf(instruction, "%s 0x%02x", table_pic14[n].instr, k);
          return 2;
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

  return 2;
}

void list_output_pic14(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  uint32_t opcode;
  char instruction[128];
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_pic14(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: 0x%04x %-40s cycles: ", start / 2, opcode, instruction);

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
    {
      fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
    }

    start += count;
  }
}

void disasm_range_pic14(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  uint16_t opcode;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    disasm_pic14(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: 0x%04x %-40s ", start / 2, opcode, instruction);

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
    {
      printf("%d-%d\n", cycles_min, cycles_max);
    }

    start = start + 2;
  }
}

