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
#include <stdint.h>

#include "asm/arc.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/arc.h"

#define MAX_OPERANDS 3

#define COMPUTE_B(a) (((a & 0x7) << 24) | (((a >> 3) & 0x7) << 12))

#define REG_GP 26
//#define REG_FP 27
#define REG_SP 28
//#define REG_BLINK 31
//#define REG_LP_COUNT 60
//#define REG_LONG_IMMEDIATE 62
#define LIMM 62

enum
{
  OPERAND_NONE,
  OPERAND_REG,
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
  uint8_t reg16;
};

struct _condition_codes
{
  uint8_t code;
  char *name;
};

static struct _condition_codes condition_codes[] =
{
  { 0x00, "al" },
  { 0x00, "ra" },
  { 0x01, "eq" },
  { 0x01, "z" },
  { 0x02, "ne" },
  { 0x02, "nz" },
  { 0x03, "pl" },
  { 0x03, "p" },
  { 0x04, "mi" },
  { 0x04, "n" },
  { 0x05, "cs" },
  { 0x05, "c" },
  { 0x05, "lo" },
  { 0x06, "cc" },
  { 0x06, "nc" },
  { 0x06, "hs" },
  { 0x07, "vs" },
  { 0x07, "v" },
  { 0x08, "vc" },
  { 0x08, "nv" },
  { 0x09, "gt" },
  { 0x0a, "ge" },
  { 0x0b, "lt" },
  { 0x0c, "le" },
  { 0x0d, "hi" },
  { 0x0e, "ls" },
  { 0x0f, "pnz" },
};

#if 0
static int compute_s12(int s12)
{
  int data;

  s12 = s12 & 0xfff;

  data = ((s12 & 0x3f) << 6) | ((s12 >> 6) & 0x3f);

  return data;
}
#endif

static void add_bin(
  struct _asm_context *asm_context,
  uint32_t opcode,
  int flags)
{
  add_bin16(asm_context, opcode >> 16, flags);
  add_bin16(asm_context, opcode & 0xffff, flags);
}

static int get_register_arc(char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    const char *s = token + 1;
    int n = 0, count = 0;

    while (*s != 0)
    {
      if (*s < '0' || *s > '9') { return -1; }
      n = (n * 10) + (*s - '0');
      count++;

      // Disallow leading 0's on registers.
      if (n == 0 && count >1) { return -1; }

      s++;
    }

    // This token was just r or R.
    if (count == 0) { return -1; }

    return n;
  }

  if (strcasecmp(token, "gp") == 0) { return 26; }
  if (strcasecmp(token, "fp") == 0) { return 27; }
  if (strcasecmp(token, "sp") == 0) { return 28; }
  if (strcasecmp(token, "ilink1") == 0) { return 29; }
  if (strcasecmp(token, "ilink2") == 0) { return 30; }
  if (strcasecmp(token, "blink") == 0) { return 31; }
  if (strcasecmp(token, "lp_count") == 0) { return 60; }
  if (strcasecmp(token, "pcl") == 0) { return 63; }

  return -1;
}

#if 0
static int map_16bit_reg(int r32)
{
  // Page 40 of the ARCompact Instruction Set Architecture... looks
  // like for 16 bit instructions, only 8 of the ARC's registers can
  // be used and they aren't in numerical order.
  if (r32 >= 0 && r32 <= 3) { return r32; }
  if (r32 >= 12 && r32 <= 15) { return r32 - 8; }

  return -1;
}
#endif

