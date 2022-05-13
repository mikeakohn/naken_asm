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
#include <ctype.h>

#include "asm/common.h"
#include "asm/ps2_ee_vu.h"
#include "disasm/ps2_ee_vu.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/ps2_ee_vu.h"

#define MAX_OPERANDS 3
#define OFFSET_MIN (-(1 << 10))
#define OFFSET_MAX ((1 << 10) - 1)

enum
{
  OPERAND_VFREG,
  OPERAND_VIREG,
  OPERAND_I,
  OPERAND_Q,
  OPERAND_P,
  OPERAND_R,
  OPERAND_ACC,
  OPERAND_NUMBER,
  OPERAND_OFFSET_BASE,
  OPERAND_BASE,
  OPERAND_BASE_DEC,
  OPERAND_BASE_INC,
};

enum
{
  FIELD_X = 8,
  FIELD_Y = 4,
  FIELD_Z = 2,
  FIELD_W = 1,
};

struct _operand
{
  int value;
  int type;
  int field_mask;
  int base_reg;
};

static int get_register_ps2_ee_vu(
  char *token,
  int *type,
  int *value,
  int *field_mask)
{
  int ptr = 2;

  if (token[0] == '$') { token++; }
  if (token[0] != 'v') { return -1; }

  if (token[1] == 'f')
  {
    *type = OPERAND_VFREG;
  }
    else
  if (token[1] == 'i')
  {
    *type = OPERAND_VIREG;
  }
    else
  {
    return -1;
  }

  *value = 0;

  while (1)
  {
    if (token[ptr] < '0' || token[ptr] > '9') { break; }
    *value = ((*value) * 10) + (token[ptr] - '0');
    ptr++;
  }

  if (ptr == 2) { return -1; }

  while (token[ptr] != 0)
  {
    char c = tolower(token[ptr]);
    if (c == 'x') { *field_mask |= FIELD_X; }
    else if (c == 'y') { *field_mask |= FIELD_Y; }
    else if (c == 'z') { *field_mask |= FIELD_Z; }
    else if (c == 'w') { *field_mask |= FIELD_W; }
    else
    {
      return -1;
    }
    ptr++;
  }

  return 0;
}

static int get_field_number(int field_mask)
{
  uint8_t value[16] =
  {
    -1,  3,  2, -1,  // 0
     1, -1, -1, -1,  // 4
     0, -1, -1, -1,  // 8
    -1, -1, -1, -1,  // 12
  };

  return value[field_mask];
}

static int get_field_bits(
  struct _asm_context *asm_context,
  char *token,
  int *dest)
{
  int n;

  for (n = 0; token[n] != 0; n++)
  {
    char c = tolower(token[n]);

    if (c == 'x') { *dest |= FIELD_X; }
    else if (c == 'y') { *dest |= FIELD_Y; }
    else if (c == 'z') { *dest |= FIELD_Z; }
    else if (c == 'w') { *dest |= FIELD_W; }
    else
    {
      printf("Error: Unknown component '%c' at %s:%d\n",
        token[n],
        asm_context->tokens.filename,
        asm_context->tokens.line);

      return -1;
    }
  }

  return 0;
}

