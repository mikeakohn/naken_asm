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

#include "disasm/avr8.h"
#include "table/avr8.h"

//#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) memory_read_m(memory, a)|(memory_read_m(memory, a+1)<<8)

int get_register_avr8(char *token)
{
  int n;
  int r;

  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[1] == 0) return -1;

    r = 0; n = 1;
    while(1)
    {
      if (token[n] == 0) return r;
      if (token[n] < '0' || token[n] > '9') return -1;
      r = (r * 10) + (token[n] - '0');
      n++;
    }
  }

  return -1;
}

int get_cycle_count_avr8(uint16_t opcode)
{
  int n = 0;
  while(table_avr8[n].instr != NULL)
  {
    if (table_avr8[n].opcode == opcode)
    {
      return table_avr8[n].cycles_min;
    }
    n++;
  }

  return -1;
}

int disasm_avr8(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int n;
  int rd,rr,k;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM16(address);

  n = 0;
  while(table_avr8[n].instr != NULL)
  {
    if ((opcode & table_avr8[n].mask) == table_avr8[n].opcode)
    {
      *cycles_min = table_avr8[n].cycles_min;
      *cycles_max = table_avr8[n].cycles_max;

      switch(table_avr8[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_avr8[n].instr);
          return 2;
        case OP_BRANCH_S_K:
          k = (opcode >> 3) & 0x7f;
          if ((k & 0x40) != 0) { k = (char)(0x80 | k); }
          sprintf(instruction, "%s %d, 0x%x (%d)", table_avr8[n].instr, opcode & 0x7, (address / 2) + 1 + k, k);
          return 2;
        case OP_BRANCH_K:
          k = (opcode >> 3) & 0x7f;
          if ((k & 0x40) != 0) { k = (char)(0x80 | k); }
          sprintf(instruction, "%s 0x%x (%d)", table_avr8[n].instr, (address/2)+1+k, k);
          return 2;
        case OP_TWO_REG:
          rd = (opcode >> 4) & 0x1f;
          rr = ((opcode & 0x200) >> 5) | ((opcode) & 0xf);
          sprintf(instruction, "%s r%d, r%d", table_avr8[n].instr, rd, rr);
          return 2;
        case OP_REG_IMM:
          rd = ((opcode >> 4) & 0xf) + 16;
          k = ((opcode & 0xf00) >> 4) | (opcode & 0xf);
          sprintf(instruction, "%s r%d, 0x%x", table_avr8[n].instr, rd, k);
          return 2;
        case OP_ONE_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_BIT:
          rd = (opcode >> 4) & 0x1f;
          k = opcode & 0x7;
          sprintf(instruction, "%s r%d, %d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_REG_IMM_WORD:
          rd = (((opcode >> 4) & 0x3) << 1) + 24;
          k = ((opcode & 0xc0) >> 2) | (opcode & 0xf);
          sprintf(instruction, "%s r%d, %d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_IOREG_BIT:
          rd = (opcode >> 3) & 0x1f;
          k = opcode & 0x7;
          sprintf(instruction, "%s 0x%x, %d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_SREG_BIT:
          k = (opcode >> 4) & 0x7;
          sprintf(instruction, "%s %d", table_avr8[n].instr, k);
          return 2;
        case OP_REG_4:
          rd = ((opcode >> 4) & 0xf) + 16;
          sprintf(instruction, "%s r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_IN:
          rd = (opcode >> 4) & 0xf;
          k = ((opcode & 0x600) >> 5) | (opcode & 0xf);
          sprintf(instruction, "%s r%d, 0x%x", table_avr8[n].instr, rd, k);
          return 2;
        case OP_OUT:
          rd = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x600) >> 5) | (opcode & 0xf);
          sprintf(instruction, "%s 0x%x, r%d", table_avr8[n].instr, k, rd);
          return 2;
        case OP_MOVW:
          rd = ((opcode >> 4) & 0xf) << 1;
          rr = ( opcode & 0xf) << 1;
          sprintf(instruction, "%s r%d, r%d", table_avr8[n].instr, rd, rr);
          return 2;
        case OP_RELATIVE:
          k = opcode & 0xfff;
          if (k & 0x800) { k = -(((~k) & 0xfff) + 1); }
          sprintf(instruction, "%s 0x%x (%d)", table_avr8[n].instr, (address/2)+1+k, k);
          return 2;
        case OP_JUMP:
          k = ((((opcode & 0x1f0) >> 3) | (opcode & 0x1)) << 16) | READ_RAM16(address + 2);
          sprintf(instruction, "%s 0x%x", table_avr8[n].instr, k);
          return 4;
        case OP_SPM_Z_PLUS:
          sprintf(instruction, "%s Z+", table_avr8[n].instr);
          return 2;
        case OP_REG_X:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, X", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Y:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, Y", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Z:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, Z", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_X_PLUS:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, X+", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Y_PLUS:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, Y+", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Z_PLUS:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, Z+", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_MINUS_X:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, -X", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_MINUS_Y:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, -Y", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_MINUS_Z:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s r%d, -Z", table_avr8[n].instr, rd);
          return 2;
        case OP_X_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s X, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Y_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s Y, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Z_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s Z, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_X_PLUS_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s X+, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Y_PLUS_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s Y+, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Z_PLUS_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s Z+, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_MINUS_X_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s -X, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_MINUS_Y_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s -Y, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_MINUS_Z_REG:
          rd = (opcode >> 4) & 0x1f;
          sprintf(instruction, "%s -Z, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_FMUL:
          rd = ((opcode >> 4) & 0x7) + 16;
          rr = (opcode & 0x7) + 16;
          sprintf(instruction, "%s r%d, r%d", table_avr8[n].instr, rd, rr);
          return 2;
        case OP_MULS:
          rd = ((opcode >> 4) & 0xf) + 16;
          rr = (opcode & 0xf) + 16;
          sprintf(instruction, "%s r%d, r%d", table_avr8[n].instr, rd, rr);
          return 2;
        case OP_DATA4:
          k = (opcode >> 4) & 0xf;
          sprintf(instruction, "%s %d", table_avr8[n].instr, k);
          return 2;
        case OP_REG_SRAM:
          rd = (opcode >> 4) & 0x1f;
          k = READ_RAM16(address + 2);
          sprintf(instruction, "%s r%d, 0x%x", table_avr8[n].instr, rd, k);
          return 4;
        case OP_SRAM_REG:
          rr = (opcode >> 4) & 0x1f;
          k = READ_RAM16(address + 2);
          sprintf(instruction, "%s 0x%x, r%d", table_avr8[n].instr, k, rr);
          return 4;
        case OP_REG_Y_PLUS_Q:
          rd = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x2000) >> 8) | ((opcode & 0xc00) >> 7) | (opcode & 0x7);
          sprintf(instruction, "%s r%d, Y+%d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_REG_Z_PLUS_Q:
          rd = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x2000) >> 8) | ((opcode & 0xc00) >> 7) | (opcode & 0x7);
          sprintf(instruction, "%s r%d, Z+%d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_Y_PLUS_Q_REG:
          rr = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x2000) >> 8) | ((opcode & 0xc00) >> 7) | (opcode & 0x7);
          sprintf(instruction, "%s Y+%d, r%d", table_avr8[n].instr, k, rr);
          return 2;
        case OP_Z_PLUS_Q_REG:
          rr = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x2000) >> 8) | ((opcode & 0xc00) >> 7) | (opcode & 0x7);
          sprintf(instruction, "%s Z+%d, r%d", table_avr8[n].instr, k, rr);
          return 2;
        default:
          sprintf(instruction, "%s", table_avr8[n].instr);
          return 2;
      }
    }
    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_avr8(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  int count,opcode;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_avr8(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read_m(&asm_context->memory, start) | (memory_read_m(&asm_context->memory, start + 1) << 8);

    fprintf(asm_context->list, "0x%04x: %04x %-40s cycles: ", start / 2, opcode, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    for (n = 2; n < count; n += 2)
    {
      opcode = memory_read_m(&asm_context->memory, start + n) |
              (memory_read_m(&asm_context->memory, start + n + 1) << 8);
      fprintf(asm_context->list, "        %04x\n", opcode);
    }

    start += count;
  }
}

void disasm_range_avr8(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min=0,cycles_max=0;
  int count,opcode;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_avr8(memory, start, instruction, &cycles_min, &cycles_max);

    //opcode=(memory_read_m(memory, address)<<8)|memory_read_m(>memory, address+1);
    opcode = READ_RAM16(start);

    if (cycles_min < 1)
    {
      printf("0x%04x: %04x %-40s ?\n", start / 2, opcode, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: %04x %-40s %d\n", start / 2, opcode, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %04x %-40s %d-%d\n", start / 2, opcode, instruction, cycles_min, cycles_max);
    }

    for (n = 2; n < count; n += 2)
    {
      opcode = READ_RAM16(start + n);
      printf("     %04x\n", opcode);
    }

    start = start + count;
  }
}

