/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https//www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/dspic.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "disasm/dspic.h"
#include "table/dspic.h"

enum
{
  OPTYPE_ERROR,
  OPTYPE_REGISTER,
  OPTYPE_WREG,
  OPTYPE_LIT,
  OPTYPE_NUM,
  //OPTYPE_INDEX,
  //OPTYPE_INDEX_PRE_INC,
  //OPTYPE_INDEX_PRE_DEC,
  //OPTYPE_INDEX_POST_INC,
  //OPTYPE_INDEX_POST_DEC,
  OPTYPE_ACCUM,
  OPTYPE_W_PLUS_LIT,
  //OPTYPE_W_PLUS_W,   <-- hmmm
  OPTYPE_W_OP_EQ_NUM,
  //OPTYPE_W_MINUS_EQ_KX,
  //OPTYPE_W_PLUS_EQ_KX,
  //OPTYPE_W_MINUS_EQ_KY,
  //OPTYPE_W_PLUS_EQ_KY,
  OPTYPE_W_MUL_W,
};

static int is_condition(char *token)
{
  char *cond[] = {
    "c", "ge", "geu", "gt", "gtu", "le",
    "leu", "lt", "ltu", "n", "nc", "nn",
    "nov", "nz", "oa", "ob", "ov", "sa",
    "sb", "z", NULL };
  int n;

  n = 0;
  while (cond[n] != NULL)
  {
    if (strcasecmp(cond[n], token) == 0) { return 1; }
    n++;
  }

  return 0;
}

#define REG_NORMAL 0
#define REG_INDIRECT 1
#define REG_INDIRECT_POST_DEC 2
#define REG_INDIRECT_POST_INC 3
#define REG_INDIRECT_PRE_DEC 4
#define REG_INDIRECT_PRE_INC 5
#define REG_INDIRECT_W_PLUS_W 6

struct _operand
{
  int value;
  int type;
  int attribute;
  int reg2;
};

static int get_register_dspic(char *token)
{
  if (token[0] == 'w' || token[0] == 'W')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
    {
      return token[1] - '0';
    }
      else
    if (token[3] == 0 && token[1]=='1' && (token[2] >= '0' && token[2] <= '5'))
    {
      return 10 + (token[2] - '0');
    }
  }

  if (strcasecmp(token, "wreg") == 0) { return 0; }
  if (strcasecmp(token, "sp") == 0) { return 15; }

  return -1;
}

static int check_f(struct _asm_context *asm_context, int value)
{
  if ((value & 1) != 0)
  {
    print_error("Address not on 16 bit boundary", asm_context);
    return -1;
  }

  if (value < 0 || value > 0xffff)
  {
    print_error_range("Address", 0, 0xffff, asm_context);
    return -1;
  }

  return 0;
}

static int check_f_flag(struct _asm_context *asm_context, int value, int flag)
{
  if (flag != FLAG_B && (value & 1) != 0)
  {
    print_error("Address not on 16 bit boundary", asm_context);
  }

  if (value < 0 || value > 0x1fff)
  {
    print_error_range("Address", 0, 0x1fff, asm_context);
    return -1;
  }

  return 0;
}

static int check_f_64k(struct _asm_context *asm_context, int value)
{
  if ((value & 1) != 0)
  {
    print_error("Address not on 16 bit boundary", asm_context);
  }

  if (value < 0 || value > 0xfffe)
  {
    print_error_range("Address", 0, 0xfffe, asm_context);
    return -1;
  }

  return 0;
}

static int parse_wx_wy(struct _operand *operands, int w, int *iiii)
{
  int num;

  if (operands->type == OPTYPE_REGISTER && operands->attribute == REG_INDIRECT)
  {
    *iiii=(operands->value & 1) == 0 ? 0 : 8;
  }
    else
  if (operands->type == OPTYPE_REGISTER &&
      operands->attribute == REG_INDIRECT_W_PLUS_W &&
      operands->value == w + 1 &&
      operands->reg2 == 12)
  {
    *iiii = 12;
  }
    else
  if (operands->type == OPTYPE_W_OP_EQ_NUM)
  {
    *iiii = (operands->value & 1) == 0 ? 0 : 8;
    num = operands->attribute;
    if ((num & 1) != 0 || num < -6 || num > 6) { return -1; }
    *iiii |= (num / 2) & 0x7;
  }
    else
  {
    return -1;
  }

  return 0;
}

static int parse_dsp_op(struct _operand *operands, int operand, int operand_count, int w, int *xx, int *iiii)
{
  if (operand + 2 > operand_count ||
      operands[operand + 1].value < 4 || operands[operand + 1].value > 7)
  {
    return -1;
  }

  *xx = operands[operand + 1].value - 4;

  return parse_wx_wy(&operands[operand], w, iiii);
}

static int parse_awb(struct _asm_context *asm_context, struct _operand *operands, int *aa)
{
  if (operands->type == OPTYPE_REGISTER &&
      operands->value == 13 &&
      operands->attribute == REG_NORMAL)
  {
    if (operands->attribute > 1) { return -1; }
    *aa = 0;
  }
    else
  if (operands->type == OPTYPE_W_OP_EQ_NUM &&
      operands->value == 13 &&
      operands->attribute == 2)
  {
    *aa = 1;
  }
    else
  {
    return -1;
  }

  return 0;
}

