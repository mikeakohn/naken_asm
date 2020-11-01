/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/sparc.h"
#include "table/sparc.h"

int get_cycle_count_sparc(unsigned short int opcode)
{
  return -1;
}

int disasm_sparc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  const char *cc_value[] = { "icc", "?", "xcc", "?" };
  uint32_t opcode;
  int32_t offset;
  int annul, pt, cc;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read32_m(memory, address);

  n = 0;

  while (table_sparc[n].instr != NULL)
  {
    if ((opcode & mask_sparc[table_sparc[n].type]) == table_sparc[n].opcode)
    {
      const uint32_t rd = (opcode >> 25) & 0x1f;
      const uint32_t rs1 = (opcode >> 14) & 0x1f;
      const uint32_t rs2 = opcode & 0x1f;
      const char *instr = table_sparc[n].instr;
      int16_t simm13 = opcode & 0x1fff;

      if ((simm13 & 0x1000) != 0) { simm13 |= 0xf000; }

      switch (table_sparc[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", instr);
          break;
        case OP_REG_REG_REG:
          sprintf(instruction, "%s r%d, r%d, r%d", instr, rd, rs2, rs1);
          break;
        case OP_REG_SIMM13_REG:
          sprintf(instruction, "%s r%d, %d, r%d", instr, rd, simm13, rs1);
          break;
        case OP_FREG_FREG_FREG_FREG:
          //sprintf(instruction, "%s r%d, %d, r%d", instr, rd, simm13, rs1);
          sprintf(instruction, "%s ???", instr);
          break;
        case OP_FREG_FREG_IMM5_FREG:
          //sprintf(instruction, "%s r%d, %d, r%d", instr, rd, simm13, rs1);
          sprintf(instruction, "%s ???", instr);
          break;
        case OP_FREG_FREG_FREG:
          //sprintf(instruction, "%s r%d, %d, r%d", instr, rd, simm13, rs1);
          sprintf(instruction, "%s ???", instr);
          break;
        case OP_BRANCH:
          annul = (opcode >> 29) & 1;
          offset = opcode & 0x3fffff;
          if ((offset & 0x200000) != 0) { offset |= 0xffc00000; }
          offset *= 4;

          sprintf(instruction, "%s%s 0x%04x (offset=%d)",
            instr, annul == 1 ? ",a" : "", address + offset, offset);
          break;
        case OP_BRANCH_P:
          annul = (opcode >> 29) & 1;
          pt = (opcode >> 19) & 1;
          cc = (opcode >> 20) & 3;
          offset = opcode & 0x7ffff;
          if ((offset & 0x40000) != 0) { offset |= 0xfff80000; }
          offset *= 4;

          sprintf(instruction, "%s%s%s %s, 0x%04x (offset=%d)",
            instr,
            annul == 1 ? ",a" : "",
            pt == 1 ? ",pt" : ",pn",
            cc_value[cc],
            address + offset,
            offset);
          break;
        case OP_BRANCH_P_REG:
          annul = (opcode >> 29) & 1;
          pt = (opcode >> 19) & 1;
          offset = ((opcode & 0x300000) >> 6) | (opcode & 0x3fff);
          if ((offset & 0x8000) != 0) { offset |= 0xffff0000; }
          offset *= 4;

          sprintf(instruction, "%s%s%s r%d, 0x%04x (offset=%d)",
            instr,
            annul == 1 ? ",a" : "",
            pt == 1 ? ",pt" : ",pn",
            rs1,
            address + offset,
            offset);
          break;
        case OP_CALL:
          offset = opcode & 0x3fffffff;
          if ((offset & 0x20000000) != 0) { offset |= 0xc0000000; }
          offset *= 4;

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            instr,
            address + offset,
            offset);
          break;
        case OP_IMM_ASI_REG_REG:
        case OP_ASI_REG_REG:
          // FIXME: Implement.
          sprintf(instruction, "%s ???", instr);
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

  return 4;
}

void list_output_sparc(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_sparc(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == -1)
    {
      fprintf(asm_context->list, "\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      fprintf(asm_context->list, "%d\n", cycles_min);
    }
      else
    {
      fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
    }

    start += 4;
  }
}

void disasm_range_sparc(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
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

    count = disasm_sparc(memory, start, instruction, &cycles_min, &cycles_max);

    printf("0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == -1)
    {
      printf("\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("%d\n", cycles_min);
    }
      else
    {
      printf("%d-%d\n", cycles_min, cycles_max);
    }

    if (count == 0) { count = 4; }

    start = start + count;
  }
}

