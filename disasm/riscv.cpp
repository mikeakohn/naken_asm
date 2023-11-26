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

#include "disasm/riscv.h"
#include "table/riscv.h"

const char *riscv_reg_names[32] =
{
  "zero", "ra",  "sp",  "gp", "tp", "t0", "t1", "t2",
    "fp", "s1",  "a0",  "a1", "a2", "a3", "a4", "a5",
    "a6", "a7",  "s2",  "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// REVIEW: Probably don't need this since compressed register
// can be mapped to reg + 8. Still not sure what to do with s0/fp.
#if 0
const char *riscv_reg_comp_names[8] =
{
  // "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15"
  "s0/fp", "s1",  "a0",  "a1",  "a2",  "a3",  "a4",  "a5"
}
#endif

static const char *rm_string[] =
{
  ", rne",
  ", rtz",
  ", rdn",
  ", rup",
  ", rmm",
  ", [error]",
  ", [error]",
  "",
};

static const char *fence_string[] =
{
  "sw",
  "sr",
  "so",
  "si",
  "pw",
  "pr",
  "po",
  "pi",
};

static int32_t permutate_branch(uint32_t opcode)
{
  int32_t immediate;

  immediate = ((opcode >> 31) & 0x1) << 12;
  immediate |= ((opcode >> 8) & 0xf) << 1;
  immediate |= ((opcode >> 7) & 0x1) << 11;
  immediate |= ((opcode >> 25) & 0x3f) << 5;
  if ((immediate & 0x1000) != 0) { immediate |= 0xffffe000; }

  return immediate;
}

static int32_t permutate_jal(uint32_t opcode)
{
  int32_t offset;

  offset = ((opcode >> 31) & 0x1) << 20;
  offset |= ((opcode >> 12) & 0xff) << 12;
  offset |= ((opcode >> 20) & 0x1) << 11;
  offset |= ((opcode >> 21) & 0x3ff) << 1;
  if ((offset & 0x100000) != 0) { offset |= 0xfff00000; }

  return offset;
}

int disasm_riscv(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t opcode;
  uint32_t immediate;
  int32_t offset;
  int32_t simmediate;
  int n;
  char temp[16];
  int count, i;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory->read32(address);

  //if ((opcode & 3) != 3 || (address & 3) != 0)
  if ((opcode & 3) != 3)
  {
    return disasm_riscv_comp(
      memory,
      address,
      instruction,
      length,
      flags,
      cycles_min,
      cycles_max);
  }

  for (n = 0; table_riscv[n].instr != NULL; n++)
  {
    if ((opcode & table_riscv[n].mask) == table_riscv[n].opcode)
    {
      uint32_t rd = (opcode >> 7) & 0x1f;
      uint32_t rs1 = (opcode >> 15) & 0x1f;
      uint32_t rs2 = (opcode >> 20) & 0x1f;
      uint32_t rs3 = (opcode >> 27) & 0x1f;
      uint32_t rm = (opcode >> 12) & 0x7;
      const char *instr = table_riscv[n].instr;

      switch (table_riscv[n].type)
      {
        case OP_NONE:
          snprintf(instruction, length, "%s", instr);
          break;
        case OP_R_TYPE:
          snprintf(instruction, length, "%s %s, %s, %s",
            instr,
            riscv_reg_names[rd],
            riscv_reg_names[rs1],
            riscv_reg_names[rs2]);
          break;
        case OP_R_R:
          snprintf(instruction, length, "%s %s, %s",
            instr,
            riscv_reg_names[rd],
            riscv_reg_names[rs1]);
          break;
        case OP_I_TYPE:
          immediate = opcode >> 20;
          simmediate = immediate;
          if ((simmediate & 0x800) != 0) { simmediate |= 0xfffff000; }
          snprintf(instruction, length, "%s %s, %s, %d (0x%06x)",
            instr,
            riscv_reg_names[rd],
            riscv_reg_names[rs1],
            simmediate,
            immediate);
          break;
        case OP_UI_TYPE:
          immediate = opcode >> 20;
          snprintf(instruction, length, "%s %s, 0x%x",
            instr,
            riscv_reg_names[rd],
            immediate);
          break;
        case OP_SB_TYPE:
          immediate = permutate_branch(opcode);
          snprintf(instruction, length, "%s %s, %s, 0x%x (%d)",
            instr,
            riscv_reg_names[rs1],
            riscv_reg_names[rs2],
            address + immediate,
            immediate);
          break;
        case OP_U_TYPE:
          immediate = opcode >> 12;
          snprintf(instruction, length, "%s %s, 0x%06x",
            instr,
            riscv_reg_names[rd],
            immediate);
          break;
        case OP_UJ_TYPE:
          offset = permutate_jal(opcode);
          snprintf(instruction, length, "%s %s, 0x%x (offset=%d)",
            instr,
            riscv_reg_names[rd],
            address + offset,
            offset);
          break;
        case OP_SHIFT:
          immediate = (opcode >> 20) & 0x1f;
          snprintf(instruction, length, "%s %s, %s, %d",
            instr,
            riscv_reg_names[rd],
            riscv_reg_names[rs1],
            immediate);
          break;
        case OP_FENCE:
          immediate = (opcode >> 20) & 0xff;
          count = 0;
          snprintf(instruction, length, "%s", instr);
          for (i = 7; i >= 0; i--)
          {
            if ((immediate & (1 << i)) != 0)
            {
              if (count == 0) { strcat(instruction, " "); }
              else { strcat(instruction, ", "); }
              strcat(instruction, fence_string[i]);
              count++;
            }
          }
          break;
        case OP_FFFF:
          snprintf(instruction, length, "%s", instr);
          break;
        case OP_READ:
          snprintf(instruction, length, "%s %s", instr, riscv_reg_names[rd]);
          break;
        case OP_RD_INDEX_R:
          immediate = opcode >> 20;
          simmediate = immediate;
          if ((simmediate & 0x800) != 0) { simmediate |= 0xfffff000; }
          snprintf(instruction, length, "%s %s, %d(%s)",
            instr,
            riscv_reg_names[rd],
            simmediate,
            riscv_reg_names[rs1]);
          break;
        case OP_FD_INDEX_R:
          immediate = opcode >> 20;
          simmediate = immediate;
          if ((simmediate & 0x800) != 0) { simmediate |= 0xfffff000; }
          snprintf(instruction, length, "%s f%d, %d(%s)",
            instr,
            rd,
            simmediate,
            riscv_reg_names[rs1]);
          break;
        case OP_RS_INDEX_R:
          immediate = ((opcode >> 25) & 0x7f) << 5;
          immediate |= ((opcode >> 7) & 0x1f);
          if ((immediate & 0x800) != 0) { immediate |= 0xfffff000; }
          snprintf(instruction, length, "%s %s, %d(%s)",
            instr,
            riscv_reg_names[rs2],
            immediate,
            riscv_reg_names[rs1]);
          break;
        case OP_FS_INDEX_R:
          immediate = ((opcode >> 25) & 0x7f) << 5;
          immediate |= ((opcode >> 7) & 0x1f);
          if ((immediate & 0x800) != 0) { immediate |= 0xfffff000; }
          snprintf(instruction, length, "%s f%d, %d(%s)",
            instr,
            rs2,
            immediate,
            riscv_reg_names[rs1]);
          break;
        case OP_LR:
          temp[0] = 0;
          if ((opcode & (1 << 26)) != 0) { strcat(temp, ".aq"); }
          if ((opcode & (1 << 25)) != 0) { strcat(temp, ".rl"); }
          snprintf(instruction, length, "%s%s %s, (%s)",
            instr,
            temp,
            riscv_reg_names[rd],
            riscv_reg_names[rs1]);
          break;
        case OP_STD_EXT:
          temp[0] = 0;
          if ((opcode & (1 << 26)) != 0) { strcat(temp, ".aq"); }
          if ((opcode & (1 << 25)) != 0) { strcat(temp, ".rl"); }
          // FIXME - The docs say rs2 and rs1 are reversed. gnu-as is like this.
          snprintf(instruction, length, "%s%s %s, %s, (%s)",
            instr,
            temp,
            riscv_reg_names[rd],
            riscv_reg_names[rs2],
            riscv_reg_names[rs1]);
          break;
        case OP_R_FP_RM:
          snprintf(instruction, length, "%s %s, f%d%s",
            instr,
            riscv_reg_names[rd],
            rs1,
            rm_string[rm]);
          break;
        case OP_R_FP_FP:
          snprintf(instruction, length, "%s %s, f%d, f%d",
            instr,
            riscv_reg_names[rd],
            rs1,
            rs2);
          break;
        case OP_FP:
          snprintf(instruction, length, "%s f%d", instr, rd);
          break;
        case OP_FP_FP:
          snprintf(instruction, length, "%s f%d, f%d", instr, rd, rs1);
          break;
        case OP_FP_FP_FP:
          snprintf(instruction, length, "%s f%d, f%d, f%d", instr, rd, rs1, rs2);
          break;
        case OP_FP_FP_RM:
          snprintf(instruction, length, "%s f%d, f%d%s", instr, rd, rs1, rm_string[rm]);
          break;
        case OP_FP_R:
          snprintf(instruction, length, "%s f%d, %s",
            instr,
            rd,
            riscv_reg_names[rs1]);
          break;
        case OP_FP_R_RM:
          snprintf(instruction, length, "%s f%d, %s%s",
            instr,
            rd,
            riscv_reg_names[rs1],
            rm_string[rm]);
          break;
        case OP_FP_FP_FP_RM:
          snprintf(instruction, length, "%s f%d, f%d, f%d%s", instr, rd, rs1, rs2, rm_string[rm]);
          break;
        case OP_FP_FP_FP_FP_RM:
          snprintf(instruction, length, "%s f%d, f%d, f%d, f%d%s", instr, rd, rs1, rs2, rs3, rm_string[rm]);
          break;
        case OP_ALIAS_RD_RS1:
          snprintf(instruction, length, "%s %s, %s",
            instr,
            riscv_reg_names[rd],
            riscv_reg_names[rs1]);
          break;
        case OP_ALIAS_RD_RS2:
          snprintf(instruction, length, "%s %s, %s",
            instr,
            riscv_reg_names[rd],
            riscv_reg_names[rs2]);
          break;
        case OP_ALIAS_FP_FP:
          if (rs1 != rs2) { continue; }
          snprintf(instruction, length, "%s f%d, f%d", instr, rd, rs1);
          break;
        case OP_ALIAS_BR_RS_X0:
          immediate = permutate_branch(opcode);
          snprintf(instruction, length, "%s %s, 0x%x (%d)",
            instr,
            riscv_reg_names[rs1],
            address + immediate,
            immediate);
          break;
        case OP_ALIAS_BR_X0_RS:
          immediate = permutate_branch(opcode);
          snprintf(instruction, length, "%s %s, 0x%x (%d)",
            instr,
            riscv_reg_names[rs2],
            address + immediate,
            immediate);
          break;
        case OP_ALIAS_BR_RS_RT:
          immediate = permutate_branch(opcode);
          snprintf(instruction, length, "%s %s, %s, 0x%x (%d)",
            instr,
            riscv_reg_names[rs2],
            riscv_reg_names[rs1],
            address + immediate,
            immediate);
          break;
        case OP_ALIAS_JAL:
          offset = permutate_jal(opcode);
          snprintf(instruction, length, "%s 0x%x (offset=%d)",
            instr,
            address + offset,
            offset);
          break;
        case OP_ALIAS_JALR:
          immediate = opcode >> 20;
          simmediate = immediate;
          if ((simmediate & 0x800) != 0) { simmediate |= 0xfffff000; }
          snprintf(instruction, length, "%s %s, %d (0x%06x)",
            instr,
            riscv_reg_names[rs1],
            simmediate,
            immediate);
          break;
        default:
          strcpy(instruction, "???");
          break;
      }

      return 4;
    }
  }

  strcpy(instruction, "???");

  return -1;
}

static int permutate_16(int opcode, int8_t *table)
{
  int value = 0;

  for (int n = 0; n < 11; n++)
  {
    const int bit = table[n];

    if (bit == -1) { continue; }
    int i = (opcode >> (12 - n)) & 1;

    value |= i << bit;
  }

  return value;
}

int disasm_riscv_comp(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  strcpy(instruction, "???");

  int opcode = memory->read16(address);
  int rd = (opcode >> 2) & 7;
  int rs1 = (opcode >> 7) & 7;
  int rs1_32 = (opcode >> 7) & 0x1f;
  int rs2_32 = (opcode >> 2) & 0x1f;
  int immediate;
  //int funct3 = opcode >> 13;

  for (int n = 0; table_riscv_comp[n].instr != NULL; n++)
  {
    if ((opcode & table_riscv_comp[n].mask) == table_riscv_comp[n].opcode)
    {
      const char *instr = table_riscv_comp[n].instr;

      switch (table_riscv_comp[n].type)
      {
        case OP_NONE:
          snprintf(instruction, length, "%s", instr);
          return 2;
        case OP_COMP_RD_NZUIMM:
          immediate = permutate_16(opcode, RiscvPerm::nzuimm);
          snprintf(instruction, length, "%s %s, sp, 0x%x",
            instr,
            riscv_reg_names[rd + 8],
            immediate);
          return 2;
        case OP_COMP_UIMM53_76:
          immediate = permutate_16(opcode, RiscvPerm::uimm53_76);

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            snprintf(instruction, length, "%s %s, %d(%s)",
              instr,
              riscv_reg_names[rd + 8],
              immediate,
              riscv_reg_names[rs1 + 8]);
          }
            else
          {
            snprintf(instruction, length, "%s f%d, %d(%s)",
              instr,
              rd + 8,
              immediate,
              riscv_reg_names[rs1 + 8]);
          }

          return 2;
        case OP_COMP_UIMM548_76:
          immediate = permutate_16(opcode, RiscvPerm::uimm548_76);
          snprintf(instruction, length, "%s %s, %d(%s)",
            instr,
            riscv_reg_names[rd + 8],
            immediate,
            riscv_reg_names[rs1 + 8]);
          return 2;
        case OP_COMP_UIMM53_26:
          immediate = permutate_16(opcode, RiscvPerm::uimm53_26);

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            snprintf(instruction, length, "%s %s, %d(%s)",
              instr,
              riscv_reg_names[rd + 8],
              immediate,
              riscv_reg_names[rs1 + 8]);
          }
            else
          {
            snprintf(instruction, length, "%s f%d, %d(%s)",
              instr,
              rd + 8,
              immediate,
              riscv_reg_names[rs1 + 8]);
          }
          return 2;
        case OP_COMP_JUMP:
          immediate = permutate_16(opcode, RiscvPerm::jump);
          if ((immediate & 0x800) != 0) { immediate |= 0xfffff000; }
          snprintf(instruction, length, "%s 0x%04x (offset=%d)",
            instr,
            address + immediate,
            immediate);
          return 2;
        case OP_COMP_9_46875:
          immediate = permutate_16(opcode, RiscvPerm::imm9_46875);
          snprintf(instruction, length, "%s 0x%04x",
            instr,
            immediate);
          return 2;
        case OP_COMP_RD_NZIMM5:
          immediate = permutate_16(opcode, RiscvPerm::nzimm5);
          snprintf(instruction, length, "%s %s, 0x%04x",
            instr,
            riscv_reg_names[rs1_32],
            immediate);
          return 2;
        case OP_COMP_RD_IMM5:
          immediate = permutate_16(opcode, RiscvPerm::imm5);
          snprintf(instruction, length, "%s %s, 0x%04x",
            instr,
            riscv_reg_names[rs1_32],
            immediate);
          return 2;
        case OP_COMP_RD_17_1612:
          immediate = permutate_16(opcode, RiscvPerm::imm17_1612);
          snprintf(instruction, length, "%s %s, 0x%04x",
            instr,
            riscv_reg_names[rs1_32],
            immediate);
          return 2;
        case OP_COMP_RD_NZ5_40:
        case OP_COMP_RD_5_40:
          immediate = permutate_16(opcode, RiscvPerm::imm5);
          snprintf(instruction, length, "%s %s, %d",
            instr,
            riscv_reg_names[rs1 + 8],
            immediate);
          return 2;
        case OP_COMP_RD:
          snprintf(instruction, length, "%s %s",
            instr,
            riscv_reg_names[rd + 8]);
          return 2;
        case OP_COMP_RD_RS2:
          snprintf(instruction, length, "%s %s, %s",
            instr,
            riscv_reg_names[rs1 + 8],
            riscv_reg_names[rd + 8]);
          return 2;
        case OP_COMP_BRANCH:
          immediate = permutate_16(opcode, RiscvPerm::branch);
          if ((immediate & 0x100) != 0) { immediate |= 0xffffff00; }
          snprintf(instruction, length, "%s 0x%04x (offset=%d)",
            instr,
            address + immediate,
            immediate);
          return 2;
        case OP_COMP_RD32:
          snprintf(instruction, length, "%s %s",
            instr,
            riscv_reg_names[rs1_32]);
          return 2;
        case OP_COMP_RD_5_4386:
          immediate = permutate_16(opcode, RiscvPerm::uimm5_4386);

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            snprintf(instruction, length, "%s %s, %d(sp)",
              instr,
              riscv_reg_names[rs1_32],
              immediate);
          }
            else
          {
            snprintf(instruction, length, "%s f%d, %d(sp)",
              instr,
              rs1_32,
              immediate);
          }
          return 2;
        case OP_COMP_RD_5_496:
          immediate = permutate_16(opcode, RiscvPerm::uimm5_496);
          snprintf(instruction, length, "%s %s, %d(sp)",
            instr,
            riscv_reg_names[rs1_32],
            immediate);
          return 2;
        case OP_COMP_RD_5_4276:
          immediate = permutate_16(opcode, RiscvPerm::uimm5_4276);

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            snprintf(instruction, length, "%s %s, %d(sp)",
              instr,
              riscv_reg_names[rs1_32],
              immediate);
          }
            else
          {
            snprintf(instruction, length, "%s f%d, %d(sp)",
              instr,
              rs1_32,
              immediate);
          }
          return 2;
        case OP_COMP_RS1_RS2:
          snprintf(instruction, length, "%s %s, %s",
            instr,
            riscv_reg_names[rs1_32],
            riscv_reg_names[rs2_32]);
          return 2;
        case OP_COMP_5386_RS2:
          immediate = permutate_16(opcode, RiscvPerm::uimm5386);

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            snprintf(instruction, length, "%s %s, %d(sp)",
              instr,
              riscv_reg_names[rs2_32],
              immediate);
          }
            else
          {
            snprintf(instruction, length, "%s f%d, %d(sp)",
              instr,
              rs2_32,
              immediate);
          }
          return 2;
        case OP_COMP_5496_RS2:
          immediate = permutate_16(opcode, RiscvPerm::uimm5496);
          snprintf(instruction, length, "%s %s, %d(sp)",
            instr,
            riscv_reg_names[rs2_32],
            immediate);
          return 2;
        case OP_COMP_5276_RS2:
          immediate = permutate_16(opcode, RiscvPerm::uimm5276);
          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            snprintf(instruction, length, "%s %s, %d(sp)",
              instr,
              riscv_reg_names[rs2_32],
              immediate);
          }
            else
          {
            snprintf(instruction, length, "%s f%d, %d(sp)",
              instr,
              rs2_32,
              immediate);
          }
          return 2;
        case OP_COMP_HUA_043_21:
          immediate = permutate_16(opcode, RiscvPerm::uimm043_21);
          snprintf(instruction, length, "%s %s, %d(%s)",
            instr,
            riscv_reg_names[rd],
            immediate,
            riscv_reg_names[rs1]);
          return 2;
        case OP_COMP_HUA_53_21:
          immediate = permutate_16(opcode, RiscvPerm::uimm53_21);
          snprintf(instruction, length, "%s %s, %d(%s)",
            instr,
            riscv_reg_names[rd],
            immediate,
            riscv_reg_names[rs1]);
          return 2;
        default:
          strcpy(instruction, "???");
          return 2;
      }
    }
  }

  return 2;
}

void list_output_riscv(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  uint32_t opcode;
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    opcode = memory->read32(start);

    count = disasm_riscv(
      memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    if (count == 2)
    {
      fprintf(asm_context->list, "0x%08x: 0x%04x     %-40s cycles: ",
        start, opcode, instruction);
    }
      else
    {
      fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ",
        start, opcode, instruction);
    }

    if (cycles_min == -1)
    {
      fprintf(asm_context->list, "\n");
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

    start += 4;
  }
}

void disasm_range_riscv(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  uint32_t opcode;
  int cycles_min = 0, cycles_max = 0;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    opcode = memory->read32(start);

    count = disasm_riscv(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    if (count == 2)
    {
      printf("0x%08x: 0x%04x     %-40s cycles: ", start, opcode, instruction);
    }
      else
    {
      printf("0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);
    }

    if (cycles_min == -1)
    {
      printf("\n");
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

    start = start + count;
  }
}

