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

#include "asm/common.h"
#include "asm/z80.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/z80.h"

// http://wikiti.brandonw.net/index.php?title=Z80_Instruction_Set
// http://map.grauw.nl/resources/z80instr.php

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_REG8,
  OPERAND_REG_IHALF,
  OPERAND_INDEX_REG16_XY,
  //OPERAND_INDEX_REG16_XY_R,
  OPERAND_INDEX_REG16,
  OPERAND_INDEX_REG8,
  OPERAND_REG16_XY,
  OPERAND_REG_SPECIAL,
  OPERAND_REG16,
  OPERAND_COND,
  OPERAND_INDEX_NUMBER,
  OPERAND_IR,
};

enum
{
  REG_B=0,
  REG_C,
  REG_D,
  REG_E,
  REG_H,
  REG_L,
  REG_INDEX_HL,  // hmmmm?
  REG_A=7,
};

enum
{
  REG_IXH=0,
  REG_IXL,
  REG_IYH,
  REG_IYL,
};

enum
{
  REG_IX=0,
  REG_IY,
};

enum
{
  REG_BC=0,
  REG_DE,
  REG_HL,
  REG_SP,
};

enum
{
  REG_AF,
  REG_AF_TICK,
  REG_F,
};

enum
{
  REG_I=0,
  REG_R,
};

enum
{
  COND_NZ,
  COND_Z,
  COND_NC,
  COND_C,
  COND_PO,
  COND_PE,
  COND_P,
  COND_M,
};

struct _operand
{
  int value;
  int type;
  int offset;
};

static int get_cond(char *token)
{
  char *cond[] = { "nz","z","nc","c", "po","pe","p","m" };
  int n;

  for (n = 0; n < 8; n++)
  {
    if (strcasecmp(token, cond[n]) == 0) { return n; }
  }

  return -1;
}

static int get_reg8(char *token)
{
  char *reg8[] = { "b","c","d","e","h","l","(hl)","a" };
  int n;

  for (n = 0; n < 8; n++)
  {
    if (strcasecmp(token, reg8[n]) == 0) { return n; }
  }

  return -1;
}

static int get_reg_ihalf(char *token)
{
  char *reg_ihalf[] = { "ixh","ixl","iyh","iyl" };
  int n;

  for (n = 0; n < 4; n++)
  {
    if (strcasecmp(token, reg_ihalf[n]) == 0) { return n; }
  }

  return -1;
}

static int get_reg_index(char *token)
{
  char *reg_index[] = { "ix","iy" };
  int n;

  for (n = 0; n < 2; n++)
  {
    if (strcasecmp(token, reg_index[n]) == 0) { return n; }
  }

  return -1;
}

static int get_reg16(char *token)
{
  char *reg16[] = { "bc","de","hl","sp" };
  int n;

  for (n = 0; n < 4; n++)
  {
    if (strcasecmp(token, reg16[n]) == 0) { return n; }
  }

  return -1;
}

static int get_reg_special(char *token)
{
  char *reg_special[] = { "af","af'","f" };
  int n;

  for (n = 0; n < 3; n++)
  {
    if (strcasecmp(token, reg_special[n]) == 0) { return n; }
  }

  return -1;
}

static int compute_reg8(struct _operand *operand)
{
  if (operand->type == OPERAND_REG8) { return operand->value; }
  if (operand->type == OPERAND_INDEX_REG16 &&
      operand->value == REG_HL)
  {
    return 6;
  }

  return -1;
}

static int check_disp8(struct _asm_context *asm_context, struct _operand *operand)
{
  if (operand->offset < -128 || operand->offset > 127)
  {
    print_error_range("Displacement", -128, 127, asm_context);
    return -1;
  }

  return 0;
}

static int check_const8(struct _asm_context *asm_context, struct _operand *operand)
{
  if (operand->value < -128 || operand->value > 255)
  {
    print_error_range("Constant", -128, 255, asm_context);
    return -1;
  }

  return 0;
}

static int check_addr8(struct _asm_context *asm_context, struct _operand *operand)
{
  if (operand->value < 0 || operand->value > 255)
  {
    print_error_range("Address", 0, 255, asm_context);
    return -1;
  }

  return 0;
}

