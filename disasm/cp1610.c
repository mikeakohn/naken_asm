/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2018 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/cp1610.h"
#include "table/cp1610.h"

#define READ_RAM16(a) (memory_read_m(memory, a+1)<<8)|memory_read_m(memory, a)

int get_cycle_count_cp1610(uint16_t opcode)
{
  return -1;
}

int disasm_cp1610(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int o, d;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM16(address);

  n = 0;

  while(table_cp1610[n].instr != NULL)
  {
    if ((opcode & table_cp1610[n].mask) == table_cp1610[n].opcode)
    {
      switch(table_cp1610[n].type)
      {
        case CP1610_OP_NONE:
        {
          strcpy(instruction, table_cp1610[n].instr);
          return 1;
        }
        case CP1610_OP_REG:
        {
          o = opcode & 0x7;

          sprintf(instruction, "%s r%d", table_cp1610[n].instr, o);

          return 2;
        }
        case CP1610_OP_REG_REG:
        {
          d = opcode & 0x7;
          o = (opcode >> 3) & 0x7;

          sprintf(instruction, "%s r%d, r%d", table_cp1610[n].instr, o, d);

          return 2;
        }
        case CP1610_OP_IMMEDIATE_REG:
        {
          return 4;
        }
        case CP1610_OP_ADDRESS_REG:
        {
          return 4;
        }
        case CP1610_OP_BRANCH:
        {
          return 4;
        }
        case CP1610_OP_JUMP:
        {
          return 6;
        }
        case CP1610_OP_1OP:
        {
          return 2;
        }
        default:
        {
printf("FUCK\n");
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

void list_output_cp1610(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  uint32_t opcode;
  char instruction[128];
  char temp[32];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_cp1610(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read_m(&asm_context->memory, start);
    sprintf(temp, "%02x", opcode);

    for (n = 1; n < count; n++)
    {
      char temp2[4];
      sprintf(temp2, " %02x", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-8s %-40s", start, temp, instruction);

    start += count;
  }
}

void disasm_range_cp1610(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int count;
  uint16_t opcode;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_cp1610(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: 0x%04x %-40s", start / 2, opcode, instruction);

    start = start + count;
  }
}

