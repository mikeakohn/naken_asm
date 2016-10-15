/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/riscv.h"
#include "table/riscv.h"

#define READ_RAM(a) (memory_read_m(memory, a+3)<<24)|(memory_read_m(memory, a+2)<<16)|(memory_read_m(memory, a+1)<<8)|memory_read_m(memory, a)

int get_cycle_count_riscv(unsigned short int opcode)
{
  return -1;
}

int disasm_riscv(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);

  n = 0;
  while(table_riscv[n].instr != NULL)
  {
    if ((opcode & table_riscv[n].mask) == table_riscv[n].opcode)
    {
      uint32_t rd = (opcode >> 7) & 0x1f;
      uint32_t rs1 = (opcode >> 15) & 0x1f;
      uint32_t rs2 = (opcode >> 20) & 0x1f;
      const char *instr = table_riscv[n].instr;

      switch(table_riscv[n].type)
      {
        case OP_R_TYPE:
          sprintf(instruction, "%s r%d, r%d, r%d", instr, rd, rs1, rs2);
          break;
        case OP_I_TYPE:
        case OP_S_TYPE:
        case OP_SB_TYPE:
        case OP_U_TYPE:
        case OP_UJ_TYPE:
        case OP_SHIFT:
        case OP_FENCE:
        case OP_FFFF:
        case OP_READ:
        case OP_LR:
        case OP_STD_EXT:
        case OP_R_FP1:
        case OP_R_FP2:
        case OP_R_FP3:
        case OP_R_FP2_RM:
        case OP_R_FP3_RM:
        case OP_R_FP4_RM:
        default:
          strcpy(instruction, "???");
          break;
      }

      return 4;
    }

    n++;
  }

  strcpy(instruction, "???");

  return -1;
}

void list_output_riscv(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_riscv(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += 4;
  }
}

void disasm_range_riscv(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_riscv(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", READ_RAM(start + n));
      strcat(bytes, temp);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: %-9s %-40s ?\n", start, bytes, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %-9s %-40s %d\n", start, bytes, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-9s %-40s %d-%d\n", start, bytes, instruction, cycles_min, cycles_max);
    }

    start = start + count;
  }
}


