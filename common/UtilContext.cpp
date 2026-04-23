/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
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
#ifndef NO_MSP430
  disasm_range = disasm_range_msp430;
  simulate = SimulateMsp430::init(&memory);
  flags = 0;
  bytes_per_address = 1;
  alignment = 1;
#else
  disasm_range = cpu_list[0].disasm_range;
  simulate = SimulateNull::init(&memory);
  flags = cpu_list[0].flags;
  bytes_per_address = cpu_list[0].bytes_per_address;
  alignment = cpu_list[0].alignment;
#endif
}

UtilContext::~UtilContext()
{
  delete simulate;
}

int UtilContext::is_supported_cpu(const char *name)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(name, cpu_list[n].name) == 0) { return 1; }
    n++;
  }

  return 0;
}

int UtilContext::set_cpu_by_type(uint8_t cpu_type)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (cpu_list[n].type == cpu_type)
    {
      copy_cpu_info(&cpu_list[n]);
      return 1;
    }

    n++;
  }

  return 0;
}

int UtilContext::set_cpu_by_name(const char *name)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(name, cpu_list[n].name) == 0)
    {
      copy_cpu_info(&cpu_list[n]);
      return 1;
    }

    n++;
  }

  return 0;
}

void UtilContext::disasm(const char *token)
{
  uint32_t start, end;

  if (get_range(token, &start, &end) == -1) { return; }

  disasm_range(
    &memory,
    flags,
    start,
    end);
}

void UtilContext::disasm(uint32_t start, uint32_t end)
{
  uint32_t page_size, page_mask;
  int curr_start = start;
  int valid_page_start = 1;
  int address_min,address_max;
  int curr_end;

  start = start * bytes_per_address;
  end = end * bytes_per_address;

  page_size = memory.get_page_size();
  page_mask = page_size - 1;
  curr_end = start | page_mask;

  int data_size = 0;

  uint32_t n = start;

  while (n <= end)
  {
    data_size = page_size - (n & page_mask);

    if (memory.in_use(n))
    {
      if (valid_page_start == 0)
      {
        curr_start = n & (~page_mask);
        valid_page_start = 1;
      }
      curr_end = n | page_mask;
    }
      else
    {
      if (valid_page_start == 1)
      {
        address_min = memory.get_page_address_min(curr_start);
        address_max = memory.get_page_address_max(curr_end);

        disasm_range(
          &memory,
          flags,
          address_min,
          address_max);

        valid_page_start = 0;
      }
    }

    n += data_size;
  }

  if (valid_page_start == 1)
  {
    address_min = memory.get_page_address_min(curr_start);
    address_max = memory.get_page_address_max(curr_end);

    disasm_range(
      &memory,
      flags,
      address_min,
      address_max);
  }
}

void UtilContext::print8(const char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  // FIXME - is this right?
  if (get_range(token, &start, &end) == -1) { return; }
  if (start >= end) { end = start + 128; }

  while (start < end)
  {
    if ((ptr & 0x0f) == 0)
    {
      chars[ptr] = 0;
      if (ptr != 0) { printf(" %s\n", chars); }
      ptr = 0;
      printf("0x%04x:", start / bytes_per_address);
    }

    uint8_t data = memory.read8(start);

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

void UtilContext::print16(const char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (get_range(token, &start, &end) == -1) { return; }
  if (start >= end) { end = start + 128; }

  int mask = (alignment - 1) & 0x1;

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
      printf("0x%04x:", start / bytes_per_address);
    }

    int num = memory.read16(start);

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

void UtilContext::print32(const char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (get_range(token, &start, &end) == -1) { return; }
  if (start >= end) { end = start + 128; }

  if ((start & (alignment - 1)) != 0)
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
      printf("0x%04x:", start / bytes_per_address);
    }

    uint32_t num = memory.read32(start);

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

void UtilContext::write8(const char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  token = get_address(token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  int n = address;

  while (true)
  {
    // FIXME: This is already done in util_get_num().
    while (*token == ' ' && *token != 0) { token++; }

    token = get_num(token, &num);
    if (token == NULL) { break; }
    memory.write8(address++, num);
    count++;
  }

  printf("Wrote %d bytes starting at address 0x%04x\n",
    count, n / bytes_per_address);
}

void UtilContext::write16(const char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  token = get_address(token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  int mask = (alignment - 1) & 0x1;

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

    token = get_num(token, &num);
    if (token == NULL) { break; }
    memory.write16(address, num);
    address += 2;
    count++;
  }

  printf("Wrote %d int16's starting at address 0x%04x\n",
    count, n / bytes_per_address);
}

void UtilContext::write32(const char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  token = get_address(token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  if ((address & (alignment - 1)) != 0)
  {
    printf("Error: write32 address is not 32 bit aligned\n");
    return;
  }

  int n = address;

  while (true)
  {
    // FIXME: This is already done in util_get_num().
    while (*token == ' ' && *token != 0) { token++; }

    token = get_num(token, &num);
    if (token == NULL) { break; }
    memory.write32(address, num);
    address += 4;
    count++;
  }

  printf("Wrote %d int32's starting at address 0x%04x\n",
    count, n / bytes_per_address);
}

const char *UtilContext::get_address(const char *token, uint32_t *address)
{
  int ret;

  // Skip spaces at beginning.
  while (*token == ' ' && *token != 0) { token++; }

  // Search symbol table
  ret = symbols.lookup(token, address);

  if (ret == 0)
  {
    while (*token != ' ' && *token != 0) { token++; }
    return token;
  }

  token = get_num(token, address);

  *address *= bytes_per_address;

  return token;
}

const char *UtilContext::get_num(const char *token, uint32_t *num)
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
    return get_hex(token + 2, num);
  }

  // Look for end incase there is an h there.
  s = 0;
  while (token[s] != 0) { s++; }

  if (s == 0) { return NULL; }

  if (token[s-1] == 'h')
  {
    return get_hex(token, num);
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

int UtilContext::get_range(const char *token, uint32_t *start, uint32_t *end)
{
  String data;

  *start = 0;
  *end = 0;

  token = get_token(data, token);
  if (token == NULL) { return -1; }

  // If not a - then this must be an address. Resolve it and then grab
  // another token which should be NULL or "-".
  if (data.equals("-") == false)
  {
    // Look up start_string in symbol table or use number.
    if (get_address(data.value(), start) == NULL)
    {
      return -1;
    }

    token = get_token(data, token);

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
  token = get_token(data, token);
  if (token == NULL)
  {
    *end = memory.high_address;
    return 0;
  }

  // Look up end_string in symbol table or use number.
  if (get_address(data.value(), end) == NULL) { return -1; }

  // Should not be any more characters at end of line.
  token = get_token(data, token);
  if (token != NULL) { return -1; }

  return 0;
}

void UtilContext::copy_cpu_info(CpuList *cpu_info)
{
  cpu_name          = cpu_info->name;
  disasm_range      = cpu_info->disasm_range;
  flags             = cpu_info->flags;
  bytes_per_address = cpu_info->bytes_per_address;
  memory.endian     = cpu_info->default_endian;
  alignment         = cpu_info->alignment;

  if (simulate != NULL)
  {
    delete simulate;
    simulate = NULL;
  }

  if (cpu_info->simulate_init != NULL)
  {
    simulate = cpu_info->simulate_init(&memory);
  }
    else
  {
    simulate = SimulateNull::init(&memory);
  }
}

const char *UtilContext::get_hex(const char *token, uint32_t *num)
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

const char *UtilContext::get_token(String &value, const char *source)
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

