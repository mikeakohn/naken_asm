/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common/assembler.h"
#include "common/cpu_list.h"
#include "common/String.h"
#include "common/UtilContext.h"
#include "disasm/msp430.h"
#include "simulate/null.h"

UtilContext::UtilContext() :
  simulate          (NULL),
  cpu_name          (NULL),
  flags             (0),
  bytes_per_address (1),
  alignment         (1),
  allow_unknown_cpu (false),
  disasm_range      (NULL)
{
}

UtilContext::~UtilContext()
{
  delete simulate;
}

static const char *util_get_hex(const char *token, uint32_t *num)
{
  int s = 0;
  int n = 0;

  while (token[s] != 0 && token[s] != ' ' && token[s] != '-' && token[s] != 'h')
  {
    if (token[s] >= '0' && token[s] <= '9')
    {
      n = (n * 16) + (token[s] - '0');
    }
      else
    if (token[s] >= 'a' && token[s] <= 'f')
    {
      n = (n * 16) +((token[s] - 'a') + 10);
    }
      else
    if (token[s] >= 'A' && token[s] <= 'F')
    {
      n = (n * 16) + ((token[s] - 'A') + 10);
    }
      else
    {
      printf("Illegal number '%s'\n", token);
      return NULL;
    }

    s++;
  }

  *num = n;

  if (token[s] != '-') s++;

  return token + s;
}

void util_init(UtilContext *util_context)
{
#ifndef NO_MSP430
  util_context->disasm_range = disasm_range_msp430;
  util_context->simulate = SimulateMsp430::init(&util_context->memory);
  util_context->flags = 0;
  util_context->bytes_per_address = 1;
  util_context->alignment = 1;
#else
  util_context->disasm_range = cpu_list[0].disasm_range;
  util_context->simulate = SimulateNull::init(&util_context->memory);
  util_context->flags = cpu_list[0].flags;
  util_context->bytes_per_address = cpu_list[0].bytes_per_address;
  util_context->alignment = cpu_list[0].alignment;
#endif
}

static const char *util_get_token(String &value, const char *source)
{
  value = "";

  // Remove whitespace.
  while (*source == ' ') { source++; }

  while (*source != ' ' && *source != 0)
  {
    if (*source == '-' && value.len() != 0) { break; }

    value.append(*source);
    source++;

    if (value.char_at(-1) == '-') { break; }
  }

  // If no characters left to parse.
  if (value.len() == 0) { return NULL; }

  return source;
}

int util_get_range(
  UtilContext *util_context,
  const char *token,
  uint32_t *start,
  uint32_t *end)
{
  String data;

  *start = 0;
  *end = 0;

  token = util_get_token(data, token);
  if (token == NULL) { return -1; }

  // If not a - then this must be an address. Resolve it and then grab
  // another token which should be NULL or "-".
  if (data.equals("-") == false)
  {
    // Look up start_string in symbol table or use number.
    if (util_get_address(util_context, data.value(), start) == NULL)
    {
      return -1;
    }

    token = util_get_token(data, token);

    if (token == NULL)
    {
      *end = *start;
      return 0;
    }
  }

  // If the next token isn't a "-", there is for sure a problem now.
  if (data.equals("-") == false)
  {
    return -1;
  }

  // Next token should be end address, if not end address is end of code.
  token = util_get_token(data, token);
  if (token == NULL)
  {
    *end = util_context->memory.high_address;
    return 0;
  }

  // Look up end_string in symbol table or use number.
  if (util_get_address(util_context, data.value(), end) == NULL) { return -1; }

  // Should not be any more characters at end of line.
  token = util_get_token(data, token);
  if (token != NULL) { return -1; }

  return 0;
}

int util_is_supported_cpu(const char *name)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(name, cpu_list[n].name) == 0) { return 1; }
    n++;
  }

  return 0;
}

