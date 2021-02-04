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
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "disasm/webasm.h"
#include "table/webasm.h"

int get_cycle_count_webasm(uint16_t opcode)
{
  return -1;
}

static uint64_t get_varuint(
  struct _memory *memory,
  uint32_t address,
  int *length)
{
  uint8_t ch;
  int shift = 0;
  int done = 0;
  uint64_t num = 0;

  *length = 0;

  while (done == 0)
  {
    ch = memory_read_m(memory, address++);

    *length += 1;

    if ((ch & 0x80) == 0)
    {
      done = 1;
    }

    ch = ch & 0x7f;

    num |= ch << shift;
    shift += 7;
  }

  return num;
}

static int64_t get_varint(
  struct _memory *memory,
  uint32_t address,
  int *length)
{
  uint8_t ch;
  int shift = 0;
  int done = 0;
  int64_t num = 0;

  *length = 0;

  while (done == 0)
  {
    ch = memory_read_m(memory, address++);

    *length += 1;

    if ((ch & 0x80) == 0)
    {
      done = 1;
    }

    ch = ch & 0x7f;

    num |= ch << shift;
    shift += 7;
  }

  if ((num & (1ULL << (shift - 1))) != 0)
  {
    num |= ~((1ULL << shift) - 1);
  }

  return num;
}

static const char *get_type(int type)
{
  int n = 0;

  while (webasm_types[n].name != NULL)
  {
    if (webasm_types[n].type == type)
    {
      return webasm_types[n].name;
    }

    n++;
  }

  return "???";
}

static int print_table(struct _memory *memory, uint32_t address, FILE *out)
{
  int length = 0, total_length, entry, n, count = 0;

  count = get_varint(memory, address, &total_length);

  address += total_length;
  length += count;

  for (n = 0; n < count; n++)
  {
    entry = get_varint(memory, address, &length);

    fprintf(out, "    %04x\n", entry);

    address += length;
    total_length += length;
  }

  return total_length;
}

int disasm_webasm(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  uint8_t opcode;
  uint64_t i;
  int64_t v;
  uint32_t count;
  int n, length;

  instruction[0] = 0;

  *cycles_min = 0;
  *cycles_max = 0;

  opcode = memory_read_m(memory, address);

  n = 0;
  while(table_webasm[n].instr != NULL)
  {
    if (opcode != table_webasm[n].opcode)
    {
      n++;
      continue;
    }

    switch(table_webasm[n].type)
    {
      case WEBASM_OP_NONE:
        sprintf(instruction, "%s", table_webasm[n].instr);
        return 1;
      case WEBASM_OP_UINT32:
        i = memory_read32_m(memory, address + 1);
        sprintf(instruction, "%s 0x%04" PRIx64, table_webasm[n].instr, i);
        return 5;
      case WEBASM_OP_UINT64:
        i = memory_read32_m(memory, address + 1);
        i |= ((uint64_t)memory_read32_m(memory, address + 5)) << 32;
        sprintf(instruction, "%s 0x%04" PRIx64, table_webasm[n].instr, i);
        return 9;
      case WEBASM_OP_VARINT64:
        v = get_varint(memory, address + 1, &length);
        sprintf(instruction, "%s 0x%04" PRIx64, table_webasm[n].instr, v);
        return length + 1;
      case WEBASM_OP_VARINT32:
        v = get_varint(memory, address + 1, &length);
        sprintf(instruction, "%s 0x%04" PRIx64, table_webasm[n].instr, v);
        return length + 1;
      case WEBASM_OP_FUNCTION_INDEX:
      case WEBASM_OP_LOCAL_INDEX:
      case WEBASM_OP_GLOBAL_INDEX:
        i = get_varuint(memory, address + 1, &length);
        sprintf(instruction, "%s 0x%04" PRIx64, table_webasm[n].instr, i);
        return length + 1;
      case WEBASM_OP_BLOCK_TYPE:
        i = get_varuint(memory, address + 1, &length);
        sprintf(instruction, "%s %s", table_webasm[n].instr, get_type(i));
        return length + 1;
      case WEBASM_OP_RELATIVE_DEPTH:
        v = get_varint(memory, address + 1, &length);
        sprintf(instruction, "%s 0x%04" PRIx64, table_webasm[n].instr, v);
        return length + 1;
      case WEBASM_OP_TABLE:
        count = get_varint(memory, address + 1, &length);

        sprintf(instruction, "%s %d ...", table_webasm[n].instr, count);

        return length + 1;
      case WEBASM_OP_INDIRECT:
      case WEBASM_OP_MEMORY_IMMEDIATE:
      default:
        return 1;
    }
  }

  return 1;
}

void list_output_webasm(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  count = disasm_webasm(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  hex[0] = 0;

  for (n = 0; n < count; n++)
  {
    opcode = memory_read_m(&asm_context->memory, start + n);

    sprintf(temp, "%02x ", opcode);
    strcat(hex, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-20s %-40s\n", start, hex, instruction);

  opcode = memory_read_m(&asm_context->memory, start);

  if (opcode == 0x0e)
  {
    start += print_table(&asm_context->memory, start + 1, asm_context->list);
  }
}

void disasm_range_webasm(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_webasm(memory, start, instruction, &cycles_min, &cycles_max);

    hex[0] = 0;

    for (n = 0; n < count; n++)
    {
      opcode = memory_read_m(memory, start + n);

      sprintf(temp, "%02x ", opcode);
      strcat(hex, temp);
    }

    printf("0x%04x: %-20s %-40s\n", start, hex, instruction);

    opcode = memory_read_m(memory, start);

    if (opcode == 0x0e)
    {
      start += print_table(memory, start + 1, stdout);
    }

    start += count;
  }
}

