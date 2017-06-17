/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/arc.h"
#include "table/arc.h"

#define READ_RAM16(a) ((memory_read_m(memory, a + 0) << 8) | \
                       (memory_read_m(memory, a + 1)))

#define READ_RAM32(a) ((memory_read_m(memory, a + 0) << 24) | \
                       (memory_read_m(memory, a + 1) << 16) | \
                       (memory_read_m(memory, a + 2) << 8) | \
                       (memory_read_m(memory, a + 3)))

static char *condition_codes[] =
{
 "",
 ".eq",
 ".ne",
 ".pl",
 ".mi",
 ".cs",
 ".cc",
 ".vs",
 ".vc",
 ".gt",
 ".ge",
 ".lt",
 ".le",
 ".hi",
 ".ls",
 ".pnz",
};

int map16_bit_register(int r)
{
  if (r <= 3) { return r; }

  return r + 8;
}

int get_cycle_count_arc(unsigned short int opcode)
{
  return -1;
}

int disasm_arc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int n, a, c, b, q, h, f, u5, u6, s12;
  int limm;
  char *cc = "";

  opcode = (memory_read16_m(memory, address) << 16) |
            memory_read16_m(memory, address + 2);

  a = opcode & 0x3f;
  c = (opcode >> 6) & 0x3f;
  b = (((opcode >> 12) & 0x7) << 3) | ((opcode >> 24) & 0x7);
  f = (opcode >> 15) & 0x1;
  q = opcode & 0x1f;
  u6 = (opcode >> 6) & 0x3f;

  if (q <= 0xf) { cc = condition_codes[q]; }

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;
  while(table_arc[n].instr != NULL)
  {
    if ((opcode & table_arc[n].mask) == table_arc[n].opcode)
    {
      switch(table_arc[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_arc[n].instr);
          return 4;
        }
        case OP_B_C:
        {
          sprintf(instruction, "%s%s r%d, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, c);

          return 4;
        }
        case OP_B_U6:
        {
          sprintf(instruction, "%s%s r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, u6);

          return 4;
        }
        case OP_B_LIMM:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, limm);

          return 8;
        }
        case OP_0_C:
        {
          sprintf(instruction, "%s%s 0, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            c);

          return 4;
        }
        case OP_0_U6:
        {
          sprintf(instruction, "%s%s 0, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            u6);

          return 4;
        }
        case OP_0_LIMM:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s 0, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            limm);

          return 8;
        }
        case OP_A_B_C:
        {
          sprintf(instruction, "%s%s r%d, r%d, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            a, b, c);

          return 4;
        }
        case OP_A_B_U6:
        {
          sprintf(instruction, "%s%s r%d, r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            a, b, u6);

          return 4;
        }
        case OP_B_B_S12:
        {
          s12 = opcode & 0xfff;

          if ((s12 & 0x800) != 0) { s12 |= 0xfffff000; }

          sprintf(instruction, "%s%s r%d, r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            a, b, u6);

          return 4;
        }
        case OP_B_B_C:
        {
          sprintf(instruction, "%s%s%s r%d, r%d, r%d",
            table_arc[n].instr,
            cc,
            f == 0 ? "" : ".f",
            b, b, c);

          return 4;
        }
        case OP_B_B_U6:
        {
          sprintf(instruction, "%s%s%s r%d, r%d, %d",
            table_arc[n].instr,
            cc,
            f == 0 ? "" : ".f",
            b, b, u6);

          return 4;
        }
        case OP_A_LIMM_C:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s r%d, %d, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            a, limm, c);

          return 8;
        }
        case OP_A_B_LIMM:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s r%d, r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            a, b, limm);

          return 8;
        }
        case OP_B_B_LIMM:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s%s r%d, r%d, %d",
            table_arc[n].instr,
            cc,
            f == 0 ? "" : ".f",
            b, b, limm);

          return 8;
        }
        case OP_0_B_C:
        {
          sprintf(instruction, "%s%s 0, r%d, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, c);

          return 4;
        }
        case OP_0_B_U6:
        {
          sprintf(instruction, "%s%s 0, r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, u6);

          return 4;
        }
        case OP_0_B_LIMM:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s 0, r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, limm);

          return 8;
        }
        case OP_0_LIMM_C:
        {
          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s%s 0, %d, r%d",
            table_arc[n].instr,
            cc,
            f == 0 ? "" : ".f",
            limm, c);

          return 8;
        }
        default:
        {
          break;
        }
      }
    }

    n++;
  }

  opcode = memory_read16_m(memory, address);
  c = map16_bit_register((opcode >> 5) & 0x7);
  b = map16_bit_register((opcode >> 8) & 0x7);
  a = map16_bit_register(opcode & 0x7);
  h = ((opcode >> 5) & 0x7) | ((opcode & 0x7) << 3);
  u5 = opcode & 0x1f;

  n = 0;
  while(table_arc16[n].instr != NULL)
  {
    if ((opcode & table_arc16[n].mask) == table_arc16[n].opcode)
    {
      switch(table_arc16[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_arc16[n].instr);
          return 2;
        }
        case OP_B_C:
        {
          sprintf(instruction, "%s r%d, r%d",
            table_arc16[n].instr,
            b, c);

          return 2;
        }
        case OP_A_B_C:
        {
          sprintf(instruction, "%s r%d, r%d, r%d",
            table_arc16[n].instr,
            a, b, c);

          return 2;
        }
        case OP_C_B_U3:
        {
          int u3 = opcode & 0x7;
          sprintf(instruction, "%s r%d, r%d, %d",
            table_arc16[n].instr,
            a, b, u3);

          return 2;
        }
        case OP_B_B_LIMM:
        {
          limm = (memory_read16_m(memory, address + 2) << 16) |
                  memory_read16_m(memory, address + 4);

          sprintf(instruction, "%s r%d, r%d, %d",
            table_arc16[n].instr,
            b, b, limm);

          return 6;
        }
        case OP_B_B_H:
        {
          sprintf(instruction, "%s r%d, r%d, r%d",
            table_arc16[n].instr,
            b, b, h);

          return 2;
        }
        case OP_B_B_U7:
        {
          int u7 = opcode & 0x7f;

          sprintf(instruction, "%s r%d, r%d, %d",
            table_arc16[n].instr,
            b, b, u7);

          return 2;
        }
        case OP_B_SP_U5:
        {
          sprintf(instruction, "%s r%d, sp, %d",
            table_arc16[n].instr,
            b, u5);

          return 2;
        }
        case OP_SP_SP_U5:
        {
          sprintf(instruction, "%s sp, sp, %d",
            table_arc16[n].instr, u5);

          return 2;
        }
        case OP_R0_GP_S9:
        {
          int s9 = opcode & 0x1ff;
          if ((s9 & 0x100) != 0) { s9 |= 0xffffff00; }

          sprintf(instruction, "%s r0, gp, %d",
            table_arc16[n].instr, s9);

          return 2;
        }
        default:
        {
          break;
        }
      }
    }

    n++;
  }

  strcpy(instruction, "???");

  return 4;
}

void list_output_arc(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint32_t opcode;
  int count, n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    struct _memory *memory = &asm_context->memory;

    count = disasm_arc(memory, start, instruction, &cycles_min, &cycles_max);

    if (count < 4)
    {
      opcode = memory_read16_m(memory, start);
      fprintf(asm_context->list, "0x%04x: %04x     %-40s\n", start, opcode, instruction);
    }
      else
    {
      opcode = (memory_read16_m(memory, start) << 16) |
                memory_read16_m(memory, start + 2);
      fprintf(asm_context->list, "0x%04x: %08x %-40s\n", start, opcode, instruction);
    }

    for (n = 4; n < count; n = n + 4)
    {
      opcode = (memory_read16_m(memory, start + n) << 16) |
                memory_read16_m(memory, start + n + 2);

      fprintf(asm_context->list, "        %08x\n", opcode);
    }

    start += count;
  }
}

void disasm_range_arc(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
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
    count = disasm_arc(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = (memory_read16_m(memory, start) << 16) |
              memory_read16_m(memory, start + 2);

    printf("0x%04x: %04x %-40s\n", start, opcode, instruction);

    start = start + count;
  }
}