int parse_instruction_arc(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int found = 0;
  uint32_t opcode;
  uint8_t cc_flag = 0;
  uint8_t cc_value = 0;
  uint8_t f_flag = 0;
  union
  {
    struct
    {
      uint8_t di_flag: 1;
      uint8_t x_flag : 1;
      uint8_t aa_flag : 2;
    };
    uint8_t set;
  } load_flags;
  int8_t open_paren = -1;
  int8_t close_paren = -1;
  int a, b, c;
  //int offset;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));
  load_flags.set = 0;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (operand_count == 0 && IS_TOKEN(token, '.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, 'f') || IS_TOKEN(token, 'F'))
      {
        f_flag = 1;
        continue;
      }

#if 0
      if (IS_TOKEN(token, 'd') || IS_TOKEN(token, 'D'))
      {
        d_flag = 1;
        continue;
      }
#endif

      if (strcasecmp(token, "di") == 0)
      {
        load_flags.di_flag = 1;
        continue;
      }

      if (strcasecmp(token, "x") == 0)
      {
        load_flags.x_flag = 1;
        continue;
      }

      if ((strcasecmp(token, "a") == 0 ||
           strcasecmp(token, "as") == 0) &&
           load_flags.aa_flag == 0)
      {
        load_flags.aa_flag = 1;
        continue;
      }

      if (strcasecmp(token, "ab") == 0 && load_flags.aa_flag == 0)
      {
        load_flags.aa_flag = 2;
        continue;
      }

      if (strcasecmp(token, "as") == 0 && load_flags.aa_flag == 0)
      {
        load_flags.aa_flag = 3;
        continue;
      }

      int len = sizeof(condition_codes) / sizeof(struct _condition_codes);

      for (n = 0; n < len; n++)
      {
        if (strcasecmp(token, condition_codes[n].name) == 0)
        {
          cc_flag = 1;
          cc_value = condition_codes[n].code;
          break;
        }
      }

      if (n < len) { continue; }

      print_error_unexp(token, asm_context);
      return -1;
    }

    if (IS_TOKEN(token, '['))
    {
      if (open_paren != -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      open_paren = operand_count;
      continue;
    }

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    num = get_register_arc(token);

    if (num != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_REG;

      if (num >= 0 && num <= 3)
      {
        operands[operand_count].reg16 = num;
      }
        else
      if (num >= 12 && num <= 15)
      {
        operands[operand_count].reg16 = (num - 12) + 4;
      }
        else
      {
        operands[operand_count].reg16 = 8;
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          num = 0;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = num;
    }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token, ']'))
    {
      close_paren = operand_count;

      if (open_paren == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type != TOKEN_EOL)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }

    operand_count++;

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == MAX_OPERANDS)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#if 0
for (n = 0; n < operand_count; n++)
{
  printf("%d) type=%d value=%d\n", n, operands[n].type, operands[n].value);
}
#endif

  // ALU instructions.
  for (n = 0; table_arc_alu[n].instr != NULL; n++)
  {
    if (strcasecmp(instr_case, table_arc_alu[n].instr) != 0) { continue; }
    if (operand_count != 3) { continue; }

    opcode = 0x20000000 | (table_arc_alu[n].opcode << 16) | (f_flag << 15);
    if (table_arc_alu[n].is_extended == 1) { opcode |= 1 << 27; }

    if (load_flags.set != 0 || open_paren != -1 || close_paren != -1)
    {
      //print_error("Instruction doesn't take flags.", asm_context);
      //return -1;
      break;
    }

    found = 1;

    a = operands[0].value;
    b = operands[1].value;
    c = operands[2].value;

    if (cc_flag != 0) { a = cc_value; }

    // xxx.f a,b,c
    // xxx.cc.f b,b,c
    if (operands[0].type == OPERAND_REG &&
        operands[1].type == OPERAND_REG &&
        operands[2].type == OPERAND_REG)
    {
      if (cc_flag == 0)
      {
        opcode |= COMPUTE_B(b) | (c << 6) | a;
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
        else
      if (operands[0].value == operands[1].value)
      {
        opcode |= COMPUTE_B(b) | (3 << 22) | (c << 6) | a;
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
    }

    // xxx.f 0,b,c
    if (operands[0].type == OPERAND_NUMBER &&
        operands[1].type == OPERAND_REG &&
        operands[2].type == OPERAND_REG &&
        operands[0].value == 0 &&
        cc_flag == 0)
    {
      opcode |= COMPUTE_B(b) | (c << 6) | LIMM;
      add_bin(asm_context, opcode, IS_OPCODE);
      return 4;
    }

    // xxx.cc.f b,b,u6
    // xxx.cc.f b,b,limm
    // xxx.f b,b,s12
    if (operands[0].type == OPERAND_REG &&
        operands[1].type == OPERAND_REG &&
        operands[2].type == OPERAND_NUMBER &&
        operands[0].value == operands[1].value)
    {
      if (cc_flag == 1)
      {
        if (operands[2].value >= 0 && operands[2].value <= 63)
        {
          opcode |= COMPUTE_B(b) | (3 << 22) | (c << 6) | (1 << 5) | cc_value;
          add_bin(asm_context, opcode, IS_OPCODE);
          return 4;
        }
          else
        {
          opcode |= COMPUTE_B(b) | (3 << 22) | (LIMM << 6) | cc_value;
          add_bin(asm_context, opcode, IS_OPCODE);
          add_bin(asm_context, c & 0xffffffff, IS_OPCODE);
          return 8;
        }
      }
        else
      if (operands[2].value >= -2048 && operands[2].value <= 2047)
      {
        c = operands[2].value & 0xfff;
        c = (c >> 6) | ((c & 0x3f) << 6);
        opcode |= COMPUTE_B(b) | (2 << 22) | c;
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
    }

    // xxx.f 0,b,u6
    // xxx.f 0,b,limm
    if (operands[0].type == OPERAND_NUMBER &&
        operands[1].type == OPERAND_REG &&
        operands[2].type == OPERAND_NUMBER &&
        operands[0].value == 0 &&
        cc_flag == 0)
    {
      if (operands[2].value >= 0 && operands[2].value <= 63)
      {
        opcode |= COMPUTE_B(b) | (1 << 22) | (operands[2].value << 6) | LIMM;
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
        else
      {
        opcode |= COMPUTE_B(b) | (LIMM << 6) | LIMM;
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, c & 0xffffffff, IS_OPCODE);
        return 8;
      }
    }

    // xxx.f a,b,u6
    // xxx.f a,b,limm
    if (operands[0].type == OPERAND_REG &&
        operands[1].type == OPERAND_REG &&
        operands[2].type == OPERAND_NUMBER &&
        cc_flag == 0)
    {
      if (operands[2].value >= 0 && operands[2].value <= 63)
      {
        opcode |= COMPUTE_B(b) | (1 << 22) | (c << 6) | a;
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
        else
      {
        opcode |= COMPUTE_B(b) | (LIMM << 6) | a;
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, c & 0xffffffff, IS_OPCODE);
        return 8;
      }
    }

    // xxx.f a,limm,c
    if (operands[0].type == OPERAND_REG &&
        operands[1].type == OPERAND_NUMBER &&
        operands[2].type == OPERAND_REG &&
        cc_flag == 0)
    {
      opcode |= COMPUTE_B(LIMM) | (c << 6) | a;
      add_bin(asm_context, opcode, IS_OPCODE);
      add_bin(asm_context, b & 0xffffffff, IS_OPCODE);
      return 8;
    }

    // xxx.cc.f 0,limm,c
    if (operands[0].type == OPERAND_NUMBER &&
        operands[1].type == OPERAND_NUMBER &&
        operands[2].type == OPERAND_REG &&
        operands[0].value == 0)
    {
      if (cc_flag == 0)
      {
        // NOTE: This was done only because GNU as was doing this
        // as xxx.f r62, limm, c (since r62 isn't writable, it works).
        // Without this if-statement, naken_asm would product an also
        // valid: xxx.al.f 0, limm, c (which seems more accurate).
        // Using GNU as's ways to make testing easier.
        opcode |= COMPUTE_B(LIMM) | (c << 6) | LIMM;
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, b & 0xffffffff, IS_OPCODE);
        return 8;
      }
        else
      {
        opcode |= COMPUTE_B(LIMM) | (3 << 22) | (c << 6) | cc_value;
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, b & 0xffffffff, IS_OPCODE);
        return 8;
      }
    }
  }

  for (n = 0; table_arc_single[n].instr != NULL; n++)
  {
    if (strcasecmp(instr_case, table_arc_single[n].instr) != 0) { continue; }

    opcode = 0x20000000 | (f_flag << 15) | (0x2f << 16) | table_arc_single[n].opcode;
    found = 1;
    if (operand_count != 2) { continue; }

    if (load_flags.set != 0 || open_paren != -1 || close_paren != -1)
    {
      //print_error("Instruction doesn't take flags.", asm_context);
      //return -1;
      break;
    }

    b = operands[0].value;
    c = operands[1].value;

    // xxx.f b,c
    if (operands[0].type == OPERAND_REG &&
        operands[1].type == OPERAND_REG)
    {
      opcode |= COMPUTE_B(b) | (c << 6);
      add_bin(asm_context, opcode, IS_OPCODE);
      return 4;
    }

    // xxx.f b,u6
    // xxx.f b,limm
    if (operands[0].type == OPERAND_REG &&
        operands[1].type == OPERAND_NUMBER)
    {
      if (operands[1].value >= 0 && operands[1].value <= 0x3f)
      {
        opcode |= COMPUTE_B(b) | (1 << 22) | (c << 6);
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
        else
      {
        opcode |= COMPUTE_B(b) | (LIMM << 6);
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, c & 0xffffffff, IS_OPCODE);
        return 8;
      }
    }

    // xxx.f 0,c
    if (operands[0].type == OPERAND_NUMBER &&
        operands[1].type == OPERAND_REG &&
        operands[0].value == 0)
    {
      opcode |= COMPUTE_B(LIMM) | (c << 6);
      add_bin(asm_context, opcode, IS_OPCODE);
      return 4;
    }

    // xxx.f 0,u6
    // xxx.f 0,limm
    if (operands[0].type == OPERAND_NUMBER &&
        operands[1].type == OPERAND_NUMBER &&
        operands[0].value == 0)
    {
      if (operands[1].value >= 0 && operands[1].value <= 0x3f)
      {
        opcode |= COMPUTE_B(LIMM) | (1 << 22) | (c << 6);
        add_bin(asm_context, opcode, IS_OPCODE);
        return 4;
      }
        else
      {
        opcode |= COMPUTE_B(LIMM) | (LIMM << 6);
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, c & 0xffffffff, IS_OPCODE);
        return 8;
      }
    }
  }

  for (n = 0; table_arc16[n].instr != NULL; n++)
  {
    if (strcasecmp(instr_case, table_arc16[n].instr) != 0) { continue; }

    opcode = table_arc16[n].opcode;
    found = 1;
    if (operand_count != table_arc16[n].operand_count) { continue; }

    if (load_flags.set != 0 || open_paren != -1 || close_paren != -1)
    {
      //print_error("Instruction doesn't take flags.", asm_context);
      //return -1;
      break;
    }

    int i, limm = 0;

    for (i = 0; i < table_arc16[n].operand_count; i++)
    {
      switch (table_arc16[n].operands[i])
      {
        case OP_A:
          if (operands[i].type != OPERAND_REG ||
              operands[i].reg16 > 7)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].reg16;
          break;
        case OP_B:
          if (operands[i].type != OPERAND_REG ||
              operands[i].reg16 > 7)
          {
            i = 100;
            break;
          }

          if (i > 0 &&
              table_arc16[n].operands[i] == table_arc16[n].operands[i - 1] &&
               operands[i].reg16 != operands[i - 1].reg16)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].reg16 << 8;
          break;
        case OP_C:
          if (operands[i].type != OPERAND_REG ||
              operands[i].reg16 > 7)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].reg16 << 5;
          break;
        case OP_H:
          if (operands[i].type != OPERAND_REG)
          {
            i = 100;
            break;
          }

          opcode |= ((operands[i].value & 0x7) << 5) | (operands[i].value >> 3);
          break;
        case OP_LIMM:
          if (operands[i].type != OPERAND_NUMBER)
          {
            i = 100;
            break;
          }

          limm = operands[i].value;
          break;
        case OP_U3:
          if (operands[i].type != OPERAND_NUMBER ||
              operands[i].value < 0 ||
              operands[i].value > 7)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].value;
          break;
        case OP_U5:
          if (operands[i].type != OPERAND_NUMBER ||
              operands[i].value < 0 ||
              operands[i].value > 31)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].value;
          break;
        case OP_U7:
          if (operands[i].type != OPERAND_NUMBER ||
              operands[i].value < 0 ||
              operands[i].value > 127 ||
             (operands[i].value & 0x3) != 0)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].value >> 2;
          break;
        case OP_U8:
          if (operands[i].type != OPERAND_NUMBER ||
              operands[i].value < 0 ||
              operands[i].value > 255)
          {
            i = 100;
            break;
          }

          opcode |= operands[i].value;
          break;
        case OP_S11:
          if (operands[i].type != OPERAND_NUMBER ||
              operands[i].value < -1024 ||
              operands[i].value > 1023)
          {
            i = 100;
            break;
          }

          opcode |= (operands[i].value >> 2) & 0x1ff;
          break;
        case OP_R0:
          if (operands[i].type != OPERAND_REG ||
              operands[i].value != 0)
          {
            i = 100;
          }

          break;
        case OP_GP:
          if (operands[i].type != OPERAND_REG ||
              operands[i].value != REG_GP)
          {
            i = 100;
          }

          break;
        case OP_SP:
          if (operands[i].type != OPERAND_REG ||
              operands[i].value != REG_SP)
          {
            i = 100;
          }

          break;
        default:
          i = 100;
          break;
      }
    }

    if (i == table_arc16[n].operand_count)
    {
      add_bin16(asm_context, opcode, IS_OPCODE);

      if (limm != 0)
      {
        add_bin(asm_context, limm, IS_OPCODE);
        return 6;
      }

      return 2;
    }
  }

  for (n = 0; table_arc_load_store[n].instr != NULL; n++)
  {
    if (strcasecmp(instr_case, table_arc_load_store[n].instr) != 0)
    {
      continue;
    }

    opcode = table_arc_load_store[n].opcode;
    found = 1;

    //if (operand_count != 3) { continue; }
    if (f_flag == 1) { continue; }

    switch (table_arc_load_store[n].type)
    {
      case OP_A_PAREN_B_S9:
      {
        if (operand_count == 3 &&
            operands[1].type == OPERAND_REG &&
            operands[2].type == OPERAND_NUMBER &&
            open_paren == 1 &&
            close_paren == 2)
        {
          // ld<zz><.x><.aa><di> a, [b, s9]
          // ld<zz><.x><.aa><di> 0, [b, s9]
          if (operands[0].type == OPERAND_REG)
          {
            if (operands[2].value >= -256 &&
                operands[2].value <= 255)
            {
              opcode |=
                operands[0].value |
                COMPUTE_B(operands[1].value) |
              ((operands[2].value & 0xff) << 16) |
             (((operands[2].value & 0x100) >> 8) << 15) |
               (load_flags.di_flag << 11) |
               (load_flags.aa_flag << 9) |
               (load_flags.x_flag << 7);

              add_bin(asm_context, opcode, IS_OPCODE);

              return 4;
            }
          }
            else
          if (operands[0].type == OPERAND_NUMBER && operands[0].value == 0)
          {
            if (operands[2].value >= -256 &&
                operands[2].value <= 255)
            {
              opcode |=
                operands[0].value |
                COMPUTE_B(LIMM) |
              ((operands[2].value & 0xff) << 16) |
             (((operands[2].value & 0x100) >> 8) << 15) |
               (load_flags.di_flag << 11) |
               (load_flags.aa_flag << 9) |
               (load_flags.x_flag << 7);

              add_bin(asm_context, opcode, IS_OPCODE);

              return 4;
            }
          }
        }
          else
        if (operand_count == 2 &&
            operands[1].type == OPERAND_NUMBER &&
            open_paren == 1 &&
            close_paren == 1)
        {
          // ld<zz><.x><di>      a, [limm]       (s = 0, b = limm, aa = 0)
          // ld<zz><.x><di>      0, [limm]       (s = 0, b = limm, aa = 0)
          if (operands[0].type == OPERAND_REG)
          {
              opcode |=
                operands[0].value |
                COMPUTE_B(LIMM) |
               (load_flags.di_flag << 11) |
               (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[1].value, IS_OPCODE);

            return 8;
          }
            else
          if (operands[0].type == OPERAND_NUMBER && operands[0].value == 0)
          {
              opcode |=
                LIMM |
                COMPUTE_B(LIMM) |
               (load_flags.di_flag << 11) |
               (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[1].value, IS_OPCODE);

            return 8;
          }
        }

        break;
      }
      case OP_A_PAREN_B_C:
      {
        if (operand_count == 3 &&
            operands[1].type == OPERAND_REG &&
            operands[2].type == OPERAND_REG &&
            open_paren == 1 &&
            close_paren == 2)
        {
          // ld<zz><.x><.aa><di> a, [b, c]
          // ld<zz><.x><.aa><di> 0, [b, c]
          if (operands[0].type == OPERAND_REG)
          {
            opcode |=
              operands[0].value |
              COMPUTE_B(operands[1].value) |
             (operands[2].value << 6) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9) |
             (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);

            return 4;
          }
            else
          if (operands[0].type == OPERAND_NUMBER && operands[0].value == 0)
          {
            opcode |=
              LIMM |
              COMPUTE_B(operands[1].value) |
             (operands[2].value << 6) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9) |
             (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);

            return 4;
          }
        }
          else
        if (operand_count == 3 &&
            operands[1].type == OPERAND_REG &&
            operands[2].type == OPERAND_NUMBER &&
            open_paren == 1 &&
            close_paren == 2)
        {
          // ld<zz><.x><.aa><di> a, [b, limm]    (c = limm)
          // ld<zz><.x><.aa><di> 0, [b, limm]    (c = limm)
          if (operands[0].value == OPERAND_REG)
          {
            opcode |=
              operands[0].value |
              COMPUTE_B(operands[1].value) |
             (LIMM << 6) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9) |
             (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[2].value, IS_OPCODE);

            return 8;
          }
            else
          if (operands[0].value == OPERAND_NUMBER)
          {
            opcode |=
              LIMM |
              COMPUTE_B(operands[1].value) |
             (LIMM << 6) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9) |
             (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[2].value, IS_OPCODE);

            return 8;
          }
        }
          else
        if (operand_count == 3 &&
            operands[1].type == OPERAND_NUMBER &&
            operands[2].type == OPERAND_REG &&
            open_paren == 1 &&
            close_paren == 2 &&
            load_flags.aa_flag == 0)
        {
          // ld<zz><.x><di>      a, [limm, c]    (b = limm)
          // ld<zz><.x><di>      0, [limm, c]    (b = limm)
          if (operands[0].value == OPERAND_REG)
          {
            opcode |=
              operands[0].value |
              COMPUTE_B(LIMM) |
             (operands[2].value << 6) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9) |
             (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[1].value, IS_OPCODE);

            return 8;
          }
            else
          if (operands[0].value == OPERAND_NUMBER)
          {
            opcode |=
              LIMM |
              COMPUTE_B(LIMM) |
             (operands[2].value << 6) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9) |
             (load_flags.x_flag << 7);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[1].value, IS_OPCODE);

            return 8;
          }
        }

        break;
      }
      case OP_C_PAREN_B_S9:
      {
        // st<zz><.aa><.di>    c, [b, s9]
        // st<zz><.di>         c, [limm]       (s = 0, b = limm, aa = 0)
        // st<zz><.aa><.di>    limm, [b, s9]   (c = limm)
        if (operand_count == 3 &&
            operands[0].type == OPERAND_REG &&
            operands[1].type == OPERAND_REG &&
            operands[2].type == OPERAND_NUMBER &&
            open_paren == 1 &&
            close_paren == 2 &&
            load_flags.x_flag == 0)
        {
          if (operands[2].value >= -256 &&
              operands[2].value <= 255)
          {
            opcode |=
              operands[0].value |
              COMPUTE_B(operands[1].value) |
            ((operands[2].value & 0xff) << 16) |
           (((operands[2].value & 0x100) >> 8) << 15) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9);

            add_bin(asm_context, opcode, IS_OPCODE);

            return 4;
          }
        }
          else
        if (operand_count == 2 &&
            operands[0].type == OPERAND_REG &&
            operands[1].type == OPERAND_NUMBER &&
            open_paren == 1 &&
            close_paren == 1 &&
            load_flags.x_flag == 0 &&
            load_flags.aa_flag == 0)
        {
            opcode |=
              operands[0].value |
              COMPUTE_B(LIMM) |
             (load_flags.di_flag << 11);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[1].value, IS_OPCODE);

            return 8;
        }
          else
        if (operand_count == 3 &&
            operands[0].type == OPERAND_NUMBER &&
            operands[1].type == OPERAND_REG &&
            operands[2].type == OPERAND_NUMBER &&
            open_paren == 1 &&
            close_paren == 2 &&
            load_flags.x_flag == 0)
        {
          if (operands[2].value >= -256 &&
              operands[2].value <= 255)
          {
            opcode |=
              LIMM |
              COMPUTE_B(operands[1].value) |
            ((operands[2].value & 0xff) << 16) |
           (((operands[2].value & 0x100) >> 8) << 15) |
             (load_flags.di_flag << 11) |
             (load_flags.aa_flag << 9);

            add_bin(asm_context, opcode, IS_OPCODE);
            add_bin(asm_context, operands[0].value, IS_OPCODE);

            return 8;
          }
        }

        break;
      }
      default:
      {
printf("%d %d\n", table_arc_load_store[n].type, OP_A_PAREN_B_S9);
        print_error_internal(asm_context, __FILE__, __LINE__);
        return -1;
      }
    }
  }

  if (found == 1)
  {
    print_error_illegal_operands(instr, asm_context);
    return -1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}


