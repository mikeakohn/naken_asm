/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "asm/8051.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/8051.h"

enum
{
  OPERAND_REG,
  OPERAND_AT_REG,
  OPERAND_A,
  OPERAND_C,
  OPERAND_AB,
  OPERAND_DPTR,
  OPERAND_AT_A_PLUS_DPTR,
  OPERAND_AT_A_PLUS_PC,
  OPERAND_AT_DPTR,
  OPERAND_DATA,
  OPERAND_NUM,
  OPERAND_SLASH_BIT_ADDRESS,
  OPERAND_BIT_ADDRESS,
};

struct _operand
{
  int value;
  int type;
};

struct _address_map
{
  const char *name;
  uint8_t address;
  uint8_t is_bit_addressable;
};

static struct _address_map address_map[] =
{
  { "B", 0xf0, 1 },
  { "ACC", 0xe0, 1 },
  { "PSW", 0xd0, 1 },
  { "T2CON", 0xc8, 0 },
  { "T2MOD", 0xc9 , 0},
  { "RCAP2L", 0xca, 0 },
  { "RCAP2H", 0xcb, 0 },
  { "TL2", 0xcc, 0 },
  { "TH2", 0xcd, 0 },
  { "IP", 0xb8, 1 },
  { "P3", 0xb0, 1 },
  { "IE", 0xa8, 1 },
  { "P2", 0xa0, 1 },
  { "AUXR1", 0xa2, 0 },
  { "WDTRST", 0xa6, 0 },
  { "SCON", 0x98, 1 },
  { "SBUF", 0x99, 0 },
  { "P1", 0x90, 1 },
  { "TCON", 0x88, 1 },
  { "TMOD", 0x89, 0 },
  { "TL0", 0x8a, 0 },
  { "TL1", 0x8b, 0 },
  { "TH0", 0x8c, 0 },
  { "TH1", 0x8d, 0 },
  { "AUXR", 0x8e, 0 },
  { "P0", 0x80, 1 },
  { "SP", 0x81, 0 },
  { "DPL", 0x82, 0 },
  { "DPH", 0x83, 0 },
  { "DP0L", 0x82, 0 },
  { "DP0H", 0x83, 0 },
  { "DP1L", 0x84, 0 },
  { "DP1H", 0x85, 0 },
  { "PCON", 0x87, 0 },
};

static struct _address_map address_map_psw[] =
{
  { "CY", 0xd7 },
  { "AC", 0xd6 },
  { "F0", 0xd5 },
  { "RS1", 0xd4 },
  { "RS0", 0xd3 },
  { "OV", 0xd2 },
  { "UD", 0xd1 },
  { "P", 0xd0 },
};

static int get_register_8051(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  if (token[1] >= '0' && token[1] <= '7' && token[2] == 0)
  {
    return token[1] - '0';
  }

  return -1;
}

static int get_bit_address(struct _asm_context *asm_context, int *num, uint8_t *is_bit_address)
{
  char token[TOKENLEN];
  int token_type;

  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_NUMBER)
  {
    tokens_push(asm_context, token, token_type);
    return 0;
  }

  int bit = atoi(token);
  if (bit < 0 || bit > 7)
  {
    tokens_push(asm_context, token, token_type);
    return 0;
  }

  *num += bit;
  *is_bit_address = 1;

  return 0;
}

static int get_bit_address_alias(const char *token)
{
  int n;

  for (n = 0; n < sizeof(address_map_psw) / sizeof(struct _address_map); n++)
  {
    if (strcasecmp(token, address_map_psw[n].name) == 0)
    {
      return address_map_psw[n].address;
    }
  }

  return -1;
}

