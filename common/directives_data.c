/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
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

int parse_db(struct _asm_context *asm_context, int null_term_flag)
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
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;

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
          print_error_illegal_expression("db", asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        data32 = 0;
      }

      if (data32 < -128 || data32 > 0xff)
      {
        print_error_range("db", -128, 0xff, asm_context);
        return -1;
      }

      memory_write_inc(asm_context, (uint8_t)data32, DL_DATA);
      asm_context->data_count++;
    }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(",", token, asm_context);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc16(struct _asm_context *asm_context)
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
    // if the user has a comma at the end, but no data, this is okay
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;
    tokens_push(asm_context, token, token_type);

    if (eval_expression(asm_context, &data32) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression("dc16", asm_context);
        return -1;
      }

      ignore_operand(asm_context);
      data32 = 0;
    }

    if (data32 < -32768 || data32 > 0xffff)
    {
      print_error_range("dc16", -32768, 0xffff, asm_context);
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
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(",", token, asm_context);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc32(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  //int data32;
  struct _var var;
  uint32_t udata32;

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
        print_error_illegal_expression("dc32", asm_context);
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
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(",", token, asm_context);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc64(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  struct _var var;
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
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;
    tokens_push(asm_context, token, token_type);

    if (eval_expression_ex(asm_context, &var) == -1)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression("dc64", asm_context);
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
      print_error_expecting(",", token, asm_context);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}

int parse_dc(struct _asm_context *asm_context)
{
  char token[TOKENLEN];

  if (expect_token_s(asm_context, ".") != 0) { return -1; }
  tokens_get(asm_context, token, TOKENLEN);

  if (strcasecmp(token, "b") == 0) { return parse_db(asm_context,0); }
  if (strcasecmp(token, "w") == 0) { return parse_dc16(asm_context); }
  if (strcasecmp(token, "l") == 0) { return parse_dc32(asm_context); }
  if (strcasecmp(token, "d") == 0) { return parse_dc64(asm_context); }

  print_error_unexp(token, asm_context);
  return -1;
}

#if 0
int parse_dq(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  struct _var var;
  uint32_t udata64;
  union
  {
    double f64;
    uint64_t u64;
  } data;

  if (asm_context->segment == SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
    return -1;
  }

  while (1)
  {
    // if the user has a comma at the end, but no data, this is okay
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;
    tokens_push(asm_context, token, token_type);

    if (eval_expression_ex(asm_context, &var) == -1)
    {
      ignore_operand(asm_context);
    }

    data.f64 = var_get_float(&var);
    udata64 = data.u64;

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
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_expecting(",", token, asm_context);
      return -1;
    }
  }

  asm_context->tokens.line++;

  return 0;
}
#endif

#if 0
int parse_ds(struct _asm_context *asm_context, int n)
{
  char token[TOKENLEN];
  int token_type;
  int num;

  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_NUMBER)
  {
    printf("Parse error: memory length on line %d.\n", asm_context->tokens.line);
    return -1;
  }

  num = atoi(token) * n;

  if (num == 0 && asm_context->pass == 1)
  {
    printf("Warning: Reserving %d byte at %s:%d\n", num, asm_context->tokens.filename, asm_context->tokens.line);
  }

  for (n = 0; n < num; n++)
  {
    if (asm_context->segment != SEGMENT_BSS)
    {
      memory_write_inc(asm_context, 0, DL_DATA);
    }
      else
    {
      asm_context->address++;
    }

    if (asm_context->address >= asm_context->memory.size)
    {
       printf("Error: ds overran %d boundary at %s:%d", asm_context->memory.size, asm_context->tokens.filename, asm_context->tokens.line);
       return -1;
    }
  }

  asm_context->data_count += num;

  return 0;
}
#endif

int parse_data_fill(struct _asm_context *asm_context)
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
      print_error_illegal_expression("data_fill", asm_context);
      return -1;
    }
  }

  if (value < -128 || value > 255)
  {
    print_error_range("data_fill", -128, 0xff, asm_context);
    return -1;
  }

  if (expect_token_s(asm_context, ",") != 0) { return -1; }

  if (eval_expression(asm_context, &count) == -1)
  {
    print_error_illegal_expression("data_fill", asm_context);
    return -1;
  }

  if (count < 1)
  {
    print_error("data_fill length is less than 1", asm_context);
    return -1;
  }

  for (n = 0; n < count; n++)
  {
    memory_write_inc(asm_context, value & 0xff, DL_DATA);
  }

  return 0;
}

int parse_varuint(struct _asm_context *asm_context, int fixed_size)
{
  char token[TOKENLEN];
  int token_type;
  struct _var var;
  uint32_t udata32;
  int length;

  while (1)
  {
    // if the user has a comma at the end, but no data, this is okay
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;
    tokens_push(asm_context, token, token_type);

    if (eval_expression_ex(asm_context, &var) == -1)
    {
      if (asm_context->pass == 2 || fixed_size == 0)
      {
        print_error_illegal_expression("varuint", asm_context);
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
      print_error_expecting(",", token, asm_context);
      return -1;
    }
  }

  return 0;
}

int parse_resb(struct _asm_context *asm_context, int size)
{
  int num;

  if (eval_expression(asm_context, &num) != 0)
  {
    if (asm_context->pass == 2)
    {
      print_error_illegal_expression("resb", asm_context);
      return -1;
    }

    print_error("Parse Error: Expected data count.", asm_context);
    return -1;
  }

  asm_context->address += (num * size);

  return 0;
}

static int parse_align(struct _asm_context *asm_context, int num)
{
  int mask;

  if (num > 1024)
  {
    print_error("align constant too large", asm_context);
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

int parse_align_bits(struct _asm_context *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1 || (num % 8) != 0)
  {
    print_error("align expects 16, 32, 64, 128, etc bits", asm_context);
    return -1;
  }

  num = num / 8;

  return parse_align(asm_context, num);
}

int parse_align_bytes(struct _asm_context *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error_illegal_expression("align", asm_context);
    return -1;
  }

  return parse_align(asm_context, num);
}