static int check_offset8(struct _asm_context *asm_context, struct _operand *operand, int address, int *offset)
{
  int o = operand->value - address;

  if (asm_context->pass == 1)
  {
    *offset = 0;
    return 0;
  }

  if (o < -128 || o > 127)
  {
    print_error_range("Offset", -128, 127, asm_context);
    return -1;
  }

  *offset = o;

  return 0;
}

static int check_bit(struct _asm_context *asm_context, struct _operand *operand)
{
  if (operand->value < 0 || operand->value > 7)
  {
    print_error_range("Bit", 0, 7, asm_context);
    return -1;
  }

  return 0;
}


int parse_instruction_z80(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  struct _operand operands[3];
  int operand_count=0;
  int offset=0;
  int matched=0;
  int instr_enum;
  int num;
  int n,reg;

  lower_copy(instr_case, instr);

  memset(&operands, 0, sizeof(operands));

  // Find instruction
  n = 0;
  while (table_instr_z80[n].instr != NULL)
  {
    if (strcmp(instr_case, table_instr_z80[n].instr) == 0) { break; }
    n++;
  }

  if (table_instr_z80[n].instr == NULL)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  //instr_index = n;
  instr_enum = table_instr_z80[n].instr_enum;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count >= 3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if (IS_TOKEN(token,'('))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if ((n = get_reg16(token)) != -1)
      {
        operands[operand_count].type = OPERAND_INDEX_REG16;
        operands[operand_count].value = n;
      }
        else
      if ((n = get_reg8(token)) != -1)
      {
        operands[operand_count].type = OPERAND_INDEX_REG8;
        operands[operand_count].value = n;
      }
        else
      if ((n = get_reg_index(token)) != -1)
      {
        operands[operand_count].type = OPERAND_INDEX_REG16_XY;
        operands[operand_count].value = n;
        token_type = tokens_get(asm_context, token, TOKENLEN);
        tokens_push(asm_context, token, token_type);
        if (IS_NOT_TOKEN(token,')'))
        {
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
          operands[operand_count].offset = num;
        }
      }
        else
      {
        tokens_push(asm_context, token, token_type);
        if (eval_expression(asm_context, &num) != 0)
        {
          if (asm_context->pass == 1)
          {
            int paren_count = 1;
            while (1)
            {
              token_type = tokens_get(asm_context, token, TOKENLEN);
              if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
              if (IS_TOKEN(token,'(')) { paren_count++; }
              if (IS_TOKEN(token,')'))
              {
                paren_count--;
                if (paren_count == 0) { break; }
              }
            }

            tokens_push(asm_context, ")", TOKEN_SYMBOL);
            num = 0;
          }
            else
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }
        }

        operands[operand_count].type = OPERAND_INDEX_NUMBER;
        operands[operand_count].value = num;

        //print_error_unexp(token, asm_context);
        //return -1;
      }

      if (expect_token_s(asm_context,")") != 0) { return -1; }
    }
      else
    if ((n = get_reg8(token)) != -1)
    {
      operands[operand_count].type = OPERAND_REG8;
      operands[operand_count].value = n;
    }
      else
    if ((n = get_reg_ihalf(token)) != -1)
    {
      operands[operand_count].type = OPERAND_REG_IHALF;
      operands[operand_count].value = n;
    }
      else
    if ((n = get_reg16(token)) != -1)
    {
      operands[operand_count].type = OPERAND_REG16;
      operands[operand_count].value = n;
    }
      else
    if ((n = get_reg_index(token)) != -1)
    {
      operands[operand_count].type = OPERAND_REG16_XY;
      operands[operand_count].value = n;
    }
      else
    if ((n = get_reg_special(token)) != -1)
    {
      operands[operand_count].type = OPERAND_REG_SPECIAL;
      operands[operand_count].value = n;
    }
      else
    if ((n = get_cond(token)) != -1)
    {
      operands[operand_count].type = OPERAND_COND;
      operands[operand_count].value = n;
    }
      else
    if (token_type == TOKEN_STRING &&
        (IS_TOKEN(token,'i') || IS_TOKEN(token,'I')))
    {
      operands[operand_count].type = OPERAND_IR;
      operands[operand_count].value = REG_I;
    }
      else
    if (token_type==TOKEN_STRING &&
        (IS_TOKEN(token,'r') || IS_TOKEN(token,'R')))
    {
      operands[operand_count].type = OPERAND_IR;
      operands[operand_count].value = REG_R;
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

      if (num < -32768 || num > 65535)
      {
        print_error_range("Constant", -32768, 65535, asm_context);
        return -1;
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;
    if (IS_NOT_TOKEN(token,',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#if DEBUG
printf("operand_count=%d\n", operand_count);
for (n = 0; n < operand_count; n++)
{
printf("-- %d %d %d\n", operands[n].type, operands[n].value, operands[n].offset);
}
#endif

  // Instruction is parsed, now find matching opcode

  n = 0;
  while (table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].instr_enum == instr_enum)
    {
      matched = 1;
      switch (table_z80[n].type)
      {
        case OP_NONE:
          if (operand_count == 0)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
        case OP_NONE16:
          if (operand_count == 0)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
        case OP_NONE24:
          if (operand_count == 0)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, 0, IS_OPCODE);
            return 3;
          }
        case OP_A_REG8:
          reg = compute_reg8(&operands[1]);
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              reg != -1)
          {
            add_bin8(asm_context, table_z80[n].opcode | reg, IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG8:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 1 &&
              reg != -1)
          {
            add_bin8(asm_context, table_z80[n].opcode | reg, IS_OPCODE);
            return 1;
          }
          break;
        case OP_A_REG_IHALF:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[1].value >> 1);
            uint8_t l = (operands[1].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_B_REG_IHALF:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_B &&
              operands[1].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[1].value >> 1);
            uint8_t l = (operands[1].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_C_REG_IHALF:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_C &&
              operands[1].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[1].value >> 1);
            uint8_t l = (operands[1].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_D_REG_IHALF:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_D &&
              operands[1].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[1].value >> 1);
            uint8_t l = (operands[1].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_E_REG_IHALF:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_E &&
              operands[1].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[1].value >> 1);
            uint8_t l = (operands[1].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_INDEX:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_disp8(asm_context,&operands[1]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[1].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].offset, IS_OPCODE);
            return 3;
          }
          break;
        case OP_A_NUMBER8:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_const8(asm_context,&operands[1]) == -1) { return -1; }
