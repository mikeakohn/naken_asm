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

#include "disasm/mips.h"

#define READ_RAM(a) memory_read_m(memory, a)

static  const char *reg[32] =
{
  "$0",  "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
  "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
  "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
  "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

static char *id_reg[] =
{
  "STATUS_FLAG",
  "MAC_FLAG",
  "CLIPPING_FLAG",
  "RES",
  "R",
  "I",
  "Q",
  "RES",
  "RES",
  "RES",
  "TPC",
  "CMSAR0",
  "FBRST",
  "VPU_STAT",
  "RES",
  "CMSAR1",
};

int get_cycle_count_mips(uint32_t opcode)
{
  return -1;
}

static int disasm_vector(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t opcode;
  int n, r;
  char temp[32];
  int ft, fs, fd, dest;
  //int16_t offset;
  int immediate;
  int32_t offset;
  char *scaler[] = { "x", "y", "z", "w" };

  opcode = memory_read32_m(memory, address);

  instruction[0] = 0;

  for (n = 0; mips_ee_vector[n].instr != NULL; n++)
  {
    if (mips_ee_vector[n].opcode == (opcode & mips_ee_vector[n].mask))
    {
      strcpy(instruction, mips_ee_vector[n].instr);

      dest = (opcode >> 21) & 0xf;
      ft = (opcode >> 16) & 0x1f;
      fs = (opcode >> 11) & 0x1f;
      fd = (opcode >> 6) & 0x1f;

      if ((mips_ee_vector[n].flags & FLAG_DEST) != 0)
      {
        strcat(instruction, ".");
        if ((dest & 8) != 0) { strcat(instruction, "x"); }
        if ((dest & 4) != 0) { strcat(instruction, "y"); }
        if ((dest & 2) != 0) { strcat(instruction, "z"); }
        if ((dest & 1) != 0) { strcat(instruction, "w"); }
      }

      for (r = 0; r < mips_ee_vector[n].operand_count; r++)
      {
        if (r != 0) { strcat(instruction, ","); }

        switch (mips_ee_vector[n].operand[r])
        {
          case MIPS_OP_VFT:
            sprintf(temp, " $vf%d", ft);
            if ((mips_ee_vector[n].flags & FLAG_TE) != 0)
            {
              strcat(temp, scaler[(dest >> 2) & 0x3]);
            }
            break;
          case MIPS_OP_VFS:
            sprintf(temp, " $vf%d", fs);
            if ((mips_ee_vector[n].flags & FLAG_SE) != 0)
            {
              strcat(temp, scaler[dest & 0x3]);
            }
            break;
          case MIPS_OP_VFD:
            sprintf(temp, " $vf%d", fd);
            break;
          case MIPS_OP_VIT:
            sprintf(temp, " $vi%d", ft);
            break;
          case MIPS_OP_VIS:
            sprintf(temp, " $vi%d", fs);
            break;
          case MIPS_OP_VID:
            sprintf(temp, " $vi%d", fd);
            break;
          case MIPS_OP_VI01:
            sprintf(temp, " $vi01");
            break;
          case MIPS_OP_VI27:
            sprintf(temp, " $vi27");
            break;
          case MIPS_OP_I:
            strcpy(temp, " I");
            break;
          case MIPS_OP_Q:
            strcpy(temp, " Q");
            break;
          case MIPS_OP_P:
            strcpy(temp, " P");
            break;
          case MIPS_OP_R:
            strcpy(temp, " R");
            break;
          case MIPS_OP_ACC:
            strcpy(temp, " ACC");
            break;
          case MIPS_OP_OFFSET_VBASE:
            offset = opcode & 0x7ff;
            if ((offset & 0x400) != 0) { offset |= 0xf800; }
            sprintf(temp, " %d($vi%d)", offset, (opcode >> 11) & 0x1f);
            break;
          case MIPS_OP_VBASE:
            sprintf(temp, " ($vi%d)", fs);
            break;
          case MIPS_OP_VBASE_DEC:
            if (mips_ee_vector[n].operand[0] == MIPS_OP_VFS)
            {
              sprintf(temp, " (--$vi%d)", ft);
            }
              else
            {
              sprintf(temp, " (--$vi%d)", fs);
            }
            break;
          case MIPS_OP_VBASE_INC:
            if (mips_ee_vector[n].operand[0] == MIPS_OP_VFS)
            {
              sprintf(temp, " ($vi%d++)", ft);
            }
              else
            {
              sprintf(temp, " ($vi%d++)", fs);
            }
            break;
          case MIPS_OP_IMMEDIATE15_2:
            immediate = (opcode >> 6) & 0x7ff;
            sprintf(temp, " 0x%04x", immediate << 3);
            break;
          case MIPS_OP_IMMEDIATE5:
            immediate = (opcode >> 6) & 0x1f;
            if ((immediate & 0x10) != 0) { immediate |= 0xfffffff0; }
            sprintf(temp, " %d", immediate);
            break;
          default:
            strcpy(temp, " ?");
            break;
        }

        strcat(instruction, temp);
      }

      return 4;
    }
  }

  strcpy(instruction, "???");

  return 4;
}

static int disasm_n64_rsp(
  uint32_t opcode,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int n;

  int base = (opcode >> 21) & 0x1f;
  int vt = (opcode >> 16) & 0x1f;
  int element = (opcode >> 7) & 0xf;
  int offset = opcode & 0x7f;

  for (n = 0; mips_rsp_vector[n].instr != NULL; n++)
  {
    if ((opcode & mips_rsp_vector[n].mask) == mips_rsp_vector[n].opcode)
    {
      switch (mips_rsp_vector[n].type)
      {
        case OP_MIPS_RSP_NONE:
        {
          strcpy(instruction, mips_rsp_vector[n].instr);
          return 4;
        }
        case OP_MIPS_RSP_LOAD_STORE:
        {
          if ((offset & 0x40) != 0) { offset |= 0xffffff80; }
          offset = offset << mips_rsp_vector[n].shift;

          sprintf(instruction, "%s $v%d[%d], %d(%s)",
            mips_rsp_vector[n].instr,
            vt, element,
            offset, reg[base]);

          return 4;
        }
        case OP_MIPS_RSP_REG_MOVE:
        {
          int rt = (opcode >> 16) & 0x1f;
          int vd = (opcode >> 11) & 0x1f;

          sprintf(instruction, "%s %s, $v%d[%d]",
            mips_rsp_vector[n].instr,
            reg[rt],
            vd, element);

          return 4;
        }
        case OP_MIPS_RSP_REG_2:
        {
          int vd = (opcode >> 6) & 0x1f;
          int de = (opcode >> 11) & 0x1f;
          int e = (opcode >> 21) & 0xf;

          sprintf(instruction, "%s $v%d[%d], $v%d[%d]",
            mips_rsp_vector[n].instr,
            vd, de,
            vt, e);

          return 4;
        }
        case OP_MIPS_RSP_ALU:
        {
          int vd = (opcode >> 6) & 0x1f;
          int vs = (opcode >> 11) & 0x1f;
          int e = (opcode >> 21) & 0xf;

          if (e == 0)
          {
            sprintf(instruction, "%s $v%d, $v%d, $v%d",
              mips_rsp_vector[n].instr, vd, vs, vt);
          }
            else
          if (((e >> 1) & 7) == 1)
          {
            e &= 0x1;

            sprintf(instruction, "%s $v%d, $v%d, $v%d[%dq]",
              mips_rsp_vector[n].instr, vd, vs, vt, e);
          }
            else
          if (((e >> 2) & 3) == 1)
          {
            e &= 0x3;

            sprintf(instruction, "%s $v%d, $v%d, $v%d[%dh]",
              mips_rsp_vector[n].instr, vd, vs, vt, e);
          }
            else
          if (((e >> 3) & 1) == 1)
          {
            e &= 0x7;

            sprintf(instruction, "%s $v%d, $v%d, $v%d[%d]",
              mips_rsp_vector[n].instr, vd, vs, vt, e);
          }
            else
          {
            sprintf(instruction, "%s $v%d, $v%d, $v%d[?] (e=%d)",
              mips_rsp_vector[n].instr, vd, vs, vt, e);
          }

          return 4;
        }
        default:
        {
          return 0;
        }
      }
    }
  }

  return 0;
}

int disasm_mips(
  struct _memory *memory,
  uint32_t flags,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t opcode;
  int function, format, operation;
  int n, r;
  char temp[32];
  int rs, rt, rd, sa, wt, ws, wd;
  int immediate;

  *cycles_min = 1;
  *cycles_max = 1;
  opcode = memory_read32_m(memory, address);

  instruction[0] = 0;

  if (opcode == 0)
  {
    strcpy(instruction, "nop");
    return 4;
  }

  if (flags & MIPS_RSP)
  {
    int count = disasm_n64_rsp(opcode, instruction, cycles_min, cycles_max);

    if (count != 0) { return count; }
  }

  format = (opcode >> 26) & 0x3f;

  if (format == FORMAT_SPECIAL0 ||
      format == FORMAT_SPECIAL2 ||
      format == FORMAT_SPECIAL3)
  {
    // Special2 / Special3
    function = opcode & 0x3f;

    for (n = 0; mips_special_table[n].instr != NULL; n++)
    {
      // Check of this specific MIPS chip uses this instruction.
      if ((mips_special_table[n].version & flags) == 0)
      {
        continue;
      }

      if (mips_special_table[n].format == format &&
          mips_special_table[n].function == function)
      {
        uint8_t operand_reg[4] = { 0 };
        int shift;

        if (mips_special_table[n].type == SPECIAL_TYPE_REGS)
        {
          operation = (opcode >> 6) & 0x1f;
          shift = 21;
        }
          else
        if (mips_special_table[n].type == SPECIAL_TYPE_SA)
        {
          operation = (opcode >> 21) & 0x1f;
          shift = 16;
        }
          else
        if (mips_special_table[n].type == SPECIAL_TYPE_BITS ||
            mips_special_table[n].type == SPECIAL_TYPE_BITS2)
        {
          operation = 0;
          shift = 21;
        }
          else
        {
          sprintf(instruction, "internal error");
          return 4;
        }

        if (mips_special_table[n].operation != operation)
        {
          continue;
        }

        for (r = 0; r < 4; r++)
        {
          int operand_index = mips_special_table[n].operand[r];

          if (operand_index != -1)
          {
            operand_reg[operand_index] = (opcode >> shift) & 0x1f;
          }

          if (r == 2 && mips_special_table[n].type == SPECIAL_TYPE_BITS)
          {
            operand_reg[operand_index]++;
          }
            else
          if (r == 3 && mips_special_table[n].type == SPECIAL_TYPE_BITS2)
          {
            operand_reg[operand_index + 1] -= operand_reg[operand_index];
            operand_reg[operand_index + 1]++;
          }

          shift -= 5;
        }

        strcpy(instruction, mips_special_table[n].instr);

        for (r = 0; r < mips_special_table[n].operand_count; r++)
        {
          if (r < 2 || mips_special_table[n].type == SPECIAL_TYPE_REGS)
          {
            sprintf(temp, "%s", reg[(int)operand_reg[r]]);
          }
            else
          {
            sprintf(temp, "%d", operand_reg[r]);
          }

          if (r != 0) { strcat(instruction, ", "); }
          else { strcat(instruction, " "); }

          strcat(instruction, temp);
        }

        return 4;
      }
    }
  }

  for (n = 0; mips_other[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_other[n].version & flags) == 0)
    {
      continue;
    }

    if (mips_other[n].opcode == (opcode & mips_other[n].mask))
    {
      strcpy(instruction, mips_other[n].instr);

      rs = (opcode >> 21) & 0x1f;
      rt = (opcode >> 16) & 0x1f;
      rd = (opcode >> 11) & 0x1f;
      sa = (opcode >> 6) & 0x1f;
      immediate = opcode & 0xffff;

      for (r = 0; r < mips_other[n].operand_count; r++)
      {
        if (r != 0) { strcat(instruction, ","); }

        switch (mips_other[n].operand[r])
        {
          case MIPS_OP_RS:
            sprintf(temp, " %s", reg[rs]);
            break;
          case MIPS_OP_RT:
            sprintf(temp, " %s", reg[rt]);
            break;
          case MIPS_OP_RD:
            sprintf(temp, " %s", reg[rd]);
            break;
          case MIPS_OP_FT:
            sprintf(temp, " $f%d", rt);
            break;
          case MIPS_OP_FS:
            sprintf(temp, " $f%d", rd);
            break;
          case MIPS_OP_FD:
            sprintf(temp, " $f%d", sa);
            break;
          case MIPS_OP_VIS:
            sprintf(temp, " $vi%d", rd);
            break;
          case MIPS_OP_VFS:
            sprintf(temp, " $vf%d", rd);  // here
            break;
          case MIPS_OP_VFT:
            sprintf(temp, " $vf%d", rt);
            break;
          case MIPS_OP_SA:
            sprintf(temp, " %d", sa);
            break;
          case MIPS_OP_IMMEDIATE_SIGNED:
            sprintf(temp, " %d", (int16_t)immediate);
            break;
          case MIPS_OP_IMMEDIATE_RS:
            sprintf(temp, " %d(%s)", (int16_t)immediate, reg[rs]);
            break;
          case MIPS_OP_LABEL:
            if ((immediate & 0x8000) != 0) { immediate |= 0xffff0000; }
            immediate = immediate << 2;
            sprintf(temp, " 0x%08x (%d)", address + 4 + immediate, immediate);
            break;
          case MIPS_OP_PREG:
            sprintf(temp, " %d", (immediate >> 1) & 0x1f);
            break;
          case MIPS_OP_ID_REG:
            if (rd < 16)
            {
              sprintf(temp, " $vi%d [%d]", rd, rd);
            }
              else
            {
              sprintf(temp, " %s [%d]", id_reg[rd - 16], rd);
            }
            break;
          case MIPS_OP_OPTIONAL:
            immediate = (opcode >> 6) & 0xfffff;
            if (immediate != 0) { sprintf(temp, " 0x%x", immediate); }
            else { temp[0] = 0; }
            break;
          default:
            strcpy(temp, " ?");
            break;
        }

        strcat(instruction, temp);
      }

      return 4;
    }
  }

  for (n = 0; mips_ee[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_ee[n].version & flags) == 0) { continue; }

    if (mips_ee[n].opcode == (opcode & mips_ee[n].mask))
    {
      strcpy(instruction, mips_ee[n].instr);

      rs = (opcode >> 21) & 0x1f;
      rt = (opcode >> 16) & 0x1f;
      rd = (opcode >> 11) & 0x1f;
      sa = (opcode >> 6) & 0x1f;
      immediate = opcode & 0xffff;

      for (r = 0; r < mips_ee[n].operand_count; r++)
      {
        if (r != 0) { strcat(instruction, ","); }

        switch (mips_ee[n].operand[r])
        {
          case MIPS_OP_RS:
            sprintf(temp, " %s", reg[rs]);
            break;
          case MIPS_OP_RT:
            sprintf(temp, " %s", reg[rt]);
            break;
          case MIPS_OP_RD:
            sprintf(temp, " %s", reg[rd]);
            break;
          case MIPS_OP_FT:
            sprintf(temp, " $f%d", rt);
            break;
          case MIPS_OP_FS:
            sprintf(temp, " $f%d", rd);
            break;
          case MIPS_OP_FD:
            sprintf(temp, " $f%d", sa);
            break;
          case MIPS_OP_VIS:
            sprintf(temp, " $vi%d", rd);
            break;
          case MIPS_OP_VFS:
            sprintf(temp, " $vf%d", rd);  // here
            break;
          case MIPS_OP_VFT:
            sprintf(temp, " $vf%d", rt);
            break;
          case MIPS_OP_SA:
            sprintf(temp, " %d", sa);
            break;
          case MIPS_OP_IMMEDIATE_SIGNED:
            sprintf(temp, " %d", (int16_t)immediate);
            break;
          case MIPS_OP_IMMEDIATE_RS:
            sprintf(temp, " %d(%s)", (int16_t)immediate, reg[rs]);
            break;
          case MIPS_OP_LABEL:
            if ((immediate & 0x8000) != 0) { immediate |= 0xffff0000; }
            immediate = immediate << 2;
            sprintf(temp, " 0x%08x (%d)", address + 4 + immediate, immediate);
            break;
          case MIPS_OP_PREG:
            sprintf(temp, " %d", (immediate >> 1) & 0x1f);
            break;
          case MIPS_OP_ID_REG:
            if (rd < 16)
            {
              sprintf(temp, " $vi%d [%d]", rd, rd);
            }
              else
            {
              sprintf(temp, " %s [%d]", id_reg[rd - 16], rd);
            }
            break;
          case MIPS_OP_OPTIONAL:
            immediate = (opcode >> 6) & 0xfffff;
            if (immediate != 0) { sprintf(temp, " 0x%x", immediate); }
            else { temp[0] = 0; }
            break;
          default:
            strcpy(temp, " ?");
            break;
        }

        strcat(instruction, temp);
      }

      return 4;
    }
  }

  for (n = 0; mips_four_reg[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_four_reg[n].version & flags) == 0) { continue; }

    if (mips_four_reg[n].opcode == (opcode & mips_four_reg[n].mask))
    {
      int fr = (opcode >> 21) & 0x1f;
      int ft = (opcode >> 16) & 0x1f;
      int fs = (opcode >> 11) & 0x1f;
      int fd = (opcode >> 6) & 0x1f;

      sprintf(instruction,"%s $f%d, $f%d, $f%d, $f%d",
        mips_four_reg[n].instr, fd, fr, fs, ft);

      return 4;
    }
  }

  for (n = 0; mips_msa[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_msa[n].version & flags) == 0)
    {
      continue;
    }

    if (mips_msa[n].opcode == (opcode & mips_msa[n].mask))
    {
      strcpy(instruction, mips_msa[n].instr);

      wt = (opcode >> 16) & 0x1f;
      ws = (opcode >> 11) & 0x1f;
      wd = (opcode >> 6) & 0x1f;

      for (r = 0; r < mips_msa[n].operand_count; r++)
      {
        if (r != 0) { strcat(instruction, ","); }

        switch (mips_msa[n].operand[r])
        {
          case MIPS_OP_WT:
            sprintf(temp, " $w%d", wt);
            break;
          case MIPS_OP_WS:
            sprintf(temp, " $w%d", ws);
            break;
          case MIPS_OP_WD:
            sprintf(temp, " $w%d", wd);
            break;
          default:
            strcpy(temp, " ?");
            break;
        }

        strcat(instruction, temp);
      }

      return 4;
    }
  }

  for (n = 0; mips_branch_table[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_branch_table[n].version & flags) == 0)
    {
      continue;
    }

    if (mips_branch_table[n].op_rt == -1)
    {
      if ((opcode >> 26) == mips_branch_table[n].opcode)
      {
        rs = (opcode >> 21) & 0x1f;
        rt = (opcode >> 16) & 0x1f;
        int16_t offset = (opcode & 0xffff) << 2;

        sprintf(instruction, "%s %s, %s, 0x%x (offset=%d)", mips_branch_table[n].instr, reg[rs], reg[rt],  address + 4 + offset, offset);

        return 4;
      }
    }
      else
    {
      if ((opcode >> 26) == mips_branch_table[n].opcode &&
         ((opcode >> 16) & 0x1f) == mips_branch_table[n].op_rt)
      {
        rs = (opcode >> 21) & 0x1f;
        int16_t offset = (opcode & 0xffff) << 2;

        sprintf(instruction, "%s %s, 0x%x (offset=%d)", mips_branch_table[n].instr, reg[rs], address + 4 + offset, offset);

        return 4;
      }
    }
  }

  if (format == 0)
  {
    // R-Type Instruction [ op 6, rs 5, rt 5, rd 5, sa 5, function 6 ]
    function = opcode & 0x3f;

    for (n = 0; mips_r_table[n].instr != NULL; n++)
    {
      // Check of this specific MIPS chip uses this instruction.
      if ((mips_r_table[n].version & flags) == 0)
      {
        continue;
      }

      if (mips_r_table[n].function == function)
      {
        rs = (opcode >> 21) & 0x1f;
        rt = (opcode >> 16) & 0x1f;
        rd = (opcode >> 11) & 0x1f;
        sa = (opcode >> 6) & 0x1f;

        strcpy(instruction, mips_r_table[n].instr);

        for (r = 0; r < 3; r++)
        {
          if (mips_r_table[n].operand[r] == MIPS_OP_NONE) { break; }

          if (mips_r_table[n].operand[r] == MIPS_OP_RS)
          {
            sprintf(temp, "%s", reg[rs]);
          }
            else
          if (mips_r_table[n].operand[r] == MIPS_OP_RT)
          {
            sprintf(temp, "%s", reg[rt]);
          }
            else
          if (mips_r_table[n].operand[r] == MIPS_OP_RD)
          {
            sprintf(temp, "%s", reg[rd]);
          }
            else
          if (mips_r_table[n].operand[r] == MIPS_OP_SA)
          {
            sprintf(temp, "%d", sa);
          }
            else
          { temp[0] = 0; }

          if (r != 0) { strcat(instruction, ", "); }
          else { strcat(instruction, " "); }

          strcat(instruction, temp);
        }

        break;
      }
    }
  }
    else
  if ((opcode >> 27) == 1)
  {
    // J-Type Instruction [ op 6, target 26 ]
    uint32_t upper = (address + 4) & 0xf0000000;

    if ((opcode >> 26) == 2)
    {
      sprintf(instruction, "j 0x%08x", ((opcode & 0x03ffffff) << 2) | upper);
    }
      else
    {
      sprintf(instruction, "jal 0x%08x", ((opcode & 0x03ffffff) << 2) | upper);
    }
  }
    else
  if ((flags & MIPS_EE_VU) && (opcode >> 26) == 0x12)
  {
    disasm_vector(memory, address, instruction, cycles_min, cycles_max);
  }
    else
  {
    int op = opcode >> 26;
    // I-Type?  [ op 6, rs 5, rt 5, imm 16 ]

    for (n = 0; mips_i_table[n].instr != NULL; n++)
    {
      // Check of this specific MIPS chip uses this instruction.
      if ((mips_i_table[n].version & flags) == 0)
      {
        continue;
      }

      if (mips_i_table[n].function == op)
      {
        rs = (opcode >> 21) & 0x1f;
        rt = (opcode >> 16) & 0x1f;
        immediate = opcode & 0xffff;

        strcpy(instruction, mips_i_table[n].instr);

        for (r = 0; r < 3; r++)
        {
          if (mips_i_table[n].operand[r] == MIPS_OP_NONE) { break; }

          if (mips_i_table[n].operand[r] == MIPS_OP_RS)
          {
            sprintf(temp, "%s", reg[rs]);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_RT)
          {
            sprintf(temp, "%s", reg[rt]);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_HINT ||
              mips_i_table[n].operand[r] == MIPS_OP_CACHE)
          {
            sprintf(temp, "%d", rt);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_FT)
          {
            sprintf(temp, "$f%d", rt);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_IMMEDIATE)
          {
            sprintf(temp, "0x%x", immediate);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_IMMEDIATE_SIGNED)
          {
            sprintf(temp, "0x%x (%d)", immediate, (int16_t)immediate);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_IMMEDIATE_RS)
          {
            sprintf(temp, "0x%x(%s)", immediate, reg[rs]);
          }
            else
          if (mips_i_table[n].operand[r] == MIPS_OP_LABEL)
          {
            int32_t offset = (int16_t)immediate;

            offset = offset << 2;

            sprintf(temp, "0x%x (offset=%d)", address + 4 + offset, offset);
          }
            else
          { temp[0] = 0; }

          if (r != 0) { strcat(instruction, ", "); }
          else { strcat(instruction, " "); }
          strcat(instruction, temp);
        }

        break;
      }
    }

    if (mips_i_table[n].instr == NULL)
    {
      //printf("Internal Error: Unknown MIPS opcode %08x, %s:%d\n", opcode, __FILE__, __LINE__);
      strcpy(instruction, "???");
    }
  }

  return 4;
}

void list_output_mips(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_mips(
      &asm_context->memory,
      asm_context->flags,
      start,
      instruction,
      &cycles_min,
      &cycles_max);

    fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += 4;
  }
}

void disasm_range_mips(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start < end)
  {
    // FIXME - Need to check endian
#if 0
    num = READ_RAM(start) |
         (READ_RAM(start + 1) << 8) |
         (READ_RAM(start + 2) << 16) |
         (READ_RAM(start + 3) << 24);
#endif
    num = memory_read32_m(memory, start);

    disasm_mips(memory, flags, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

    start += 4;
  }
}

