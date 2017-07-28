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

static int compute_s12(int data)
{
  int s12 = ((data & 0x3f) << 6) | ((data >> 6) & 0x3f);
  if ((s12 & 0x800) != 0) { s12 |= 0xfffff000; }

  return s12;
}

int map16_bit_register(int r)
{
  if (r <= 3) { return r; }

  return r + 8;
}

int get_cycle_count_arc(unsigned short int opcode)
{
  return -1;
}

uint32_t calc_address(struct _memory *memory, uint32_t address, int offset, int d)
{
  //int cycles_min, cycles_max;
  //char instruction[128];

  if (d == 1)
  {
    //address += disasm_arc(memory, address + 4, instruction, &cycles_min, & cycles_max);
    address += 4;
  }

  return address + 4 + offset;
}

int disasm_arc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int offset;
  int n, a, c, b, d, q, h, f, u6, s12;
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
          if (b == 62)
          {
             limm = (memory_read16_m(memory, address + 4) << 16) |
                     memory_read16_m(memory, address + 6);

             sprintf(instruction, "%s%s r%d, %d, r%d",
               table_arc[n].instr,
               f == 0 ? "" : ".f",
               a, limm, c);

             return 8;
          }

          if (c == 62)
          {
            limm = (memory_read16_m(memory, address + 4) << 16) |
                    memory_read16_m(memory, address + 6);

            sprintf(instruction, "%s%s r%d, r%d, %d",
              table_arc[n].instr,
              f == 0 ? "" : ".f",
              a, b, limm);

            return 8;
          }

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
          s12 = compute_s12(opcode & 0xfff);

          sprintf(instruction, "%s%s r%d, r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, b, s12);

          return 4;
        }
        case OP_B_B_C:
        {
          if (b == 62)
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

          if (c == 62)
          {
             limm = (memory_read16_m(memory, address + 2) << 16) |
                     memory_read16_m(memory, address + 4);

             sprintf(instruction, "%s r%d, r%d, %d",
               table_arc16[n].instr,
               b, b, limm);

             return 6;
          }

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
        case OP_B_C_O9:
        {
          offset = (opcode >> 17) & 0x1ff;
          d = (opcode >> 5) & 0x1;

          if ((offset & 0x100) != 0) { offset = 0xfffffff0 | offset; }

          sprintf(instruction, "%s%s r%d, r%d, 0x%d (offset=%d)",
            table_arc[n].instr,
            d == 0 ? "" : ".d",
            b, c,
            calc_address(memory, address, offset, d), offset);

          return 4;
        }
        case OP_B_U6_O9:
        {
          offset = (opcode >> 17) & 0x1ff;
          d = (opcode >> 5) & 0x1;

          if ((offset & 0x100) != 0) { offset = 0xfffffff0 | offset; }

          sprintf(instruction, "%s%s r%d, 0x%x, 0x%d (offset=%d)",
            table_arc[n].instr,
            d == 0 ? "" : ".d",
            u6, c,
            calc_address(memory, address, offset, d), offset);

          return 4;
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
        case OP_B_B_C:
        {
          sprintf(instruction, "%s r%d, r%d, r%d",
            table_arc16[n].instr,
            b, b, c);

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
        case OP_B_B_U5:
        {
          int u5 = opcode & 0x1f;

          sprintf(instruction, "%s r%d, r%d, %d",
            table_arc16[n].instr,
            b, b, u5);

          return 2;
        }
        case OP_B_SP_U7:
        {
          int u7 = (opcode & 0x1f) * 4;

          sprintf(instruction, "%s r%d, sp, %d",
            table_arc16[n].instr,
            b, u7);

          return 2;
        }
        case OP_SP_SP_U7:
        {
          int u7 = (opcode & 0x1f) * 4;

          sprintf(instruction, "%s sp, sp, %d",
            table_arc16[n].instr, u7);

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
  uint32_t opcode;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-7s  %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- -------- ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_arc(memory, start, instruction, &cycles_min, &cycles_max);

    if (count < 4)
    {
      opcode = memory_read16_m(memory, start);
      printf("0x%04x: %04x     %-40s\n", start, opcode, instruction);
    }
      else
    {
      opcode = (memory_read16_m(memory, start) << 16) |
                memory_read16_m(memory, start + 2);

      printf("0x%04x: %08x %-40s\n", start, opcode, instruction);
    }

    for (n = 4; n < count; n = n + 4)
    {
      opcode = (memory_read16_m(memory, start + n) << 16) |
                memory_read16_m(memory, start + n + 2);

      printf("        %08x\n", opcode);
    }

    start = start + count;
  }
}

