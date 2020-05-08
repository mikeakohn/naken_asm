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

#include "disasm/tms9900.h"
#include "table/tms9900.h"

#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

static uint16_t masks[] =
{
  0xf000, // OP_DUAL,
  0xfc00, // OP_DUAL_MULTIPLE,
  0xfc00, // OP_XOP,
  0xffc0, // OP_SINGLE,
  0xfc00, // OP_CRU_MULTIBIT,
  0xff00, // OP_CRU_SINGLEBIT,
  0xff00, // OP_JUMP,
  0xff00, // OP_SHIFT,
  0xffe0, // OP_IMMEDIATE,
  0xffe0, // OP_INT_REG_LD,
  0xffe0, // OP_INT_REG_ST,
  0xffe0, // OP_RTWP,
  0xffe0, // OP_EXTERNAL,
};

int get_cycle_count_tms9900(unsigned short int opcode)
{
  return -1;
}

static void get_operand(char *operand, int t, int reg, int data)
{
  if (t == 0) { sprintf(operand, "r%d", reg); }
  else if (t == 1) { sprintf(operand, "*r%d", reg); }
  else if (t == 3) { sprintf(operand, "*r%d+", reg); }
  else
  {
    if (reg == 0)
    {
      sprintf(operand, "@0x%04x", data);
    }
      else
    {
      sprintf(operand, "@0x%04x(r%d)", data, reg);
    }
  }
}

int disasm_tms9900(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  char operand_s[32];
  char operand_d[32];
  int data_s = 0;
  int data_d = 0;
  int count = 2;
  int opcode;
  int ts,td;
  int s,d;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM16(address);

  n = 0;

  while(table_tms9900[n].instr != NULL)
  {
    if ((opcode&masks[table_tms9900[n].type]) == table_tms9900[n].opcode)
    {
      *cycles_min = table_tms9900[n].cycles_min;
      *cycles_max = table_tms9900[n].cycles_max;

      switch(table_tms9900[n].type)
      {
        case OP_DUAL:
        {
          ts = (opcode >> 4) & 0x3;
          td = (opcode >> 10) & 0x3;
          s = opcode & 0xf;
          d = (opcode >> 6) & 0xf;
          if (ts == 2) { address += 2; count += 2; data_s=READ_RAM16(address); }
          if (td == 2) { address += 2; count += 2; data_d=READ_RAM16(address); }
          get_operand(operand_s, ts, s, data_s);
          get_operand(operand_d, td, d, data_d);
          sprintf(instruction, "%s %s, %s", table_tms9900[n].instr, operand_s, operand_d);
          return count;
        }
        case OP_DUAL_MULTIPLE:
        case OP_XOP:
        {
          ts = (opcode >> 4) & 0x3;
          td = 0;
          s = opcode & 0xf;
          d = (opcode >> 6) & 0xf;
          if (ts == 2) { address += 2; count += 2; data_s=READ_RAM16(address); }
          get_operand(operand_s, ts, s, data_s);
          get_operand(operand_d, td, d, data_d);
          sprintf(instruction, "%s %s, %s", table_tms9900[n].instr, operand_s, operand_d);
          return count;
        }
        case OP_SINGLE:
        {
          ts = (opcode >> 4) & 0x3;
          s = opcode & 0xf;
          if (ts == 2) { address += 2; count += 2; data_s=READ_RAM16(address); }
          get_operand(operand_s, ts, s, data_s);
          sprintf(instruction, "%s %s", table_tms9900[n].instr, operand_s);
          return count;
        }
        case OP_CRU_MULTIBIT:
        {
          ts = (opcode >> 4) & 0x3;
          s = opcode & 0xf;
          data_d = (opcode >> 6) & 0xf;
          if (ts == 2) { address += 2; count += 2; data_s=READ_RAM16(address); }
          get_operand(operand_s, ts, s, data_s);
          sprintf(instruction, "%s %s, %d", table_tms9900[n].instr, operand_s, data_d);
          return count;
        }
        case OP_CRU_SINGLEBIT:
        {
          data_s = ((char)(opcode & 0xff));
          sprintf(instruction, "%s %d", table_tms9900[n].instr, data_s);
          return count;
        }
        case OP_JUMP:
        {
          data_s = ((char)(opcode & 0xff));
          int offset = data_s * 2;
          sprintf(instruction, "%s 0x%04x (%d)", table_tms9900[n].instr, address + 2 + offset, offset);
          return count;
        }
        case OP_SHIFT:
        {
          s = opcode & 0xf;
          data_d = (opcode >> 4) & 0xf;
          sprintf(instruction, "%s r%d, %d", table_tms9900[n].instr, s, data_d);
          return count;
        }
        case OP_IMMEDIATE:
        {
          s = opcode & 0xf;
          data_d = READ_RAM16(address + 2);
          sprintf(instruction, "%s r%d, 0x%04x (%d)",
            table_tms9900[n].instr, s, data_d, data_d);
          return count + 2;
        }
        case OP_INT_REG_LD:
        {
          data_d = READ_RAM16(address + 2);
          sprintf(instruction, "%s %d", table_tms9900[n].instr, data_d);
          return count + 2;
        }
        case OP_INT_REG_ST:
        {
          s = opcode & 0xf;
          sprintf(instruction, "%s r%d", table_tms9900[n].instr, s);
          return count;
        }
        case OP_RTWP:
        case OP_EXTERNAL:
        default:
          sprintf(instruction, "%s", table_tms9900[n].instr);
          break;
      }

      return count;
    }

    n++;
  }

  sprintf(instruction, "???");

  return 2;
}

void list_output_tms9900(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  int count;
  int n;
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = (memory_read_m(&asm_context->memory, start) << 8) | memory_read_m(&asm_context->memory, start + 1);
    count = disasm_tms9900(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%04x: %04x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    for (n = 2; n < count; n = n + 2)
    {
      opcode = (memory_read_m(&asm_context->memory, start + n) << 8) | memory_read_m(&asm_context->memory, start + n + 1);
      fprintf(asm_context->list, "0x%04x: %04x\n", start + n, opcode);
    }

    start += count;
  }
}

void disasm_range_tms9900(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_tms9900(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[8];
      sprintf(temp, "%04x ", READ_RAM16(start + n));
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

