/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/super_fx.h"
#include "table/super_fx.h"

#define READ_RAM32(a) (memory_read_m(memory, a + 3) << 24) | \
                      (memory_read_m(memory, a + 2) << 16) | \
                      (memory_read_m(memory, a + 1) << 8) | \
                       memory_read_m(memory, a + 0)

int get_cycle_count_super_fx(unsigned short int opcode)
{
  return -1;
}

int disasm_super_fx(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int n;
  int reg, num;
  int8_t offset;
  int count, alt;

  opcode = memory_read_m(memory, address);

  if (opcode == 0x3d)
  {
    alt = 1;
    address++;
  }
    else
  if (opcode == 0x3e)
  {
    alt = 2;
    address++;
  }
    else
  if (opcode == 0x3f)
  {
    alt = 3;
    address++;
  }
    else
  {
    alt = 0;
  }

  count = (alt == 0) ? 0 : 1;

  opcode = memory_read_m(memory, address);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;
  while(table_super_fx[n].instr != NULL)
  {
    if (((opcode & table_super_fx[n].mask) == table_super_fx[n].opcode) &&
        table_super_fx[n].alt == alt)
    {
      switch(table_super_fx[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_super_fx[n].instr);
          return count + 1;
        }
        case OP_REG:
        {
          reg = opcode & 0xf;
          if (((1 << reg) & table_super_fx[n].reg_mask) == 0) { break; }
          sprintf(instruction, "%s r%d", table_super_fx[n].instr, reg);
          return count + 1;
        }
        case OP_ATREG:
        {
          reg = opcode & 0xf;
          if (((1 << reg) & table_super_fx[n].reg_mask) == 0) { break; }
          sprintf(instruction, "%s (r%d)", table_super_fx[n].instr, reg);
          return count + 1;
        }
        case OP_N:
        {
          num = opcode & 0xf;
          if (table_super_fx[n].reg_mask != 0)
          {
            if (num < (table_super_fx[n].reg_mask >> 8) ||
                num > (table_super_fx[n].reg_mask & 0xff))
            {
              break;
            }
          }
          sprintf(instruction, "%s #%d", table_super_fx[n].instr, num);
          return count + 1;
        }
#if 0
        case OP_XX:
        {
          num = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s #%d", table_super_fx[n].instr, num);
          return count + 2;
        }
#endif
        case OP_OFFSET:
        {
          offset = (int8_t)memory_read_m(memory, address + 1);
          sprintf(instruction, "%s 0x%04x (offset=%d)", table_super_fx[n].instr, address + 2 + offset, offset);
          return count + 2;
        }
        case OP_REG_PP:
        {
          reg = opcode & 0xf;
          num = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s r%d, #0x%02x", table_super_fx[n].instr, reg, num);
          return count + 2;
        }
        case OP_REG_XX:
        {
          reg = opcode & 0xf;
          num = memory_read_m(memory, address + 1);
          num |= memory_read_m(memory, address + 2) << 8;
          sprintf(instruction, "%s r%d, #0x%04x", table_super_fx[n].instr, reg, num);
          return count + 3;
        }
        case OP_REG_ATXX:
        {
          reg = opcode & 0xf;
          if (((1 << reg) & table_super_fx[n].reg_mask) == 0) { break; }
          num = memory_read_m(memory, address + 1);
          num |= memory_read_m(memory, address + 2) << 8;
          sprintf(instruction, "%s r%d, (0x%04x)", table_super_fx[n].instr, reg, num);
          return count + 3;
        }
        case OP_REG_ATYY:
        {
          reg = opcode & 0xf;
          if (((1 << reg) & table_super_fx[n].reg_mask) == 0) { break; }
          num = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s r%d, (0x%02x)", table_super_fx[n].instr, reg, num * 2);
          return count + 2;
        }
        case OP_ATXX_REG:
        {
          reg = opcode & 0xf;
          if (((1 << reg) & table_super_fx[n].reg_mask) == 0) { break; }
          num = memory_read_m(memory, address + 1);
          num |= memory_read_m(memory, address + 2) << 8;
          sprintf(instruction, "%s (0x%04x), r%d", table_super_fx[n].instr, num, reg);
          return count + 3;
        }
        case OP_ATYY_REG:
        {
          reg = opcode & 0xf;
          if (((1 << reg) & table_super_fx[n].reg_mask) == 0) { break; }
          num = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s (0x%02x), r%d", table_super_fx[n].instr, num * 2, reg);
          return count + 2;
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

void list_output_super_fx(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  char temp[32];
  char temp2[4];
  int cycles_min, cycles_max;
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_super_fx(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;
    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(&asm_context->memory, start+n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-10s %-40s\n", start, temp, instruction);

    start += count;
  }
}

void disasm_range_super_fx(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char temp[32];
  char temp2[4];
  int cycles_min, cycles_max;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_super_fx(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;
    for (n = 0; n < count; n++)
    {
      //sprintf(temp2, "%02x ", READ_RAM(start+n));
      sprintf(temp2, "%02x ", memory_read_m(memory, start+n));
      strcat(temp, temp2);
    }

    printf("0x%04x: %-10s %-40s\n", start, temp, instruction);

    start = start + count;
  }
}

