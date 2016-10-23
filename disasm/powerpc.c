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

#include "disasm/powerpc.h"
#include "table/powerpc.h"

#define READ_RAM(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a+1)<<16)|(memory_read_m(memory, a+2)<<8)|memory_read_m(memory, a+3)

int get_cycle_count_powerpc(unsigned short int opcode)
{
  return -1;
}

int disasm_powerpc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int32_t offset;
  int32_t temp;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);

  n = 0;
  while(table_powerpc[n].instr != NULL)
  {
    if ((opcode & table_powerpc[n].mask) == table_powerpc[n].opcode)
    {
      uint32_t rd = (opcode >> 21) & 0x1f;
      uint32_t ra = (opcode >> 16) & 0x1f;
      uint32_t rb = (opcode >> 11) & 0x1f;
      uint32_t rc = opcode & 0x1;
      uint32_t bo = rd;
      uint32_t bi = ra;
      int16_t simm = opcode & 0xffff;
      uint16_t uimm = opcode & 0xffff;
      const char *instr = table_powerpc[n].instr;

      switch(table_powerpc[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", instr);
          break;
        case OP_RD_RA_RB:
          sprintf(instruction, "%s%s r%d, r%d, r%d",
            instr, (rc == 1) ? "." : "", rd, ra, rb);
          break;
        case OP_RA_RS_RB:
          sprintf(instruction, "%s%s r%d, r%d, r%d",
            instr, (rc == 1) ? "." : "", ra, rd, rb);
          break;
        case OP_RD_RA:
          sprintf(instruction, "%s%s r%d, r%d",
            instr, (rc == 1) ? "." : "", rd, ra);
          break;
        case OP_RD_RA_SIMM:
          sprintf(instruction, "%s%s r%d, r%d, %d (0x%04x)",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            rd, ra, simm, simm);
          break;
        case OP_RD_SIMM:
          sprintf(instruction, "%s%s r%d, %d (0x%04x)",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            rd, simm, simm);
          break;
        case OP_RA_RS_UIMM:
          sprintf(instruction, "%s%s r%d, r%d, %d (0x%04x)",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            ra, rd, uimm, uimm);
          break;
        case OP_BRANCH:
          offset = opcode & 0x03fffffc;
          if ((offset & 0x02000000) != 0) { offset |= 0xfc000000; }
          sprintf(instruction, "%s 0x%x (offset=%d)", instr, address + offset, offset);
          break;
        case OP_JUMP:
          temp = opcode & 0x03fffffc;
          if ((temp & 0x02000000) != 0) { temp |= 0xfc000000; }
          sprintf(instruction, "%s 0x%x", instr, temp);
          break;
        case OP_BRANCH_COND_BD:
          offset = opcode & 0xfffc;
          if ((offset & 0x8000) != 0) { offset |= 0xffff0000; }
          sprintf(instruction, "%s %d, %d, 0x%x (offset=%d)", instr, bo, bi, address + offset, offset);
          break;
        case OP_JUMP_COND_BD:
          temp = opcode & 0xfffc;
          if ((temp & 0x8000) != 0) { temp |= 0xffff0000; }
          sprintf(instruction, "%s %d, %d, 0x%x", instr, bo, bi, temp);
          break;
        case OP_BRANCH_COND:
          sprintf(instruction, "%s %d, %d", instr, bo, bi);
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

void list_output_powerpc(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_powerpc(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

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

void disasm_range_powerpc(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
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

    count = disasm_powerpc(memory, start, instruction, &cycles_min, &cycles_max);

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


