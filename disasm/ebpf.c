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

#include "disasm/ebpf.h"
#include "table/ebpf.h"

#define READ_RAM16(a) \
  (memory_read_m(memory, a + 0) << 8) | \
   memory_read_m(memory, a + 1)

int disasm_ebpf(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int *cycles_min,
  int *cycles_max)
{
  int n;

  uint8_t opcode = memory_read_m(memory, address);
  uint8_t regs = memory_read_m(memory, address + 1);
  //int16_t offset = memory_read16_m(memory, address + 2);
  int32_t immediate = memory_read32_m(memory, address + 4);

  int dst_reg;
  int src_reg;

  if (memory->endian == ENDIAN_LITTLE)
  {
    src_reg = regs >> 4;
    dst_reg = regs & 0xf;
  }
    else
  {
    dst_reg = regs >> 4;
    src_reg = regs & 0xf;
  }

  *cycles_min = -1;
  *cycles_max = -1;

  int bytes = 8;

  for (n = 0; table_ebpf[n].instr != NULL; n++)
  {
    if (opcode == table_ebpf[n].opcode)
    {
      switch (table_ebpf[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_ebpf[n].instr);
          return bytes;
        }
        case OP_IMM:
        {
          sprintf(instruction, "%s %d\n", table_ebpf[n].instr, immediate);
          return bytes;
        }
        case OP_REG:
        {
          sprintf(instruction, "%s r%d\n", table_ebpf[n].instr, dst_reg);
          return bytes;
        }
        case OP_REG_REG:
        {
          sprintf(instruction, "%s r%d, r%d\n",
            table_ebpf[n].instr, dst_reg, src_reg);
          return bytes;
        }
        case OP_REG_IMM:
        {
          sprintf(instruction, "%s r%d, %d\n",
            table_ebpf[n].instr, dst_reg, immediate);
          return bytes;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_ebpf(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_ebpf(
      &asm_context->memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    fprintf(asm_context->list,
      "0x%04x: %02x %02x %02x %02x %02x %02x %02x %02x %-40s\n",
      start / 8,
      memory_read_m(&asm_context->memory, start + 0),
      memory_read_m(&asm_context->memory, start + 1),
      memory_read_m(&asm_context->memory, start + 2),
      memory_read_m(&asm_context->memory, start + 3),
      memory_read_m(&asm_context->memory, start + 4),
      memory_read_m(&asm_context->memory, start + 5),
      memory_read_m(&asm_context->memory, start + 6),
      memory_read_m(&asm_context->memory, start + 7),
      instruction);

    start += count;
  }
}

void disasm_range_ebpf(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_ebpf(
      memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    printf(
      "0x%04x: %02x %02x %02x %02x %02x %02x %02x %02x %-40s\n",
      start / 8,
      memory_read_m(memory, start + 0),
      memory_read_m(memory, start + 1),
      memory_read_m(memory, start + 2),
      memory_read_m(memory, start + 3),
      memory_read_m(memory, start + 4),
      memory_read_m(memory, start + 5),
      memory_read_m(memory, start + 6),
      memory_read_m(memory, start + 7),
      instruction);

    start += count;
  }
}

