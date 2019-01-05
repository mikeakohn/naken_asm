/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/java.h"
#include "table/java.h"

int get_cycle_count_java(unsigned short int opcode)
{
  return -1;
}

int disasm_java(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  *cycles_min = 0;
  *cycles_max = 0;

  uint8_t opcode = memory_read_m(memory, address);

  switch(table_java[opcode].op_type)
  {
    case JAVA_OP_ILLEGAL:
    case JAVA_OP_NONE:
      sprintf(instruction, "%s", table_java[opcode].instr);
      return 1;
    case JAVA_OP_CONSTANT_INDEX:
    case JAVA_OP_FIELD_INDEX:
    case JAVA_OP_INTERFACE_INDEX:
    case JAVA_OP_LOCAL_INDEX:
    case JAVA_OP_METHOD_INDEX:
    case JAVA_OP_CLASS_INDEX:
    case JAVA_OP_SPECIAL_INDEX:
    case JAVA_OP_STATIC_INDEX:
    case JAVA_OP_VIRTUAL_INDEX:

    case JAVA_OP_CLASS_INDEX_TYPE:
    case JAVA_OP_INDEX_LOCAL_CONST:
    case JAVA_OP_ARRAY_TYPE:
    case JAVA_OP_CONSTANT16:
    case JAVA_OP_CONSTANT8:
    case JAVA_OP_OFFSET16:
    case JAVA_OP_OFFSET32:

    case JAVA_OP_WARN:
      sprintf(instruction, "[%s]", table_java[opcode].instr);
      return 1;
    default:
      sprintf(instruction, "<error>");
      return 1;
  }

  return 1;
}

void list_output_java(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode;

  fprintf(asm_context->list, "\n");

  disasm_java(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);
  opcode = memory_read_m(&asm_context->memory, start);

  fprintf(asm_context->list, "0x%04x: %04x %-40s", start, opcode, instruction);
}

void disasm_range_java(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int opcode;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_java(memory, start, instruction, &cycles_min, &cycles_max);
    opcode = memory_read_m(memory, start);

    printf("0x%04x: %04x %-40s\n", start, opcode, instruction);

    start += count;
  }
}

