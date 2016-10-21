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
      int16_t simm = opcode & 0xffff;
      const char *instr = table_powerpc[n].instr;

      switch(table_powerpc[n].type)
      {
        case OP_R_R_R:
          sprintf(instruction, "%s%s r%d, r%d, r%d",
            instr, (rc == 1) ? "." : "", rd, ra, rb);
          break;
        case OP_R_R:
          sprintf(instruction, "%s%s r%d, r%d",
            instr, (rc == 1) ? "." : "", rd, ra);
          break;
        case OP_R_R_SIMM:
          sprintf(instruction, "%s%s r%d, r%d, %d",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            rd, ra, simm);
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
  char bytes[10];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_powerpc(memory, start, instruction, &cycles_min, &cycles_max);

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