static int parse_mmm(struct _asm_context *asm_context, struct _operand *operands, int *mmm)
{
  if (operands->type != OPTYPE_W_MUL_W) { return -1; }
  if (operands->value == 4)
  {
    if (operands->reg2 == 5) { *mmm = 0; }
    else if (operands->reg2 == 6) { *mmm = 1; }
    else if (operands->reg2 == 7) { *mmm = 2; }
    else { return -1; }
  }
    else
  if (operands->value == 5)
  {
    if (operands->reg2 == 6) { *mmm = 4; }
    if (operands->reg2 == 7) { *mmm = 5; }
    else { return -1; }
  }
    else
  if (operands->value == 6 && operands->reg2 == 7)
  {
    *mmm = 6;
  }
    else
  { return -1; }

  return 0;
}

static int parse_mm(struct _asm_context *asm_context, struct _operand *operands, int *mm)
{
  if (operands->type != OPTYPE_W_MUL_W) { return -1; }
  if (operands->value != operands->reg2) { return -1; }
  if (operands->value < 4 || operands->reg2 > 7) { return -1; }

  *mm=operands->value - 4;

  return 0;
}

static int parse_movsac(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int opcode)
{
int operand = 0;
int xx = 0, yy = 0, iiii = 4, jjjj = 4, aa = 2;

  if (operand_count < 1 ||
      operands[operand].type != OPTYPE_ACCUM) { return -1; }
  opcode |= (operands[operand++].value << 15);

  do
  {
    if (operand >= operand_count) { break; }

    if (operands[operand].value == 8 || operands[operand].value == 9)
    {
      if (parse_dsp_op(operands, operand, operand_count, 8, &xx, &iiii) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand >= operand_count) { break; }

    if (operands[operand].value == 10 || operands[operand].value == 11)
    {
      if (parse_dsp_op(operands, operand, operand_count, 10, &yy, &jjjj) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand >= operand_count) { break; }

    if (parse_awb(asm_context, &operands[operand], &aa) == -1) { return -1; }

    operand++;

    if (operand < operand_count) { return -1; }

  } while (0);

  opcode |= (xx << 12) | (yy << 10) | (iiii << 6) | (jjjj << 2) | aa;

  return opcode;
}

static int parse_mpy_n(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int opcode)
{
  int operand = 0;
  int xx = 0, yy = 0, iiii = 4, jjjj = 4, mmm = 0;

  if (operand >= operand_count) { return -1; }

  if (parse_mmm(asm_context, operands, &mmm) == -1) { return -1; }

  operand++;

  if (operand >= operand_count) { return -1; }

  if (operands[operand].type != OPTYPE_ACCUM) { return -1; }

  opcode |= (operands[operand++].value << 15);

  do
  {
    if (operand >= operand_count) { break; }

    if (operands[operand].value == 8 || operands[operand].value == 9)
    {
      if (parse_dsp_op(operands, operand, operand_count, 8, &xx, &iiii) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand >= operand_count) { break; }

    if (operands[operand].value == 10 || operands[operand].value == 11)
    {
      if (parse_dsp_op(operands, operand, operand_count, 10, &yy, &jjjj) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand < operand_count) { return -1; }

  } while (0);

  opcode |= (mmm << 16) | (xx << 12) | (yy << 10) | (iiii << 6) | (jjjj << 2);

  return opcode;
}

static int parse_mac_m_m(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int opcode)
{
  int operand = 0;
  int xx = 0, yy = 0, iiii = 4, jjjj = 4, mm = 0;

  if (operand >= operand_count) { return -1; }
  if (parse_mm(asm_context, operands, &mm) == -1) { return -1; }

  operand++;

  if (operand >= operand_count) { return -1; }
  if (operands[operand].type != OPTYPE_ACCUM) { return -1; }

  opcode |= (operands[operand++].value << 15);

  do
  {
    if (operand >= operand_count) { break; }

    if (operands[operand].value == 8 || operands[operand].value == 9)
    {
      if (parse_dsp_op(operands, operand, operand_count, 8, &xx, &iiii) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand >= operand_count) { break; }

    if (operands[operand].value == 10 || operands[operand].value == 11)
    {
      if (parse_dsp_op(operands, operand, operand_count, 10, &yy, &jjjj) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand < operand_count) { return -1; }

  } while (0);

  opcode |= (mm << 16) | (xx << 12) | (yy << 10) | (iiii << 6) | (jjjj << 2);

  return opcode;
}

static int parse_ed_m_m(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int opcode)
{
  int xx = 0, iiii = 4, jjjj = 4, mm = 0;

  if (operand_count != 5) { return -1; }
  if (parse_mm(asm_context, operands, &mm) == -1) { return -1; }
  if (operands[1].type != OPTYPE_ACCUM) { return -1; }

  opcode |= (operands[1].value << 15);

  if (operands[2].value != 8 && operands[2].value != 9) { return -1; }
  if (parse_wx_wy(&operands[2], 8, &iiii) == -1) { return -1; }

  if (operands[3].value != 10 && operands[3].value != 11) { return -1; }
  if (parse_wx_wy(&operands[3], 10, &jjjj) == -1) { return -1; }

  if (operands[4].type != OPTYPE_REGISTER || operands[4].attribute != 0)
  {
    return -1;
  }

  xx = operands[4].value - 4;

  opcode |= (mm << 16) | (xx << 12) | (iiii << 6) | (jjjj << 2);

  return opcode;
}

static int parse_mac_m_n(struct _asm_context *asm_context, struct _operand *operands, int operand_count, int opcode)
{
int operand = 0;
int xx = 0, yy = 0, iiii = 4, jjjj = 4, mmm = 0, aa = 2;

  if (operand >= operand_count) { return -1; }

  if (parse_mmm(asm_context, operands, &mmm) == -1) { return -1; }
  operand++;

  if (operand >= operand_count) { return -1; }

  if (operands[operand].type != OPTYPE_ACCUM) { return -1; }
  opcode |= (operands[operand++].value << 15);

  do
  {
    if (operand >= operand_count) { break; }

    if (operands[operand].value == 8 || operands[operand].value == 9)
    {
      if (parse_dsp_op(operands, operand, operand_count, 8, &xx, &iiii) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand >= operand_count) { break; }

    if (operands[operand].value == 10 || operands[operand].value == 11)
    {
      if (parse_dsp_op(operands, operand, operand_count, 10, &yy, &jjjj) == -1)
      {
        return -1;
      }

      operand += 2;
    }

    if (operand >= operand_count) { break; }

    if (parse_awb(asm_context, &operands[operand], &aa) == -1) { return -1; }

    operand++;

    if (operand < operand_count) { return -1; }

  } while (0);

  opcode |= (mmm << 16) | (xx << 12) | (yy << 10) | (iiii << 6) | (jjjj << 2) | aa;

  return opcode;
}

int parse_instruction_dspic(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[7];
  int operand_count = 0;
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  int flag=FLAG_NONE;
  int token_type;
  int matched;
  int opcode = 0;
  int num;
  int n;

  memset(&operands, 0, sizeof(operands));

  lower_copy(instr_case, instr);

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }

    if (operand_count == 7)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    num = 0;

    if (operand_count == 0)
    {
      if (strcmp(instr_case, "bra") == 0)
      {
        if (is_condition(token) == 1)
        {
          char *s = token;
          while (*s != 0) { *s = tolower(*s); s++; }
          strcat(instr_case, " ");
          strcat(instr_case, token);
          token_type=tokens_get(asm_context, token, TOKENLEN);

          if (IS_NOT_TOKEN(token,','))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);
        }
      }
        else
      if (IS_TOKEN(token,'.'))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        char s[TOKENLEN];
        lower_copy(s, token);

        if (IS_TOKEN(s,'b')) { flag = FLAG_B; }
        else if (IS_TOKEN(s,'w')) { flag = FLAG_W; }
        else if (IS_TOKEN(s,'c')) { flag = FLAG_C; }
        else if (IS_TOKEN(s,'z')) { flag = FLAG_Z; }
        else if (IS_TOKEN(s,'n')) { flag = FLAG_N; }
        else if (IS_TOKEN(s,'d')) { flag = FLAG_D; }
        else if (IS_TOKEN(s,'u')) { flag = FLAG_U; }
        else if (IS_TOKEN(s,'s')) { flag = FLAG_S; }
        else if (IS_TOKEN(s,'r')) { flag = FLAG_R; }
        else if (strcmp(s,"sw") == 0) { flag = FLAG_SW; }
        else if (strcmp(s,"sd") == 0) { flag = FLAG_SD; }
        else if (strcmp(s,"uw") == 0) { flag = FLAG_UW; }
        else if (strcmp(s,"ud") == 0) { flag = FLAG_UD; }
        else if (strcmp(s,"ss") == 0) { flag = FLAG_SS; }
        else if (strcmp(s,"su") == 0) { flag = FLAG_SU; }
        else if (strcmp(s,"us") == 0) { flag = FLAG_US; }
        else if (strcmp(s,"uu") == 0) { flag = FLAG_UU; }
        else
        {
          return -1;
          print_error_unexp(token, asm_context);
        }

        //token_type=tokens_get(asm_context, token, TOKENLEN);
        //if (token_type<0) return -1;

        continue;
      }
    }

    if (IS_TOKEN(token,'a') || IS_TOKEN(token,'A'))
    {
      operands[operand_count].type = OPTYPE_ACCUM;
      num = 0;
    }
      else
    if (IS_TOKEN(token,'b') || IS_TOKEN(token,'B'))
    {
      operands[operand_count].type = OPTYPE_ACCUM;
      num = 1;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      operands[operand_count].type = OPTYPE_LIT;

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
    }
      else
    if (IS_TOKEN(token,'['))
    {
      operands[operand_count].type = OPTYPE_REGISTER;
      operands[operand_count].attribute = REG_INDIRECT;
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'+'))
      {
        if (expect_token(asm_context, '+') == -1) { return -1; }

        operands[operand_count].attribute = REG_INDIRECT_PRE_INC;
        token_type = tokens_get(asm_context, token, TOKENLEN);
      }
        else
      if (IS_TOKEN(token,'-'))
      {
        if (expect_token(asm_context, '-') == -1) { return -1; }
        operands[operand_count].attribute = REG_INDIRECT_PRE_DEC;
        token_type = tokens_get(asm_context, token, TOKENLEN);
      }

      num = get_register_dspic(token);
      if (num < 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      operands[operand_count].value = num;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'+'))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,'+'))
        {
          // Check for: [W + #] and [W + W]
          if (IS_TOKEN(token,'#'))
          {
            int a;
            operands[operand_count].type = OPTYPE_W_PLUS_LIT;
            if (eval_expression(asm_context, &a) != 0)
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
            operands[operand_count].attribute = a;
          }
            else
          {
            operands[operand_count].reg2 = get_register_dspic(token);
            if (operands[operand_count].reg2 == -1)
            {
              int a;
              operands[operand_count].type = OPTYPE_W_PLUS_LIT;
              tokens_push(asm_context, token, token_type);

              if (eval_expression(asm_context, &a) != 0)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              operands[operand_count].attribute = a;
            }
              else
            {
              operands[operand_count].attribute = REG_INDIRECT_W_PLUS_W;
            }
          }
        }
          else
        {
          operands[operand_count].attribute = REG_INDIRECT_POST_INC;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);
      }
        else
      if (IS_TOKEN(token,'-'))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, '-'))
        {
          operands[operand_count].attribute = REG_INDIRECT_POST_DEC;
        }
          else
        {
          int a;
          operands[operand_count].type = OPTYPE_W_PLUS_LIT;
          tokens_push(asm_context, token, token_type);

          if (eval_expression(asm_context, &a) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].attribute = -a;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);
      }

      if (IS_NOT_TOKEN(token,']'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'-') || IS_TOKEN(token,'+'))
      {
        operands[operand_count].type = OPTYPE_W_OP_EQ_NUM;
        int a = 1;

        if (IS_TOKEN(token,'-')) { a = -1; }
        if (expect_token(asm_context, '=') == -1) { return -1; }

        int temp;
        if (eval_expression(asm_context, &temp) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].attribute = a * temp;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (strcasecmp(token, "wreg") == 0)
    {
      operands[operand_count].type=OPTYPE_WREG;
    }
      else
    {
      num=get_register_dspic(token);
      if (num >= 0)
      {
        operands[operand_count].type = OPTYPE_REGISTER;
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token,'*'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          operands[operand_count].reg2 = get_register_dspic(token);
          if (operands[operand_count].reg2 == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
          operands[operand_count].type = OPTYPE_W_MUL_W;
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }
      }
        else
      {
        operands[operand_count].type = OPTYPE_NUM;
        tokens_push(asm_context, token, token_type);

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
      }
    }

    operands[operand_count++].value = num;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
  printf("-------- instr=%s  flag=%d operand_count=%d\n", instr, flag, operand_count);
  char *types[]= { "ERROR", "REGISTER", "WREG", "LIT", "NUM", "ACCUM", "W_PLUS_LIT", "W_OP_EQ_NUM", "W_MUL_W" };
  for (n = 0; n < operand_count; n++)
  {
    printf("operand %d: value=%d type=%s (%d) attribute=%d reg2=%d\n",
      n,
      operands[n].value,
      types[operands[n].type],
      operands[n].type,
      operands[n].attribute,
      operands[n].reg2);
  }
#endif

  // On pass 1 only calculate address.
  if (asm_context->pass == 1)
  {
    add_bin32(asm_context, 0x0000000, IS_OPCODE);

    if (strcmp("do", instr_case) == 0 ||
        (((strcmp("goto", instr_case) == 0 || strcmp("call", instr_case) == 0)
         && operand_count == 1 &&
         operands[0].type != OPTYPE_REGISTER)))
    {
      add_bin32(asm_context, 0x0000000, IS_OPCODE);
      return 8;
    }

    return 4;
  }

  n = 0;
  matched = 0;

  while (table_dspic[n].name != NULL)
  {
    if (asm_context->cpu_type == CPU_TYPE_PIC24 && table_dspic[n].dspic)
    {
      n++;
      continue;
    }

    if (strcmp(table_dspic[n].name, instr_case) == 0)
    {
      matched = 1;

      switch (table_dspic[n].type)
      {
        case OP_NONE:
          if (operand_count == 0 && flag == FLAG_NONE)
          {
            add_bin32(asm_context, table_dspic[n].opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_F:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_NUM)
          {
            if (check_f(asm_context, operands[0].value) == -1) { return -1; }

            add_bin32(asm_context, table_dspic[n].opcode | operands[0].value, IS_OPCODE);
            return 4;
          }
          break;
        case OP_F_WREG:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_NUM)
          {
            if (check_f_flag(asm_context, operands[0].value, flag) == -1)
            {
              return -1;
            }
            add_bin32(asm_context, table_dspic[n].opcode | (flag == FLAG_B ? (1 << 14) : 0) | (1 << 13) | operands[0].value, IS_OPCODE);
            return 4;
          }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_NUM && operands[1].type == OPTYPE_WREG)
          {
            if (check_f_flag(asm_context, operands[0].value, flag) == -1)
            {
              return -1;
            }
            add_bin32(asm_context, table_dspic[n].opcode | (flag == FLAG_B ? (1 << 14) : 0) | operands[0].value, IS_OPCODE);
            return 4;
          }
          break;
        case OP_F_OR_WREG:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_NUM)
          {
            if (check_f_flag(asm_context, operands[0].value, flag) == -1)
            {
              return -1;
            }
            add_bin32(asm_context, table_dspic[n].opcode | (flag == FLAG_B ? (1 << 14) : 0) | (1 << 13) | operands[0].value, IS_OPCODE);
            return 4;
          }
          if (operand_count == 1 && operands[0].type == OPTYPE_WREG)
          {
            add_bin32(asm_context, table_dspic[n].opcode | (flag == FLAG_B ? (1 << 14) : 0) | operands[0].value, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WREG_F:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_WREG && operands[1].type == OPTYPE_NUM)
          {
            if (check_f_flag(asm_context, operands[0].value, flag )== -1)
            {
              return -1;
            }
            opcode = table_dspic[n].opcode | operands[1].value;
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_ACC:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_ACCUM)
          {
            add_bin32(asm_context, table_dspic[n].opcode | (operands[0].value << 15), IS_OPCODE);
            return 4;
          }
          break;
        case OP_ACC_LIT4_WD:
          if (flag != FLAG_NONE && flag != FLAG_R) { break; }
          opcode = table_dspic[n].opcode | ((flag == FLAG_R) ? (1 << 16) : 0);
          opcode |= (operands[0].value << 15);

          if (operand_count == 2 &&
              operands[0].type == OPTYPE_ACCUM &&
              operands[1].type == OPTYPE_REGISTER)
          {
            opcode |= operands[1].value;
            opcode |= (operands[1].attribute << 4);
            if (operands[1].attribute == 6) { opcode |= (operands[1].reg2 << 11); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          if (operand_count == 3 &&
              operands[0].type == OPTYPE_ACCUM &&
              operands[1].type == OPTYPE_LIT &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (check_range(asm_context, "Literal", operands[1].value, -8, 7) == -1) { return -1; }
            opcode |= (((uint32_t)(operands[1].value & 0xf)) << 7);
            opcode |= operands[2].value;
            opcode |= (operands[2].attribute << 4);
            if (operands[2].attribute == 6) { opcode |= (operands[2].reg2 << 11); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_ACC_LIT6:
          if (flag != FLAG_NONE) { break; }

          if (operand_count == 2 &&
              operands[0].type == OPTYPE_ACCUM &&
              operands[1].type == OPTYPE_LIT)
          {
            if (check_range(asm_context, "Literal", operands[1].value, -16, 16) == -1) { return -1; }
            opcode = table_dspic[n].opcode;
            opcode |= (operands[0].value << 15);
            opcode |= ((uint32_t)(operands[1].value & 0x3f));
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_ACC_WB:
          if (flag != FLAG_NONE) { break; }

          if (operand_count == 2 &&
              operands[0].type == OPTYPE_ACCUM &&
              operands[1].type == OPTYPE_REGISTER)
          {
            opcode = table_dspic[n].opcode;
            opcode |= (operands[0].value << 15);
            opcode |= operands[1].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_BRA:
          if (flag != FLAG_NONE) { break; }

          if (operand_count == 1 && operands[0].type == OPTYPE_NUM)
          {
            int offset=operands[0].value-((asm_context->address/2)+2);
            if ((offset & 1) != 0)
            {
              print_error("Address not on 16 bit boundary", asm_context);
              return -1;
            }
            if (check_range(asm_context, "Offset", offset, -32768*2, 32767*2) == -1) { return -1; }
            offset = offset / 2;
            opcode = table_dspic[n].opcode;
            opcode |= (offset & 0xffff);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_CP0_WS:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute>5) { break; }

            opcode = table_dspic[n].opcode;
            opcode |= operands[0].value;
            opcode |= operands[0].attribute << 4;
            if (flag == FLAG_B) { opcode |= (1 << 10); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_CP_F:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_NUM)
          {
            if (check_f_flag(asm_context, operands[0].value, flag) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[0].value;
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_D_WNS_WND_1:
          if (flag != FLAG_D) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[1].attribute > 5) { break; }
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute == 0 && (operands[1].value & 1) != 0)
            {
              print_error("Illegal register for double", asm_context);
              return -1;
            }
            if ((operands[0].value & 1) != 0)
            {
              print_error("Illegal register for double", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | ((operands[0].value / 2) << 1) |
                   (operands[1].attribute << 11) | (operands[1].value << 7);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_D_WNS_WND_2:
          if (flag != FLAG_D) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute > 5) { break; }
            if (operands[1].attribute != 0) { break; }
            if (operands[0].attribute == 0 && (operands[0].value & 1) != 0)
            {
              print_error("Illegal register for double", asm_context);
              return -1;
            }
            if ((operands[1].value & 1) != 0)
            {
              print_error("Illegal register for double", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | operands[0].value |
                   (operands[0].attribute << 4) |
                  ((operands[1].value / 2) << 8);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_F_BIT4:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_NUM &&
              operands[1].type == OPTYPE_LIT)
          {
            if (check_f_flag(asm_context, operands[0].value, flag) == -1) { return -1; }
            opcode = table_dspic[n].opcode;
            if (flag == FLAG_B)
            {
              if (check_range(asm_context, "Literal", operands[1].value, 0, 7) == -1) { return -1; }
              opcode |= operands[0].value;
              opcode |= operands[1].value << 13;
            }
              else
            {
              if (check_range(asm_context, "Literal", operands[1].value, 0, 15) == -1) { return -1; }
              opcode |= operands[0].value;
              opcode |= (operands[1].value & 0x7) << 13;

              if (operands[1].value > 7) { opcode |= 1; }
            }

            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_F_BIT4_2:
          if (flag == FLAG_NONE) { flag = FLAG_Z; }
          if (flag != FLAG_C && flag != FLAG_Z) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT)
          {
            if (check_range(asm_context, "Literal", operands[1].value, 0, 15) == -1) { return -1; }
            if (operands[0].attribute>5) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[1].value << 12);
            if (flag == FLAG_Z) { opcode |= (1 << 11); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_F_WND:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_NUM &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (check_f_64k(asm_context, operands[0].value) == -1) { return -1; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[1].value |
                   (operands[0].value << 3);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_GOTO:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_NUM)
          {
            if (check_range(asm_context, "Address", operands[0].value, 0, 0x7ffffe) == -1) { return -1; }
            opcode = table_dspic[n].opcode | (operands[0].value & 0xffff);
            add_bin32(asm_context, opcode, IS_OPCODE);
            add_bin32(asm_context, operands[0].value>>16, IS_OPCODE);
            return 8;
          }
          break;
        case OP_LIT1:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_LIT)
          {
            if (check_range(asm_context, "Literal", operands[0].value, 0, 1) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_LIT10_WN:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_LIT &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (check_range(asm_context, "Literal", operands[0].value, 0, 0x3ff) == -1) { return -1; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[1].value;
            opcode |= operands[0].value << 4;
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_LIT14:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_LIT)
          {
            if (check_range(asm_context, "Literal", operands[0].value, 0, 0x3fff) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_LIT14_EXPR:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_LIT &&
              operands[1].type == OPTYPE_NUM)
          {
            int offset=operands[1].value-((asm_context->address/2)+2);
            if (check_range(asm_context, "Literal", operands[0].value, 0, 0x3fff) == -1) { return -1; }
            if ((offset & 1) != 0)
            {
              print_error("Address not on 16 bit boundary", asm_context);
              return -1;
            }
            if (check_range(asm_context, "Offset", offset, -32768*2, 32767*2) == -1) { return -1; }
            offset = offset / 2;
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            add_bin32(asm_context, offset & 0xffff, IS_OPCODE);
            return 8;
          }
          break;
        case OP_LIT16_WND:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_LIT &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (check_range(asm_context, "Literal", operands[0].value, -32768, 0xffff) == -1) { return -1; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode |
                   ((operands[0].value & 0xffff) << 4) |
                     operands[1].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }

          break;
        case OP_LIT8_WND:
          if (flag != FLAG_B) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_LIT &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (check_range(asm_context, "Literal", operands[0].value, -128, 0xff) == -1) { return -1; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | ((operands[0].value & 0xff) << 4) |
                     operands[1].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_LNK_LIT14:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_LIT)
          {
            if (check_range(asm_context, "Literal", operands[0].value, 0, 0x3ffe) == -1) { return -1; }
            if ((operands[0].value & 1) != 0)
            {
              print_error("Address not on 16 bit boundary", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_POP_D_WND:
          if (flag != FLAG_D) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if ((operands[0].value & 1) != 0)
            {
              print_error("Illegal register for double", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | ((operands[0].value / 2) << 8);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_POP_S:
          if (flag != FLAG_S) { break; }
          if (operand_count == 0)
          {
            add_bin32(asm_context, table_dspic[n].opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_POP_WD:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            opcode = table_dspic[n].opcode | (operands[0].value << 7)|
                   (operands[0].attribute << 11) | (operands[0].reg2 << 15);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_PUSH_D_WNS:
          if (flag != FLAG_D) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if ((operands[0].value & 1)!= 0)
            {
              print_error("Illegal register for double", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | ((operands[0].value / 2) << 1);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_SS_WB_WS_WND:
        case OP_SU_WB_WS_WND:
        case OP_US_WB_WS_WND:
        case OP_UU_WB_WS_WND:
          if (flag != FLAG_SS && table_dspic[n].type == OP_SS_WB_WS_WND) { break; }
          if (flag != FLAG_SU && table_dspic[n].type == OP_SU_WB_WS_WND) { break; }
          if (flag != FLAG_US && table_dspic[n].type == OP_US_WB_WS_WND) { break; }
          if (flag != FLAG_UU && table_dspic[n].type == OP_UU_WB_WS_WND) { break; }
          if (operand_count == 3 && operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute > 5) { break; }
            if (operands[2].attribute != 0) { break; }
            if ((operands[2].value & 1) != 0)
            {
              print_error("Illegal register for Wnd", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | (operands[0].value << 11) |
                   operands[1].value | (operands[1].attribute << 4) |
                   (operands[2].value << 7);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_SU_WB_LIT5_WND:
        case OP_UU_WB_LIT5_WND:
          if (flag != FLAG_SU && table_dspic[n].type == OP_SU_WB_LIT5_WND) {break; }
          if (flag != FLAG_UU && table_dspic[n].type == OP_UU_WB_LIT5_WND) {break; }
          if (operand_count == 3 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[2].attribute != 0) { break; }
            if (check_range(asm_context, "Literal", operands[1].value, 0, 31) == -1) { return -1; }
            if ((operands[2].value & 1) != 0)
            {
              print_error("Illegal register for Wnd", asm_context);
              return -1;
            }
            opcode = table_dspic[n].opcode | (operands[0].value << 11) |
                     operands[1].value | (operands[2].value << 7);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_S_WM_WN:
        case OP_U_WM_WN:
          if ((flag != FLAG_U && flag != FLAG_UW && flag != FLAG_UD) &&
               table_dspic[n].type == OP_U_WM_WN) { break; }
          if ((flag != FLAG_S && flag != FLAG_SW && flag != FLAG_SD) &&
               table_dspic[n].type == OP_S_WM_WN) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[1].value |
                   (operands[0].value << 7);
            if (flag == FLAG_UD || flag == FLAG_SD)
            {
              if ((operands[0].value & 1) != 0)
              {
                print_error("Illegal register for double", asm_context);
                return -1;
              }
              opcode |= (1 << 6);
              opcode |= ((operands[0].value + 1) << 11);
            }

            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_LIT4_WND:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 3 && operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[2].attribute != 0) { break; }
            if (check_range(asm_context, "Literal", operands[1].value, 0, 15) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[1].value |
                   (operands[0].value << 11) | (operands[2].value << 7);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_LIT5:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT)
          {
            if (operands[0].attribute != 0) { break; }
            if (check_range(asm_context, "Literal", operands[1].value, 0, 31) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[1].value |
                    (operands[0].value << 11);
            if (flag == FLAG_B) { opcode |= (1 << 10); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_LIT5_WD:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 3 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[2].attribute == 6) { break; }
            if (check_range(asm_context, "Literal", operands[1].value, 0, 31) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[1].value |
                    (operands[0].value << 15)|(operands[2].value << 7) |
                    (operands[2].attribute << 11);
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_WN:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 && operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[1].value |
                    (operands[0].value << 11);
            if (flag == FLAG_B) { opcode |= (1 << 10); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_WNS_WND:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 3 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute != 0) { break; }
            if (operands[2].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[1].value |
                    (operands[0].value << 11) | (operands[2].value << 7);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_WS:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute > 5) { break; }
            opcode = table_dspic[n].opcode | operands[1].value |
                   (operands[0].value << 11) | (operands[1].attribute << 4);
            if (flag == FLAG_B) { opcode |= (1 << 10); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WB_WS_WD:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 3 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER &&
              operands[2].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute > 5) { break; }
            if (operands[2].attribute > 5) { break; }
            opcode = table_dspic[n].opcode | operands[1].value|
                    (operands[0].value << 15) | (operands[1].attribute << 4) |
                    (operands[2].value << 7) | (operands[2].attribute << 11);
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WD:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute > 5) { break; }
            opcode = table_dspic[n].opcode | (operands[0].value << 7) |
                    (operands[0].attribute << 11);
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WN:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_B_WN:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value;
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_DAW_B_WN:
          if (flag != FLAG_B) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WN_EXPR:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_NUM)
          {
            if (operands[0].attribute != 0) { break; }
            int offset=operands[1].value-((asm_context->address/2)+2);
            if ((offset & 1) != 0)
            {
              print_error("Address not on 16 bit boundary", asm_context);
              return -1;
            }
            if (check_range(asm_context, "Offset", offset, -32768*2, 32767*2) == -1) { return -1; }
            offset = offset / 2;
            opcode = table_dspic[n].opcode | operands[0].value;
            add_bin32(asm_context, opcode, IS_OPCODE);
            add_bin32(asm_context, offset & 0xffff, IS_OPCODE);
            return 8;
          }
          break;
        case OP_WNS_F:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_NUM)
          {
            if (operands[0].attribute != 0) { break; }
            if (check_f_64k(asm_context, operands[1].value) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[0].value |
                   ((operands[1].value >> 1) << 4);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WNS_WD_LIT10:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_W_PLUS_LIT)
          {
            if (operands[0].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                   (operands[1].value << 7);
            num = operands[1].attribute;
            if (flag == FLAG_B)
            {
              if (check_range(asm_context, "Literal", num, -512, 511) == -1) { return -1; }
              opcode |= (1 << 14);
            }
              else
            {
              if (check_range(asm_context, "Literal", num, -1024, 1022) == -1) { return -1; }
              if ((num & 1) == 1)
              {
                print_error("Literal not an even number", asm_context);
                return -1;
              }
              num = num / 2;
            }
            opcode |= (num & 0x7) << 4;
            opcode |= ((num >> 3) & 0x7) << 11;
            opcode |= ((num >> 6) & 0xf) << 15;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WNS_WND:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[1].value << 7);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_BIT4:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT)
          {
            if (operands[0].attribute > 5) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[1].value << 12);
            if (flag == FLAG_B)
            {
              opcode |= (1 << 10);
              if (check_range(asm_context, "Literal", operands[1].value, 0, 7) == -1) { return -1; }
            }
              else
            {
              if (check_range(asm_context, "Literal", operands[1].value, 0, 15) == -1) { return -1; }
            }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_BIT4_2:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT)
          {
            if (operands[0].attribute>5) { break; }
            if (check_range(asm_context, "Literal", operands[1].value, 0, 15) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[1].value << 12);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_LIT10_WND:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_W_PLUS_LIT &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[1].value << 7);
            num = operands[0].attribute;
            if (flag == FLAG_B)
            {
              if (check_range(asm_context, "Literal", num, -512, 511) == -1) { return -1; }
              opcode |= (1 << 14);
            }
              else
            {
              if (check_range(asm_context, "Literal", num, -1024, 1022) == -1) { return -1; }
              if ((num & 1) == 1)
              {
                print_error("Literal not an even number", asm_context);
                return -1;
              }
              num = num / 2;
            }
            opcode |= (num & 0x7) << 4;
            opcode |= ((num >> 3) & 0x7) << 11;
            opcode |= ((num >> 6) & 0xf) << 15;
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_LIT4_ACC:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 3 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_LIT &&
              operands[2].type == OPTYPE_ACCUM)
          {
            if (check_range(asm_context, "Literal", operands[1].value, -8, 7) == -1) { return -1; }
            opcode = table_dspic[n].opcode | operands[0].value |
                   (operands[0].attribute << 4) |
                  ((operands[1].value & 0xf) << 7) |
                   (operands[0].reg2 << 11)|
                   (operands[2].value << 15);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_ACCUM)
          {
            opcode = table_dspic[n].opcode | operands[0].value |
                   (operands[0].attribute << 4)|
                   (operands[0].reg2 << 11) | (operands[1].value << 15);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_PLUS_WB:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 1 && operands[0].type == OPTYPE_REGISTER)
          {
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[0].reg2 << 15);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_WB:
          if (flag == FLAG_NONE) { flag = FLAG_Z; }
          if (flag != FLAG_C && flag != FLAG_Z) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute == 6) { break; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[1].value << 11);
            if (flag == FLAG_Z) { opcode |= (1 << 15); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_WB_WD_WB:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute == 6 && operands[1].attribute == 6 &&
                operands[0].reg2 != operands[1].reg2)
            {
              print_error("Source and destination Wb don't match", asm_context);
              return -1;
            }

            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[0].reg2 << 15) |
                    (operands[1].value << 7) | (operands[1].attribute << 11);
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_WD:
          if (flag != FLAG_NONE && flag != FLAG_B && flag != FLAG_W) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute == 6) { break; }
            if (operands[1].attribute == 6) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) |
                    (operands[1].value << 7) | (operands[1].attribute << 11);
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WS_WND:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute == 6) { break; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[0].value |
                    (operands[0].attribute << 4) | (operands[1].value << 7);
            if (flag == FLAG_B) { opcode |= (1 << 14); }
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_WM_WN:
          if (flag != FLAG_NONE) { break; }
          if (operand_count == 2 &&
              operands[0].type == OPTYPE_REGISTER &&
              operands[1].type == OPTYPE_REGISTER)
          {
            if (operands[0].attribute != 0) { break; }
            if (operands[1].attribute != 0) { break; }
            opcode = table_dspic[n].opcode | operands[1].value |
                    (operands[0].value << 11);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        case OP_A_WX_WY_AWB:
          if (flag != FLAG_NONE) { break; }
          opcode = parse_movsac(asm_context, operands, operand_count, table_dspic[n].opcode);
          if (opcode == -1) { break; }
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
          break;
        case OP_N_WM_WN_ACC_WX_WY:
          if (flag != FLAG_N) { break; }
          opcode = parse_mpy_n(asm_context, operands, operand_count, table_dspic[n].opcode);
          if (opcode == -1) { break; }
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
          break;
        case OP_WM_WM_ACC_WX_WY:
          if (flag != FLAG_NONE) { break; }
          opcode = parse_mac_m_m(asm_context, operands, operand_count, table_dspic[n].opcode);
          if (opcode == -1) { break; }
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
          break;
        case OP_WM_WN_ACC_WX_WY:
          if (flag != FLAG_NONE) { break; }
          opcode = parse_mpy_n(asm_context, operands, operand_count, table_dspic[n].opcode);
          if (opcode == -1) { break; }
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
          break;
        case OP_WM_WM_ACC_WX_WY_WXD:
          if (flag != FLAG_NONE) { break; }
          opcode = parse_ed_m_m(asm_context, operands, operand_count, table_dspic[n].opcode);
          if (opcode == -1) { break; }
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
          break;
        case OP_WM_WN_ACC_WX_WY_AWB:
          if (flag != FLAG_NONE) { break; }
          opcode = parse_mac_m_n(asm_context, operands, operand_count, table_dspic[n].opcode);
          if (opcode == -1) { break; }
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
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

  asm_context->error = 1;

  return 4;
}

