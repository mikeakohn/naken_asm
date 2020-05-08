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

#include "disasm/tms340.h"
#include "table/tms340.h"

int get_cycle_count_tms340(unsigned short int opcode)
{
  return -1;
}

int disasm_tms340(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  char operand[32];
  uint32_t start = address;
  uint32_t ilw;
  int16_t displacement;
  int opcode;
  int n, i;
  char r;
  int rs, rd;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read16_m(memory, address);

  address += 2;

  r = (opcode & 0x0010) == 0 ? 'a' : 'b';
  rs = (opcode >> 5) & 0xf;
  rd = opcode & 0xf;

  for (n = 0; table_tms340[n].instr != NULL; n++)
  {
    if ((opcode & table_tms340[n].mask) == table_tms340[n].opcode)
    {
      sprintf(instruction, "%s", table_tms340[n].instr);

      for (i = 0; i < table_tms340[n].operand_count; i++)
      {
        if (i == 0)
        {
          strcat(instruction, " ");
        }
          else
        {
          strcat(instruction, ", ");
        }

        switch (table_tms340[n].operand_types[i])
        {
          case OP_RS:
            sprintf(operand, "%c%d\n", r, rs);
            strcat(instruction, operand);
            break;
          case OP_RD:
            sprintf(operand, "%c%d\n", r, rd);
            strcat(instruction, operand);
            break;
          case OP_P_RS:
            sprintf(operand, "*%c%d\n", r, rs);
            strcat(instruction, operand);
            break;
          case OP_P_RD:
            sprintf(operand, "*%c%d\n", r, rd);
            strcat(instruction, operand);
            break;
          case OP_P_RS_DISP:
            displacement = memory_read16_m(memory, address);
            sprintf(operand, "*%c%d(%d)\n", r, rs, displacement);
            strcat(instruction, operand);
            address += 2;
            break;
          case OP_P_RD_DISP:
            displacement = memory_read16_m(memory, address);
            sprintf(operand, "*%c%d(%d)\n", r, rd, displacement);
            strcat(instruction, operand);
            address += 2;
            break;
          case OP_P_RS_P:
            sprintf(operand, "*%c%d+\n", r, rs);
            strcat(instruction, operand);
            break;
          case OP_P_RD_P:
            sprintf(operand, "*%c%d+\n", r, rd);
            strcat(instruction, operand);
            break;
          case OP_P_RS_XY:
            sprintf(operand, "*%c%d.XY\n", r, rd);
            strcat(instruction, operand);
            break;
          case OP_P_RD_XY:
            sprintf(operand, "*%c%d.XY\n", r, rs);
            strcat(instruction, operand);
            break;
          case OP_MP_RS:
            sprintf(operand, "-*%c%d\n", r, rd);
            strcat(instruction, operand);
            break;
          case OP_MP_RD:
            sprintf(operand, "-*%c%d\n", r, rs);
            strcat(instruction, operand);
            break;
          case OP_ADDRESS:
            break;
          case OP_AT_ADDR:
            ilw = memory_read16_m(memory, address);
            ilw |= memory_read16_m(memory, address + 2) << 16;
            sprintf(operand, "@0x%08x\n", ilw);
            strcat(instruction, operand);
            address += 4;
            break;
          case OP_LIST:
          case OP_B:
          case OP_F:
            strcat(instruction, (opcode & 0x0200) == 0 ? "0" : "1");
            break;
          case OP_K:
          case OP_L:
          case OP_N:
            break;
          case OP_Z:
            strcat(instruction, (opcode & 0x0080) == 0 ? "0" : "1");
            break;
          case OP_FE:
          case OP_FS:
          case OP_IL:
          case OP_IW:
          case OP_NN:
            break;
          case OP_XY:
            strcat(instruction, "XY");
            break;
        }
      }

      return address - start;
    }
  }

  sprintf(instruction, "???");

  return address - start;
}

void list_output_tms340(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  int count;
  int n;
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    opcode = (memory_read_m(&asm_context->memory, start) << 8) |
              memory_read_m(&asm_context->memory, start + 1);

    count = disasm_tms340(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%04x: %04x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == cycles_max)
    {
      fprintf(asm_context->list, "%d\n", cycles_min);
    }
      else
    {
      fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
    }

    for (n = 2; n < count; n = n + 2)
    {
      opcode = (memory_read_m(&asm_context->memory, start + n) << 8) | memory_read_m(&asm_context->memory, start + n + 1);
      fprintf(asm_context->list, "0x%04x: %04x\n", start + n, opcode);
    }

    start += count;
  }
}

void disasm_range_tms340(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_tms340(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp[8];
      sprintf(temp, "%04x ", memory_read16_m(memory, start + n));
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

