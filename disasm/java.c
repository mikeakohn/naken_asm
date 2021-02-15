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

#include "disasm/java.h"
#include "table/java.h"

int get_cycle_count_java(uint16_t opcode)
{
  return -1;
}

const char *array_types[] =
{
  "",
  "",
  "",
  "",
  "boolean",
  "char",
  "float",
  "double",
  "byte",
  "short",
  "int",
  "long",
};

static const char *get_array_type(int index)
{
  if (index > (sizeof(array_types) / sizeof(char *)))
  {
    return "";
  }

  return array_types[index];
}

int disasm_java(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  uint16_t index;
  uint8_t opcode;
  int8_t const8;
  int8_t index8;
  int16_t offset16;
  int offset;
  int wide = 0;

  *cycles_min = 0;
  *cycles_max = 0;

  opcode = memory_read_m(memory, address);

  if (opcode == 0xc4)
  {
    wide = 1;
    opcode = memory_read_m(memory, address + 1);
  }

  switch (table_java[opcode].op_type)
  {
    case JAVA_OP_ILLEGAL:
    case JAVA_OP_NONE:
      // Note: These instructions should never be wide.
      sprintf(instruction, "%s", table_java[opcode].instr);
      return wide + 1;
    case JAVA_OP_CONSTANT_INDEX8:
      // Note: These instructions should never be wide.
      index8 = memory_read_m(memory, address + wide + 1);
      sprintf(instruction, "%s %d", table_java[opcode].instr, index8);
      return wide + 2;
    case JAVA_OP_CONSTANT_INDEX:
    case JAVA_OP_FIELD_INDEX:
    case JAVA_OP_INTERFACE_INDEX:
    case JAVA_OP_METHOD_INDEX:
    case JAVA_OP_CLASS_INDEX:
    case JAVA_OP_SPECIAL_INDEX:
    case JAVA_OP_STATIC_INDEX:
    case JAVA_OP_VIRTUAL_INDEX:
      // Note: These instructions should never be wide.
      index = memory_read16_m(memory, address + wide + 1);
      sprintf(instruction, "%s %d", table_java[opcode].instr, index);
      return wide + 3;
    case JAVA_OP_LOCAL_INDEX:
      if (wide == 0)
      {
        index = memory_read_m(memory, address + wide + 1);
        sprintf(instruction, "%s %d", table_java[opcode].instr, index);
        return wide + 2;
      }
        else
      {
        index = memory_read16_m(memory, address + wide + 1);
        sprintf(instruction, "%s %d", table_java[opcode].instr, index);
        return wide + 3;
      }

    case JAVA_OP_LOCAL_INDEX_CONST:
      if (wide == 0)
      {
        index = memory_read_m(memory, address + wide + 1);
        const8 = memory_read_m(memory, address + wide + 2);
        sprintf(instruction, "%s %d, %d", table_java[opcode].instr, index, const8);
        return wide + 3;
      }
        else
      {
        index = memory_read16_m(memory, address + wide + 1);
        const8 = memory_read_m(memory, address + wide + 3);
        sprintf(instruction, "%s %d, %d", table_java[opcode].instr, index, const8);
        return wide + 4;
      }
    case JAVA_OP_ARRAY_TYPE:
      // Note: These instructions should never be wide.
      index = memory_read_m(memory, address + wide + 1);
      sprintf(instruction, "%s %d (%s)", table_java[opcode].instr, index, get_array_type(index));
      return wide + 2;
    case JAVA_OP_CONSTANT16:
      // Note: These instructions should never be wide.
      index = memory_read16_m(memory, address + wide + 1);
      sprintf(instruction, "%s %d", table_java[opcode].instr, index);
      return wide + 3;
    case JAVA_OP_CONSTANT8:
      // Note: These instructions should never be wide.
      index = memory_read_m(memory, address + wide + 1);
      sprintf(instruction, "%s %d", table_java[opcode].instr, index);
      return wide + 2;
    case JAVA_OP_OFFSET16:
      // Note: These instructions should never be wide.
      offset16 = memory_read16_m(memory, address + wide + 1);
      sprintf(instruction, "%s 0x%04x (offset=%d)", table_java[opcode].instr, address + offset16, offset16);
      return wide + 3;
    case JAVA_OP_OFFSET32:
      // Note: These instructions should never be wide.
      offset = memory_read32_m(memory, address + wide + 1);
      sprintf(instruction, "%s 0x%04x (offset=%d)", table_java[opcode].instr, address + offset, offset);
      return wide + 5;
    case JAVA_OP_WARN:
      sprintf(instruction, "[%s]", table_java[opcode].instr);
      return wide + 1;
    default:
      sprintf(instruction, "<error>");
      return wide + 1;
  }

  return wide + 1;
}

void list_output_java(
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

  count = disasm_java(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  hex[0] = 0;

  for (n = 0; n < count; n++)
  {
    opcode = memory_read_m(&asm_context->memory, start + n);

    sprintf(temp, "%02x ", opcode);
    strcat(hex, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-20s %-40s\n", start, hex, instruction);
}

void disasm_range_java(
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

  while (start <= end)
  {
    count = disasm_java(memory, start, instruction, &cycles_min, &cycles_max);

     hex[0] = 0;

     for (n = 0; n < count; n++)
     {
       opcode = memory_read_m(memory, start + n);

       sprintf(temp, "%02x ", opcode);
       strcat(hex, temp);
     }

    printf("0x%04x: %-20s %-40s\n", start, hex, instruction);

    start += count;
  }
}

