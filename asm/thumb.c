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
#include "asm/thumb.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/thumb.h"

#define REG_SP (13 - 8)
#define REG_LR (14 - 8)
#define REG_PC (15 - 8)

enum
{
  OPERAND_NONE,
  OPERAND_REGISTER,
  OPERAND_REGISTER_INC,
  OPERAND_H_REGISTER,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_TWO_REG_IN_BRACKETS,      // [ Rb, Ro ]
  OPERAND_REG_AND_NUM_IN_BRACKETS,  // [ Rb, #IMM ]
  OPERAND_PC_AND_REG_IN_BRACKETS,   // [ PC, Rb ]
  OPERAND_PC_AND_NUM_IN_BRACKETS,   // [ PC, #IMM ]
  OPERAND_SP_AND_REG_IN_BRACKETS,   // [ SP, Rb ]
  OPERAND_SP_AND_NUM_IN_BRACKETS,   // [ SP, #IMM ]
  OPERAND_REGISTER_LIST,
  OPERAND_SPECIAL_REG,
};

struct _operand
{
  int value;
  int type;
  int second_value;
};

static int get_register_thumb(char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '7'))
    {
      return token[1] - '0';
    }
  }

  return -1;
}

static int get_h_register_thumb(char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '8' && token[1] <= '9'))
    {
      return (token[1] - '0') - 8;
    }
      else
    if (token[3] == 0 && token[1] == '1' &&
       (token[2] >= '0' && token[2] <= '5'))
    {
      return (token[2] - '0') + 10 - 8;
    }
  }

  if (strcasecmp("sp", token) == 0) { return REG_SP; }
  if (strcasecmp("lr", token) == 0) { return REG_LR; }
  if (strcasecmp("pc", token) == 0) { return REG_PC; }

  return -1;
}

static int get_special_register(char *token)
{
  int n = 0;

  while (special_reg_thumb[n].name != NULL)
  {
    if (strcasecmp(special_reg_thumb[n].name, token) == 0)
    {
      return special_reg_thumb[n].value;
    }

    n++;
  }

  return -1;
}

static int check_reg_lower(struct _asm_context *asm_context, int value)
{
  if (value > 7)
  {
    print_error_range("Register", 0, 7, asm_context);
    return -1;
  }

  return 0;
}

static int is_4_byte_aligned(struct _asm_context *asm_context, int num)
{
  if ((num & 0x3) != 0)
  {
    print_error("Offset not 4 byte aligned", asm_context);
    return -1;
  }

  return 0;
}

static int is_2_byte_aligned(struct _asm_context *asm_context, int num)
{
  if ((num & 0x1) != 0)
  {
    print_error("Offset not 2 byte aligned", asm_context);
    return -1;
  }

  return 0;
}

static int read_register_list(struct _asm_context *asm_context, struct _operand *operand)
{
  int token_type;
  char token[TOKENLEN];
  int reg_start, reg_end;

  while (1)
  {
    tokens_get(asm_context, token, TOKENLEN);

    if ((strcasecmp(token,"lr") == 0 || strcasecmp(token,"r14") == 0) &&
        operand->second_value == 0)
    {
      operand->second_value = 1;
    }
      else
    if ((strcasecmp(token,"pc") == 0 || strcasecmp(token,"r15") == 0) &&
        operand->second_value == 0)
    {
      operand->second_value = 2;
    }
      else
    if ((reg_start = get_register_thumb(token)) != -1)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '-'))
      {
        tokens_get(asm_context, token, TOKENLEN);
        if ((reg_end = get_register_thumb(token)) == -1 || reg_end < reg_start)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        while (reg_start<=reg_end)
        {
          operand->value |= 1 << reg_start;
          reg_start++;
        }
      }
        else
      {
        operand->value |= 1 << reg_start;
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    tokens_get(asm_context, token, TOKENLEN);
    if (IS_TOKEN(token,'}'))
    {
      break;
    }
      else
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return 0;
}