static void util_copy_cpu_info(UtilContext *util_context, CpuList *cpu_info)
{
  util_context->cpu_name          = cpu_info->name;
  util_context->disasm_range      = cpu_info->disasm_range;
  util_context->flags             = cpu_info->flags;
  util_context->bytes_per_address = cpu_info->bytes_per_address;
  util_context->memory.endian     = cpu_info->default_endian;
  util_context->alignment         = cpu_info->alignment;

  if (util_context->simulate != NULL)
  {
    delete util_context->simulate;
    util_context->simulate = NULL;
  }

  if (cpu_info->simulate_init != NULL)
  {
    util_context->simulate = cpu_info->simulate_init(&util_context->memory);
  }
    else
  {
    util_context->simulate = SimulateNull::init(&util_context->memory);
  }
}

int util_set_cpu_by_type(UtilContext *util_context, uint8_t cpu_type)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (cpu_list[n].type == cpu_type)
    {
      util_copy_cpu_info(util_context, &cpu_list[n]);
      return 1;
    }

    n++;
  }

  return 0;
}

int util_set_cpu_by_name(UtilContext *util_context, const char *name)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(name, cpu_list[n].name) == 0)
    {
      util_copy_cpu_info(util_context, &cpu_list[n]);
      return 1;
    }

    n++;
  }

  return 0;
}

const char *util_get_num(const char *token, uint32_t *num)
{
  uint32_t n;

  int s;

  *num = 0;

  // Skip spaces at beginning.
  while (*token == ' ' && *token != 0) { token++; }

  if (*token == 0) { return NULL; }

  // Check if number is hex.
  if (token[0] == '0' && token[1] == 'x')
  {
    return util_get_hex(token + 2, num);
  }

  // Look for end incase there is an h there.
  s = 0;
  while (token[s] != 0) { s++; }

  if (s == 0) { return NULL; }

  if (token[s-1] == 'h')
  {
    return util_get_hex(token, num);
  }

  s = 0;
  n = 0;
  int sign = 1;
  if (token[s] == '-') { s++; sign = -1; }

  while (token[s] != 0 && token[s] != '-')
  {
    if (token[s] >= '0' && token[s] <= '9')
    {
      n = (n * 10) + (token[s] - '0');
    }
      else
    if (token[s] == ' ')
    {
      break;
    }
      else
    {
      printf("Illegal number '%s'\n", token);
      return NULL;
    }

    s++;
  }

  *num = (n * sign) & 0xffffffff;

  return token + s;
}

const char *util_get_address(
  UtilContext *util_context,
  const char *token,
  uint32_t *address)
{
  int ret;

  // Skip spaces at beginning.
  while (*token == ' ' && *token != 0) { token++; }

  // Search symbol table
  ret = util_context->symbols.lookup(token, address);

  if (ret == 0)
  {
    while (*token != ' ' && *token != 0) { token++; }
    return token;
  }

  token = util_get_num(token, address);

  *address *= util_context->bytes_per_address;

  return token;
}

void util_print8(UtilContext *util_context, const char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  // FIXME - is this right?
  if (util_get_range(util_context, token, &start, &end) == -1) { return; }
  if (start >= end) { end = start + 128; }

  while (start < end)
  {
    if ((ptr & 0x0f) == 0)
    {
      chars[ptr] = 0;
      if (ptr != 0) { printf(" %s\n", chars); }
      ptr = 0;
      printf("0x%04x:", start / util_context->bytes_per_address);
    }

    uint8_t data = util_context->memory.read8(start);

    printf(" %02x", data);

    if (data >= ' ' && data <= 126)
    { chars[ptr++] = data; }
      else
    { chars[ptr++] = '.'; }

    start++;
  }

  chars[ptr] = 0;

  if (ptr != 0)
  {
    int n;
    for (n = ptr; n < 16; n++) printf("   ");
    printf(" %s\n", chars);
  }
}

