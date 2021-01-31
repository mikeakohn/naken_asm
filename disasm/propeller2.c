/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
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

static const char *conditions_cz[] =
{
  "_clr",
  "_nc_and_nz",
  "_nc_and_z",
  "_nc",
  "_c_and_nz",
  "_nz",
  "_c_ne_z",
  "_nc_or_nz",
  "_c_and_z",
  "_c_eq_z",
  "_z",
  "_nc_or_z",
  "_c",
  "_c_or_nz",
  "_c_or_z",
  "_set",
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
  int i, l, d, s, r;
  int cond;
  int wz, wc, wcz;
  int need_effect = 0;
  uint32_t value;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM32(address);

  i = (opcode >> 18) & 1;
  s = opcode & 0x1ff;
  d = (opcode >> 9) & 0x1ff;
  wc = (opcode >> 20) & 1;
  wz = (opcode >> 19) & 1;
  wcz = 0;
  char operands[64];
  cond = (opcode >> 28) & 0xf;
  const char *condition = conditions[cond];
  const char *instr = "???";

  need_effect = wc | wz;

  if (wc == 1 && wz == 1)
  {
    wc = 0;
    wz = 0;
    wcz = 1;
  }

  operands[0] = 0;

  for (n = 0; table_propeller2[n].instr != NULL; n++)
  {
    if ((opcode & table_propeller2[n].mask) != table_propeller2[n].opcode)
    {
      continue;
    }

    *cycles_min = table_propeller2[n].cycles8_min;
    *cycles_max = table_propeller2[n].cycles8_min;
    instr = table_propeller2[n].instr;

    if (table_propeller2[n].operand_count == 0) { condition = ""; }

    if (need_effect == 1 && table_propeller2[n].operand_count > 0)
    {
      need_effect = 2;
    }

    if (table_propeller2[n].wc == 0) { wc = 0; }
    if (table_propeller2[n].wz == 0) { wz = 0; }
    if (table_propeller2[n].wcz == 0) { wcz = 0; }
    if (wc == 0 && wz == 0 && wcz == 0) { need_effect = 0; }

    int t;

    for (t = 0; t < table_propeller2[n].operand_count; t++)
    {
      char temp[64];

      if (t != 0) { strcat(operands, ", "); }

      switch (table_propeller2[n].operands[t])
      {
        case OP_D:
          if (d >= 0x1f0 && d <= 0x1ff)
          {
            sprintf(temp, "%s [0x%x]", registers_propeller2[d - 0x1f0].name, d);
          }
            else
          {
            sprintf(temp, "0x%x", d);
          }
          strcat(operands, temp);
          break;
        case OP_NUM_D:
          if (t == 0 &&
             (table_propeller2[n].operands[1] == OP_BRANCH ||
              table_propeller2[n].operands[1] == OP_NUM_S ||
              table_propeller2[n].operands[1] == OP_NUM_SP))
          {
            l = (opcode >> 19) & 1;
          }
            else
          {
            l = i;
          }

          if (l == 1) { strcat(operands, "#"); }
          if (l == 0 && d >= 0x1f0 && d <= 0x1ff)
          {
            sprintf(temp, "%s [0x%x]", registers_propeller2[d - 0x1f0].name, d);
          }
            else
          {
            sprintf(temp, "0x%x", d);
          }
          strcat(operands, temp);
          break;
        case OP_NUM_S:
          if (i == 1) { strcat(operands, "#"); }
          if (i == 0 && s >= 0x1f0 && s <= 0x1ff)
          {
            sprintf(temp, "%s [0x%x]", registers_propeller2[s - 0x1f0].name, s);
          }
            else
          {
            sprintf(temp, "0x%x", s);
          }
          strcat(operands, temp);
          break;
        case OP_NUM_SP:
          strcpy(temp, "???");

          if (i == 0)
          {
            sprintf(temp, "0x%x", s);
          }
            else
          if ((s & 0x100) == 0)
          {
            sprintf(temp, "#0x%x", s);
          }
            else
          {
            const char *ptr = (s & 0x80) == 0 ? "ptra" : "ptrb";
            int d = s & 0x3f;
            int n = s & 0x0f;
            //const char *sign = (s & 0x10) == 0 ? "++" : "--";
            int sign = (s >> 4) & 1;

            if (sign == 0)
            {
              if (n == 0) { n = 16; }
            }
              else
            {
              n |= 0xfffffff0;
            }

            if ((s & 0x40) == 0)
            {
              if (d == 0)
              {
                sprintf(temp, "%s", ptr);
              }
                else
              {
                sprintf(temp, "%s[%d]", ptr, d);
              }
            }
              else
            if (d == 0x21)
            {
              sprintf(temp, "%s++", ptr);
            }
              else
            if (d == 0x3f)
            {
              sprintf(temp, "%s--", ptr);
            }
              else
            if (d == 0x01)
            {
              sprintf(temp, "++%s", ptr);
            }
              else
            if (d == 0x1f)
            {
              sprintf(temp, "--%s", ptr);
            }
              else
            {
              if (sign == 1) { n = -n; }

              if ((s & 0x20) == 0)
              {
                sprintf(temp, "%s%s[%d]", sign == 0 ? "++" : "--", ptr, n);
              }
                else
              {
                sprintf(temp, "%s%s[%d]", ptr, sign == 0 ? "++" : "--", n);
              }
            }
          }

          strcat(operands, temp);
          break;
        case OP_N_1:
          r = (opcode >> 19) & 0x1;
          sprintf(temp, "#0x%x", r);
          strcat(operands, temp);
          break;
        case OP_N_2:
          r = (opcode >> 19) & 0x3;
          sprintf(temp, "#0x%x", r);
          strcat(operands, temp);
          break;
        case OP_N_3:
          r = (opcode >> 19) & 0x7;
          sprintf(temp, "#0x%x", r);
          strcat(operands, temp);
          break;
        case OP_N_23:
          value = opcode & 0x7fffff;
          sprintf(temp, "#0x%04x", value << 9);
          strcat(operands, temp);
          break;
        case OP_A:
          r = (opcode >> 20) & 1;

          if (r == 0)
          {
            r = opcode & 0xfffff;
            sprintf(temp, "#\\0x%04x", r);
          }
            else
          {
            int offset = opcode & 0xfffff;

            if ((offset & 0x80000) != 0)
            {
              offset |= 0xfff00000;
            }

            r = address + 4 + offset;

            if (r < 0x400 * 4) { r = r / 4; }

            sprintf(temp, "#0x%04x (offset=%d)", r, offset);
          }

          strcat(operands, temp);
          break;
        case OP_BRANCH:
          r = (opcode >> 18) & 1;

          if (r == 0)
          {
            r = opcode & 0x1ff;
            sprintf(temp, "0x%04x", r);
          }
            else
          {
            int offset = opcode & 0x1ff;

            if ((offset & 0x100) != 0)
            {
              offset |= 0xfffffe00;
            }

            r = (address / 4) + 1 + offset;
            sprintf(temp, "#0x%04x (offset=%d)", r, offset);
          }

          strcat(operands, temp);
          break;
        case OP_P:
          r = (opcode >> 21) & 0x3;
          strcat(operands, registers_propeller2[r + 6].name);
          break;
        case OP_C:
          r = (opcode >> 13) & 0xf;
          strcat(operands, conditions_cz[r]);
          break;
        case OP_Z:
          r = (opcode >> 9) & 0xf;
          strcat(operands, conditions_cz[r]);
          break;
        default:
          printf("Internal Error: %s:%d\n", __FILE__, __LINE__);
          exit(1);
      }
    }

    break;
  }

  if (need_effect != 0)
  {
    if (need_effect == 2) { strcat(operands, ", "); }
    if (wc == 1) { strcat(operands, "wc"); }
    if (wz == 1) { strcat(operands, "wz"); }
    if (wcz == 1) { strcat(operands, "wcz"); }
  }

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

