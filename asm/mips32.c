/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/mips32.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/mips32.h"

extern struct _mips32_instr mips32_r_table[];
extern struct _mips32_instr mips32_i_table[];
extern struct _mips32_cop_instr mips32_cop_table[];

enum
{
  OPERAND_TREG,
  OPERAND_FREG,
  OPERAND_IMMEDIATE,
  OPERAND_IMMEDIATE_RS,
};

struct _operand
{
  int value;
  int type;
  int reg2;
};

static int get_number(char *s)
{
  int n = 0;

  while(*s != 0)
  {
    if (*s < '0' || *s > '9') return -1;
    n = (n * 10) + (*s - '0');
    s++;
  }

  return n;
}

static int get_register_mips32(char *token, char letter)
{
  int num;

  if (token[0] != '$')
  {
    if (letter != 'f' && strcasecmp(token, "zero") == 0) return 0;
    return -1;
  }
  if (token[1] == 0) { return -1; }

  num = get_number(token + 2);
  if (letter == 'f')
  {
    if (num >= 0 && num <= 31 && token[1] == letter)
    {
      return num; 
    }

    return -1;
  }

  if (token[1] >= '0' && token[1] <= '9')
  {
    num = get_number(token + 1);
    if (num >= 0 && num <= 31) return num;
    return -1;
  }

  if (token[1] == 'v' && num >= 0 && num <= 1) { return 2 + num; }
  if (token[1] == 'a' && num >= 0 && num <= 3) { return 4 + num; }
  if (token[1] == 't' && num >= 0 && num <= 7) { return 8 + num; }
  if (token[1] == 's' && num >= 0 && num <= 7) { return 16 + num; }
  if (token[1] == 't' && num >= 8 && num <= 9) { return 24 + (num - 8); }
  if (token[1] == 'k' && num >= 0 && num <= 1) { return 26 + num; }

  if (strcasecmp(token, "$gp") == 0) return 28;
  if (strcasecmp(token, "$sp") == 0) return 29;
  if (strcasecmp(token, "$s8") == 0) return 30;
  if (strcasecmp(token, "$fp") == 0) return 30;
  if (strcasecmp(token, "$ra") == 0) return 31;

  return -1;
}

static void check_for_pseudo_instruction(struct _operand *operands, int *operand_count, char *instr_case)
{
  // Check pseudo-instructions
  if (strcmp(instr_case, "move") == 0 && *operand_count == 2)
  {
    strcpy(instr_case, "add");
    operands[2].value = 0;
    operands[2].type = OPERAND_TREG;;
    *operand_count = 3;
  }
    else
#if 0
  if (strcmp(instr_case, "li") == 0 && operand_count == 2)
  {
    strcpy(instr_case, "addi");
    memcpy(&operands[2], &operands[1], sizeof(struct _operand));
    operands[1].value = 0;
    operands[1].reg2 = 0;
    operands[1].type = OPERAND_TREG;;
    *operand_count = 3;
  }
    else
#endif
  if (strcmp(instr_case, "nop") == 0 && *operand_count == 0)
  {
    strcpy(instr_case, "add");
    *operand_count = 3;
  }
    else
  if (strcmp(instr_case, "neg") == 0 && *operand_count == 1)
  {
    strcpy(instr_case, "subu");
    memcpy(&operands[1], &operands[0], sizeof(struct _operand));
    *operand_count = 3;
  }
    else
  if (strcmp(instr_case, "b") == 0 && *operand_count == 1)
  {
    strcpy(instr_case, "beq");
    memcpy(&operands[2], &operands[0], sizeof(struct _operand));
    operands[0].value = 0;
    operands[0].type = OPERAND_TREG;
    operands[1].value = 0;
    operands[1].type = OPERAND_TREG;
    *operand_count = 3;
  }
}