void util_print16(UtilContext *util_context, const char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (util_get_range(util_context, token, &start, &end) == -1) { return; }
  if (start >= end) { end = start + 128; }

  int mask = (util_context->alignment - 1) & 0x1;

  if ((start & mask) != 0)
  {
    printf("Address range 0x%04x to 0x%04x must start on a 2 byte boundary.\n", start, end);
    return;
  }

  while (start < end)
  {
    if ((ptr & 0x0f) == 0)
    {
      chars[ptr] = 0;
      if (ptr != 0) printf(" %s\n", chars);
      ptr = 0;
      printf("0x%04x:", start / util_context->bytes_per_address);
    }

    int num = util_context->memory.read16(start);

    uint8_t data0 = num & 0xff;
    uint8_t data1 = (num >> 8) & 0xff;

    if (data0 >= ' ' && data0 <= 126)
    {
      chars[ptr++] = data0;
    }
      else
    {
      chars[ptr++] = '.';
    }

    if (data1 >= ' ' && data1 <= 126)
    {
      chars[ptr++] = data1;
    }
      else
    {
      chars[ptr++] = '.';
    }

    printf(" %04x", num);

    start = start + 2;
  }

  chars[ptr] = 0;

  if (ptr != 0)
  {
    int n;
    for (n = ptr; n < 16; n += 2) { printf("     "); }
    printf(" %s\n", chars);
  }
}

void util_print32(UtilContext *util_context, const char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (util_get_range(util_context, token, &start, &end) == -1) { return; }
  if (start >= end) { end = start + 128; }

  if ((start & (util_context->alignment - 1)) != 0)
  {
    printf("Address range 0x%04x to 0x%04x must start on a 4 byte boundary.\n",
      start, end);
    return;
  }

  while (start < end)
  {
    if ((ptr & 0x07) == 0)
    {
      chars[ptr] = 0;
      if (ptr != 0) printf(" %s\n", chars);
      ptr = 0;
      printf("0x%04x:", start / util_context->bytes_per_address);
    }

    uint32_t num = util_context->memory.read32(start);

    uint8_t data0 = num & 0xff;
    uint8_t data1 = (num >> 8) & 0xff;

    if (data0 >= ' ' && data0 <= 126)
    {
      chars[ptr++] = data0;
    }
      else
    {
      chars[ptr++] = '.';
    }

    if (data1 >= ' ' && data1 <= 126)
    {
      chars[ptr++] = data1;
    }
      else
    {
      chars[ptr++] = '.';
    }

    printf(" %08x", num);

    start = start + 4;
  }

  chars[ptr] = 0;

  if (ptr != 0)
  {
    int n;
    for (n = ptr; n < 8; n += 2) { printf("     "); }
    printf(" %s\n", chars);
  }
}

void util_write8(UtilContext *util_context, const char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  token = util_get_address(util_context, token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  int n = address;

  while (true)
  {
    // FIXME: This is already done in util_get_num().
    while (*token == ' ' && *token != 0) { token++; }

    token = util_get_num(token, &num);
    if (token == NULL) { break; }
    util_context->memory.write8(address++, num);
    count++;
  }

  printf("Wrote %d bytes starting at address 0x%04x\n",
    count, n / util_context->bytes_per_address);
}

void util_write16(UtilContext *util_context, const char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  token = util_get_address(util_context, token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  int mask = (util_context->alignment - 1) & 0x1;

  if ((address & mask) != 0)
  {
    printf("Error: write16 address is not 16 bit aligned\n");
    return;
  }

  int n = address;

  while (true)
  {
    // FIXME: This is already done in util_get_num().
    while (*token == ' ' && *token != 0) { token++; }

    token = util_get_num(token, &num);
    if (token == NULL) { break; }
    util_context->memory.write16(address, num);
    address += 2;
    count++;
  }

  printf("Wrote %d int16's starting at address 0x%04x\n",
    count, n / util_context->bytes_per_address);
}

void util_write32(UtilContext *util_context, const char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  token = util_get_address(util_context, token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  if ((address & (util_context->alignment - 1)) != 0)
  {
    printf("Error: write32 address is not 32 bit aligned\n");
    return;
  }

  int n = address;

  while (true)
  {
    // FIXME: This is already done in util_get_num().
    while (*token == ' ' && *token != 0) { token++; }

    token = util_get_num(token, &num);
    if (token == NULL) { break; }
    util_context->memory.write32(address, num);
    address += 4;
    count++;
  }

  printf("Wrote %d int32's starting at address 0x%04x\n",
    count, n / util_context->bytes_per_address);
}

