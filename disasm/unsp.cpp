/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/unsp.h"
#include "table/unsp.h"

static const char *regs[] = { "sp", "r1", "r2", "r3", "r4", "bp", "sr", "pc" };

static int disasm_alu(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int n,
  int is_alu_2)
{
  int opcode = memory->read16(address);
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
      // rd, [bp+imm6]
      imm6 = opcode & 0x3f;

      snprintf(instruction, length, "%s %s, [bp+%d]",
        table_unsp[n].instr,
        regs[operand_a],
        imm6);
      break;
    }
    case 1:
    {
      // rd, #imm6
      imm6 = opcode & 0x3f;

      snprintf(instruction, length, "%s %s, #%d",
        table_unsp[n].instr,
        regs[operand_a],
        imm6);
      break;
    }
    case 3:
    {
      // rd, [rs]
      switch (opn)
      {
        case 0: snprintf(temp, sizeof(temp), "[%s]", regs[operand_b]); break;
        case 1: snprintf(temp, sizeof(temp), "[%s--]", regs[operand_b]); break;
        case 2: snprintf(temp, sizeof(temp), "[%s++]", regs[operand_b]); break;
        case 3: snprintf(temp, sizeof(temp), "[++%s]", regs[operand_b]); break;
        case 4: snprintf(temp, sizeof(temp), "D:[%s]", regs[operand_b]); break;
        case 5: snprintf(temp, sizeof(temp), "D:[%s--]", regs[operand_b]); break;
        case 6: snprintf(temp, sizeof(temp), "D:[%s++]", regs[operand_b]); break;
        case 7: snprintf(temp, sizeof(temp), "D:[++%s]", regs[operand_b]); break;
      }

      snprintf(instruction, length, "%s %s, %s",
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
          // rd, rs
          snprintf(instruction, length, "%s %s, %s",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b]);
          break;
        case 1:
          // rd, rs, #imm16
          if (is_alu_2 == 0)
          {
            snprintf(instruction, length, "%s %s, %s, #0x%x",
              table_unsp[n].instr,
              regs[operand_a],
              regs[operand_b],
              memory->read16(address + 2));
          }
            else
          {
            snprintf(instruction, length, "%s %s, #0x%x",
              table_unsp[n].instr,
              regs[operand_a],
              memory->read16(address + 2));
          }
          return 4;
        case 2:
          // rd, rs, [addr16]
          if (is_alu_2 == 0)
          {
            snprintf(instruction, length, "%s %s, %s, [0x%04x]",
              table_unsp[n].instr,
              regs[operand_a],
              regs[operand_b],
              memory->read16(address + 2));
          }
            else
          {
            snprintf(instruction, length, "%s %s, [0x%04x]",
              table_unsp[n].instr,
              regs[operand_a],
              memory->read16(address + 2));
          }
          return 4;
        case 3:
          // [addr16], rs, rd
          if (is_alu_2 == 0)
          {
            snprintf(instruction, length, "%s [0x%04x], %s, %s",
              table_unsp[n].instr,
              memory->read16(address + 2),
              regs[operand_a],
              regs[operand_b]);
          }
            else
          {
            if (table_unsp[n].opcode == 0xd000)
            {
              snprintf(instruction, length, "%s %s, [0x%04x]",
                table_unsp[n].instr,
                regs[operand_a],
                memory->read16(address + 2));
            }
              else
            {
              snprintf(instruction, length, "%s [0x%04x], %s",
                table_unsp[n].instr,
                memory->read16(address + 2),
                regs[operand_a]);
            }
          }
          return 4;
        default:
          // rd, rs SHIFT <1 to 4>
          value = opn - 3;
          snprintf(instruction, length, "%s %s, %s asr %d",
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
      // rd, rs SHIFT <1 to 4>
      if (opn < 4)
      {
        value = opn + 1;
        snprintf(instruction, length, "%s %s, %s lsl %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
        else
      {
        value = opn - 3;
        snprintf(instruction, length, "%s %s, %s lsr %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
      break;
    }
    case 6:
    {
      // rd, rs ROLL <1 to 4>
      if (opn < 4)
      {
        value = opn + 1;
        snprintf(instruction, length,"%s %s, %s rol %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
        else
      {
        value = opn - 3;
        snprintf(instruction, length, "%s %s, %s ror %d",
          table_unsp[n].instr,
          regs[operand_a],
          regs[operand_b],
          value);
      }
      break;
    }
    case 7:
    {
      // rd, [imm6]
      imm6 = opcode & 0x3f;

      snprintf(instruction, length, "%s %s, [%d]",
        table_unsp[n].instr,
        regs[operand_a],
        imm6);
      break;
    }
    default:
    {
      snprintf(instruction, length, "%s",
        table_unsp[n].instr);
    }
  }

  return 2;
}

static int disasm_pop(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int n)
{
  int opcode = memory->read16(address);
  int operand_a = (opcode >> 9) & 0x7;
  int operand_b = opcode & 0x7;
  int opn = (opcode >> 3) & 0x7;

  if (opn == 0)
  {
    snprintf(instruction, length, "%s", table_unsp[n].instr);
  }
    else
  if (opn == 1)
  {
    snprintf(instruction, length, "%s %s, [%s]",
      table_unsp[n].instr,
      regs[operand_a + 1],
      regs[operand_b]);
  }
    else
  {
    snprintf(instruction, length, "%s r%d-r%d, [%s]",
      table_unsp[n].instr,
      operand_a + 1,
      operand_a + opn,
      regs[operand_b]);
  }

  return 2;
}

static int disasm_push(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int n)
{
  int opcode = memory->read16(address);
  int operand_a = (opcode >> 9) & 0x7;
  int operand_b = opcode & 0x7;
  int opn = (opcode >> 3) & 0x7;

  if (opn == 0)
  {
    snprintf(instruction, length, "%s", table_unsp[n].instr);
  }
    else
  if (opn == 1)
  {
    snprintf(instruction, length, "%s %s, [%s]",
      table_unsp[n].instr,
      regs[operand_a],
      regs[operand_b]);
  }
    else
  {
    snprintf(instruction, length, "%s r%d-r%d, [%s]",
      table_unsp[n].instr,
      operand_a - (opn - 1),
      operand_a,
      regs[operand_b]);
  }

  return 2;
}

int disasm_unsp(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int operand_a, operand_b;
  int offset, data;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory->read16(address);

  operand_a = (opcode >> 9) & 0x7;
  operand_b = opcode & 0x7;

  n = 0;
  while (table_unsp[n].instr != NULL)
  {
    if ((opcode & table_unsp[n].mask) == table_unsp[n].opcode)
    {
      switch (table_unsp[n].type)
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
            memory->read16(address + 2);

          snprintf(instruction, length, "%s 0x%04x",
            table_unsp[n].instr,
            data);
          return 4;
        }
        case UNSP_OP_MUL:
        {
          snprintf(instruction, length, "%s %s, %s",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b]);
          return 2;
        }
        case UNSP_OP_MAC:
        {
          int opn = (opcode >> 3) & 0xf;
          if (opn == 0) { opn = 16; }

          snprintf(instruction, length, "%s [%s], [%s], %d",
            table_unsp[n].instr,
            regs[operand_a],
            regs[operand_b],
            opn);
          return 2;
        }
        case UNSP_OP_JMP:
        {
          offset = opcode & 0x3f;

          if (((opcode >> 6) & 0x1) != 0) { offset = -offset; }

          snprintf(instruction, length, "%s 0x%04x (offset=%d)",
            table_unsp[n].instr,
            (address / 2) + 1 + offset,
            offset);
          return 2;
        }
        case UNSP_OP_ALU:
        {
          return disasm_alu(memory, address, instruction, length, n, 0);
        }
        case UNSP_OP_ALU_2:
        {
          return disasm_alu(memory, address, instruction, length, n, 1);
        }
        case UNSP_OP_POP:
        {
          return disasm_pop(memory, address, instruction, length, n);
        }
        case UNSP_OP_PUSH:
        {
          return disasm_push(memory, address, instruction, length, n);
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

void list_output_unsp(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int count;
  int n;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_unsp(
      memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    for (n = 0; n < count; n += 2)
    {
      uint16_t data = memory->read16(start + n);

      if (n == 0)
      {
        fprintf(asm_context->list, "0x%04x: %04x %s\n", start / 2, data, instruction);
      }
        else
      {
        fprintf(asm_context->list, "        %04x\n", data);
      }
    }

    start += count;
  }
}

void disasm_range_unsp(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-8s %-9s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("-------  --------- ------------------------------           ------\n");

  while (start <= end)
  {
    count = disasm_unsp(
      memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    for (n = 0; n < count; n += 2)
    {
      uint16_t data = memory->read16(start + n);

      if (n == 0)
      {
        printf("0x%04x: %04x %s\n", start / 2, data, instruction);
      }
        else
      {
        printf("        %04x\n", data);
      }
    }

    start = start + count;
  }
}

