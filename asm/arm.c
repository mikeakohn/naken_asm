/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/arm.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "disasm/arm.h"
#include "table/arm.h"

#define ARM_ERROR_OPCOMBO -1
#define ARM_UNKNOWN_INSTRUCTION -2
#define ARM_ILLEGAL_OPERANDS -3
#define ARM_ERROR_ADDRESS -4

static char *arm_cond_a[16] =
{
  "eq", "ne", "cs", "cc",
  "mi", "pl", "vs", "vc",
  "hi", "ls", "ge", "lt",
  "gt", "le", "al", "nv"
};

enum
{
  OPERAND_NOTHING,
  OPERAND_REG,
  OPERAND_REG_WRITE_BACK,
  OPERAND_IMMEDIATE,
  OPERAND_SHIFT_IMMEDIATE,
  OPERAND_SHIFT_REG,
  OPERAND_NUMBER,
  OPERAND_CONSTANT,
  OPERAND_PSR,
  OPERAND_PSRF,
  OPERAND_MULTIPLE_REG,
  OPERAND_REG_INDEXED,
  OPERAND_REG_INDEXED_OPEN,
  OPERAND_REG_INDEXED_CLOSE,
  OPERAND_IMM_INDEXED_CLOSE,
  OPERAND_SHIFT_IMM_INDEXED_CLOSE,
  OPERAND_SHIFT_IMM_INDEXED_CLOSE_WB,
  OPERAND_SHIFT_REG_INDEXED_CLOSE,
  OPERAND_SHIFT_REG_INDEXED_CLOSE_WB,
};

struct _operand
{
  uint32_t value;
  int type;
  int sub_type;
  uint8_t set_cond : 1;
};

#if 0
static void print_error_extra_condition(asm_context, char *instr)
{
  printf("Error: Instruction '%s' takes no conditionals at %s:%d\n", instr, asm_context->tokens.filename, asm_context->tokens.line);
}
#endif

static int get_register_arm(char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
    {
      return token[1] - '0';
    }
      else
    if (token[3] == 0 && token[1] == '1' && token[2] >= '0' && token[2] <= '5')
    {
      return 10 + (token[2] - '0');
    }
  }

  if (strcasecmp("sp", token) == 0) { return 13; }
  if (strcasecmp("lr", token) == 0) { return 14; }
  if (strcasecmp("pc", token) == 0) { return 15; }

  return -1;
}

static int parse_condition(char **instr_case)
{
  int cond;
  char *instr = *instr_case;

  for (cond = 0; cond < 16; cond++)
  {
    if (strncmp(instr, arm_cond_a[cond], 2) == 0)
    { *instr_case += 2; break; }
  }
  if (cond == 16) { cond = 14; }

  return cond;
}

static int compute_immediate(int immediate)
{
  unsigned int i = immediate;
  int n;

  for (n = 0; n < 16; n++)
  {
    if (i < 256) { return i | (n << 8); }
    i=((i & 0xc0000000) >> 30) | (i << 2);
  }

  return -1;
}

