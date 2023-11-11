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
#include <string.h>

#include "disasm/f100_l.h"
#include "table/f100_l.h"

int disasm_f100_l(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int immediate;
  int jump_address;
  int n;

  opcode = memory->read16(address);
  int r = (opcode >> 8) & 0x3;
  int i = (opcode >> 11) & 0x1;
  int b = (opcode & 0xf);

  *cycles_min = -1;
  *cycles_max = -1;

  for (n = 0; table_f100_l[n].instr != NULL; n++)
  {
    if ((opcode & table_f100_l[n].mask) == table_f100_l[n].opcode)
    {
      switch (table_f100_l[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_f100_l[n].instr);
          return 2;
        }
        case OP_ALU:
        {
          if (i == 0)
          {
            immediate = opcode & 0x7ff;

            if (immediate != 0)
            {
              snprintf(instruction, length, "%s 0x%04x",
                table_f100_l[n].instr,
                immediate);
            }
              else
            {
              immediate = memory->read16(address + 2);
              snprintf(instruction, length, "%s ,0x%04x",
                table_f100_l[n].instr,
                immediate);
              return 4;
            }
          }
            else
          {
            immediate = opcode & 0xff;

            if (immediate != 0)
            {
              if ((r & 1) == 0)
              {
                snprintf(instruction, length, "%s /0x%02x",
                  table_f100_l[n].instr,
                  immediate);
              }
                else
              if (r == 1)
              {
                snprintf(instruction, length, "%s /0x%02x+",
                  table_f100_l[n].instr,
                  immediate);
              }
                else
              {
                snprintf(instruction, length, "%s /0x%02x-",
                  table_f100_l[n].instr,
                  immediate);
              }
            }
              else
            {
              immediate = memory->read16(address + 2);
              snprintf(instruction, length, "%s .0x%04x",
                table_f100_l[n].instr,
                immediate);
              return 4;
            }
          }
          return 2;
        }
        case OP_BIT:
        {
          if (r == 3)
          {
            immediate = memory->read16(address + 2);
            snprintf(instruction, length, "%s %d, 0x%04x",
              table_f100_l[n].instr,
              opcode & 0xf,
              immediate);
            return 4;
          }
            else
          {
            snprintf(instruction, length, "%s %d, CR",
              table_f100_l[n].instr,
              opcode & 0xf);
            return 2;
          }
        }
        case OP_HALT:
        {
          immediate = opcode & 0x3ff;

          if (immediate == 0)
          {
            snprintf(instruction, length, "%s", table_f100_l[n].instr);
          }
            else
          {
            snprintf(instruction, length, "%s %d",
              table_f100_l[n].instr,
              immediate);
          }

          return 2;
        }
        case OP_INC:
        {
          jump_address = memory->read16(address + 2);

          if (i == 0)
          {
            immediate = opcode & 0x7ff;

            if (immediate != 0)
            {
              snprintf(instruction, length, "%s 0x%04x, %04x",
                table_f100_l[n].instr,
                immediate,
                jump_address);
            }
              else
            {
              immediate = memory->read16(address + 2);
              jump_address = memory->read16(address + 4);
              snprintf(instruction, length, "%s ,0x%04x, %04x",
                table_f100_l[n].instr,
                immediate,
                jump_address);
              return 6;
            }
          }
            else
          {
            immediate = opcode & 0xff;

            if (immediate != 0)
            {
              if ((r & 1) == 0)
              {
                snprintf(instruction, length, "%s /0x%02x, 0x%04x",
                  table_f100_l[n].instr,
                  immediate,
                  jump_address);
              }
                else
              if (r == 1)
              {
                snprintf(instruction, length, "%s /0x%02x+, 0x%04x",
                  table_f100_l[n].instr,
                  immediate,
                  jump_address);
              }
                else
              {
                snprintf(instruction, length, "%s /0x%02x-, 0x%04x",
                  table_f100_l[n].instr,
                  immediate,
                  jump_address);
              }
            }
              else
            {
              immediate = memory->read16(address + 2);
              jump_address = memory->read16(address + 4);
              snprintf(instruction, length, "%s .0x%04x, 0x%04x",
                table_f100_l[n].instr,
                immediate,
                jump_address);
              return 6;
            }
          }
          return 4;
        }
        case OP_COND_JMP:
        {
          if ((r & 1) == 0)
          {
            jump_address = memory->read16(address + 2);
            snprintf(instruction, length, "%s %d, a, 0x%04x",
              table_f100_l[n].instr,
              b,
              jump_address);
            return 4;
          }
            else
          if (r == 1)
          {
            jump_address = memory->read16(address + 2);
            snprintf(instruction, length, "%s %d, cr, 0x%04x",
              table_f100_l[n].instr,
              b,
              jump_address);
            return 4;
          }
            else
          if (r == 3)
          {
            immediate = memory->read16(address + 2);
            jump_address = memory->read16(address + 4);
            snprintf(instruction, length, "%s %d, 0x%04x, 0x%04x",
              table_f100_l[n].instr,
              b,
              immediate,
              jump_address);
            return 6;
          }
        }
        case OP_SHIFT:
        {
          if ((r & 1) == 0)
          {
            snprintf(instruction, length, "%s %d, a", table_f100_l[n].instr, b);
            return 2;
          }
            else
          if (r == 1)
          {
            snprintf(instruction, length, "%s %d, cr",
              table_f100_l[n].instr,
              b);
            return 2;
          }
            else
          if (r == 3)
          {
            immediate = memory->read16(address + 2);
            snprintf(instruction, length, "%s %d, 0x%04x",
              table_f100_l[n].instr,
              b,
              immediate);
            return 4;
          }
        }
        case OP_SHIFT_D:
        {
          b = opcode & 0x1f;

          if ((r & 1) == 0)
          {
            snprintf(instruction, length, "%s %d, a", table_f100_l[n].instr, b);
            return 2;
          }
            else
          if (r == 1)
          {
            snprintf(instruction, length, "%s %d, cr",
              table_f100_l[n].instr,
              b);
            return 2;
          }
            else
          if (r == 3)
          {
            immediate = memory->read16(address + 2);
            snprintf(instruction, length, "%s %d, 0x%04x",
              table_f100_l[n].instr,
              b,
              immediate);
            return 4;
          }
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_f100_l(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int opcode;
  int cycles_min, cycles_max;
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_f100_l(
      memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    fprintf(asm_context->list, "0x%04x: %04x  %-40s\n",
      start / 2,
      opcode,
      instruction);

    for (int n = 2; n < count; n += 2)
    {
      opcode = memory->read16(start + n);
      fprintf(asm_context->list, "0x%04x: %04x\n", (start + n) / 2, opcode);
    }

    start += count;
  }
}

void disasm_range_f100_l(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int opcode;
  int cycles_min, cycles_max;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_f100_l(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    printf("0x%04x: %04x  %-40s\n",
      start / 2,
      opcode,
      instruction);

    for (int n = 2; n < count; n += 2)
    {
      opcode = memory->read16(start + n);
      printf("0x%04x: %04x\n", (start + n) / 2, opcode);
    }

    start = start + count;
  }
}