#if 0
            if (operands[1].value<-128 || operands[1].value>255)
            {
              print_error_range("Constant", -128, 255, asm_context);
              return -1;
            }
#endif
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_HL_REG16_1:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[0].value == REG_HL &&
              operands[1].type == OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode | (operands[1].value << 4), IS_OPCODE);
            return 1;
          }
          break;
        case OP_HL_REG16_2:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[0].value == REG_HL &&
              operands[1].type == OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[1].value << 4), IS_OPCODE);
            return 2;
          }
          break;
        case OP_XY_REG16:
          if (operands[0].type == OPERAND_REG16_XY &&
              operands[0].type == operands[1].type &&
              operands[0].value == operands[1].value)
          {
            operands[1].type = OPERAND_REG16;
            operands[1].value = REG_HL;
          }
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16_XY &&
              operands[1].type == OPERAND_REG16)
          {
            operands[0].value &= 0x1;
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[0].value << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[1].value << 4), IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_INDEX_HL:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_INDEX_REG16 &&
              operands[1].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_HL:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_NUMBER8:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_NUMBER)
          {
            if (check_const8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[0].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG_IHALF:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[0].value >> 1);
            uint8_t l = (operands[0].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_disp8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[0].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[0].offset, IS_OPCODE);
            return 3;
          }
          break;
        case OP_INDEX_LONG:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_disp8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[0].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[0].offset, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].extra_opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_BIT_REG8:
          reg = compute_reg8(&operands[1]);
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              reg != -1)
          {
            if (check_bit(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[0].value << 3) | reg, IS_OPCODE);
            return 2;
          }
          break;
        case OP_BIT_INDEX_HL:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_INDEX_REG16 &&
              operands[1].value == REG_HL)
          {
            if (check_bit(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff ) | (operands[0].value << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_BIT_INDEX:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_bit(asm_context,&operands[0]) == -1) { return -1; }
            if (check_disp8(asm_context,&operands[1]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[1].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].offset, IS_OPCODE);
            add_bin8(asm_context, 0x46 | (operands[0].value << 3), IS_OPCODE);
            return 4;
          }
          break;
        case OP_ADDRESS:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)(operands[0].value >> 8), IS_OPCODE);
            return 3;
          }
          break;
        case OP_COND_ADDRESS:
          if (operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_C)
          {
            operands[0].type = OPERAND_COND;
            operands[0].value = COND_C;
          }
          if (operand_count == 2 &&
              operands[0].type == OPERAND_COND &&
              operands[1].type == OPERAND_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode|(operands[0].value<<3), IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)(operands[1].value >> 8), IS_OPCODE);
            return 3;
          }
          break;
        case OP_REG8_V2:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 1 && reg != -1)
          {
            add_bin8(asm_context, table_z80[n].opcode | (reg << 3), IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG_IHALF_V2:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[0].value >> 1);
            uint8_t l = (operands[0].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (l << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG16:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode | (operands[0].value << 4), IS_OPCODE);
            return 1;
          }
          break;
        case OP_XY:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[0].value << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_SP_HL:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_SP &&
              operands[1].type == OPERAND_REG16 &&
              operands[1].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_SP_XY:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_SP &&
              operands[1].type == OPERAND_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[1].value << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_AF_AF_TICK:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG_SPECIAL &&
              operands[0].value == REG_AF &&
              operands[1].type == OPERAND_REG_SPECIAL &&
              operands[1].value == REG_AF_TICK)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_DE_HL:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[0].value == REG_DE &&
              operands[1].type == OPERAND_REG16 &&
              operands[1].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_IM_NUM:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_NUMBER)
          {
            if (operands[0].value < 0 || operands[0].value > 2)
            {
              print_error_range("Constant", 0, 2, asm_context);
              return -1;
            }
            if (operands[0].value!=0) { operands[0].value++; }

            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[0].value << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_INDEX_N:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_INDEX_NUMBER)
          {
            if (check_addr8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG8_INDEX_C:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 2 &&
              reg != -1 &&
              operands[1].type == OPERAND_INDEX_REG8 &&
              operands[1].value == REG_C)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (reg << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_F_INDEX_C:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG_SPECIAL &&
              operands[0].value == REG_F &&
              operands[1].type == OPERAND_INDEX_REG8 &&
              operands[1].value == REG_C)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_XY:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_disp8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[0].value) << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_OFFSET8:
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (check_offset8(asm_context, &operands[0], asm_context->address + 2, &offset) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);
            return 2;
          }
          break;
        case OP_JR_COND_ADDRESS:
          if (operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_C)
          {
            operands[0].type = OPERAND_COND;
            operands[0].value = COND_C;
          }
          if (operand_count == 2 &&
              operands[0].type == OPERAND_COND &&
              operands[0].value < 4 &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_offset8(asm_context, &operands[1], asm_context->address + 2, &offset) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode | ((operands[0].value) << 3), IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG8_REG8:
          reg = compute_reg8(&operands[0]);
          int src = compute_reg8(&operands[1]);
          if (operand_count == 2 && reg != -1 && src != -1)
          {
            add_bin8(asm_context, table_z80[n].opcode | (reg << 3) | src, IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG8_REG_IHALF:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 2 &&
              reg != -1 &&
              operands[1].type == OPERAND_REG_IHALF)
          {
            uint8_t y = (operands[1].value >> 1);
            uint8_t l = (operands[1].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode&0xff)|(reg<<3)|l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG_IHALF_REG8:
          reg = compute_reg8(&operands[1]);
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG_IHALF &&
              reg != -1)
          {
            uint8_t y = (operands[0].value >> 1);
            uint8_t l = (operands[0].value & 0x1);
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (l << 3)|reg, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG_IHALF_REG_IHALF:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG_IHALF &&
              operands[1].type == OPERAND_REG_IHALF &&
              operands[0].value != operands[1].value &&
              (operands[0].value >> 1) == (operands[1].value >> 1))
          {
            uint8_t y = (operands[0].value >> 1);
            uint8_t l = ((operands[0].value & 0x1) == 1) ? 0x8 : 0x1;
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (y << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff)|l, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG8_NUMBER8:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 2 &&
              reg != -1 &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (operands[1].value < -128 || operands[1].value > 255)
            {
              print_error_range("Immediate", -128, 255, asm_context);
              return -1;
            }

            add_bin8(asm_context, (table_z80[n].opcode) | (reg << 3), IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG8_INDEX_HL:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 2 &&
              reg != -1 &&
              operands[1].type == OPERAND_INDEX_REG16 &&
              operands[1].value == REG_HL)
          {
            add_bin8(asm_context, (table_z80[n].opcode) | (reg << 3), IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG8_INDEX:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 2 &&
              reg != -1 &&
              operands[1].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_disp8(asm_context,&operands[1]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[1].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (reg << 3), IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].offset, IS_OPCODE);
            return 3;
          }
          break;
        case OP_INDEX_HL_REG8:
          reg = compute_reg8(&operands[1]);
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_HL &&
              reg != -1)
          {
            add_bin8(asm_context, table_z80[n].opcode | reg, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode | reg, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_HL_NUMBER8:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_HL &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_const8(asm_context,&operands[1]) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].value, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_REG8:
          reg = compute_reg8(&operands[1]);
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16_XY &&
              reg != -1)
          {
            if (check_disp8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[0].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | reg, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[0].offset, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_NUMBER8:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16_XY &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_disp8(asm_context,&operands[0]) == -1) { return -1; }
            if (check_const8(asm_context,&operands[1]) == -1) { return -1; }
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | ((operands[0].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[0].offset, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].value, IS_OPCODE);
            return 1;
          }
          break;
        case OP_A_INDEX_BC:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_INDEX_REG16 &&
              operands[1].value == REG_BC)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_A_INDEX_DE:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_INDEX_REG16 &&
              operands[1].value == REG_DE)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_A_INDEX_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_INDEX_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value >> 8, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_BC_A:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_BC &&
              operands[1].type == OPERAND_REG8 &&
              operands[1].value == REG_A)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_DE_A:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_DE &&
              operands[1].type == OPERAND_REG8 &&
              operands[1].value == REG_A)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_INDEX_ADDRESS_A:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_NUMBER &&
              operands[1].type == OPERAND_REG8 &&
              operands[1].value == REG_A)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
            return 2;
          }
          break;
        case OP_IR_A:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_IR &&
              operands[1].type == OPERAND_REG8 &&
              operands[1].value == REG_A)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[0].value << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_A_IR:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_A &&
              operands[1].type == OPERAND_IR)
          {
            add_bin8(asm_context,table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[1].value << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG16_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[1].type == OPERAND_NUMBER)
          {
            add_bin8(asm_context,table_z80[n].opcode | (operands[0].value << 4), IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value >> 8, IS_OPCODE);
            return 3;
          }
          break;
        case OP_XY_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16_XY &&
              operands[1].type == OPERAND_NUMBER)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[0].value << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value >> 8, IS_OPCODE);
            return 4;
          }
          break;
        case OP_REG16_INDEX_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[1].type == OPERAND_INDEX_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[0].value << 4), IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value >> 8, IS_OPCODE);
            return 4;
          }
          break;
        case OP_HL_INDEX_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[0].value == REG_HL &&
              operands[1].type == OPERAND_INDEX_NUMBER)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value >> 8, IS_OPCODE);
            return 3;
          }
          break;
        case OP_XY_INDEX_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16_XY &&
              operands[1].type == OPERAND_INDEX_NUMBER)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[0].value << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[1].value >> 8, IS_OPCODE);
            return 3;
          }
          break;
        case OP_INDEX_ADDRESS_REG16:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_NUMBER &&
              operands[1].type == OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff) | (operands[1].value << 4), IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
            return 4;
          }
          break;
        case OP_INDEX_ADDRESS_HL:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_NUMBER &&
              operands[1].type == OPERAND_REG16 &&
              operands[1].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
            return 3;
          }
          break;
        case OP_INDEX_ADDRESS_XY:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_NUMBER &&
              operands[1].type == OPERAND_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[1].value << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
            return 3;
          }
          break;
        case OP_SP_HL:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[0].value == REG_SP &&
              operands[1].type == OPERAND_REG16 &&
              operands[1].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            return 1;
          }
          break;
        case OP_SP_XY:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG16 &&
              operands[0].value == REG_SP &&
              operands[1].type == OPERAND_REG16_XY)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[1].value << 5), IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_ADDRESS8_A:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_NUMBER &&
              operands[1].type == OPERAND_REG8 &&
              operands[1].value == REG_A)
          {
            if (check_addr8(asm_context,&operands[0]) == -1) { return -1; }
            add_bin8(asm_context, table_z80[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_C_REG8:
          reg=compute_reg8(&operands[1]);
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG8 &&
              operands[0].value == REG_C &&
              reg != -1)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8) | (operands[1].value << 5), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff )| (reg << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_C_ZERO:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDEX_REG8 &&
              operands[0].value == REG_C &&
              operands[1].type == OPERAND_NUMBER &&
              operands[1].value == 0)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG16P:
          if (operands[0].type == OPERAND_REG_SPECIAL &&
              operands[0].value == REG_AF)
          {
            operands[0].type = OPERAND_REG16;
            operands[0].value = REG_SP;
          }
          if (operand_count == 1 &&
              operands[0].type == OPERAND_REG16)
          {
            add_bin8(asm_context, table_z80[n].opcode | (operands[0].value << 4), IS_OPCODE);
            return 1;
          }
          break;
        case OP_COND:
          if (operands[0].type == OPERAND_REG8 &&
              operands[0].value == REG_C)
          {
            operands[0].type = OPERAND_COND;
            operands[0].value = COND_C;
          }
          if (operand_count == 1 &&
              operands[0].type == OPERAND_COND)
          {
            add_bin8(asm_context, table_z80[n].opcode | ((operands[0].value) << 3), IS_OPCODE);
            return 1;
          }
          break;
        case OP_REG8_CB:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 1 &&
              reg != -1)
          {
            add_bin8(asm_context, (table_z80[n].opcode >> 8), IS_OPCODE);
            add_bin8(asm_context, (table_z80[n].opcode & 0xff)|reg, IS_OPCODE);
            return 2;
          }
          break;
        case OP_INDEX_HL_CB:
          reg = compute_reg8(&operands[0]);
          if (operand_count == 1 &&
              operands[0].type == OPERAND_INDEX_REG16 &&
              operands[0].value == REG_HL)
          {
            add_bin8(asm_context, table_z80[n].opcode >> 8, IS_OPCODE);
            add_bin8(asm_context, table_z80[n].opcode & 0xff, IS_OPCODE);
            return 2;
          }
          break;
        case OP_RESTART_ADDRESS:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_NUMBER)
          {
            if ((operands[0].value % 8) != 0 || operands[0].value > 0x38 ||
                operands[0].value < 0)
            {
              printf("Error: Illegal restart address at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
            int i = operands[0].value / 8;
            add_bin8(asm_context, table_z80[n].opcode | (i << 3), IS_OPCODE);
            return 1;
          }
          break;
      }
    }
    n++;
  }

  n = 0;
  while (table_z80_4_byte[n].instr_enum != Z80_NONE)
  {
    if (table_z80_4_byte[n].instr_enum == instr_enum)
    {
      matched = 1;
      switch (table_z80_4_byte[n].type)
      {
        case OP_BIT_INDEX_V2:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_INDEX_REG16_XY)
          {
            if (check_bit(asm_context,&operands[0]) == -1) { return -1; }
            if (check_disp8(asm_context,&operands[1]) == -1) { return -1; }
            add_bin8(asm_context, 0xdd|((operands[1].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, 0xcb, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].offset, IS_OPCODE);
            add_bin8(asm_context, table_z80_4_byte[n].opcode | (operands[0].value << 3), IS_OPCODE);
            return 4;
          }
          break;
        case OP_BIT_INDEX_REG8:
          if (operand_count == 3 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_INDEX_REG16_XY &&
              operands[2].type == OPERAND_REG8)
          {
            if (check_bit(asm_context,&operands[0]) == -1) { return -1; }
            if (check_disp8(asm_context,&operands[1]) == -1) { return -1; }
            reg=operands[2].value;
            add_bin8(asm_context, 0xdd | ((operands[1].value & 0x1) << 5), IS_OPCODE);
            add_bin8(asm_context, 0xcb, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)operands[1].offset, IS_OPCODE);
            add_bin8(asm_context, table_z80_4_byte[n].opcode | (operands[0].value << 3)|reg, IS_OPCODE);
            return 4;
          }
          break;
      }
    }

    n++;
  }

  if (matched==1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