static int imm_shift_to_immediate(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int pos)
{
  if (operands[pos].value >= 256 || (int32_t)operands[pos].value < 0)
  {
    printf("Error: Immediate out of range for #imm, shift at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
    return -1;
  }

  if ((operands[pos+1].value&1) == 1 || (operands[pos+1].sub_type != 3) ||
       operands[pos+1].value > 30 || (int32_t)operands[pos+1].value < 0)
  {
    printf("Error: Bad shift value for #imm, shift at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
    return -1;
  }

  return operands[pos].value | (((operands[pos+1].value >> 1) << 8));
}

static int compute_range(int r1, int r2)
{
  int value = 0;

  if (r1 == r2) { return 1 << r1; }
  if (r2 < r1) { int temp = r1; r1 = r2; r2 = temp; }

  value = (1 << (r2 + 1)) - 1;
  if (r1 != 0) { value ^= (1 << r1) - 1; }

  return value;
}

static int parse_alu_3(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int immediate = 0;
  int s = 0; // S flag
  int rd = 0,rn = 0;
  int i = 0;

  int cond = parse_condition(&instr);

  if (instr[0] == 's') { s = 1; instr++; }

  if (*instr != 0)
  {
    return ARM_UNKNOWN_INSTRUCTION;
  }

  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG &&
      operands[2].type == OPERAND_IMMEDIATE)
  {
    // add rd, rn, #imm
    rd = operands[0].value;
    rn = operands[1].value;
    immediate = compute_immediate(operands[2].value);
    i = 1;
  }
    else
  if (operand_count == 4 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG &&
      operands[2].type == OPERAND_IMMEDIATE &&
      operands[3].type == OPERAND_SHIFT_IMMEDIATE)
  {
    // add rd, rn, #imm, shift
    rd = operands[0].value;
    rn = operands[1].value;
    immediate = imm_shift_to_immediate(asm_context, operands, operand_count, 2);
    i = 1;
    if (immediate < 0) { return -1; }
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG &&
      operands[2].type == OPERAND_REG)
  {
    // add rd, rn, rm
    rd = operands[0].value;
    rn = operands[1].value;
    immediate = (0 << 4) | operands[2].value;
  }
    else
  if (operand_count == 4 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG &&
      operands[2].type == OPERAND_REG &&
      (operands[3].type == OPERAND_SHIFT_IMMEDIATE ||
       operands[3].type == OPERAND_SHIFT_REG))
  {
    rd = operands[0].value;
    rn = operands[1].value;

    if (operands[3].type == OPERAND_SHIFT_IMMEDIATE)
    {
      // add rd, rn, rm, shift #
      immediate = operands[2].value | (((operands[3].value << 3) | (operands[3].sub_type << 1)) << 4);
    }
      else
    {
      // add rd, rn, rm, shift reg
      immediate = operands[2].value | (((operands[3].value << 4) | (operands[3].sub_type << 1) | 1) << 4);
    }
  }
    else
  {
    //print_error_illegal_operands(instr, asm_context);
    return ARM_ILLEGAL_OPERANDS;
  }

  opcode |= (cond<<28)|(i<<25)|(s<<20)|(rn<<16)|(rd<<12)|immediate;

  add_bin32(asm_context, opcode, IS_OPCODE);
  return 4;
}

static int parse_alu_2(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode, int use_d)
{
  int immediate = 0;
  int s = 0; // S flag
  int rd = 0,rn = 0;
  int i = 0;
  //int reg_offset = (use_d == 1) ? 12 : 16;

  // Change mov rd, #0xffffffff to mvn rd, #0
  if (opcode == 0x01a00000 && operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_IMMEDIATE &&
      operands[1].value == 0xffffffff)
  {
    //strncpy(instr_case, "mvn", 3);
    opcode = 0x01e00000,
    operands[1].value = 0x0;
  }

  // Change mvn rd, #0xffffffff to mov rd, #0
  if (opcode == 0x01e00000 && operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_IMMEDIATE &&
      operands[1].value == 0xffffffff)
  {
    //strncpy(instr_case, "mov", 3);
    opcode = 0x01a00000;
    operands[1].value = 0;
  }

  int cond = parse_condition(&instr);

  if (instr[0] == 's') { s = 1; instr++; }
  // According to some doc, tst, teq, cmp, cmn should set S all the time
  // But it doesn't agree with gcc.
  int n = (opcode >> 21) & 0xf;
  if (n >= 8 && n < 12) { s = 1; }

  if (*instr != 0)
  {
    return ARM_UNKNOWN_INSTRUCTION;
  }

  if (operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG)
  {
    // mov rd, rn
    if (use_d == 1) { rd = operands[0].value; }
    else { rn = operands[0].value; }

    immediate = (0 << 4) | operands[1].value;
    i = 0;
  }
    else
  if (operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_IMMEDIATE)
  {
    // mov rd, #imm
    if (use_d == 1) { rd = operands[0].value; }
    else { rn = operands[0].value; }

    immediate = compute_immediate(operands[1].value);
    i = 1;
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_IMMEDIATE &&
      operands[2].type == OPERAND_SHIFT_IMMEDIATE)
  {
    // mov rd, #imm, shift
    if (use_d == 1) { rd = operands[0].value; }
    else { rn = operands[0].value; }

    immediate = imm_shift_to_immediate(asm_context, operands, operand_count, 1);
    i = 1;
    if (immediate < 0) { return -1; }
  }
    else
  {
    //print_error_illegal_operands(instr, asm_context);
    return ARM_ILLEGAL_OPERANDS;
  }

  opcode |= (cond<<28)|(i<<25)|(s<<20)|(rn<<16)|(rd<<12)|immediate;

  add_bin32(asm_context, opcode, IS_OPCODE);
  return 4;
}

static int parse_branch(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  if (asm_context->pass == 1)
  {
    add_bin32(asm_context, opcode, IS_OPCODE);
    return 4;
  }

  if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
  {
    int cond = parse_condition(&instr);

    // address+8 (to allow for the pipeline)
    int32_t offset = operands[0].value - (asm_context->address + 8);

    if ((offset & 0x3) != 0)
    {
      print_error_align(asm_context, 4);
      return ARM_ERROR_ADDRESS;
    }

    if ((offset & 0xff000000) != 0 && (offset & 0xff000000) != 0xff000000)
    {
      print_error_range("Offset", -(1 << 25), (1 << 25) - 1, asm_context);
      return ARM_ERROR_ADDRESS;
    }

    offset >>= 2;

    add_bin32(asm_context, opcode | (cond << 28) | (offset & 0x00ffffff), IS_OPCODE);
    return 4;
  }

  return ARM_ERROR_OPCOMBO;
}

static int parse_branch_exchange(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  if (operand_count != 1 || operands[0].type != OPERAND_REG)
  {
    //print_error_illegal_operands(instr, asm_context);
    return ARM_ILLEGAL_OPERANDS;
  }

  int cond = parse_condition(&instr);
  //unsigned int offset=(asm_context->address+4)-operands[0].value;
  //if (offset<(1<<23) || offset>=(1<<23))
  opcode = BRANCH_EXCH_OPCODE | (cond << 28) | operands[0].value;
  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int parse_ldr_str(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int offset = 0;
  int reg_base = 0;
  int reg_sd = 0;
  int i = 0;
  int pr = 0;
  int u = 1;
  int b = 0;
  int w = 0;

#if 0
printf("%d  %d %d %d\n",
  operand_count,
  operands[0].type,
  operands[1].type,
  operands[2].type);
#endif

  if (asm_context->pass == 1)
  {
    add_bin32(asm_context, opcode, IS_OPCODE);
    return 4;
  }

  int cond = parse_condition(&instr);

  if (instr[0] == 'b') { b = 1; instr++; }
  if (*instr != 0)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  if (operands[1].type == OPERAND_REG_WRITE_BACK)
  {
    operands[1].type = OPERAND_REG_INDEXED_CLOSE;
    w = 1;
  }
    else
  if (operands[2].type == OPERAND_REG_WRITE_BACK)
  {
    operands[2].type = OPERAND_REG_INDEXED_CLOSE;
    w = 1;
  }
    else
  if (operands[3].type == OPERAND_SHIFT_IMM_INDEXED_CLOSE_WB)
  {
    operands[3].type = OPERAND_SHIFT_IMM_INDEXED_CLOSE;
    w = 1;
  }
    else
  if (operands[3].type == OPERAND_SHIFT_REG_INDEXED_CLOSE_WB)
  {
    operands[3].type = OPERAND_SHIFT_REG_INDEXED_CLOSE;
    w = 1;
  }

  reg_sd = operands[0].value;
  reg_base = operands[1].value;

  if (operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_NUMBER)
  {
    offset = operands[1].value - (asm_context->address + 8);
    pr = 1;

    if (offset < -4095 || offset > 4095)
    {
      print_error_range("Offset", -4095, 4095, asm_context);
      return -1;
    }

    if (offset < 0)
    {
      offset = -offset;
      u = 0;
    }
      else
    {
      u = 1;
    }

    reg_base = 15;
  }
    else
  if (operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED)
  {
    pr = 1;  // gcc sets this for some reason
    offset = 0;
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED_OPEN &&
      operands[2].type == OPERAND_IMM_INDEXED_CLOSE)
  {
    offset = operands[2].value;
    pr = 1;
    if (offset < 0 && offset > -4096) { offset = -offset; u = 0; }
    if (offset < 0 || offset > 4095)
    {
      print_error_range("Offset", 0, 4095, asm_context);
      return -1;
    }
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED &&
      operands[2].type == OPERAND_IMMEDIATE)
  {
    offset=operands[2].value;
    if (offset < 0 && offset > -4096) { offset = -offset; u = 0; }
    if (offset < 0 || offset > 4095)
    {
      print_error_range("Offset", 0, 4095, asm_context);
      return -1;
    }
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED &&
      operands[2].type == OPERAND_REG)
  {
    offset = operands[2].value | (0 << 4);
    i = 1;
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED_OPEN &&
      operands[2].type == OPERAND_REG_INDEXED_CLOSE)
  {
    offset = operands[2].value | (0 << 4);
    pr = 1;
    i = 1;
  }
    else
  if (operand_count == 4 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED &&
      operands[2].type == OPERAND_REG &&
     (operands[3].type == OPERAND_SHIFT_IMMEDIATE ||
      operands[3].type == OPERAND_SHIFT_REG))
  {
    offset = operands[2].value;

    if (operands[3].type == OPERAND_SHIFT_IMMEDIATE)
    {
      // ldr rd, [rn], rm, shift #
      offset |= (((operands[3].value << 3) |
                  (operands[3].sub_type << 1)) << 4);
    }
      else
    {
      // ldr rd, [rn], rm, shift rs
      offset |= (((operands[3].value << 4) |
                  (operands[3].sub_type << 1) | 1) << 4);
    }

    i = 1;
  }
    else
  if (operand_count == 4 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_REG_INDEXED_OPEN &&
      operands[2].type == OPERAND_REG &&
     (operands[3].type == OPERAND_SHIFT_IMM_INDEXED_CLOSE ||
      operands[3].type == OPERAND_SHIFT_REG_INDEXED_CLOSE))
  {
    offset = operands[2].value;
    pr = 1;

    if (operands[3].type == OPERAND_SHIFT_IMM_INDEXED_CLOSE)
    {
      // ldr rd, [rn, rm, shift #]
      offset |= (((operands[3].value << 3) |
                  (operands[3].sub_type << 1)) << 4);
    }
       else
    {
      // ldr rd, [rn, rm, shift rs]
      offset |= (((operands[3].value<<4) |
                  (operands[3].sub_type << 1) | 1) << 4);
    }

    i = 1;
  }
    else
  {
    //print_error_illegal_operands(instr, asm_context);
    return ARM_ILLEGAL_OPERANDS;
  }

  add_bin32(asm_context, opcode | (cond << 28) | (i << 25) | (pr << 24) |
            (u << 23) | (b << 22) | (w << 21) | (reg_base << 16) |
            (reg_sd << 12) | offset, IS_OPCODE);

  return 4;
}

static int parse_ldm_stm(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int pr = 1;  // gcc sets this to 1 if it's not used
  int u = 0;
  int ls = (opcode >> 20) & 1; // LS = 1 for load
  int w = (operands[0].type == OPERAND_REG_WRITE_BACK) ? 1 : 0;

  int cond = parse_condition(&instr);

  if (strncmp(instr, "fd", 2) == 0)
  {
    if (ls == 0) { instr[0] = 'd'; instr[1] = 'b'; }
    else { instr[0] = 'i'; instr[1] = 'a'; }
  }
    else
  if (strncmp(instr, "fa", 2) == 0)
  {
    if (ls == 0) { instr[0] = 'i'; instr[1] = 'b'; }
    else { instr[0] = 'd'; instr[1] = 'a'; }
  }
    else
  if (strncmp(instr, "ed", 2) == 0)
  {
    if (ls == 0) { instr[0] = 'd'; instr[1] = 'a'; }
    else { instr[0] = 'i'; instr[1] = 'b'; }
  }
    else
  if (strncmp(instr, "ea", 2) == 0)
  {
    if (ls == 0) { instr[0] = 'i'; instr[1] = 'a'; }
    else { instr[0] = 'b'; instr[1] = 'b'; }
  }

  //if (instr[0] == 's') { s = 1; instr++; }
  if (strncmp(instr, "ia", 2) == 0) { instr += 2; pr = 0; u = 1; }
  else if (strncmp(instr, "ib", 2) == 0) { instr += 2; pr = 1; u = 1; }
  else if (strncmp(instr, "da", 2) == 0) { instr += 2; pr = 0; u = 0; }
  else if (strncmp(instr, "db", 2) == 0) { instr += 2; pr = 1; u = 0; }

  if (*instr != 0)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  if (operand_count == 2 &&
      (operands[0].type == OPERAND_REG ||
       operands[0].type == OPERAND_REG_WRITE_BACK) &&
       operands[1].type == OPERAND_MULTIPLE_REG)
  {
    opcode |= (cond << 28) |
              (pr << 24) |
              (u << 23) |
              (operands[1].set_cond << 22) |
              (w << 21) |
              (operands[0].value << 16) |
              operands[1].value,

    //add_bin32(asm_context, opcode | (cond << 28) | (pr << 24) | (u << 23) | (s << 22) | (w << 21) | (operands[0].value << 16) | operands[1].value, IS_OPCODE);
    add_bin32(asm_context, opcode, IS_OPCODE);

     return 4;
  }

  return ARM_ERROR_OPCOMBO;
}

static int parse_swap(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int b = 0; // B flag

  int cond = parse_condition(&instr);

  if ((*instr == 'b' || *instr == 0) &&
       operand_count == 3 &&
       operands[0].type == OPERAND_REG &&
       operands[1].type == OPERAND_REG &&
       operands[2].type == OPERAND_REG_INDEXED)
  {
    if (*instr == 'b') b = 1;

    add_bin32(asm_context, SWAP_OPCODE | (cond << 28) | (b << 22) | (operands[2].value << 16) | (operands[0].value << 12) | operands[1].value, IS_OPCODE);
    return 4;
  }

  return ARM_ERROR_OPCOMBO;
}

static int parse_mrs(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int ps = 0; // PS flag

  int cond = parse_condition(&instr);

  if (*instr == 0 && operand_count == 2 &&
      operands[0].type == OPERAND_REG &&
      operands[1].type == OPERAND_PSR)
  {
    ps = operands[1].value;
    add_bin32(asm_context, MRS_OPCODE | (cond<<28) | (ps<<22) | (operands[0].value<<12), IS_OPCODE);
    return 4;
  }

  return ARM_ERROR_OPCOMBO;
}

static int parse_msr(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int ps = 0; // PS flag

  //opcode = MSR_FLAG_OPCODE;

  // This is for MSR(all) and MSR(flag).

  int cond = parse_condition(&instr);

  ps = operands[0].value;
  opcode = MSR_FLAG_OPCODE | (cond << 28) | (ps << 22);

  if (operands[0].type == OPERAND_PSR)
  {
    opcode |= (1 << 16);
  }
    else
  {
    operands[0].type = OPERAND_PSR;
  }

  if (*instr == 0 && operand_count == 2 && operands[0].type == OPERAND_PSR)
  {
    if (operands[1].type == OPERAND_REG)
    {
      //ps = operands[0].value;
      add_bin32(asm_context, opcode | (operands[1].value << 12), IS_OPCODE);
      return 4;
    }
      else
    if (operands[1].type == OPERAND_REG)
    {
      //ps = operands[0].value;
      add_bin32(asm_context, opcode | (operands[1].value), IS_OPCODE);
      return 4;
    }
      else
    if (operands[1].type == OPERAND_IMMEDIATE)
    {
      //ps = operands[0].value;
      int source_operand = compute_immediate(operands[1].value);
      if (source_operand == -1)
      {
        printf("Error: Can't create a constant for immediate value %d at %s:%d\n", operands[1].value, asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

      add_bin32(asm_context, opcode | (1 << 25) | source_operand, IS_OPCODE);
      return 4;
    }
  }
    else
  if (operand_count == 3 &&
      operands[0].type == OPERAND_PSR &&
      operands[1].type == OPERAND_IMMEDIATE &&
      operands[2].type == OPERAND_NUMBER)
  {
    int source_operand = imm_shift_to_immediate(asm_context, operands, operand_count, 1);
    if (source_operand < 0) { return -1; }
    add_bin32(asm_context, opcode | (1 << 25) | source_operand, IS_OPCODE);
    return 4;
  }

  return ARM_ERROR_OPCOMBO;
}

static int parse_swi(struct _asm_context *asm_context, struct _operand *operands, int operand_count, char *instr, uint32_t opcode)
{
  int cond = parse_condition(&instr);

  if (*instr != 0)
  {
    return ARM_UNKNOWN_INSTRUCTION;
  }

  if (operand_count != 1)
  {
    print_error_opcount(instr, asm_context);
    return -1;
  }

  if (operands[0].type != OPERAND_NUMBER ||
      (int32_t)operands[0].value < 0 || operands[0].value > 0xffffff)
  {
    return ARM_ILLEGAL_OPERANDS;
    return -1;
  }

  add_bin32(asm_context, CO_SWI_OPCODE | (cond << 28) | operands[0].value, IS_OPCODE);
  return 4;
}

static int parse_multiply(struct _asm_context *asm_context, struct _operand *operands, int operand_count,  char *instr, uint32_t opcode)
{
  int s = 0;
  int rn;

  // MUL / MLA (multiply or multiply and accumulate)
  int cond = parse_condition(&instr);

  if (instr[0] == 's') { s = 1; instr++; }
  if (*instr != 0)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  if (operands[0].type != OPERAND_REG ||
      operands[1].type != OPERAND_REG ||
      operands[2].type != OPERAND_REG)
  {
    //print_error_illegal_operands(instr, asm_context);
    return ARM_ILLEGAL_OPERANDS;
  }

  // FIXME - Check this?
  if (operand_count == 3 && opcode == 0x00000090)
  {
    rn = 0;
  }
    else
  if (operand_count == 4 && opcode == 0x00200090 &&
      operands[3].type == OPERAND_REG)
  {
    rn = operands[3].value;
  }
    else
  {
    //print_error_illegal_operands(instr, asm_context);
    return ARM_ILLEGAL_OPERANDS;
  }

  add_bin32(asm_context, MUL_OPCODE | (cond<<28) | (s<<20) | (operands[0].value<<16) | (operands[1].value) | (operands[2].value<<8) | (rn<<12), IS_OPCODE);

  return 4;
}

int parse_instruction_arm(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[4];
  int operand_count;
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  int token_type;
  int n;
  int matched = 0;
  int bytes = -1;
  int num;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));
  operand_count = 0;

  // First parse instruction into the operands structures.
  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }

#if DEBUG
    printf("arm: %s  (%d)\n", token, token_type);
#endif
    n = get_register_arm(token);

    if (n != -1)
    {
      operands[operand_count].value = n;
      operands[operand_count].type = OPERAND_REG;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, ']'))
      {
        operands[operand_count].type = OPERAND_REG_INDEXED_CLOSE;
        token_type = tokens_get(asm_context, token, TOKENLEN);
      }

      if (IS_TOKEN(token, '!'))
      {
        operands[operand_count].type = OPERAND_REG_WRITE_BACK;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token, '#'))
    {
      operands[operand_count].type = OPERAND_IMMEDIATE;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }

      operands[operand_count].value = num;

      token_type=tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, ']'))
      {
        operands[operand_count].type = OPERAND_IMM_INDEXED_CLOSE;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token, '['))
    {
      operands[operand_count].type = OPERAND_REG_INDEXED_OPEN;
      token_type = tokens_get(asm_context, token, TOKENLEN);
      n = get_register_arm(token);

      if (n == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = n;
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, ']'))
      {
        operands[operand_count].type = OPERAND_REG_INDEXED;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (strcasecmp(token, "cpsr") == 0 || strcasecmp(token, "cpsr_all") == 0)
    {
      operands[operand_count].type = OPERAND_PSR;
      operands[operand_count].value = 0;
    }
      else
    if (strcasecmp(token, "spsr") == 0 || strcasecmp(token, "spsr_all") == 0)
    {
      operands[operand_count].type = OPERAND_PSR;
      operands[operand_count].value = 1;
    }
      else
    if (strcasecmp(token, "cpsr_flg") == 0)
    {
      operands[operand_count].type = OPERAND_PSRF;
      operands[operand_count].value = 0;
    }
      else
    if (strcasecmp(token, "spsr_flg") == 0)
    {
      operands[operand_count].type = OPERAND_PSRF;
      operands[operand_count].value = 1;
    }
      else
    if (IS_TOKEN(token, '{'))
    {
      operands[operand_count].type = OPERAND_MULTIPLE_REG;
      operands[operand_count].value = 0;
      while (1)
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        int r1 = get_register_arm(token);
        int r2;
        if (r1 == -1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token, '-'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          r2 = get_register_arm(token);

          if (r2 == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);
        }
        else
        {
          r2 = r1;
        }

        operands[operand_count].value |= compute_range(r1,r2);

        if (IS_TOKEN(token, '}')) { break; }
        if (IS_NOT_TOKEN(token, ','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '^'))
      {
        operands[operand_count].set_cond = 1;
      }
      else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    {
      if (strcasecmp(token, "asl") == 0)
      {
        strcpy(token, "lsl");
      }

      for (n = 0; n < 4; n++)
      {
        if (strcasecmp(token, arm_shift[n]) == 0)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          operands[operand_count].sub_type = n;

          if (IS_TOKEN(token, '#'))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (token_type != TOKEN_NUMBER)
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            operands[operand_count].value = atoi(token);

            if ((int32_t)operands[operand_count].value < 0 ||
                operands[operand_count].value > 31)
            {
              print_error_range("Shift value", 0, 31, asm_context);
              return -1;
            }

            operands[operand_count].type = OPERAND_SHIFT_IMMEDIATE;
          }
            else
          {
            int r = get_register_arm(token);
            if (r == -1)
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            operands[operand_count].value = r;
            operands[operand_count].type = OPERAND_SHIFT_REG;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, ']'))
          {
            if (operands[operand_count].type == OPERAND_SHIFT_REG)
            {
              operands[operand_count].type = OPERAND_SHIFT_REG_INDEXED_CLOSE;
            }
              else
            if (operands[operand_count].type == OPERAND_SHIFT_IMMEDIATE)
            {
              operands[operand_count].type = OPERAND_SHIFT_IMM_INDEXED_CLOSE;
            }

            token_type = tokens_get(asm_context, token, TOKENLEN);

            if (IS_TOKEN(token, '!'))
            {
              operands[operand_count].type++;
            }
              else
            {
              tokens_push(asm_context, token, token_type);
            }
          }
            else
          {
            tokens_push(asm_context, token, token_type);
          }

          break;
        }
      }

      if (n == 4)
      {
        operands[operand_count].type = OPERAND_NUMBER;

        if (asm_context->pass == 1)
        {
          operands[operand_count].value = 0;
          ignore_operand(asm_context);
        }
          else
        {
          tokens_push(asm_context, token, token_type);

          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].value = num;
        }
      }
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count==4)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
  printf("--------- new operand %s ----------\n", instr);
  for (n = 0; n < operand_count; n++)
  {
    printf("operand_type=%d operand_value=%d sub_type=%d\n", operands[n].type, operands[n].value, operands[n].sub_type);
  }
