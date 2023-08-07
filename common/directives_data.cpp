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

#include "asm/common.h"
#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/eval_expression_ex.h"
#include "common/tokens.h"
#include "common/print_error.h"

int parse_db(AsmContext *asm_context, int null_term_flag)
{
  char token[TOKENLEN];
  int token_type;
  int data32;

  if (asm_context->segment == SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n",
      asm_context->tokens.filename,
      asm_context->tokens.line);
    return -1;
  }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (token_type == TOKEN_QUOTED)
    {
      uint8_t *s = (uint8_t *)token;

      while (*s != 0)
      {
        if (*s == '\\')
        {
          if (s[1] == '0')
          {
            s++;
            *s = 0;
          }
        }

        memory_write_inc(asm_context, *s, DL_DATA);

        asm_context->data_count++;
        s++;
      }

      if (null_term_flag == 1)
      {
        memory_write_inc(asm_context, 0, DL_DATA);
        asm_context->data_count++;
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &data32) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(asm_context, "db");
          return -1;
        }

        ignore_operand(asm_context);
        data32 = 0;
      }

      if (data32 < -128 || data32 > 0xff)
      {
        print_error_range(asm_context, "db", -128, 0xff);
        return -1;
      }

      memory_write_inc(asm_context, (uint8_t)data32, DL_DATA);
      asm_context->data_count++;
    }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(asm_context, ",", token);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc16(AsmContext *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  int data32;
  uint16_t data16;

  if (asm_context->segment == SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
    return -1;
  }

  while (1)
  {
    // If the user has a comma at the end, but no data, this is okay.
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    tokens_push(asm_context, token, token_type);

    if (eval_expression(asm_context, &data32) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(asm_context, "dc16");
        return -1;
      }

      ignore_operand(asm_context);
      data32 = 0;
    }

    if (data32 < -32768 || data32 > 0xffff)
    {
      print_error_range(asm_context, "dc16", -32768, 0xffff);
      return -1;
    }

    data16 = (uint16_t)data32;

    if (asm_context->memory.endian == ENDIAN_LITTLE)
    {
      memory_write_inc(asm_context, data16 & 255, DL_DATA);
      memory_write_inc(asm_context, data16 >> 8, DL_DATA);
    }
      else
    {
      memory_write_inc(asm_context, data16 >> 8, DL_DATA);
      memory_write_inc(asm_context, data16 & 255, DL_DATA);
    }

    asm_context->data_count += 2;
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(asm_context, ",", token);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc32(AsmContext *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  Var var;
  uint32_t udata32;

  if (asm_context->segment == SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
    return -1;
  }

  while (1)
  {
    // If the user has a comma at the end, but no data, this is okay.
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    tokens_push(asm_context, token, token_type);

    if (eval_expression_ex(asm_context, &var) == -1)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(asm_context, "dc32");
        return -1;
      }

      ignore_operand(asm_context);
    }

    udata32 = var_get_bin32(&var);

#if 0
    if (eval_expression(asm_context, &data32) != 0)
    {
      if (asm_context->pass == 2)
      {
        return -1;
      }

      ignore_operand(asm_context);
      data32 = 0;
    }
    udata32 = (uint32_t)data32;
#endif

    if (asm_context->memory.endian == ENDIAN_LITTLE)
    {
      memory_write_inc(asm_context, udata32 & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata32 >> 8) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata32 >> 16) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata32 >> 24) & 0xff, DL_DATA);
    }
      else
    {
      memory_write_inc(asm_context, (udata32 >> 24) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata32 >> 16) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata32 >> 8) & 0xff, DL_DATA);
      memory_write_inc(asm_context, udata32 & 0xff, DL_DATA);
    }

    asm_context->data_count += 4;
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(asm_context, ",", token);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc64(AsmContext *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  Var var;
  uint64_t udata64;

  if (asm_context->segment == SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
    return -1;
  }

  while (1)
  {
    // if the user has a comma at the end, but no data, this is okay
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    tokens_push(asm_context, token, token_type);

    if (eval_expression_ex(asm_context, &var) == -1)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(asm_context, "dc64");
        return -1;
      }

      ignore_operand(asm_context);
    }

    udata64 = (uint64_t)var_get_bin64(&var);

    if (asm_context->memory.endian == ENDIAN_LITTLE)
    {
      memory_write_inc(asm_context, udata64 & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 8) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 16) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 24) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 32) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 40) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 48) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 56) & 0xff, DL_DATA);
    }
      else
    {
      memory_write_inc(asm_context, (udata64 >> 56) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 48) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 40) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 32) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 24) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 16) & 0xff, DL_DATA);
      memory_write_inc(asm_context, (udata64 >> 8) & 0xff, DL_DATA);
      memory_write_inc(asm_context, udata64 & 0xff, DL_DATA);
    }

    asm_context->data_count += 8;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(asm_context, ",", token);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

