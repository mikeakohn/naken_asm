/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cinttypes>

#include "disasm/arm64.h"
#include "table/arm64.h"

static char reg_size[] = { 'w', 'x' };
static char scalar_size[] = { 'b', 'h', 's', 'd', 'q' };
static const char *vec_size[] =
{
  "8b", "16b", "4h", "8h", "2s", "4s", "1d", "2d"
};
static const char *shifts[] = { "lsl", "lsr", "asr", "???" };
static const char *options[] =
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

static uint64_t decode_imms(int imms, int immr, int sf)
{
  uint64_t imm = 0;
  int size = 0;

  if ((imms & 0x7e) == 0x3c)
  {
    size = 2;
  }
    else
  if ((imms & 0x7c) == 0x38)
  {
    size = 4;
  }
    else
  if ((imms & 0x78) == 0x30)
  {
    size = 8;
  }
    else
  if ((imms & 0x70) == 0x20)
  {
    size = 16;
  }
    else
  if ((imms & 0x60) == 0x00)
  {
    size = 32;
  }
    else
  if ((imms & 0x40) == 0x40)
  {
    size = 64;
  }
    else
  {
    // Issue #127: If encoding doesn't match, don't use a size of 0.
    return 0;
  }

  // Create bit battern.
  int ones_count = (imms & (size - 1)) + 1;
  int pattern = (1 << ones_count) - 1;

  // Repeat bit pattern.
  for (int n = 0; n < 64; n += size)
  {
    imm |= (pattern << n);
  }

  // Do rotate right.
  if (immr != 0)
  {
    int r = imm & ((1 << immr) - 1);
    imm = imm >> immr;
    imm |= r << (64 - immr);
  }

  if (sf == 0) { imm &= 0xffffffff; }

  return imm;
}

static const char *decode_imm5_vector(int imm, int q)
{
  int size = 0;

  if ((imm & 0x1) == 1) { size = 0; }
  else if ((imm & 0x3) == 2) { size = 2; }
  else if ((imm & 0x7) == 4) { size = 4; }
  else if ((imm & 0xf) == 8) { size = 6; }

  size = size + q;

  return vec_size[size];
}

static void decode_imm5_element(int imm, char &size, int &element)
{
  size = '?';
  element = 0;

  if ((imm & 0x1) == 1) { size = 'b'; element = (imm >> 1) & 0xf; }
  else if ((imm & 0x3) == 2) { size = 'h'; element = (imm >> 2) & 0x7; }
  else if ((imm & 0x7) == 4) { size = 's'; element = (imm >> 3) & 0x3; }
  else if ((imm & 0xf) == 8) { size = 'd'; element = (imm >> 4) & 0x1; }
}

static void get_reg_name(char *s, int length, int num, int imm, int type, int q)
{
  char size;
  int element;

  switch (type)
  {
    case ARM64_REG_B:
      snprintf(s, length, "%c%d", (imm & 8) == 0 ? 'w' : 'x', num);
      break;
    case ARM64_REG_V_DOT:
      snprintf(s, length, "v%d.%s", num, decode_imm5_vector(imm, q));
      break;
    case ARM64_REG_V_SCALAR:
      decode_imm5_element(imm, size, element);
      snprintf(s, length, "%c%d", size, num);
      break;
    case ARM64_REG_V_ELEMENT:
      decode_imm5_element(imm, size, element);
      snprintf(s, length, "v%d.%c[%d]", num, size, element);
      break;
    default:
      memset(s, 0, length);
      break;
  }
}