#endif

  n = 0;
  while (table_arm[n].instr != NULL)
  {
    if (strncmp(table_arm[n].instr, instr_case, table_arm[n].len) == 0)
    {
      char *instr_cond = instr_case + table_arm[n].len;
      matched = 1;

      switch (table_arm[n].type)
      {
        case OP_ALU_3:
          bytes = parse_alu_3(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_ALU_2_N:
          bytes = parse_alu_2(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode, 0);
          break;
        case OP_ALU_2_D:
          bytes = parse_alu_2(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode, 1);
          break;
        case OP_MULTIPLY:
          bytes = parse_multiply(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_SWAP:
          bytes = parse_swap(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_MRS:
          bytes=parse_mrs(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_MSR_ALL:
          bytes = parse_msr(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        //case OP_MSR_FLAG:
        //  bytes = parse_msr_flag(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
        //  break;
        case OP_LDR_STR:
          bytes = parse_ldr_str(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_UNDEFINED:
          matched = 0;
          bytes = ARM_UNKNOWN_INSTRUCTION;
          break;
        case OP_LDM_STM:
          bytes = parse_ldm_stm(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_BRANCH:
          bytes = parse_branch(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_BRANCH_EXCHANGE:
          bytes = parse_branch_exchange(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        case OP_SWI:
          bytes = parse_swi(asm_context, operands, operand_count, instr_cond, table_arm[n].opcode);
          break;
        default:
          print_error_internal(asm_context, __FILE__, __LINE__);
          break;
      }

      if (bytes == -1) { return -1; }

      if (bytes == ARM_UNKNOWN_INSTRUCTION)
      {
        print_error_unknown_instr(instr, asm_context);
      }
        else
      if (bytes == ARM_ILLEGAL_OPERANDS)
      {
        print_error_illegal_operands(instr, asm_context);
      }

      if (bytes != ARM_ERROR_OPCOMBO) { return bytes; }
    }

    n++;
  }

  if (matched == 1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

