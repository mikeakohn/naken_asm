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

#include "disasm/common.h"
#include "disasm/6809.h"
#include "table/6809.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

int get_cycle_count_6809(unsigned short int opcode)
{
  return -1;
}

int disasm_6809(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);

  if (opcode == 0x10 || opcode == 0x11)
  {
    opcode = READ_RAM16(address);

    n = 0;
    while(m6809_table_16[n].instr != NULL)
    {
      if (m6809_table_16[n].opcode == opcode)
      {
        strcpy(instruction, m6809_table_16[n].instr);
        *cycles_min = m6809_table_16[n].cycles_min;
        *cycles_max = m6809_table_16[n].cycles_min;

        switch(m6809_table_16[n].operand_type)
        {
          case M6809_OP_INHERENT:
          {
            return 1;
          }
          case M6809_OP_IMMEDIATE:
          {
            if (m6809_table_16[n].bytes == 4)
            {
              sprintf(instruction, "%s #0x%02x", m6809_table_16[n].instr, READ_RAM16(address + 2));
              return 4;
            }

            break;
          }
          case M6809_OP_EXTENDED:
          {
            if (m6809_table_16[n].bytes == 4)
            {
              sprintf(instruction, "%s 0x%04x", m6809_table_16[n].instr, READ_RAM16(address + 2));
              return 4;
            }

            break;
          }
          case M6809_OP_RELATIVE:
          {
            if (m6809_table_16[n].bytes == 4)
            {
              int16_t offset = READ_RAM16(address + 2);

              sprintf(instruction, "%s 0x%04x (%d)", m6809_table_16[n].instr, (address + 2 + offset) & 0xffff, offset);
              return 4;
            }

            break;
          }
          case M6809_OP_DIRECT:
          {
            if (m6809_table_16[n].bytes == 3)
            {
              sprintf(instruction, "%s >0x%02x", m6809_table_16[n].instr, READ_RAM(address + 2));
              return 3;
            }

            break;
          }
          case M6809_OP_INDEXED:
          default:
          {
            //print_error_internal(asm_context, __FILE__, __LINE__);
            break;
          }
        }
      }

      n++;
    }
  }
  else
  {
    n = 0;
    while(m6809_table[n].instr != NULL)
    {
      if (m6809_table[n].opcode == opcode)
      {
        *cycles_min = m6809_table[n].cycles_min;
        *cycles_max = m6809_table[n].cycles_min;

        switch(m6809_table[n].operand_type)
        {
          case M6809_OP_INHERENT:
          {
            strcpy(instruction, m6809_table[n].instr);
            return 1;
          }
          case M6809_OP_IMMEDIATE:
          {
            if (m6809_table[n].bytes == 2)
            {
              sprintf(instruction, "%s #0x%02x", m6809_table[n].instr, READ_RAM(address + 1));
              return 2;
            }
              else
            if (m6809_table[n].bytes == 3)
            {
              sprintf(instruction, "%s #0x%02x", m6809_table[n].instr, READ_RAM16(address + 1));
              return 3;
            }

            break;
          }
          case M6809_OP_EXTENDED:
          {
            if (m6809_table[n].bytes == 3)
            {
              sprintf(instruction, "%s 0x%04x", m6809_table[n].instr, READ_RAM16(address + 1));
              return 3;
            }

            break;
          }
          case M6809_OP_RELATIVE:
          {
            if (m6809_table[n].bytes == 2)
            {
              int8_t offset = READ_RAM(address + 1);

              sprintf(instruction, "%s 0x%04x (%d)", m6809_table[n].instr, (address + 2 + offset) & 0xffff, offset);
              return 2;
            }

            break;
          }
          case M6809_OP_DIRECT:
          {
            if (m6809_table[n].bytes == 2)
            {
              sprintf(instruction, "%s >0x%02x", m6809_table[n].instr, READ_RAM(address + 1));
              return 2;
            }

            break;
          }
          case M6809_OP_STACK:
          {
            if (m6809_table[n].bytes == 2)
            {
              uint8_t reg_list = READ_RAM(address + 1);
              const char *reg_names[] = { "pc","u","y","x","dp","b","a","cc" };
              uint8_t index = 0x80;
              uint8_t count = 0;

              sprintf(instruction, "%s", m6809_table[n].instr);
              for (n = 0; n < 8; n++)
              {
                if ((reg_list & index) != 0)
                {
                  if (count != 0) { strcat(instruction, ", "); }
                  else { strcat(instruction, " "); }
                  strcat(instruction, reg_names[n]);
                  count++;
                }

                index >>= 1;
              }

              return 2;
            }
          }
          case M6809_OP_TWO_REG:
          {
            const char *reg_post_byte[] = {
              "d", "x", "y",  "u",  "s", "pc", "?", "?",
              "a", "b", "cc", "dp", "?", "?",  "?", "?"
            };
            uint8_t post_byte = READ_RAM(address + 1);

            const char *src = reg_post_byte[post_byte >> 4];
            const char *dst = reg_post_byte[post_byte & 0xf];

            sprintf(instruction, "%s %s, %s", m6809_table[n].instr, src, dst);

            return 2;
          }
          case M6809_OP_INDEXED:
          {
            break;
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
  }

  strcpy(instruction, "???");

  return 1;
}

void list_output_6809(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char bytes[16];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_6809(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(bytes, temp);
    }

    fprintf(asm_context->list, "0x%04x: %-16s %-40s cycles: ", start, bytes, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += count;
  }
}

void disasm_range_6809(struct _memory *memory, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[16];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_6809(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", READ_RAM(start + n));
      strcat(bytes, temp);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: %-16s %-40s ?\n", start, bytes, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: %-16s %-40s %d\n", start, bytes, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-16s %-40s %d-%d\n", start, bytes, instruction, cycles_min, cycles_max);
    }

    start = start + count;
  }
}