int parse_instruction_mips32(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[3];
  int operand_count = 0;
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int paren_flag;
  int num,n,r;
  int opcode;
#if 0
  int n,cond,s=0;
  int opcode=0;
#endif

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

//printf("%s %s\n", instr_case, instr);

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    //printf("token=%s token_type=%d\n", token, token_type);

    if (operand_count == 0 && IS_TOKEN(token,'.'))
    {
      strcat(instr_case, ".");
      strcat(instr, ".");
      token_type = tokens_get(asm_context, token, TOKENLEN);
      strcat(instr, token);
      n = 0;
      while(token[n] != 0) { token[n]=tolower(token[n]); n++; }
      strcat(instr_case, token);
      continue;
    }

    do
    {
      paren_flag = 0;

      if (IS_TOKEN(token,'('))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        paren_flag = 1;
      }

      num = get_register_mips32(token, 't');
      if (num != -1)
      {
        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_TREG;
        if (paren_flag == 0) { break; }
      }
        else
      if (paren_flag == 0)
      {
        num = get_register_mips32(token, 'f');
        if (num != -1)
        {
          operands[operand_count].value = num;
          operands[operand_count].type = OPERAND_FREG;
          break;
        }
      }

      if (paren_flag == 1)
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,')'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].reg2 = operands[operand_count].value;
        operands[operand_count].value = 0;
        operands[operand_count].type = OPERAND_IMMEDIATE_RS;;

        break;
      }

      operands[operand_count].type = OPERAND_IMMEDIATE;

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        break;
      }

      tokens_push(asm_context, token, token_type);
      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = num;

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'('))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        num = get_register_mips32(token, 't');
        if (num == -1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].reg2 = num;
        operands[operand_count].type = OPERAND_IMMEDIATE_RS;;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_NOT_TOKEN(token,')'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }

    } while(0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  if (asm_context->pass == 1)
  {
    add_bin32(asm_context, 0, IS_OPCODE);
    return 4;
  }

  check_for_pseudo_instruction(operands, &operand_count, instr_case);

  // R-Type Instruction [ op 6, rs 5, rt 5, rd 5, sa 5, function 6 ]
  n = 0;
  while(mips32_r_table[n].instr != NULL)
  {
    if (strcmp(instr_case, mips32_r_table[n].instr) == 0)
    {
      char shift_table[] = { 0, 11, 21, 16, 6 };
      if (mips32_r_table[n].operand_count != operand_count)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      opcode = mips32_r_table[n].function;

      for (r = 0; r < operand_count; r++)
      {
        if (operands[r].type != OPERAND_TREG)
        {
//printf("%s %s %s\n", instr_case, mips32_r_table[n].instr, instr);
          printf("Error: '%s' expects registers at %s:%d\n", instr, asm_context->filename, asm_context->line);
          return -1;
        }
//printf("%s  %d<<%d\n", instr, operands[r].value, shift_table[(int)mips32_r_table[n].operand[r]]);
        opcode |= operands[r].value << shift_table[(int)mips32_r_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
    n++;
  }

  // J-Type Instruction [ op 6, target 26 ]
  if (strcmp(instr_case, "ja") == 0 || strcmp(instr_case, "jal") == 0)
  {
    // FIXME - what to do with this
    //unsigned int upper = (address + 4) & 0xf0000000;
    if (operand_count != 1)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (operands[0].type != OPERAND_IMMEDIATE)
    {
      printf("Error: Expecting address for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
      return -1;
    }

    if (instr_case[2] == 'l')  { opcode = 2 << 26; }
    else { opcode = 3 << 26; }

    add_bin32(asm_context, opcode | operands[0].value >> 2, IS_OPCODE);

    return 4;
  }

  // Coprocessor Instruction [ op 6, format 5, ft 5, fs 5, fd 5, funct 6 ]
  n = 0;
  while(mips32_cop_table[n].instr != NULL)
  {
    if (strcmp(instr_case, mips32_cop_table[n].instr) == 0)
    {
      char shift_table[] = { 0, 5, 11, 16 };
      if (mips32_cop_table[n].operand_count != operand_count)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      opcode = (0x11 << 26) | (mips32_cop_table[n].format << 21) | mips32_cop_table[n].function;

      for (r = 0; r < operand_count; r++)
      {
        if (operands[r].type != OPERAND_FREG)
        {
          printf("Error: '%s' expects registers at %s:%d\n", instr, asm_context->filename, asm_context->line);
          return -1;
        }
        opcode |= operands[r].value << shift_table[(int)mips32_cop_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
    n++;
  }

  // I-Type?  [ op 6, rs 5, rt 5, imm 16 ]
  n = 0;
  while(mips32_i_table[n].instr != NULL)
  {
    if (strcmp(instr_case, mips32_i_table[n].instr) == 0)
    {
      char shift_table[] = { 0, 0, 21, 16 };
      if (mips32_i_table[n].operand_count != operand_count)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      opcode = mips32_i_table[n].function << 26;

      for (r = 0; r < mips32_i_table[n].operand_count; r++)
      {
        if ((mips32_i_table[n].operand[r] == MIPS_OP_RT ||
             mips32_i_table[n].operand[r] == MIPS_OP_RS) &&
            operands[r].type == OPERAND_TREG)
        {
          opcode |= operands[r].value << shift_table[(int)mips32_i_table[n].operand[r]];
        }
          else
        if (mips32_i_table[n].operand[r] == MIPS_OP_LABEL)
        {
          int32_t offset = operands[r].value - (asm_context->address + 4);

          if (operands[r].value < -(1 << 17) ||
              operands[r].value > (1 << 17) - 1)
          {
            print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context); 
            return -1;
          }

          opcode |= (offset >> 2) & 0xffff;
        }
          else
        if (mips32_i_table[n].operand[r] == MIPS_OP_IMMEDIATE)
        {
          if (operands[r].value > 65535 || operands[r].value < -32768)
          {
            print_error_range("Constant", -32768, 65535, asm_context); 
            return -1;
          }
          opcode |= operands[r].value;
        }
          else
        if (mips32_i_table[n].operand[r] == MIPS_OP_IMMEDIATE_RS)
        {
          if (operands[r].value > 65535 || operands[r].value < -32768)
          {
            print_error_range("Constant", -32768, 65535, asm_context); 
            return -1;
          }
          opcode |= operands[r].value;
          opcode |= operands[r].reg2 << 21;
        }
          else
        if (mips32_i_table[n].operand[r] == MIPS_OP_RT_IS_0)
        {
          // Derr
          print_error_internal(asm_context, __FILE__, __LINE__);
        }
          else
        if (mips32_i_table[n].operand[r] == MIPS_OP_RT_IS_1)
        {
          opcode |= 1 << 16;
        }
          else
        {
          print_error_illegal_operands(instr, asm_context);
          return -1;
        }
        opcode |= operands[r].value << shift_table[(int)mips32_i_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);

      return 4;
    }
    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}