int get_base(
  struct _asm_context *asm_context,
  struct _operand *operand,
  int *modifier)
{
  int type, n;
  int token_type;
  char token[TOKENLEN];

  *modifier = 0;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token, '-'))
  {
    *modifier = -1;
    if (expect_token(asm_context, '-') == -1) { return -1; }
    token_type = tokens_get(asm_context, token, TOKENLEN);
  }

  n = get_register_ps2_ee_vu(token, &type,
                             &operand->base_reg,
                             &operand->field_mask);

  if (n != 0 || type != OPERAND_VIREG)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (*modifier == 0)
  {
    if (IS_TOKEN(token, '+'))
    {
      *modifier = 1;
      if (expect_token(asm_context, '+') == -1) { return -1; }
      token_type = tokens_get(asm_context, token, TOKENLEN);
    }
  }

  if (IS_NOT_TOKEN(token, ')'))
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (token_type == TOKEN_EOL ||
      token_type == TOKEN_EOF ||
      IS_TOKEN(token, ','))
  {
    tokens_push(asm_context, token, token_type);
  }
    else
  {
    if (get_field_bits(asm_context,
                       token,
                       &operand->field_mask) == -1)
    {
      return -1;
    }

    if (get_field_number(operand->field_mask == -1))
    {
      printf("Error: Only 1 dest field allowed at %s:%d\n",
        asm_context->tokens.filename,
        asm_context->tokens.line);
      return -1;
    }
  }

  return 0;
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case, int *dest, int *iemdt_bits, int is_lower)
{
  int operand_count = 0;
  int modifier;
  int n;
  int token_type;
  char token[TOKENLEN];

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    //printf("token=%s token_type=%d\n", token, token_type);

    if (operand_count == MAX_OPERANDS)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (operand_count == 0 && IS_TOKEN(token, '.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (get_field_bits(asm_context, token, dest) == -1) { return -1; }

      continue;
    }

    if (operand_count == 0 && IS_TOKEN(token, '['))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = 0;
      while (token[n] != 0)
      {
        char c = tolower(token[n]);
        if (c == 'i') { *iemdt_bits |= 16; }
        else if (c == 'e') { *iemdt_bits |= 8; }
        else if (c == 'm') { *iemdt_bits |= 4; }
        else if (c == 'd') { *iemdt_bits |= 2; }
        else if (c == 't') { *iemdt_bits |= 1; }
        else
        {
          printf("Error: Unknown flag '%c' at %s:%d\n", token[n], asm_context->tokens.filename, asm_context->tokens.line);
          return -1;
        }

        n++;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_NOT_TOKEN(token, ']'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      continue;
    }

    n = get_register_ps2_ee_vu(token,
                               &operands[operand_count].type,
                               &operands[operand_count].value,
                               &operands[operand_count].field_mask);

    if (n != -1)
    {
    }
      else
    if (IS_TOKEN(token, 'I') || IS_TOKEN(token, 'i'))
    {
      operands[operand_count].type = OPERAND_I;
    }
      else
    if (IS_TOKEN(token, 'Q') || IS_TOKEN(token, 'q'))
    {
      operands[operand_count].type = OPERAND_Q;
    }
      else
    if (IS_TOKEN(token, 'P') || IS_TOKEN(token, 'p'))
    {
      operands[operand_count].type = OPERAND_P;
    }
      else
    if (IS_TOKEN(token, 'R') || IS_TOKEN(token, 'r'))
    {
      operands[operand_count].type = OPERAND_R;
    }
      else
    if (strcasecmp(token, "acc") == 0)
    {
      operands[operand_count].type = OPERAND_ACC;
    }
      else
    if (IS_TOKEN(token, '('))
    {
      int type;
      int base_reg;
      int field_mask;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = get_register_ps2_ee_vu(token, &type, &base_reg, &field_mask);

      operands[operand_count].type = OPERAND_BASE;

      if (n == -1 && IS_NOT_TOKEN(token, '-'))
      {
        tokens_push(asm_context, token, token_type);
        tokens_push(asm_context, "(", TOKEN_SYMBOL);

        if (asm_context->pass == 1)
        {
          if (ignore_paren_expression(asm_context) == -1)
          {
            return -1;
          }

          operands[operand_count].value = 0;
        }
          else
        {
          if (eval_expression(asm_context, &operands[operand_count].value) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_NOT_TOKEN(token, '('))
        {
          tokens_push(asm_context, token, token_type);
          operands[operand_count].type = OPERAND_NUMBER;
        }
          else
        {
          operands[operand_count].type = OPERAND_OFFSET_BASE;
        }
      }
        else
      {
        // FIXME - So gross :(
        tokens_push(asm_context, token, token_type);
        strcpy(token, "(");
      }

      if (IS_TOKEN(token, '('))
      {
        if (get_base(asm_context, &operands[operand_count], &modifier) == -1)
        {
          return -1;
        }

        if (modifier == -1) { operands[operand_count].type = OPERAND_BASE_DEC; }
        else if (modifier == 1) { operands[operand_count].type = OPERAND_BASE_INC; }
      }
    }
      else
    {
      if (is_lower == 1)
      {
        operands[operand_count].type = OPERAND_NUMBER;

        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
        }
          else
        {
          tokens_push(asm_context, token, token_type);

          if (eval_expression(asm_context, &operands[operand_count].value) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          // Check for base/offset
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_TOKEN(token, '('))
          {
            if (get_base(asm_context,
                         &operands[operand_count],
                         &modifier) == -1)
            {
              return -1;
            }

            if (modifier != 0)
            {
              printf("Error: Instruction cannot have modifier at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
            }

            operands[operand_count].type = OPERAND_OFFSET_BASE;
          }
            else
          {
            tokens_push(asm_context, token, token_type);
          }
        }
      }
        else
      {
        if (operand_count == 0)
        {
          tokens_push(asm_context, token, token_type);
          break;
        }

        print_error_unexp(token, asm_context);
        return -1;
      }
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (is_lower == 0)
    {
      if (IS_NOT_TOKEN(token,','))
      {
        tokens_push(asm_context, token, token_type);
        break;
      }
    }
      else
    {
      if (token_type == TOKEN_EOL) { break; }
      if (IS_NOT_TOKEN(token,','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
  }

  return operand_count;
}

static int get_opcode(struct _asm_context *asm_context, struct _table_ps2_ee_vu *table_ps2_ee_vu, char *instr, int is_lower)
{
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  uint32_t opcode = 0;
  int offset;
  char instr_case[TOKENLEN];
  int dest = 0;
  int iemdt_bits = 0;
  int wrong_operand_count = 0;
  int n, r;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  operand_count = get_operands(asm_context, operands, instr, instr_case, &dest, &iemdt_bits, is_lower);

  if (operand_count < 0) { return -1; }

#ifdef DEBUG
  printf("operand_count=%d  dest=%d\n", operand_count, dest);
  for (n = 0; n < operand_count; n++)
  {
    printf("  type=%d value=%d field_mask=%d\n",
      operands[n].type, operands[n].value, operands[n].field_mask);
  }
#endif

  for (n = 0; table_ps2_ee_vu[n].instr != NULL; n++)
  {
    if (strcmp(instr_case, table_ps2_ee_vu[n].instr) == 0)
    {
      if (asm_context->flags == PS2_EE_VU0 &&
         (table_ps2_ee_vu[n].flags & FLAG_VU1_ONLY))
      {
        printf("Error: Instruction only valid in VU1 at %s:%d\n",
               asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

      if (operand_count != table_ps2_ee_vu[n].operand_count)
      {
        // REVIEW: Does this chip have instructions with the same name
        //         different operand count?
        wrong_operand_count = 1;
        continue;
      }

      if ((table_ps2_ee_vu[n].flags & FLAG_XYZ) != 0 && dest != 0xe)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      if ((table_ps2_ee_vu[n].flags & FLAG_DEST) == 0 && dest != 0x00)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      opcode = table_ps2_ee_vu[n].opcode;

      if (asm_context->pass == 1) { return opcode; }

      for (r = 0; r < table_ps2_ee_vu[n].operand_count; r++)
      {
        switch (table_ps2_ee_vu[n].operand[r])
        {
          case EE_VU_OP_FT:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= (operands[r].value << 16);

            if ((table_ps2_ee_vu[n].flags & FLAG_TE) != 0)
            {
              int field = get_field_number(operands[r].field_mask);
              if (field == -1) { return -1; }
              opcode |= field << 23;
            }
            break;
          case EE_VU_OP_FS:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= (operands[r].value << 11);

            if ((table_ps2_ee_vu[n].flags & FLAG_SE) != 0)
            {
              int field = get_field_number(operands[r].field_mask);
              if (field == -1) { return -1; }
              opcode |= field << 21;
            }
            break;
          case EE_VU_OP_FD:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            opcode |= (operands[r].value << 6);
            break;
          case EE_VU_OP_VIT:
            if (operands[r].type != OPERAND_VIREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            opcode |= (operands[r].value << 16);
            break;
          case EE_VU_OP_VIS:
            if (operands[r].type != OPERAND_VIREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            opcode |= (operands[r].value << 11);
            break;
          case EE_VU_OP_VID:
            if (operands[r].type != OPERAND_VIREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            opcode |= (operands[r].value << 6);
            break;
          case EE_VU_OP_VI01:
            if (operands[r].type != OPERAND_VIREG || operands[r].value != 1)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            break;
          case EE_VU_OP_I:
            if (operands[r].type != OPERAND_I)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            break;
          case EE_VU_OP_Q:
            if (operands[r].type != OPERAND_Q)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            break;
          case EE_VU_OP_P:
            if (operands[r].type != OPERAND_P)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            break;
          case EE_VU_OP_R:
            if (operands[r].type != OPERAND_R)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            break;
          case EE_VU_OP_ACC:
            if (operands[r].type != OPERAND_ACC)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            break;
          case EE_VU_OP_OFFSET:
            if (operands[r].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if ((operands[r].value & 0x7) != 0)
            {
              print_error_align(asm_context, 8);
              return -1;
            }

            offset = operands[r].value - (asm_context->address + 8);
            offset >>= 3;

            if (offset < OFFSET_MIN || offset > OFFSET_MAX)
            {
              print_error_range("Address", OFFSET_MIN, OFFSET_MAX, asm_context);
              return -1;
            }

            opcode |= offset & 0x7ff;

            break;
          case EE_VU_OP_OFFSET_BASE:
            if (operands[r].type != OPERAND_OFFSET_BASE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (get_field_number(dest) == -1)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            offset = operands[r].value;

            if (offset < -0x400 || offset > 0x3ff)
            {
              print_error_range("Address", -0x400, 0x3ff, asm_context);
              return -1;
            }

            if (table_ps2_ee_vu[n].operand[0] == EE_VU_OP_FS)
            {
              opcode |= operands[r].base_reg << 16;
            }
              else
            {
              opcode |= operands[r].base_reg << 11;
            }
            opcode |= offset & 0x7ff;

            break;
          case EE_VU_OP_BASE:
            if (operands[r].type != OPERAND_BASE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (table_ps2_ee_vu[n].operand[0] == EE_VU_OP_FS)
            {
              opcode |= operands[r].base_reg << 16;
            }
              else
            {
              opcode |= operands[r].base_reg << 11;
            }

            break;
          case EE_VU_OP_BASE_DEC:
            if (operands[r].type != OPERAND_BASE_DEC)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (table_ps2_ee_vu[n].operand[0] == EE_VU_OP_FS)
            {
              opcode |= operands[r].base_reg << 16;
            }
              else
            {
              opcode |= operands[r].base_reg << 11;
            }

            break;
          case EE_VU_OP_BASE_INC:
            if (operands[r].type != OPERAND_BASE_INC)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (table_ps2_ee_vu[n].operand[0] == EE_VU_OP_FS)
            {
              opcode |= operands[r].base_reg << 16;
            }
              else
            {
              opcode |= operands[r].base_reg << 11;
            }

            break;
          case EE_VU_OP_IMMEDIATE24:
            if (operands[r].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 0xffffff)
            {
              print_error_range("Immediate", 0, 0xffffff, asm_context);
              return -1;
            }

            opcode |= operands[r].value & 0xffffff;

            break;
          case EE_VU_OP_IMMEDIATE15:
            if (operands[r].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 0x7fff)
            {
              print_error_range("Immediate", 0, 0x7fff, asm_context);
              return -1;
            }

            opcode |= (operands[r].value & 0x7800) << 10;
            opcode |= operands[r].value & 0x7ff;

            break;
          case EE_VU_OP_IMMEDIATE12:
            if (operands[r].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 0xfff)
            {
              print_error_range("Immediate", 0, 0xfff, asm_context);
              return -1;
            }

            opcode |= (operands[r].value & 0x800) << 10;
            opcode |= operands[r].value & 0x7ff;

            break;
          case EE_VU_OP_IMMEDIATE5:
            if (operands[r].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < -16 || operands[r].value > 15)
            {
              print_error_range("Immediate", -16, 15, asm_context);
              return -1;
            }

            opcode |= (operands[r].value & 0x1f) << 6;

            break;
          default:
            print_error_illegal_operands(instr, asm_context);
            return -1;
        }
      }

      if (is_lower == 1 && iemdt_bits != 0)
      {
        printf("Error: Cannot set IEMDT bits in lower instruction at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

      return opcode | (iemdt_bits << 27) | (dest << 21);
    }
  }

  if (wrong_operand_count == 0)
  {
    printf("Error: Unknown %s instruction '%s' at %s:%d\n",
           is_lower ? "lower" : "upper",
           instr,
           asm_context->tokens.filename,
           asm_context->tokens.line);
  }
    else
  {
    printf("Error: Wrong operand count for %s instruction '%s' at %s:%d\n",
           is_lower ? "lower" : "upper",
           instr,
           asm_context->tokens.filename,
           asm_context->tokens.line);
  }

  return -1;
}

int parse_instruction_ps2_ee_vu(struct _asm_context *asm_context, char *instr)
{
  uint32_t opcode_upper;
  uint32_t opcode_lower;
  int token_type;
  char token[TOKENLEN];

  opcode_upper = get_opcode(asm_context, table_ps2_ee_vu_upper, instr, 0);

  if (opcode_upper == -1) { return -1; }

  // Get lower instruction
  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_STRING)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  opcode_lower = get_opcode(asm_context, table_ps2_ee_vu_lower, token, 1);

  if (opcode_lower == -1) { return -1; }

  add_bin32(asm_context, opcode_lower, IS_OPCODE);
  add_bin32(asm_context, opcode_upper, IS_OPCODE);

  return 8;
}

