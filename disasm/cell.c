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

#include "disasm/cell.h"
#include "table/cell.h"

#define READ_RAM(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a+1)<<16)|(memory_read_m(memory, a+2)<<8)|memory_read_m(memory, a+3)

int get_cycle_count_cell(unsigned short int opcode)
{
  return -1;
}

int disasm_cell(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  //const char *name;
  uint32_t opcode;
  //int32_t offset;
  //int32_t temp;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);

  n = 0;
  while(table_cell[n].instr != NULL)
  {
    if ((opcode & table_cell[n].mask) == table_cell[n].opcode)
    {
      //const char *instr = table_cell[n].instr;
      int32_t offset;
      uint32_t u16;
      int32_t i16;
      int32_t ro;
      int rb = (opcode >> 14) & 0x7f;
      int ra = (opcode >> 7) & 0x7f;
      int rt = (opcode >> 0) & 0x7f;
      int rc;

      switch(table_cell[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_cell[n].instr);
          break;
        case OP_RT_S10_RA:
          offset = ((opcode >> 14) & 0x3ff) << 4;
          if ((offset & (1 << 13)) != 0) { offset |= 0xffffc000; }
          sprintf(instruction, "%s r%d, %d(r%d)", table_cell[n].instr, rt, offset, ra);
          break;
        case OP_RT_RA_S10:
          i16 = (opcode >> 14) & 0x3ff;
          if ((i16 & (1 << 13)) != 0) { i16 |= 0xffffc000; }
          sprintf(instruction, "%s r%d, r%d, 0x%x (%d)", table_cell[n].instr, rt, ra, i16, i16);
          break;
        case OP_RT_RA_U10:
          u16 = (opcode >> 14) & 0x3ff;
          sprintf(instruction, "%s r%d, r%d, 0x%x (%d)", table_cell[n].instr, rt, ra, u16, u16);
          break;
        case OP_RT_RA_U7:
          u16 = (opcode >> 14) & 0x7f;
          sprintf(instruction, "%s r%d, r%d, 0x%x (%d)", table_cell[n].instr, rt, ra, u16, u16);
          break;
        case OP_RT_RA_S6:
          i16 = (opcode >> 14) & 0x7f;
          if ((i16 & (1 << 6)) != 0) { i16 |= 0xffffff80; }
          sprintf(instruction, "%s r%d, r%d, 0x%x (%d)", table_cell[n].instr, rt, ra, i16, i16);
          break;
        case OP_RT_RA:
          sprintf(instruction, "%s r%d, r%d", table_cell[n].instr, rt, ra);
          break;
        case OP_RA_RB:
          sprintf(instruction, "%s r%d, r%d", table_cell[n].instr, ra, rb);
          break;
        case OP_RA:
          sprintf(instruction, "%s r%d", table_cell[n].instr, ra);
          break;
        case OP_RT:
          sprintf(instruction, "%s r%d", table_cell[n].instr, rt);
          break;
        case OP_RT_RA_RB:
          sprintf(instruction, "%s r%d, r%d, r%d", table_cell[n].instr, rt, ra, rb);
          break;
        case OP_RA_RB_RC:
          rc = rt;
          sprintf(instruction, "%s r%d, r%d, r%d", table_cell[n].instr, rc, ra, rb);
          break;
        case OP_RT_RA_RB_RC:
          rc = rt;
          rt = (opcode >> 21) & 0x7f;
          sprintf(instruction, "%s r%d, r%d, r%d, r%d", table_cell[n].instr, rt, ra, rb, rc);
          break;
        case OP_RT_ADDRESS:
          u16 = ((opcode >> 7) & 0xffff) << 2;
          sprintf(instruction, "%s r%d, 0x%x (%d)", table_cell[n].instr, rt, u16, u16);
          break;
        case OP_RT_RELATIVE:
          i16 = ((opcode >> 7) & 0xffff) << 2;
          if ((i16 & (1 << 17)) != 0) { i16 |= 0xfffc0000; }
          sprintf(instruction, "%s r%d, 0x%x (%d)", table_cell[n].instr, rt, i16, i16);
          break;
        case OP_RT_S16:
          i16 = ((opcode >> 7) & 0xffff);
          sprintf(instruction, "%s r%d, 0x%x (%d)", table_cell[n].instr, rt, (int32_t)((int16_t)i16), (int16_t)i16);
          break;
        case OP_RT_U16:
          u16 = ((opcode >> 7) & 0xffff);
          sprintf(instruction, "%s r%d, 0x%x (%d)", table_cell[n].instr, rt, u16, u16);
          break;
        case OP_RT_U18:
          u16 = ((opcode >> 7) & 0x3ffff);
          sprintf(instruction, "%s r%d, 0x%x (%d)", table_cell[n].instr, rt, u16, u16);
          break;
        case OP_RT_S7_RA:
          offset = ((opcode >> 14) & 0x7f);
          if ((offset & (1 << 6)) != 0) { offset |= 0xffffff80; }
          sprintf(instruction, "%s r%d, %d(r%d)", table_cell[n].instr, rt, offset, ra);
          break;
        case OP_RA_S10:
          i16 = (opcode >> 14) & 0x3ff;
          if ((i16 & (1 << 9)) != 0) { i16 |= 0xfffffc00; }
          sprintf(instruction, "%s r%d, %d", table_cell[n].instr, ra, i16);
          break;
        case OP_BRANCH_RELATIVE:
          i16 = ((opcode >> 7) & 0xffff);
          if ((i16 & 0x8000) != 0) { i16 |= 0xffff0000; }
          i16 <<= 2;
          sprintf(instruction, "%s 0x%x (offset=%d)", table_cell[n].instr, address + i16, i16);
          break;
        case OP_BRANCH_ABSOLUTE:
          u16 = ((opcode >> 7) & 0xffff);
          u16 <<= 2;
          sprintf(instruction, "%s 0x%x", table_cell[n].instr, u16);
          break;
        case OP_BRANCH_RELATIVE_RT:
          i16 = ((opcode >> 7) & 0xffff);
          if ((i16 & 0x8000) != 0) { i16 |= 0xffff0000; }
          i16 <<= 2;
          sprintf(instruction, "%s r%d, 0x%x (offset=%d)", table_cell[n].instr, rt, address + i16, i16);
          break;
        case OP_HINT_RELATIVE_RO_RA:
          ro = (((opcode >> 14) & 0x3) << 7) | (opcode & 0x7f);
          if ((ro & 0x100) != 0) { ro |= 0xffffff00; }
          ro <<= 2;
          sprintf(instruction, "%s 0x%x (offset=%d), r%d", table_cell[n].instr, address + ro, ro, ra);
          break;
        case OP_HINT_ABSOLUTE_RO_I16:
          ro = (((opcode >> 23) & 0x3) << 7) | (opcode & 0x7f);
          if ((ro & 0x100) != 0) { ro |= 0xffffff00; }
          ro <<= 2;
          u16 = ((opcode >> 7) & 0xffff);
          u16 <<= 2;
          sprintf(instruction, "%s 0x%x (offset=%d), 0x%x", table_cell[n].instr, address + ro, ro, u16);
          break;
        case OP_HINT_RELATIVE_RO_I16:
          ro = (((opcode >> 23) & 0x3) << 7) | (opcode & 0x7f);
          if ((ro & 0x100) != 0) { ro |= 0xffffff00; }
          ro <<= 2;
          i16 = ((opcode >> 7) & 0xffff);
          if ((i16 & 0x8000) != 0) { i16 |= 0xffff0000; }
          i16 <<= 2;
          sprintf(instruction, "%s 0x%x (offset=%d), 0x%x (offset=%d)", table_cell[n].instr, address + ro, ro, address + i16, i16);
          break;
        case OP_RT_RA_SCALE155:
          i16 = 155 - ((opcode >> 14) & 0xff);
          sprintf(instruction, "%s r%d, r%d, %d", table_cell[n].instr, rt, ra, i16);
          break;
        case OP_RT_RA_SCALE173:
          i16 = 173 - ((opcode >> 14) & 0xff);
          sprintf(instruction, "%s r%d, r%d, %d", table_cell[n].instr, rt, ra, i16);
          break;
        case OP_U14:
          u16 = opcode & 0x3fff;
          sprintf(instruction, "%s 0x%04x", table_cell[n].instr, u16);
          break;
        case OP_RT_SA:
        case OP_RT_CA:
          u16 = (opcode >> 7) & 0x7f;
          sprintf(instruction, "%s r%d, %d", table_cell[n].instr, rt, u16);
          break;
        case OP_SA_RT:
        case OP_CA_RT:
          u16 = (opcode >> 7) & 0x7f;
          sprintf(instruction, "%s %d, r%d", table_cell[n].instr, u16, rt);
          break;
        default:
          strcpy(instruction, "???");
          break;
      }

      return 4;
    }

    n++;
  }

  strcpy(instruction, "???");

  return 0;
}

void list_output_cell(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_cell(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == -1)
    { fprintf(asm_context->list, "\n"); }
      else
    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += 4;
  }
}

void disasm_range_cell(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  uint32_t opcode;
  int cycles_min = 0,cycles_max = 0;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    opcode = memory_read32_m(memory, start);

    count = disasm_cell(memory, start, instruction, &cycles_min, &cycles_max);

    printf("0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == -1)
    { printf("\n"); }
      else
    if (cycles_min == cycles_max)
    { printf("%d\n", cycles_min); }
      else
    { printf("%d-%d\n", cycles_min, cycles_max); }

    start = start + count;
  }
}


