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

#include "disasm/arm64.h"
#include "table/arm64.h"

static char reg_size[] = { 'w', 'x' };
static char scalar_size[] = { 'b', 'h', 's', 'd' };
static char *vec_size[] = { "8b", "16b", "4h", "8h", "2s", "4s", "1d", "2d" };
static char *shifts[] = { "lsl", "lsr", "asr", "???" };
static char *options[] =
{
  "uxtb", "uxth", "uxtw", "uxtx",
  "sxtb", "sxth", "sxtw", "sxtx"
};

static const char *get_at(int value)
{
  int n;

  for (n = 0; table_arm64_at_op[n].name != NULL; n++)
  {
    if (table_arm64_at_op[n].value == value)
    {
      return table_arm64_at_op[n].name;
    }
  }

  return "???";
}

int get_cycle_count_arm64(unsigned short int opcode)
{
  return -1;
}

int disasm_arm64(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t opcode;
  int n;
  int rm, rn, rd;
  int size, sf, imm, option, shift;

  opcode = memory_read32_m(memory, address);

  *cycles_min = -1;
  *cycles_max = -1;

  rm = (opcode >> 16) & 0x1f;
  rn = (opcode >> 5) & 0x1f;
  rd = opcode & 0x1f;
  size = (opcode >> 22) & 0x3;
  sf = (opcode >> 31) & 0x1;

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
        case OP_MATH_R_R_R_OPTION:
        {
          imm = (opcode >> 10) & 0x7;
          option = (opcode >> 13) & 0x7;

          if ((sf == 0 && option == 2) ||
              (sf == 1 && option == 3))
          {
            if (imm == 0)
            {
              sprintf(instruction, "%s %c%d, %c%d, %c%d",
                table_arm64[n].instr,
                reg_size[sf], rd,
                reg_size[sf], rn,
                reg_size[sf], rm);

              return 4;
            }

            sprintf(instruction, "%s %c%d, %c%d, %c%d, lsl #%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              reg_size[sf], rm,
              imm);

            return 4;
          }

          sprintf(instruction, "%s %c%d, %c%d, %c%d, %s",
            table_arm64[n].instr,
            reg_size[sf], rd,
            reg_size[sf], rn,
            reg_size[sf], rm,
            options[option]);

          return 4;
        }
        case OP_MATH_R_R_IMM_SHIFT:
        {
          imm = (opcode >> 10) & 0xfff;
          shift = (opcode >> 22) & 0x3;

          if (shift == 0)
          {
            sprintf(instruction, "%s %c%d, %c%d, #0x%03x",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              imm);
          }
            else
          if (shift == 1)
          {
            sprintf(instruction, "%s %c%d, %c%d, #0x%06x [#0x%03x lsl #%d]",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              imm << 12,
              imm, shift * 12);
          }
            else
          {
            sprintf(instruction, "%s %c%d, %c%d, #%d [#%d] ???",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              imm,
              shift);
          }

          return 4;
        }
        case OP_MATH_R_R_R_SHIFT:
        {
          imm = (opcode >> 10) & 0x3f;
          shift = (opcode >> 22) & 0x3;

          if (imm == 0 && shift == 0)
          {
            sprintf(instruction, "%s %c%d, %c%d, %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              reg_size[sf], rm);
          }
            else
          {
            sprintf(instruction, "%s %c%d, %c%d, %c%d, %s #%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              reg_size[sf], rm,
              shifts[shift],
              imm);
          }

          return 4;
        }
        case OP_MATH_R_R_IMM6_IMM4:
        {
          int imm4 = (opcode >> 10) & 0xf;
          int imm6 = (opcode >> 16) & 0x3f;

          sprintf(instruction, "%s x%d, x%d, #%d, #%d",
            table_arm64[n].instr,
            rd,
            rn,
            imm6 * 16,
            imm4);

          return 4;
        }
        case OP_REG_RELATIVE:
        {
          imm = (opcode >> 5) & ((1 << 19) - 1);
          imm = (imm << 2) | ((opcode >> 24) & 0x3);

          if ((imm & 0x00080000) != 0) { imm &= 0xfff00000; }

          sprintf(instruction, "%s x%d, 0x%04x (offset=%d)",
            table_arm64[n].instr,
            rd,
            address + 4 + imm,
            imm);

          return 4;
        }
        case OP_REG_PAGE_RELATIVE:
        {
          imm = (opcode >> 5) & ((1 << 19) - 1);
          imm = (imm << 2) | ((opcode >> 29) & 0x3);
          imm = imm << 12;

          sprintf(instruction, "%s x%d, 0x%04x (offset=%d)",
            table_arm64[n].instr,
            rd,
           (address & (~0xfff)) + imm,
            imm);

          return 4;
        }
        case OP_VECTOR_D_V:
        {
          sprintf(instruction, "%s d%d, v%d.2d",
            table_arm64[n].instr,
            rd,
            rn);

          return 4;
        }
        case OP_VECTOR_V_V_TO_SCALAR:
        {
          sprintf(instruction, "%s d%d, v%d.%s",
            table_arm64[n].instr,
            rd,
            rn,
            vec_size[sf]);

          return 4;
        }
        case OP_MATH_R_R_IMMR_S:
        {
          int imm = (opcode >> 16) & 0x3f;
          imm |= ((opcode >> 10) & 0x3f) << 6;

          if (sf == 1) { imm |= ((opcode >> 22) & 1) << 12; }

          sprintf(instruction, "%s %c%d, %c%d, #0x%03x",
            table_arm64[n].instr,
            reg_size[sf], rd,
            reg_size[sf], rn,
            imm);

          return 4;
        }
        case OP_MATH_R_R_IMMR:
        {
          int imm = (opcode >> 16) & 0x3f;

          if (sf == 1) { imm |= ((opcode >> 22) & 1) << 6; }

          sprintf(instruction, "%s %c%d, %c%d, #%d",
            table_arm64[n].instr,
            reg_size[sf], rd,
            reg_size[sf], rn,
            imm);

          return 4;
        }
        case OP_AT:
        {
          int imm = ((opcode >> 16) & 0x7) << 4;
          imm |= (opcode >> 5) & 0x7;
          imm |= ((opcode >> 8) & 0x1) << 3;

          const char *at = get_at(imm);

          sprintf(instruction, "%s %s, x%d",
            table_arm64[n].instr,
            at,
            rd);

          return 4;
        }
        case OP_RELATIVE19:
        {
          imm = (opcode >> 5) & ((1 << 19) - 1);
          if ((imm & (1 << 18)) != 0) { imm |= 0xfff80000; }
          imm *= 4;

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_arm64[n].instr,
           (address + 4 + imm),
            imm);

          return 4;
        }
        case OP_RELATIVE26:
        {
          imm = opcode & ((1 << 26) - 1);
          if ((imm & (1 << 25)) != 0) { imm |= 0xfc000000; }
          imm *= 4;

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_arm64[n].instr,
           (address + 4 + imm),
            imm);

          return 4;
        }
        case OP_REG_BITFIELD:
        {
          int immr = (opcode >> 16) & 0x3f;
          int imms = (opcode >> 10) & 0x3f;

          sprintf(instruction, "%s %c%d, #%d, #%d",
            table_arm64[n].instr,
            reg_size[sf], rd,
            immr,
            imms);

          return 4;
        }
        case OP_SCALAR_D_D:
        {
          if (size != 3) { continue; }

          sprintf(instruction, "%s %c%d, %c%d",
            table_arm64[n].instr, scalar_size[size], rd, scalar_size[size], rn);

          return 4;
        }
        case OP_SCALAR_D_D_D:
        {
          if (size != 3) { continue; }

          sprintf(instruction, "%s %c%d, %c%d, %c%d",
            table_arm64[n].instr,
            scalar_size[size], rd,
            scalar_size[size], rn,
            scalar_size[size], rm);

          return 4;
        }
        case OP_VECTOR_V_V:
        {
          size = (size << 1) | ((opcode >> 30) & 1);

          sprintf(instruction, "%s v%d.%s, v%d.%s",
            table_arm64[n].instr, rd, vec_size[size], rn, vec_size[size]);

          return 4;
        }
        case OP_VECTOR_V_V_V:
        {
          size = (size << 1) | ((opcode >> 30) & 1);

          sprintf(instruction, "%s v%d.%s, v%d.%s, v%d.%s",
            table_arm64[n].instr,
            rd, vec_size[size],
            rn, vec_size[size],
            rm, vec_size[size]);

          return 4;
        }
        case OP_MATH_R_R_R:
        {
          if (table_arm64[n].operand_count == 1)
          {
            sprintf(instruction, "%s %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd);
          }
            else
          if (table_arm64[n].operand_count == 2)
          {
            sprintf(instruction, "%s %c%d, %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn);
          }
            else
          {
            sprintf(instruction, "%s %c%d, %c%d, %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              reg_size[sf], rm);
          }

          return 4;
        }
        case OP_REG_REG_CRYPT:
        {
          if (table_arm64[n].operand_count == 2)
          {
            sprintf(instruction, "%s v%d.16b, v%d.16b",
              table_arm64[n].instr, rd, rn);
          }
            else
          if (table_arm64[n].operand_count == 4)
          {
            int ra = (opcode >> 10) & 0x1f;

            sprintf(instruction, "%s v%d.16b, v%d.16b, v%d.16b, v%d.16b",
              table_arm64[n].instr, rd, rn, rm, ra);
          }
            else
          {
            strcpy(instruction, "???");
          }

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

void list_output_arm64(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint32_t opcode;
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_arm64(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read32_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: %08x %-40s\n", start, opcode, instruction);

    start += count;
  }
}

void disasm_range_arm64(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_arm64(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read32_m(memory, start);

    printf("0x%04x: %08x %-40s\n", start, opcode, instruction);

    start = start + count;
  }
}

