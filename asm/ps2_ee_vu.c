/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/ps2_ee_vu.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/ps2_ee_vu.h"

#define MAX_OPERANDS 3

enum
{
  OPERAND_VREG,
  OPERAND_I,
  OPERAND_Q,
  OPERAND_IMMEDIATE,
};

enum
{
  COMPONENT_X = 8,
  COMPONENT_Y = 4,
  COMPONENT_Z = 2,
  COMPONENT_W = 1,
};

struct _operand
{
  int value;
  int type;
  int component_mask;
};

static int get_register_ps2_ee_vu(char *token, int *type, int *value, int *component_mask)
{
  int ptr = 3;

  if (token[0] != '$') { return -1; }
  if (token[1] != 'v') { return -1; }
  if (token[2] != 'f') { return -1; }

  *type = OPERAND_VREG;
  *value = 0;

  while(1)
  {
    if (token[ptr] < '0' || token[ptr] > '9') { break; }
    *value = ((*value) * 10) + (token[ptr] - '0');
    ptr++;
  }

  if (token[ptr] == 2) { return -1; }

  while(token[ptr] != 0)
  {
    char c = tolower(token[ptr]);
    if (c == 'x') { *component_mask |= COMPONENT_X; }
    else if (c == 'y') { *component_mask |= COMPONENT_Y; }
    else if (c == 'z') { *component_mask |= COMPONENT_Z; }
    else if (c == 'w') { *component_mask |= COMPONENT_W; }
    else
    {
      return -1;
    }
    ptr++;
  }

  return 0;
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case, int *dest)
{
  int operand_count = 0;
  int n;
  int token_type;
  char token[TOKENLEN];

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    //printf("token=%s token_type=%d\n", token, token_type);

    if (operand_count == MAX_OPERANDS)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (operand_count == 0 && IS_TOKEN(token,'.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = 0;
      while(token[n] != 0)
      {
        char c = tolower(token[n]);
        if (c == 'x') { *dest |= COMPONENT_X; }
        else if (c == 'y') { *dest |= COMPONENT_Y; }
        else if (c == 'z') { *dest |= COMPONENT_Z; }
        else if (c == 'w') { *dest |= COMPONENT_W; }
        else
        {
          printf("Error: Unknown component '%c' at %s:%d\n", token[n], asm_context->filename, asm_context->line);
          return -1;
        }

        n++;
      }

      continue;
    }

    if (token[0] == '$')
    {
      if (get_register_ps2_ee_vu(token,
                                &operands[operand_count].type,
                                &operands[operand_count].value,
                                &operands[operand_count].component_mask) == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
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
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return operand_count;
}

int parse_instruction_ps2_ee_vu(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  uint32_t opcode = 0;
  char instr_case[TOKENLEN];
  int dest = 0;
  int n, r;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  operand_count = get_operands(asm_context, operands, instr, instr_case, &dest);

  if (operand_count < 0) { return -1; }

#ifdef DEBUG
  printf("operand_count=%d  dest=%d\n", operand_count, dest);
  for (n = 0; n < operand_count; n++)
  {
    printf("  type=%d value=%d component_mask=%d\n",
      operands[n].type, operands[n].value, operands[n].component_mask);
  }
#endif

  n = 0;
  while(table_ps2_ee_vu[n].instr != NULL)
  {
    if (strcmp(instr_case, table_ps2_ee_vu[n].instr) == 0)
    {
      if (operand_count != table_ps2_ee_vu[n].operand_count)
      {
        n++;
        continue;
      }

      opcode = table_ps2_ee_vu[n].opcode;

      for (r = 0; r < table_ps2_ee_vu[n].operand_count; r++)
      {
        switch(table_ps2_ee_vu[n].operand[r])
        {
          case EE_VU_OP_FT:
            opcode |= (operands[r].value << 16);
            break;
          case EE_VU_OP_FS:
            opcode |= (operands[r].value << 11);
            break;
          case EE_VU_OP_FD:
            opcode |= (operands[r].value << 6);
            break;
          default:
            print_error_illegal_operands(instr, asm_context);
            return -1;
        }
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
    n++;
  }

  return -1;
}


