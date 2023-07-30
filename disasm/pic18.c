/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/pic18.h"
#include "table/pic18.h"

int disasm_pic18(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int n;
  int f, d, a, b, s;
  int offset, value;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read16_m(memory, address);

  for (n = 0; table_pic18[n].instr != NULL; n++)
  {
    if ((opcode & table_pic18[n].mask) == table_pic18[n].opcode)
    {
      *cycles_min = table_pic18[n].cycles_min;
      *cycles_max = table_pic18[n].cycles_min;

      switch (table_pic18[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_pic18[n].instr);
          return 2;
        }
        case OP_F_D_A:
        {
          f = opcode & 0xff;
          d = (opcode >> 9) & 1;
          a = (opcode >> 8) & 1;

          sprintf(instruction, "%s 0x%02x, %c, %d",
            table_pic18[n].instr,
            f,
            d == 1 ? 'f' : 'w',
            a);

          return 2;
        }
        case OP_F_A:
        {
          f = opcode & 0xff;
          a = (opcode >> 8) & 1;

          sprintf(instruction, "%s 0x%02x, %d",
            table_pic18[n].instr,
            f,
            a);

          return 2;
        }
        case OP_FS_FD:
        {
          f = opcode & 0xfff;
          opcode = memory_read16_m(memory, address + 2);

          sprintf(instruction, "%s 0x%03x, 0x%03x",
            table_pic18[n].instr,
            f,
            opcode & 0xfff);

          return 4;
        }
        case OP_F_B_A:
        {
          f = opcode & 0x7f;
          a = (opcode >> 8) & 1;
          b = (opcode >> 9) & 7;

          sprintf(instruction, "%s 0x%02x, %d, %d",
            table_pic18[n].instr,
            f,
            b,
            a);

          return 2;
        }
        case OP_BRANCH_8:
        {
          offset = opcode & 0xff;
          if ((offset & 0x80) != 0) { offset |= 0xffffff00; }

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_pic18[n].instr,
            address + 2 + (offset * 2),
            offset);

          return 2;
        }
        case OP_BRANCH_11:
        {
          offset = opcode & 0x7ff;
          if ((offset & 0x400) != 0) { offset |= 0xfffff800; }

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_pic18[n].instr,
            address + 2 + (offset * 2),
            offset);

          return 2;
        }
        case OP_CALL:
        {
          s = (opcode >> 8) & 1;
          value = opcode & 0xff;
          opcode = memory_read16_m(memory, address + 2);
          value |= (opcode & 0xfff) << 8;

          sprintf(instruction, "%s 0x%04x%s",
            table_pic18[n].instr,
            value,
            s == 1 ? ", s" : "");

          return 4;
        }
        case OP_GOTO:
        {
          value = opcode & 0xff;
          opcode = memory_read16_m(memory, address + 2);
          value |= (opcode & 0xfff) << 8;

          sprintf(instruction, "%s 0x%04x",
            table_pic18[n].instr,
            value);

          return 4;
        }
        case OP_S:
        {
          s = (opcode >> 8) & 1;

          sprintf(instruction, "%s%s",
            table_pic18[n].instr,
            s == 1 ? ", s" : "");

          return 4;
        }
        case OP_K8:
        {
          value = opcode & 0xff;
          sprintf(instruction, "%s 0x%02x", table_pic18[n].instr, value);
          return 2;
        }
        case OP_F_K12:
        {
          f = (opcode >> 4) & 0x3;
          value = opcode & 0xf;
          opcode = memory_read16_m(memory, address + 2);
          value |= (opcode & 0xff) << 4;

          sprintf(instruction, "%s %d, 0x%02x",
            table_pic18[n].instr,
            f,
            value);

          return 4;
        }
        case OP_K4:
        {
          value = opcode & 0xf;
          sprintf(instruction, "%s 0x%02x", table_pic18[n].instr, value);
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

void list_output_pic18(
  AsmContext *asm_context,
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
    count = disasm_pic18(
      &asm_context->memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    opcode = memory_read16_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: 0x%04x %-40s cycles: ",
      start, opcode, instruction);

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

    opcode = memory_read16_m(&asm_context->memory, start + 2);

    if (count > 2)
    {
      fprintf(asm_context->list, "        0x%04x\n", opcode);
    }

    start += count;
  }
}

void disasm_range_pic18(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int count;
  uint16_t opcode;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_pic18(
      memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: 0x%04x %-40s ", start, opcode, instruction);

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

    start += 2;

    if (count > 2)
    {
      printf("        0x%04x\n", opcode);
      start += 2;
    }
  }
}

