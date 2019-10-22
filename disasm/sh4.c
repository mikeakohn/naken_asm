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

#include "disasm/sh4.h"
#include "table/sh4.h"

int get_cycle_count_sh4(unsigned short int opcode)
{
  return -1;
}

int disasm_sh4(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint16_t opcode;
  int rm, rn;
  int8_t imm_s8;
  uint8_t imm_u8;
  int n;

  opcode = memory_read16_m(memory, address);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;

  while (table_sh4[n].instr != NULL)
  {
    if ((opcode & table_sh4[n].mask) == table_sh4[n].opcode)
    {
      switch (table_sh4[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_sh4[n].instr);
          return 2;
        }
        case OP_REG_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d, r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_IMM_REG:
        {
          imm_s8 = (int8_t)(opcode & 0xff);
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s #%d, r%d", table_sh4[n].instr, imm_s8, rn);
          return 2;
        }
        case OP_IMM_R0:
        {
          imm_u8 = (uint16_t)(opcode & 0xff);
          sprintf(instruction, "%s #0x%02x, r0", table_sh4[n].instr, imm_u8);
          return 2;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }

    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_sh4(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_sh4(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: %04x %-40s\n", start / 2, opcode, instruction);

    start += count;
  }
}

void disasm_range_sh4(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_sh4(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: %04x %-40s\n", start / 2, opcode, instruction);

    start = start + count;
  }
}