int disasm_arm64(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t opcode;
  int n;
  int rm, rn, rd, ra;
  int size, sf, imm, option, shift, v;

  opcode = memory->read32(address);

  *cycles_min = -1;
  *cycles_max = -1;

  rm = (opcode >> 16) & 0x1f;
  ra = (opcode >> 10) & 0x1f;
  rn = (opcode >> 5) & 0x1f;
  rd = opcode & 0x1f;
  size = (opcode >> 22) & 0x3;
  sf = (opcode >> 31) & 0x1;
  v = (opcode >> 26) & 0x1;

  if ((opcode & 0x8fe08400) == 0x0e000400)
  {
    for (n = 0; table_arm64_simd_copy[n].instr != NULL; n++)
    {
      int q = opcode >> 30;
      int op = (opcode >> 28) & 3;
      int imm5 = (opcode >> 16) & 0x1f;
      int imm4 = (opcode >> 11) & 0xf;

      if (//table_arm64_simd_copy[n].q != q ||
          table_arm64_simd_copy[n].op != op)
      {
        continue;
      }

      char rn_name[16];
      char rd_name[16];

      if (table_arm64_simd_copy[n].q == 1 &&
          table_arm64_simd_copy[n].op == 1)
      {
        get_reg_name(rn_name, sizeof(rn_name), rn, imm4, table_arm64_simd_copy[n].reg_rn, q);
        get_reg_name(rd_name, sizeof(rd_name), rd, imm5, table_arm64_simd_copy[n].reg_rd, q);
      }
        else
      {
        if (table_arm64_simd_copy[n].imm4 != imm4) { continue; }

        get_reg_name(rn_name, sizeof(rn_name), rn, imm5, table_arm64_simd_copy[n].reg_rn, q);
        get_reg_name(rd_name, sizeof(rd_name), rd, imm5, table_arm64_simd_copy[n].reg_rd, q);
      }

      snprintf(instruction, length, "%s %s, %s",
        table_arm64_simd_copy[n].instr,
        rd_name,
        rn_name);

      return 4;
    }

    strcpy(instruction, "???");
    return 4;
  }

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
              snprintf(instruction, length, "%s %c%d, %c%d, %c%d",
                table_arm64[n].instr,
                reg_size[sf], rd,
                reg_size[sf], rn,
                reg_size[sf], rm);

              return 4;
            }

            snprintf(instruction, length, "%s %c%d, %c%d, %c%d, lsl #%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              reg_size[sf], rm,
              imm);

            return 4;
          }

          snprintf(instruction, length, "%s %c%d, %c%d, %c%d, %s",
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
            snprintf(instruction, length, "%s %c%d, %c%d, #0x%03x",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              imm);
          }
            else
          if (shift == 1)
          {
            snprintf(instruction, length, "%s %c%d, %c%d, #0x%06x [#0x%03x lsl #%d]",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              imm << 12,
              imm, shift * 12);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, %c%d, #%d [#%d] ???",
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
            snprintf(instruction, length, "%s %c%d, %c%d, %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn,
              reg_size[sf], rm);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, %c%d, %c%d, %s #%d",
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

          snprintf(instruction, length, "%s x%d, x%d, #%d, #%d",
            table_arm64[n].instr,
            rd,
            rn,
            imm6 * 16,
            imm4);

          return 4;
        }
        case OP_MOVE:
        {
          snprintf(instruction, length, "%s x%d, x%d",
            table_arm64[n].instr,
            rd,
            rm);
        }
        case OP_REG_RELATIVE:
        {
          imm = (opcode >> 5) & ((1 << 19) - 1);
          imm = (imm << 2) | ((opcode >> 24) & 0x3);

          if ((imm & 0x00080000) != 0) { imm &= 0xfff00000; }

          snprintf(instruction, length, "%s x%d, 0x%04x (offset=%d)",
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

          snprintf(instruction, length, "%s x%d, 0x%04x (offset=%d)",
            table_arm64[n].instr,
            rd,
           (address & (~0xfff)) + imm,
            imm);

          return 4;
        }
        case OP_VECTOR_D_V:
        {
          snprintf(instruction, length, "%s d%d, v%d.2d",
            table_arm64[n].instr,
            rd,
            rn);

          return 4;
        }
        case OP_VECTOR_V_V_TO_SCALAR:
        {
          int q = (opcode >> 30) & 1;
          int size_q = (size << 1)| q;

          snprintf(instruction, length, "%s %c%d, v%d.%s",
            table_arm64[n].instr,
            scalar_size[size],
            rd,
            rn,
            vec_size[size_q]);

          return 4;
        }
        case OP_MATH_R_R_IMMR_S:
        {
          int immr = (opcode >> 16) & 0x3f;
          int imms = (opcode >> 10) & 0x3f;

          // If 64 bit, use N as the most significant bit of imms.
          if (sf == 1) { imms |= ((opcode >> 22) & 1) << 6; }

          uint64_t imm = decode_imms(imms, immr, sf);

          snprintf(instruction, length,
            "%s %c%d, %c%d, #0x%04" PRIx64 " (immr=%d imms=0x%02x)",
            table_arm64[n].instr,
            reg_size[sf], rd,
            reg_size[sf], rn,
            imm,
            immr,
            imms);

          return 4;
        }
        case OP_MATH_R_R_IMMR:
        {
          int imm = (opcode >> 16) & 0x3f;

          if (sf == 1) { imm |= ((opcode >> 22) & 1) << 6; }

          snprintf(instruction, length, "%s %c%d, %c%d, #%d",
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

          snprintf(instruction, length, "%s %s, x%d",
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

          snprintf(instruction, length, "%s 0x%04x (offset=%d)",
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

          snprintf(instruction, length, "%s 0x%04x (offset=%d)",
            table_arm64[n].instr,
           (address + imm),
            imm);

          return 4;
        }
        case OP_REG_BITFIELD:
        {
          int immr = (opcode >> 16) & 0x3f;
          int imms = (opcode >> 10) & 0x3f;

          snprintf(instruction, length, "%s %c%d, #%d, #%d",
            table_arm64[n].instr,
            reg_size[sf], rd,
            immr,
            imms);

          return 4;
        }
        case OP_SCALAR_D_D:
        {
          if (size != 3) { continue; }

          snprintf(instruction, length, "%s %c%d, %c%d",
            table_arm64[n].instr, scalar_size[size], rd, scalar_size[size], rn);

          return 4;
        }
        case OP_SCALAR_D_D_D:
        {
          if (size != 3) { continue; }

          snprintf(instruction, length, "%s %c%d, %c%d, %c%d",
            table_arm64[n].instr,
            scalar_size[size], rd,
            scalar_size[size], rn,
            scalar_size[size], rm);

          return 4;
        }
        case OP_VECTOR_V_V:
        {
          size = (size << 1) | ((opcode >> 30) & 1);

          snprintf(instruction, length, "%s.%s v%d, v%d",
            table_arm64[n].instr, vec_size[size], rd, rn);

          return 4;
        }
        case OP_VECTOR_V_V_V:
        {
          size = (size << 1) | ((opcode >> 30) & 1);

          snprintf(instruction, length, "%s.%s v%d, v%d, v%d",
            table_arm64[n].instr,
            vec_size[size],
            rd,
            rn,
            rm);

          return 4;
        }
        case OP_MATH_R_R_R:
        {
          if (table_arm64[n].operand_count == 1)
          {
            snprintf(instruction, length, "%s %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd);
          }
            else
          if (table_arm64[n].operand_count == 2)
          {
            snprintf(instruction, length, "%s %c%d, %c%d",
              table_arm64[n].instr,
              reg_size[sf], rd,
              reg_size[sf], rn);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, %c%d, %c%d",
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
            snprintf(instruction, length, "%s v%d.16b, v%d.16b",
              table_arm64[n].instr, rd, rn);
          }
            else
          if (table_arm64[n].operand_count == 4)
          {
            int ra = (opcode >> 10) & 0x1f;

            snprintf(instruction, length,
              "%s v%d.16b, v%d.16b, v%d.16b, v%d.16b",
              table_arm64[n].instr, rd, rn, rm, ra);
          }
            else
          {
            strcpy(instruction, "???");
          }

          return 4;
        }
        case OP_RET:
        {
          snprintf(instruction, length, "%s x%d\n", table_arm64[n].instr, rn);

          return 4;
        }
        case OP_LD_ST_IMM_P:
        {
          int index_type = (opcode >> 10) & 0x3;
          char reg_name;

          if (v == 1)
          {
            size |= ((opcode >> 23) & 1) << 2;
            reg_name = scalar_size[size];
          }
            else
          {
            size = (opcode >> 30) & 1;
            reg_name = (size & 1) == 0 ? 'w' : 'x';
          }

          imm = (opcode >> 12) & 0x1ff;
          //imm = imm << size;

          if (index_type == 1)
          {
            snprintf(instruction, length, "%s %c%d, [x%d], #%d",
              table_arm64[n].instr,
              reg_name,
              rd,
              rn,
              imm);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, [x%d, #%d]!",
              table_arm64[n].instr,
              reg_name,
              rd,
              rn,
              imm);
          }

          return 4;
        }
        case OP_LD_ST_IMM:
        {
          imm = ((opcode >> 10) & 0xfff) << 1;
          char reg_name = (size & 1) == 0 ? 'x' : 'w';

          if (v == 1)
          {
            size |= ((opcode >> 23) & 1) << 2;
            reg_name = scalar_size[size];
          }

          if (imm == 0)
          {
            snprintf(instruction, length, "%s %c%d, [x%d]",
              table_arm64[n].instr,
              reg_name,
              rd,
              rn);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, [x%d, #0x%05x]",
              table_arm64[n].instr,
              reg_name,
              rd,
              rn,
              imm);
          }

          return 4;
        }
        case OP_LD_LITERAL:
        {
          imm = ((opcode >> 5) & 0x7ffff);
          imm = imm << 13;
          imm = imm >> (13 - 2);
          char reg_name = (size & 1) == 0 ? 'x' : 'w';

          if (v == 1)
          {
            size |= ((opcode >> 23) & 1) << 2;
            reg_name = scalar_size[size];
          }

          snprintf(instruction, length, "%s %c%d, 0x%04x (offset=%d)",
            table_arm64[n].instr,
            reg_name,
            rd,
            address + imm,
            imm);

          return 4;
        }
        case OP_ALIAS_REG_IMM:
        {
          imm = (opcode >> 10) & 0xfff;
          shift = (opcode >> 22) & 0x3;

          if (shift == 0)
          {
            snprintf(instruction, length, "%s %c%d, #0x%03x",
              table_arm64[n].instr,
              reg_size[sf], rn,
              imm);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, #0x%03x (#0x%03x, lsl #12)",
              table_arm64[n].instr,
              reg_size[sf], rn,
              imm << 12,
              imm);
          }

          return 4;
        }
        case OP_SCALAR_SHIFT_IMM:
        {
          snprintf(instruction, length, "%s d%d, d%d, #%d",
            table_arm64[n].instr,
            rd,
            rn,
            (opcode >> 16) & 0x3f);

          return 4;
        }
        case OP_VECTOR_SHIFT_IMM:
        {
          int q = (opcode >> 30) & 1;
          imm = (opcode >> 16) & 0x7f;

          if      ((imm & 0x08) == 0x08) { size = 0 + q; imm &= 0x07; }
          else if ((imm & 0x10) == 0x10) { size = 2 + q; imm &= 0x0f; }
          else if ((imm & 0x20) == 0x20) { size = 4 + q; imm &= 0x1f; }
          else if ((imm & 0x40) == 0x40) { size = 6 + q; imm &= 0x3f; }

          if (table_arm64[n].reg_type == 'r')
          {
            switch (size >> 1)
            {
              case 0: imm = 8 - imm; break;
              case 1: imm = 16 - imm; break;
              case 2: imm = 32 - imm; break;
              case 3: imm = 64 - imm; break;
            }
          }

          snprintf(instruction, length, "%s.%s v%d, v%d, #%d",
            table_arm64[n].instr,
            vec_size[size],
            rd,
            rn,
            imm);

          return 4;
        }
        case OP_VECTOR_V_V_V_FPU:
        {
          int q = (opcode >> 30) & 1;
          int sz = (opcode >> 22) & 1;
          const char *size = "?";

          if (sz == 0 && q == 0) { size = "2s"; }
          else if (sz == 0 && q == 1) { size = "4s"; }
          else if (sz == 1 && q == 1) { size = "2d"; }

          snprintf(instruction, length, "%s.%s v%d, v%d, v%d",
            table_arm64[n].instr,
            size,
            rd,
            rn,
            rm);

          return 4;
        }
        case OP_VECTOR_V_V_FPU:
        {
          int q = (opcode >> 30) & 1;
          int sz = (opcode >> 22) & 1;
          const char *size = "?";

          if (sz == 0 && q == 0) { size = "2s"; }
          else if (sz == 0 && q == 1) { size = "4s"; }
          else if (sz == 1 && q == 1) { size = "2d"; }

          snprintf(instruction, length, "%s.%s v%d, v%d",
            table_arm64[n].instr,
            size,
            rd,
            rn);

          return 4;
        }
        case OP_VECTOR_D_D_D_FPU:
        {
          int type = (opcode >> 22) & 3;
          char size[] = { 's', 'd', '?', '?' };

          snprintf(instruction, length, "%s %c%d, %c%d, %c%d",
            table_arm64[n].instr,
            size[type], rd,
            size[type], rn,
            size[type], rm);

          return 4;
        }
        case OP_VECTOR_D_D_FPU:
        {
          int type = (opcode >> 22) & 3;
          char size[] = { 's', 'd', '?', '?' };

          snprintf(instruction, length, "%s %c%d, %c%d",
            table_arm64[n].instr,
            size[type], rd,
            size[type], rn);

          return 4;
        }
        case OP_LD_ST_REG_REG:
        {
          int s = (opcode >> 12) & 1;
          int option = (opcode >> 13) & 7;

          size = (opcode >> 30) & 1;

          switch (option)
          {
            case 2:
              snprintf(instruction, length, "%s %c%d, [x%d, w%d, uxtw]",
                table_arm64[n].instr,
                size == 0 ? 'w' : 'x',
                rd,
                rn,
                rm);

              break;
            case 6:
              snprintf(instruction, length, "%s %c%d, [x%d, w%d, sxtw]",
                table_arm64[n].instr,
                size == 0 ? 'w' : 'x',
                rd,
                rn,
                rm);

              break;
            case 7:
              snprintf(instruction, length, "%s %c%d, [x%d, x%d, sxtx]",
                table_arm64[n].instr,
                size == 0 ? 'w' : 'x',
                rd,
                rn,
                rm);

              break;
            case 3:
            default:
              if (s == 0)
              {
                snprintf(instruction, length, "%s %c%d, [x%d, x%d]",
                  table_arm64[n].instr,
                  size == 0 ? 'w' : 'x',
                  rd,
                  rn,
                  rm);
              }
                else
              {
                snprintf(instruction, length, "%s %c%d, [x%d, x%d, lsl #%d]",
                  table_arm64[n].instr,
                  size == 0 ? 'w' : 'x',
                  rd,
                  rn,
                  rm,
                    size == 0 ? 2 : 3);
              }

              break;
          }

          return 4;
        }
        case OP_MUL_R_R_R_R:
        {
          if (table_arm64[n].operand_count == 3)
          {
            snprintf(instruction, length, "%s %c%d, %c%d, %c%d",
              table_arm64[n].instr,
              sf == 0 ? 'w' : 'x',
              rd,
              sf == 0 ? 'w' : 'x',
              rn,
              sf == 0 ? 'w' : 'x',
              rm);
          }
            else
          {
            snprintf(instruction, length, "%s %c%d, %c%d, %c%d, %c%d",
              table_arm64[n].instr,
              sf == 0 ? 'w' : 'x',
              rd,
              sf == 0 ? 'w' : 'x',
              rn,
              sf == 0 ? 'w' : 'x',
              rm,
              sf == 0 ? 'w' : 'x',
              ra);
          }

          return 4;
        }
        case OP_SMUL_R_R_R_R:
        {
          if (table_arm64[n].operand_count == 3)
          {
            snprintf(instruction, length, "%s x%d, w%d, w%d",
              table_arm64[n].instr,
              rd,
              rn,
              rm);
          }
            else
          {
            snprintf(instruction, length, "%s x%d, w%d, w%d, x%d",
              table_arm64[n].instr,
              rd,
              rn,
              rm,
              ra);
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
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint32_t opcode;
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_arm64(
      &asm_context->memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    opcode = memory->read32(start);

    fprintf(asm_context->list, "0x%04x: %08x %s\n", start, opcode, instruction);

    start += count;
  }
}

void disasm_range_arm64(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint32_t opcode;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_arm64(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    opcode = memory->read32(start);

    printf("0x%04x: %08x %s\n", start, opcode, instruction);

    start = start + count;
  }
}

