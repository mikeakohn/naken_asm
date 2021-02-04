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

#include "disasm/cp1610.h"
#include "table/cp1610.h"

int get_cycle_count_cp1610(uint16_t opcode)
{
  return -1;
}

int disasm_cp1610(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int data;
  int o, d, r, z;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read16_m(memory, address);

  for (n = 0; table_cp1610[n].instr != NULL; n++)
  {
    if ((opcode & table_cp1610[n].mask) == table_cp1610[n].opcode)
    {
      *cycles_min = table_cp1610[n].cycles_min;
      *cycles_max = table_cp1610[n].cycles_max;

      switch (table_cp1610[n].type)
      {
        case CP1610_OP_NONE:
        {
          strcpy(instruction, table_cp1610[n].instr);
          return 2;
        }
        case CP1610_OP_REG:
        {
          o = opcode & 0x7;

          sprintf(instruction, "%s r%d", table_cp1610[n].instr, o);

          return 2;
        }
        case CP1610_OP_SREG_DREG:
        {
          d = opcode & 0x7;
          o = (opcode >> 3) & 0x7;

          sprintf(instruction, "%s r%d, r%d", table_cp1610[n].instr, o, d);

          return 2;
        }
        case CP1610_OP_AREG_SREG:
        {
          d = opcode & 0x7;
          o = (opcode >> 3) & 0x7;

          sprintf(instruction, "%s r%d, r%d", table_cp1610[n].instr, d, o);

          return 2;
        }
        case CP1610_OP_IMMEDIATE_REG:
        {
          data = memory_read16_m(memory, address + 2);
          r = opcode & 0x7;

          sprintf(instruction, "%s #0x%04x, r%d", table_cp1610[n].instr, data, r);

          return 4;
        }
        case CP1610_OP_REG_IMMEDIATE:
        {
          data = memory_read16_m(memory, address + 2);
          r = opcode & 0x7;

          sprintf(instruction, "%s r%d, #0x%04x", table_cp1610[n].instr, r, data);

          return 4;
        }
        case CP1610_OP_ADDRESS_REG:
        {
          data = memory_read16_m(memory, address + 2);
          r = opcode & 0x7;

          sprintf(instruction, "%s 0x%04x, r%d", table_cp1610[n].instr, data, r);

          return 4;
        }
        case CP1610_OP_REG_ADDRESS:
        {
          data = memory_read16_m(memory, address + 2);
          r = opcode & 0x7;

          sprintf(instruction, "%s r%d, 0x%04x", table_cp1610[n].instr, r, data);

          return 4;
        }
        case CP1610_OP_1OP:
        {
          r = opcode & 0x3;
          data = ((opcode >> 2) & 0x1) + 1;

          sprintf(instruction, "%s r%d, %d", table_cp1610[n].instr, r, data);

          return 2;
        }
        case CP1610_OP_BRANCH:
        {
          z = (opcode >> 5) & 1;
          data = memory_read16_m(memory, address + 2);

          if (z == 0)
          {
            sprintf(instruction, "%s 0x%04x (offset=%d z=%d)",
              table_cp1610[n].instr,
              (address / 2) + 3 + data,
              data,
              z);
          }
            else
          {
            sprintf(instruction, "%s 0x%04x (offset=%d z=%d)",
              table_cp1610[n].instr,
              (address / 2) + 1 - data,
              data,
              z);
          }

          return 4;
        }
        case CP1610_OP_JUMP:
        {
          opcode = memory_read16_m(memory, address + 2);
          data = memory_read16_m(memory, address + 4);

          int bb = (opcode >> 8) & 0x3;
          int ii = opcode & 0x3;
          int j;

          data |= ((opcode >> 2) & 0x3f) << 10;

          for (j = 0; j < 6; j++)
          {
            if (table_cp1610_jump[j].ii == ii)
            {
              if (table_cp1610_jump[j].bb != 3 && bb != 3)
              {
                sprintf(instruction, "%s 0x%04x",
                  table_cp1610[n].instr,
                  data);
                break;
              }
                else
              if (bb == table_cp1610_jump[j].bb && bb == 3)
              {
                sprintf(instruction, "%s r%d, 0x%04x",
                  table_cp1610[n].instr,
                  bb + 4,
                  data);
                break;
              }
            }
          }

          return 6;
        }
        case CP1610_OP_JR:
        {
          r = (opcode >> 3) & 0x7;

          sprintf(instruction, "%s r%d", table_cp1610[n].instr, r);

          return 2;
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

void list_output_cp1610(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  uint32_t opcode;
  char instruction[128];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_cp1610(
      &asm_context->memory,
      start,
      instruction,
      &cycles_min,
      &cycles_max);

    opcode = memory_read16_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: %04x %-40s cycles: %d-%d\n",
            start / 2, opcode, instruction, cycles_min, cycles_max);

    for (n = 2; n < count; n = n + 2)
    {
      opcode = memory_read16_m(&asm_context->memory, start + n);

      fprintf(asm_context->list, "0x%04x: %04x\n", (start + n) / 2, opcode);
    }

    start += count;
  }
}

void disasm_range_cp1610(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;
  uint16_t opcode;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_cp1610(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: %04x %-40s %d-%d\n",
      start / 2, opcode, instruction, cycles_min, cycles_max);

    for (n = 2; n < count; n = n + 2)
    {
      opcode = memory_read16_m(memory, start + n);
      printf("0x%04x: %04x\n", (start + n) / 2, opcode);
    }

    start = start + count;
  }
}

