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
#include <stdint.h>

#include "common/assembler.h"
#include "common/cpu_list.h"
#include "common/util_context.h"

// FIXME - How to do this better?
#if 0
parse_instruction_t parse_instruction_1802 = NULL;
parse_instruction_t parse_instruction_4004 = NULL;
parse_instruction_t parse_instruction_6502 = NULL;
parse_instruction_t parse_instruction_65816 = NULL;
parse_instruction_t parse_instruction_6800 = NULL;
parse_instruction_t parse_instruction_6809 = NULL;
parse_instruction_t parse_instruction_68hc08 = NULL;
parse_instruction_t parse_instruction_68000 = NULL;
parse_instruction_t parse_instruction_8008 = NULL;
parse_instruction_t parse_instruction_8048 = NULL;
parse_instruction_t parse_instruction_8051 = NULL;
parse_instruction_t parse_instruction_86000 = NULL;
parse_instruction_t parse_instruction_arc = NULL;
parse_instruction_t parse_instruction_arm64 = NULL;
parse_instruction_t parse_instruction_avr8 = NULL;
parse_instruction_t parse_instruction_cell = NULL;
parse_instruction_t parse_instruction_common = NULL;
parse_instruction_t parse_instruction_cp1610 = NULL;
parse_instruction_t parse_instruction_dotnet = NULL;
parse_instruction_t parse_instruction_dspic = NULL;
parse_instruction_t parse_instruction_epiphany = NULL;
parse_instruction_t parse_instruction_java = NULL;
parse_instruction_t parse_instruction_lc3 = NULL;
parse_instruction_t parse_instruction_m8c = NULL;
parse_instruction_t parse_instruction_mips = NULL;
parse_instruction_t parse_instruction_msp430 = NULL;
parse_instruction_t parse_instruction_pdp8 = NULL;
parse_instruction_t parse_instruction_pic14 = NULL;
parse_instruction_t parse_instruction_powerpc = NULL;
parse_instruction_t parse_instruction_propeller = NULL;
parse_instruction_t parse_instruction_propeller2 = NULL;
parse_instruction_t parse_instruction_ps2_ee_vu = NULL;
parse_instruction_t parse_instruction_riscv = NULL;
parse_instruction_t parse_instruction_sh4 = NULL;
parse_instruction_t parse_instruction_sparc = NULL;
parse_instruction_t parse_instruction_stm8 = NULL;
parse_instruction_t parse_instruction_super_fx = NULL;
parse_instruction_t parse_instruction_sweet16 = NULL;
parse_instruction_t parse_instruction_thumb = NULL;
parse_instruction_t parse_instruction_tms340 = NULL;
parse_instruction_t parse_instruction_tms1000 = NULL;
parse_instruction_t parse_instruction_tms1100 = NULL;
parse_instruction_t parse_instruction_tms9900 = NULL;
parse_instruction_t parse_instruction_unsp = NULL;
parse_instruction_t parse_instruction_webasm = NULL;
parse_instruction_t parse_instruction_xtensa = NULL;
parse_instruction_t parse_instruction_z80 = NULL;
link_function_t link_function_mips = NULL;
link_function_t link_function_msp430 = NULL;
#endif

static char *util_get_hex(char *token, uint32_t *num)
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

int util_get_range(
  struct _util_context *util_context,
  char *token,
  uint32_t *start,
  uint32_t *end)
{
  char *start_string = NULL;
  char *end_string = NULL;
  char *s;

  // Remove white space from start;
  while (*token == ' ') { token++; }

  start_string = token;

  while (*token != '-' && *token != 0)
  {
    token++;
  }

  // Remove white space from end of start_string
  s = token - 1;

  while (s >= start_string)
  {
    if (*s != ' ') { break; }
    *s = 0;
    s--;
  }

  if (*token == '-')
  {
    *token = 0;
    end_string = token + 1;

    // Strip white space from start of end_string
    while (*end_string == ' ') end_string++;
  }

  // Remove white space from end of end_string
  s = token - 1;
  while (s >= start_string)
  {
    if (*s != ' ') { break; }
    *s = 0;
    s--;
  }

  // Look up start_string in symbol table or use number
  token = util_get_address(util_context, start_string, start);

  if (token == NULL) { return -1; }

  // If end_string is empty then end = start
  if (end_string == NULL || *end_string == 0)
  {
    *end = *start;
    return 0;
  }

  // Look up end_string in symbol table or use number
  token = util_get_address(util_context, end_string, end);

  if (token == NULL) { return -1; }

  return 0;
}