#if 0
int parse_dc(AsmContext *asm_context)
{
  char token[TOKENLEN];

  if (expect_token_s(asm_context, ".") != 0) { return -1; }
  tokens_get(asm_context, token, TOKENLEN);

  if (strcasecmp(token, "b") == 0) { return parse_db(asm_context,0); }
  if (strcasecmp(token, "w") == 0) { return parse_dc16(asm_context); }
  if (strcasecmp(token, "l") == 0) { return parse_dc32(asm_context); }
  if (strcasecmp(token, "d") == 0) { return parse_dc64(asm_context); }

  print_error_unexp(asm_context, token);
  return -1;
}
#endif

int parse_data_fill(AsmContext *asm_context)
{
  int count, value, n;

  if (eval_expression(asm_context, &value) == -1)
  {
    if (asm_context->pass == 1)
    {
      value = 0;
    }
      else
    {
      print_error_illegal_expression(asm_context, "data_fill");
      return -1;
    }
  }

  if (value < -128 || value > 255)
  {
    print_error_range(asm_context, "data_fill", -128, 0xff);
    return -1;
  }

  if (expect_token_s(asm_context, ",") != 0) { return -1; }

  if (eval_expression(asm_context, &count) == -1)
  {
    print_error_illegal_expression(asm_context, "data_fill");
    return -1;
  }

  if (count < 1)
  {
    print_error(asm_context, "data_fill length is less than 1");
    return -1;
  }

  for (n = 0; n < count; n++)
  {
    memory_write_inc(asm_context, value & 0xff, DL_DATA);
  }

  return 0;
}

int parse_varuint(AsmContext *asm_context, int fixed_size)
{
  char token[TOKENLEN];
  int token_type;
  Var var;
  uint32_t udata32;
  int length;

  while (1)
  {
    // If the user has a comma at the end, but no data, this is okay.
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    tokens_push(asm_context, token, token_type);

    if (eval_expression_ex(asm_context, &var) == -1)
    {
      if (asm_context->pass == 2 || fixed_size == 0)
      {
        print_error_illegal_expression(asm_context, "varuint");
        return -1;
      }

      ignore_operand(asm_context);
    }

    udata32 = (uint32_t)var_get_bin64(&var);

    length = add_bin_varuint(asm_context, udata32, fixed_size);

    asm_context->data_count += length;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(asm_context, ",", token);
      return -1;
    }
  }

  return 0;
}

int parse_resb(AsmContext *asm_context, int size)
{
  int num;

  if (eval_expression(asm_context, &num) != 0)
  {
    if (asm_context->pass == 2)
    {
      print_error_illegal_expression(asm_context, "resb");
      return -1;
    }

    print_error(asm_context, "Parse Error: Expected data count.");
    return -1;
  }

  asm_context->address += (num * size);

  return 0;
}

static int parse_align(AsmContext *asm_context, int num)
{
  int mask;

  if (num > 1024)
  {
    print_error(asm_context, "align constant too large");
    return -1;
  }

  mask = num - 1;

  while ((asm_context->address & mask) != 0)
  {
    // Issue #38: Can't actually write a byte here incase this is
    // aligning resb/w directives which don't write any data out.
    //memory_write_inc(asm_context, 0, DL_EMPTY);
    asm_context->address++;
  }

  return 0;
}

int parse_align_bits(AsmContext *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1 || (num % 8) != 0)
  {
    print_error(asm_context, "align expects 16, 32, 64, 128, etc bits");
    return -1;
  }

  num = num / 8;

  return parse_align(asm_context, num);
}

int parse_align_bytes(AsmContext *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error_illegal_expression(asm_context, "align");
    return -1;
  }

  return parse_align(asm_context, num);
}

