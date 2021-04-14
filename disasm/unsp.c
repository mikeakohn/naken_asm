/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/unsp.h"
#include "table/unsp.h"

static const char *regs[] = { "sp", "r1", "r2", "r3", "r4", "bp", "sr", "pc" };

int get_cycle_count_unsp(uint16_t opcode)
{
  return -1;
}

static int disasm_alu(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int n)
{
  int opcode = memory_read16_m(memory, address);
  int opcode_1 = (opcode >> 6) & 0x7;
  int operand_a = (opcode >> 9) & 0x7;
  int operand_b = opcode & 0x7;
  int opn = (opcode >> 3) & 0x7;
  int imm6, value;
  char temp[32];

  switch (opcode_1)
  {
    case 0:
    {
      imm6 = opcode & 0x3f;

      sprintf(instruction, "%s %s, [bp+%d]",
        table_unsp[n].instr,
        regs[operand_a],
        imm6);
      break;
    }
    case 1:
    {
      imm6 = opcode & 0x3f;

      sprintf(instruction, "%s %s, #%d",
        table_unsp[n].instr,
        regs[operand_a],
        imm6);
      break;
    }
    case 3:
    {
      switch (opn)
      {
        case 0: sprintf(temp, "[%s]", regs[operand_b]); break;
        case 1: sprintf(temp, "[%s--]", regs[operand_b]); break;
        case 2: sprintf(temp, "[%s++]", regs[operand_b]); break;
        case 3: sprintf(temp, "[++%s]", regs[operand_b]); break;
        case 4: sprintf(temp, "D:[%s]", regs[operand_b]); break;
        case 5: sprintf(temp, "D:[%s--]", regs[operand_b]); break;
        case 6: sprintf(temp, "D:[%s++]", regs[operand_b]); break;
        case 7: sprintf(temp, "D:[++%s]", regs[operand_b]); break;
      }

      sprintf(instruction, "%s %s, %s",
        table_unsp[n].instr,
        regs[operand_a],
        temp);
      break;
    }
    case 4:
    {
      switch (opn)
      {
        case 0:
          sprintf(instruction, "%s %s, %s",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b]);
          break;
        case 1:
          sprintf(instruction, "%s %s, #0x%d",
            table_unsp[n].instr,
            regs[operand_a],
            memory_read16_m(memory, address + 2));
          return 4;
        case 2:
          sprintf(instruction, "%s %s, %s, [0x%04x]",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b],
            memory_read16_m(memory, address + 2));
          return 4;
        case 3:
          sprintf(instruction, "%s [0x%04x], %s, %s",
            table_unsp[n].instr,
            memory_read16_m(memory, address + 2),
            regs[operand_a],
            regs[operand_b]);
          return 4;
        default:
          value = opn - 3;
          sprintf(instruction, "%s %s, %s asr %d",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b],
            value);
          break;
      }
      break;
    }
    case 5:
    {
      if (opn < 4)
      {
        value = opn + 1;
        sprintf(instruction, "%s %s, %s lsl %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
        else
      {
        value = opn - 3;
        sprintf(instruction, "%s %s, %s lsr %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
      break;
    }
    case 6:
    {
      if (opn < 4)
      {
        value = opn + 1;
        sprintf(instruction, "%s %s, %s rol %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
        else
      {
        value = opn - 3;
        sprintf(instruction, "%s %s, %s ror %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
      break;
    }
    case 7:
    {
      imm6 = opcode & 0x3f;

      sprintf(instruction, "%s %s, [%d]",
        table_unsp[n].instr,
        regs[operand_a],
        imm6);
      break;
    }
    default:
    {
      sprintf(instruction, "%s",
        table_unsp[n].instr);
    }
  }

  return 2;
}

static int disasm_stack(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int n)
{
  int opcode = memory_read16_m(memory, address);
  int operand_a = (opcode >> 9) & 0x7;
  int operand_b = opcode & 0x7;
  int opn = (opcode >> 3) & 0x7;

  if (opn == 0)
  {
    sprintf(instruction, "%s", table_unsp[n].instr);
  }
    else
  if (opn == 1)
  {
    sprintf(instruction, "%s %s, [%s]",
      table_unsp[n].instr,
      regs[operand_a],
      regs[operand_b]);
  }
    else
  {
    sprintf(instruction, "%s %s-%s, [%s]",
      table_unsp[n].instr,
      regs[operand_a - (opn - 1)],
      regs[operand_a],
      regs[operand_b]);
  }

  return 2;
}

int disasm_unsp(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int operand_a, operand_b;
  int offset, data;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read16_m(memory, address);

  operand_a = (opcode >> 9) & 0x7;
  operand_b = opcode & 0x7;

  n = 0;
  while (table_unsp[n].instr != NULL)
  {
    if ((opcode & table_unsp[n].mask) == table_unsp[n].opcode)
    {
      switch(table_unsp[n].type)
      {
        case UNSP_OP_NONE:
        {
          strcpy(instruction, table_unsp[n].instr);
          return 2;
        }
        case UNSP_OP_GOTO:
        {
          data =
            ((opcode & 0x3f) << 16) |
            memory_read16_m(memory, address + 2);

          sprintf(instruction, "%s 0x%04x",
            table_unsp[n].instr,
            data);
          return 4;
        }
        case UNSP_OP_MUL:
        {
          sprintf(instruction, "%s %s, %s",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b]);
          return 2;
        }
        case UNSP_OP_JMP:
        {
          offset = opcode & 0x3f;

          if (((opcode >> 6) & 0x1) == 0) { offset = -offset; }

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_unsp[n].instr,
            address + 2 + offset,
            offset);
          return 2;
        }
        case UNSP_OP_ALU:
        {
          return disasm_alu(memory, address, instruction, n);
        }
        case UNSP_OP_STACK:
        {
          return disasm_stack(memory, address, instruction, n);
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

  return 1;
}

void list_output_unsp(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char temp[32];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_unsp(
      &asm_context->memory,
      start,
      instruction,
      &cycles_min,
      &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp2[4];
      sprintf(temp2, " %02x", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-8s %-40s cycles: ", start, temp, instruction);

    if (cycles_min == 0)
    {
      fprintf(asm_context->list, "?\n");
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

    start += count;
  }
}

void disasm_range_unsp(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-8s %-9s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("-------  --------- ------------------------------           ------\n");

  while(start <= end)
  {
    count = disasm_unsp(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp2[4];
      sprintf(temp2, " %02x", memory_read_m(memory, start + n));
      strcat(temp, temp2);
    }

    printf("0x%04x: %-10s %-40s ", start, temp, instruction);

    if (cycles_min == 0)
    {
      printf("?\n");
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

    start = start + count;
  }
}