int parse_cps(struct _asm_context *asm_context, int disable)
{
  char token[TOKENLEN];
  int token_type;
  int n, value;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  n = 0;
  value = 0;

  while (token[n] != 0)
  {
    if (token[n] == 'i' || token[n] == 'I' )
    {
      value |= 2;
    }
    else if ( token[n] == 'f' || token[n] == 'F' )
    {
      value |= 1;
    }
    else
    {
      printf("Error: Unknown flag '%c'\n", token[n]);
      return -1;
    }

    n++;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  add_bin16(asm_context, 0xb660 | (disable << 4) | value , IS_OPCODE);

  return 2;
}

int parse_instruction_thumb(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  struct _operand operands[3];
  int operand_count = 0;
  int matched = 0;
  int num;
  int n;

  lower_copy(instr_case, instr);

  if (strcmp(instr_case, "cpsie") == 0)
  {
    return parse_cps(asm_context, 0);
  }
    else
  if (strcmp(instr_case, "cpsid") == 0)
  {
    return parse_cps(asm_context, 1);
  }

  memset(&operands, 0, sizeof(operands));

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

    if ((num = get_register_thumb(token)) != -1)
    {
      operands[operand_count].type = OPERAND_REGISTER;
      operands[operand_count].value = num;
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '!'))
      {
        operands[operand_count].type = OPERAND_REGISTER_INC;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if ((num = get_h_register_thumb(token)) != -1)
    {
      operands[operand_count].type = OPERAND_H_REGISTER;
      operands[operand_count].value = num;
    }
      else
    if (token_type == TOKEN_POUND)
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
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
      else
    if (IS_TOKEN(token,'['))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (strcasecmp(token,"pc") == 0 || strcasecmp(token,"r15") == 0)
      {
        operands[operand_count].type = OPERAND_PC_AND_REG_IN_BRACKETS;
      }
        else
      if (strcasecmp(token,"sp") == 0 || strcasecmp(token,"r13") == 0)
      {
        operands[operand_count].type = OPERAND_SP_AND_REG_IN_BRACKETS;
      }
        else
      if ((num = get_register_thumb(token)) != -1)
      {
        operands[operand_count].type = OPERAND_TWO_REG_IN_BRACKETS;
        operands[operand_count].value = num;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context,",") != 0) { return -1; }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if ((num = get_register_thumb(token)) != -1)
      {
        operands[operand_count].second_value = num;
        if (operands[operand_count].type == OPERAND_PC_AND_NUM_IN_BRACKETS)
        {
          operands[operand_count].type = OPERAND_PC_AND_REG_IN_BRACKETS;
        }
      }
        else
      if (token_type == TOKEN_POUND)
      {
        if (eval_expression(asm_context, &num) != 0)
        {
          if (asm_context->pass == 1)
          {
            ignore_operand(asm_context);
          }
            else
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }
        }

        operands[operand_count].second_value = num;
        operands[operand_count].type++;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context,"]") != 0) { return -1; }
    }
      else
    if (IS_TOKEN(token,'{'))
    {
      operands[operand_count].type = OPERAND_REGISTER_LIST;
      if (read_register_list(asm_context, &operands[operand_count]) == -1)
      {
        return -1;
      }
    }
      else
    {
      num = get_special_register(token);

      if (num != -1)
      {
        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_SPECIAL_REG;
      }
        else
      {
        tokens_push(asm_context, token, token_type);

        if (eval_expression(asm_context, &num) != 0)
        {
          if (asm_context->pass == 1)
          {
            ignore_operand(asm_context);
          }
            else
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }
        }

        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_ADDRESS;
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token,',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  n = 0;
  while (table_thumb[n].instr!=NULL)
  {
    if (strcmp(table_thumb[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_thumb[n].type)
      {
        case OP_NONE:
          if (operand_count == 0)
          {
            add_bin16(asm_context, table_thumb[n].opcode, IS_OPCODE);
            return 2;
          }
          break;
        case OP_SHIFT:
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (check_reg_lower(asm_context, operands[0].value) == -1) { return -1; }
            if (check_reg_lower(asm_context, operands[1].value) == -1) { return -1; }
            if (check_range(asm_context, "Offset", operands[2].value, 0, 31) == -1) { return -1; }
#if 0
            if (operands[2].value<0 || operands[2].value>31)
            {
              print_error_range("Offset", 0, 31, asm_context);
              return -1;
            }
#endif
            add_bin16(asm_context, table_thumb[n].opcode | (operands[2].value << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_ADD_SUB:
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (operands[2].value << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_NUMBER)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (1 << 10) | (operands[2].value << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG_IMM:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[1].value, -128, 255) == -1) { return -1; }

            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value << 8) | (operands[1].value & 0xff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_ALU:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_HI:
          if (operand_count == 2)
          {
            if (operands[0].type == OPERAND_H_REGISTER &&
                operands[1].type == OPERAND_REGISTER)
            {
              add_bin16(asm_context, table_thumb[n].opcode | (1 << 7) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
              return 2;
            }
              else
            if (operands[0].type == OPERAND_REGISTER &&
                operands[1].type == OPERAND_H_REGISTER)
            {
              add_bin16(asm_context, table_thumb[n].opcode | (1 << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
              return 2;
            }
              else
            if (operands[0].type == OPERAND_H_REGISTER &&
                operands[1].type == OPERAND_H_REGISTER)
            {
              add_bin16(asm_context, table_thumb[n].opcode | (1 << 7) | (1 << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
              return 2;
            }
          }
          break;
        case OP_HI_BX:
          if (operand_count == 1 && operands[0].type == OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value << 3), IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 1 && operands[0].type == OPERAND_H_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (1 << 6) | (operands[0].value << 3), IS_OPCODE);
            return 2;
          }
          break;
        case OP_PC_RELATIVE_LOAD:
          // REVIEW - Docs say this is a 10 bit, 4 byte aligned number
          // and it seems unsigned.  Why isn't this signed?
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_PC_AND_NUM_IN_BRACKETS)
          {
            if (check_range(asm_context, "Offset", operands[1].second_value, 0, 1020) == -1) { return -1; }
            if (is_4_byte_aligned(asm_context, operands[1].second_value) == -1) { return -1; }
#if 0
            if ((operands[1].second_value&0x3) != 0)
            {
              print_error("Offset not 4 byte aligned", asm_context);
              return -1;
            }
#endif
            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value << 8) | (operands[1].second_value >> 2), IS_OPCODE);
            return 2;
          }
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_ADDRESS)
          {
            // REVIEW: This looks so odd.  Docs say: The value of the PC will
            // be 4 bytes greater than the address of this instruction, but bit
            // 1 of the PC is forced to 0 to ensure it is word aligned.
            if (is_4_byte_aligned(asm_context, operands[1].value) == -1) { return -1; }
#if 0
            if ((operands[1].value & 0x3) != 0)
            {
              print_error("Offset not 4 byte aligned", asm_context);
              return -1;
            }
#endif
            int offset = operands[1].value - ((asm_context->address + 4) & 0xfffffffc);
            if (asm_context->pass == 1) { offset = 0; }
            if (check_range(asm_context, "Offset", offset, 0, 1020) == -1) { return -1; }
#if 0
            if (offset<0 || offset>1020)
            {
              print_error_range("Offset", 0, 1020, asm_context);
              return -1;
            }
#endif
            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value << 8) | (offset >> 2), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_TWO_REG_IN_BRACKETS)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (operands[1].second_value << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_SIGN_EXT_HALF_WORD:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_TWO_REG_IN_BRACKETS)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (operands[1].second_value << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_IMM_OFFSET_WORD:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REG_AND_NUM_IN_BRACKETS)
          {
            int offset = operands[1].second_value;
            if (check_range(asm_context, "Offset", offset, 0, 124) == -1) { return -1; }
            if (is_4_byte_aligned(asm_context, offset) == -1) { return -1; }
#if 0
            if ((offset&0x3) != 0)
            {
              print_error("Offset not 4 byte aligned", asm_context);
              return -1;
            }
#endif
            offset = offset >> 2;
            add_bin16(asm_context, table_thumb[n].opcode | (offset << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_IMM_OFFSET:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REG_AND_NUM_IN_BRACKETS)
          {
            if (check_range(asm_context, "Offset", operands[1].second_value, 0, 31) == -1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode | (operands[1].second_value << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_IMM_OFFSET_HALF_WORD:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REG_AND_NUM_IN_BRACKETS)
          {
            int offset = operands[1].second_value;
            if (check_range(asm_context, "Offset", offset, 0, 60) == -1) { return -1; }
            if (is_2_byte_aligned(asm_context, offset) == -1) { return -1; }
#if 0
            if ((offset&0x1) != 0)
            {
              print_error("Offset not 2 byte aligned", asm_context);
              return -1;
            }
#endif
            offset = offset >> 1;
            add_bin16(asm_context, table_thumb[n].opcode | (offset << 6) | (operands[1].value << 3) | (operands[0].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_STORE_SP_RELATIVE:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_SP_AND_NUM_IN_BRACKETS)
          {
            if (check_range(asm_context, "Offset", operands[1].value, 0, 1020) == -1) { return -1; }
            if (is_4_byte_aligned(asm_context, operands[1].value) == -1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value << 8) | (operands[1].second_value >> 2), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LOAD_ADDRESS:
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_H_REGISTER &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Offset", operands[2].value, 0, 1020) == -1) { return -1; }
            if (is_4_byte_aligned(asm_context, operands[2].value) == -1) { return -1; }
            if (operands[1].value == 7)
            {
              add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(operands[2].value>>2), IS_OPCODE);
              return 2;
            }
              else
            if (operands[1].value == 5)
            {
              add_bin16(asm_context, table_thumb[n].opcode | (1 << 11) | (operands[0].value << 8) | (operands[2].value >> 2), IS_OPCODE);
              return 2;
            }
          }
          break;
        case OP_ADD_OFFSET_TO_SP:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_H_REGISTER &&
              operands[0].value == 5 &&
              operands[1].type == OPERAND_NUMBER)
          {
            // FIXME - According to the docs, what I did here is wrong.
            // it says that the offset is +-508 and i have +-1020.
            if (check_range(asm_context, "Offset", operands[1].value, -1020, 1020) == -1) { return -1; }
            int s = 0;
            if (operands[1].value<0)
            {
              operands[1].value = -operands[1].value;
	      s = 1;
	    }
            if (is_4_byte_aligned(asm_context, operands[1].value) == -1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode | (s << 7) | (operands[1].value >> 2), IS_OPCODE);
            return 2;
          }
          break;
        case OP_PUSH_POP_REGISTERS:
          if (operand_count == 1 && operands[0].type == OPERAND_REGISTER_LIST)
          {
            int opcode = table_thumb[n].opcode | operands[0].value;
            if (operands[0].second_value == 1)
            {
              if (((opcode >> 11) & 1) == 1) { break; }
              opcode |= (1 << 8);
            }
              else
            if (operands[0].second_value == 2)
            {
              if (((opcode >> 11) & 1) == 0) { break; }
              opcode |= (1 << 8);
            }

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
          break;
        case OP_MULTIPLE_LOAD_STORE:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER_INC &&
              operands[1].type == OPERAND_REGISTER_LIST)
          {
            add_bin16(asm_context, table_thumb[n].opcode|(operands[0].value<<8)|(operands[1].value), IS_OPCODE);
            return 2;
          }
          break;
        case OP_CONDITIONAL_BRANCH:
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            // From the docs: The branch offset must take account of the
            // prefetch operation, which causes the PC to be 1 word (4 bytes)
            // ahead of the current instruction.
            int offset = operands[0].value - (asm_context->address + 4);
            if (asm_context->pass == 1) { offset = 0; }
            if (check_range(asm_context, "Offset", offset, -256, 255) == -1) { return -1; }
            if (is_2_byte_aligned(asm_context, offset) == -1) { return -1; }
            offset >>= 1;
            add_bin16(asm_context, table_thumb[n].opcode | (offset & 0xff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_SOFTWARE_INTERRUPT:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Comment", operands[0].value, 0, 255) == -1) { return -1; }
            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value & 0xff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_UNCONDITIONAL_BRANCH:
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            // From the docs: The branch offset must take account of the
            // prefetch operation, which causes the PC to be 1 word (4 bytes)
            // ahead of the current instruction.
            int offset = operands[0].value - (asm_context->address + 4);
            if (asm_context->pass == 1) { offset = 0; }
            if (check_range(asm_context, "Offset", offset, -2048, 2047) == -1) { return -1; }
            if (is_2_byte_aligned(asm_context, offset) == -1) { return -1; }
            offset >>= 1;
            add_bin16(asm_context, table_thumb[n].opcode | (offset & 0x7ff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_LONG_BRANCH_WITH_LINK:
          if (operand_count == 1 &&
              operands[0].type == OPERAND_ADDRESS)
          {
            // From the docs: The branch offset must take account of the
            // prefetch operation, which causes the PC to be 1 word (4 bytes)
            // ahead of the current instruction.
            int offset = operands[0].value - (asm_context->address + 4);
            if (asm_context->pass == 1) { offset = 0; }
            if (check_range(asm_context, "Offset", offset, -(1<<22), (1<<22)-1) == -1) { return -1; }
            if (is_2_byte_aligned(asm_context, offset) == -1) { return -1; }
            offset >>= 1;
            add_bin16(asm_context, table_thumb[n].opcode | ((offset >> 11) & 0x7ff), IS_OPCODE);
            add_bin16(asm_context, table_thumb[n].opcode | (1 << 11) | (offset & 0x7ff), IS_OPCODE);
            return 4;
          }
          break;
        case OP_SP_SP_IMM:
          if (operand_count == 3 &&
              operands[0].type == OPERAND_H_REGISTER &&
              operands[1].type == OPERAND_H_REGISTER &&
              operands[0].value == REG_SP &&
              operands[1].value == REG_SP &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "immediate", operands[2].value, 0, 508) == -1) { return -1; }

            if ((operands[2].value & 0x3) != 0)
            {
              print_error("Immediate not a multiple of 4 bytes.", asm_context);
              return -1;
            }

            add_bin16(asm_context, table_thumb[n].opcode | ((operands[2].value >> 2) & 0x3f), IS_OPCODE);
            return 2;
          }
          break;
        case OP_REG_REG:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER)
          {
            add_bin16(asm_context, table_thumb[n].opcode | (operands[1].value << 3) | operands[0].value, IS_OPCODE);
            return 2;
          }
        case OP_UINT8:
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[0].value, 0, 255) == -1) { return -1; }

            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value & 0xff), IS_OPCODE);
            return 2;
          }
          break;
        case OP_REGISTER_ADDRESS:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_ADDRESS)
          {
            int offset = operands[1].value - (asm_context->address + 4);
            if (asm_context->pass == 1) { offset = 0; }
            if (check_range(asm_context, "Offset", offset, 0, 1020) == -1) { return -1; }
            if (is_4_byte_aligned(asm_context, offset) == -1)
            {
              printf("       %s address: %d, data address: %d, offset: %d\n",
                instr, asm_context->address, operands[1].value, offset);
              return -1;
            }
            offset >>= 2;

            add_bin16(asm_context, table_thumb[n].opcode | (operands[0].value << 8) | offset, IS_OPCODE);
            return 2;
          }
          break;
        case OP_MRS:
          if (operand_count == 2 &&
             (operands[0].type == OPERAND_REGISTER ||
              operands[0].type == OPERAND_H_REGISTER) &&
             (operands[1].type == OPERAND_ADDRESS ||
              operands[1].type == OPERAND_SPECIAL_REG))
          {
            if (check_range(asm_context, "Special Register", operands[0].value, 0, 15) == -1) { return -1; }

            add_bin16(asm_context, table_thumb[n].opcode, IS_OPCODE);
            add_bin16(asm_context, 0x8000 |
                                   (operands[0].value << 8) |
                                    operands[1].value, IS_OPCODE);
            return 4;
          }
          break;
        case OP_MSR:
          if (operand_count == 2 &&
             (operands[0].type == OPERAND_ADDRESS ||
              operands[0].type == OPERAND_SPECIAL_REG) &&
             (operands[1].type == OPERAND_REGISTER ||
              operands[1].type == OPERAND_H_REGISTER))
          {
            if (check_range(asm_context, "Special Register", operands[1].value, 0, 15) == -1) { return -1; }

            add_bin16(asm_context, table_thumb[n].opcode |
                                   operands[1].value, IS_OPCODE);
            add_bin16(asm_context, 0x8800 | operands[0].value, IS_OPCODE);
            return 4;
          }
          break;
        case OP_REG_LOW:
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER && operands[0].value <= 7 &&
              operands[1].type == OPERAND_REGISTER && operands[1].value <= 7)
          {
            add_bin16(asm_context, table_thumb[n].opcode |
                                   operands[0].value |
                                  (operands[1].value << 3), IS_OPCODE);
            return 2;
          }
          break;
        default:
          break;
      }
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