static int get_address(struct _asm_context *asm_context, int *num, uint8_t *is_bit_address)
{
  char token[TOKENLEN];
  int token_type;
  int n;

  *is_bit_address = 0;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  for (n = 0; n < sizeof(address_map) / sizeof(struct _address_map); n++)
  {
    if (strcasecmp(token, address_map[n].name) == 0)
    {
      *num = address_map[n].address;

      if (address_map[n].is_bit_addressable == 1)
      {
        char token[TOKENLEN];
        int token_type;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token, '.'))
        {
          tokens_push(asm_context, token, token_type);
          return 0;
        }

        return get_bit_address(asm_context, num, is_bit_address);
      }

      return 0;
    }
  }

  *num = get_bit_address_alias(token);

  if (*num != -1)
  {
    *is_bit_address = 1;
    return 0;
  }

  tokens_push(asm_context, token, token_type);

  if (eval_expression(asm_context, num) != 0)
  {
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (IS_NOT_TOKEN(token, '.'))
  {
    tokens_push(asm_context, token, token_type);
    return 0;
  }

  return get_bit_address(asm_context, num, is_bit_address);
}

int parse_instruction_8051(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[3];
  uint8_t is_bit_address;
  int operand_count = 0;
  int token_type;
  int matched = 0;
  int num, n, r;
  int count = 1;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    num = get_register_8051(token);

    if (num != -1)
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = num;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token,'A') || IS_TOKEN(token,'a')))
    {
      operands[operand_count].type = OPERAND_A;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token,'C') || IS_TOKEN(token,'c')))
    {
      operands[operand_count].type = OPERAND_C;
    }
      else
    if (strcasecmp(token, "ab") == 0)
    {
      operands[operand_count].type = OPERAND_AB;
    }
      else
    if (strcasecmp(token, "dptr") == 0)
    {
      operands[operand_count].type = OPERAND_DPTR;
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      num = get_register_8051(token);
      if (num != -1)
      {
        operands[operand_count].type = OPERAND_AT_REG;
        operands[operand_count].value = num;
      }
        else
      if (strcasecmp(token, "dptr") == 0)
      {
        operands[operand_count].type = OPERAND_AT_DPTR;
      }
        else
      if (token_type == TOKEN_STRING && strcasecmp(token, "a") == 0)
      {
        do
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,'+')) break;
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (strcasecmp(token, "dptr") == 0)
          {
            operands[operand_count].type = OPERAND_AT_A_PLUS_DPTR;
          }
            else
          if (strcasecmp(token, "pc") == 0)
          {
            operands[operand_count].type = OPERAND_AT_A_PLUS_PC;
          }
        } while(0);

        if (operands[operand_count].type == 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
    }
      else
    if (token_type == TOKEN_POUND)
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          eat_operand(asm_context);
          num = 0;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_DATA;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token,'/'))
    {
      if (get_address(asm_context, &num, &is_bit_address) == -1 ||
          is_bit_address == 0)
      {
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_SLASH_BIT_ADDRESS;
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (asm_context->pass == 1)
      {
        operands[operand_count].type = OPERAND_NUM;
        operands[operand_count].value = 0;

        // Ignore tokens for this operand unless it's a . or a flag.
        while(1)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, ',') ||
              token_type == TOKEN_EOL ||
              token_type == TOKEN_EOF)
          {
            break;
          }
            else
          if (IS_TOKEN(token, '.') || get_bit_address_alias(token) != -1)
          {
            operands[operand_count].type = OPERAND_BIT_ADDRESS;
          }
        }

        tokens_push(asm_context, token, token_type);
      }
        else
      {
        if (get_address(asm_context, &num, &is_bit_address) == -1)
        {
          return -1;
        }

        if (is_bit_address == 0)
        {
          operands[operand_count].type = OPERAND_NUM;
        }
          else
        {
          operands[operand_count].type = OPERAND_BIT_ADDRESS;
        }

        operands[operand_count].value = num;
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
printf("-----\n");
for (n = 0; n < operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  for (n = 0; n < 256; n++)
  {
    if (strcmp(table_8051[n].name, instr_case) == 0)
    {
      matched = 1;
      for(r = 0; r < 3; r++)
      {
        if (table_8051[n].op[r] == OP_NONE) { break; }

        switch(table_8051[n].op[r])
        {
          case OP_REG:
            if (operands[r].type != OPERAND_REG ||
                operands[r].value != table_8051[n].range) { r = 4; }
            break;
          case OP_AT_REG:
            if (operands[r].type != OPERAND_AT_REG ||
                operands[r].value != table_8051[n].range) { r = 4; }
            break;
          case OP_A:
            if (operands[r].type != OPERAND_A) { r = 4; }
            break;
          case OP_C:
            if (operands[r].type != OPERAND_C) { r = 4; }
            break;
          case OP_AB:
            if (operands[r].type != OPERAND_AB) { r = 4; }
            break;
          case OP_DPTR:
            if (operands[r].type != OPERAND_DPTR) { r = 4; }
            break;
          case OP_AT_A_PLUS_DPTR:
            if (operands[r].type != OPERAND_AT_A_PLUS_DPTR) { r = 4; }
            break;
          case OP_AT_A_PLUS_PC:
            if (operands[r].type != OPERAND_AT_A_PLUS_PC) { r = 4; }
            break;
          case OP_AT_DPTR:
            if (operands[r].type != OPERAND_AT_DPTR) { r = 4; }
            break;
          case OP_DATA_16:
            if (operands[r].type != OPERAND_DATA ||
                (operands[r].value < -32768 || 
                 operands[r].value > 0xffff)) { r = 4; }
            break;
          case OP_CODE_ADDR:
            if (operands[r].type != OPERAND_NUM ||
                (operands[r].value < 0 || 
                 operands[r].value > 0xffff)) { r = 4; }
            break;
          case OP_RELADDR:
            if (operands[r].type != OPERAND_NUM) { r = 4; }
            break;
          case OP_DATA:
            if (operands[r].type != OPERAND_DATA ||
                (operands[r].value < -128 || 
                 operands[r].value > 255)) { r = 4; }
            break;
          case OP_SLASH_BIT_ADDR:
            if (operands[r].type != OPERAND_SLASH_BIT_ADDRESS ||
                (operands[r].value < 0 || 
                 operands[r].value > 255)) { r = 4; }
            break;
          case OP_PAGE:
            if ((operands[r].value >> 8) != table_8051[n].range)
            {
              r = 4;
              break;
            }
            break;
          case OP_BIT_ADDR:
            if (operands[r].type != OPERAND_BIT_ADDRESS ||
                (operands[r].value < 0 ||
                 operands[r].value > 255)) { r = 4; }
            break;
          case OP_IRAM_ADDR:
            if (operands[r].type != OPERAND_NUM ||
                (operands[r].value < 0 ||
                 operands[r].value > 255)) { r = 4; }
            break;
          default:
            print_error_internal(asm_context, __FILE__, __LINE__);
            return -1;
        }
      }

      if (r == operand_count)
      {
        memory_write_inc(asm_context, n, asm_context->line);

        // Holy crap :(
        if (n == 0x85)
        {
          memory_write_inc(asm_context, (uint8_t)operands[1].value & 0xff, asm_context->line);
          memory_write_inc(asm_context, (uint8_t)operands[0].value & 0xff, asm_context->line);
          break;
        }

        for(r = 0; r < 3; r++)
        {
          if (table_8051[n].op[r] == OP_NONE) { break; }
          switch(table_8051[n].op[r])
          {
            case OP_DATA_16:
            case OP_CODE_ADDR:
            {
              uint16_t value = operands[r].value & 0xffff;
              memory_write_inc(asm_context, value >> 8, asm_context->line);
              memory_write_inc(asm_context, value & 0xff, asm_context->line);
              count += 2;
              break;
            }
            case OP_RELADDR:
            {
              num = operands[r].value - (asm_context->address + 1);
              memory_write_inc(asm_context, (uint8_t)num, asm_context->line);
              count++;
              break;
            }
            case OP_DATA:
            case OP_SLASH_BIT_ADDR:
            case OP_PAGE:
            case OP_BIT_ADDR:
            case OP_IRAM_ADDR:
            {
              memory_write_inc(asm_context, (uint8_t)operands[r].value & 0xff, asm_context->line);
              count++;
              break;
            }
          }
        }

        break;
      }
    }
  }

  if (n == 256)
  {
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

  return count; 
}