int util_set_cpu_by_name(struct _util_context *util_context, const char *name)
{
  int n = 0;

  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(name, cpu_list[n].name) == 0)
    {
      util_context->disasm_range = cpu_list[n].disasm_range;
      util_context->flags = cpu_list[n].flags;
      util_context->bytes_per_address = cpu_list[n].bytes_per_address;
      util_context->memory.endian = cpu_list[n].default_endian;
      util_context->alignment = cpu_list[n].alignment;

      if (cpu_list[n].simulate_init != NULL)
      {
        util_context->simulate = cpu_list[n].simulate_init(&util_context->memory);
      }

      return 1;

      break;
    }

    n++;
  }

  return 0;
}

char *util_get_num(char *token, uint32_t *num)
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

char *util_get_address(
  struct _util_context *util_context,
  char *token,
  uint32_t *address)
{
  int ret;

  // Skip spaces at beginning.
  while (*token == ' ' && *token != 0) { token++; }

  // Search symbol table
  ret = symbols_lookup(&util_context->symbols, token, address);

  if (ret == 0)
  {
    while (*token != ' ' && *token != 0) { token++; }
    return token;
  }

  token = util_get_num(token, address);

  *address *= util_context->bytes_per_address;

  return token;
}

void util_print8(struct _util_context *util_context, char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (*token != ' ')
  {
    printf("Syntax error: no address given.\n");
    return;
  }

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

    uint8_t data = memory_read_m(&util_context->memory, start);

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

void util_print16(struct _util_context *util_context, char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (*token != ' ')
  {
    printf("Syntax error: no address given.\n");
    return;
  }

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

    int num = memory_read16_m(&util_context->memory, start);

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

void util_print32(struct _util_context *util_context, char *token)
{
  char chars[20];
  uint32_t start, end;
  int ptr = 0;

  if (*token != ' ')
  {
    printf("Syntax error: no address given.\n");
    return;
  }

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

    uint32_t num = memory_read32_m(&util_context->memory, start);

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

void util_write8(struct _util_context *util_context, char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  if (*token != ' ')
  {
    printf("Syntax error: no address given.\n");
    return;
  }

  while (*token == ' ' && *token != 0) { token++; }

  if (token == 0) { printf("Syntax error: no address given.\n"); }

  token = util_get_address(util_context, token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  int n = address;
  while (1)
  {
    if (address >= util_context->memory.size) break;
    while (*token == ' ' && *token != 0) token++;
    token = util_get_num(token, &num);
    if (token == 0) break;
    memory_write_m(&util_context->memory, address++, num);
    count++;
  }

  printf("Wrote %d bytes starting at address 0x%04x\n",
    count, n / util_context->bytes_per_address);
}

void util_write16(struct _util_context *util_context, char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  if (*token != ' ')
  {
    printf("Syntax error: no address given.\n");
    return;
  }

  while (*token == ' ' && *token != 0) { token++; }

  if (token == 0) { printf("Syntax error: no address given.\n"); }

  token = util_get_address(util_context, token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  int mask = (util_context->alignment - 1) & 0x1;

  if ((address & mask) != 0)
  {
    printf("Error: write16 address is not 16 bit aligned\n");
    return;
  }

  int n = address;

  while (1)
  {
    if (address >= util_context->memory.size) break;
    while (*token == ' ' && *token != 0) { token++; }
    token = util_get_num(token, &num);
    if (token == 0) break;
    memory_write16_m(&util_context->memory, address, num);
    address += 2;
    count++;
  }

  printf("Wrote %d int16's starting at address 0x%04x\n",
    count, n / util_context->bytes_per_address);
}

void util_write32(struct _util_context *util_context, char *token)
{
  uint32_t address = 0;
  uint32_t num;
  int count = 0;

  if (*token != ' ')
  {
    printf("Syntax error: no address given.\n");
    return;
  }

  while (*token == ' ' && *token != 0) { token++; }

  if (token == 0) { printf("Syntax error: no address given.\n"); }

  token = util_get_address(util_context, token, &address);

  if (token == NULL) { printf("Syntax error: bad address\n"); }

  if ((address & (util_context->alignment - 1)) != 0)
  {
    printf("Error: write32 address is not 32 bit aligned\n");
    return;
  }

  int n = address;

  while (1)
  {
    if (address >= util_context->memory.size) break;
    while (*token == ' ' && *token != 0) { token++; }
    token = util_get_num(token, &num);
    if (token == 0) break;
    memory_write32_m(&util_context->memory, address, num);
    address += 4;
    count++;
  }

  printf("Wrote %d int32's starting at address 0x%04x\n",
    count, n / util_context->bytes_per_address);
}
