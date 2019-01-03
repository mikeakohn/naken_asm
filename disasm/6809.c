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

#include "disasm/6809.h"
#include "table/6809.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)
#define ADD_CYCLES(a) *cycles_min += a; *cycles_max += a;

int get_cycle_count_6809(unsigned short int opcode)
{
  return -1;
}

int get_indexed(struct _memory *memory, struct _table_6809 *table, char *instruction, uint32_t address, int *cycles_min, int *cycles_max)
{
  const char *name[] = { "x", "y", "u", "s" };
  uint8_t post_byte = READ_RAM(address);
  int reg = (post_byte >> 5) & 0x3;

  if ((post_byte & 0x9f) == 0x84)
  {
    // ,R non-indirect
    sprintf(instruction, "%s ,%s", table->instr, name[reg]);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x94)
  {
    // [,R] indirect
    sprintf(instruction, "%s [,%s]", table->instr, name[reg]);
    ADD_CYCLES(3);
    return 0;
  }
    else
  if ((post_byte & 0x80) == 0x00)
  {
    // 5 bit offset, R non-indirect
    int8_t offset = post_byte & 0x1f;
    if ((offset & 0x10) != 0) { offset |= 0xe0; }
    sprintf(instruction, "%s %d,%s", table->instr, offset, name[reg]);
    ADD_CYCLES(1);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x88)
  {
    // 8 bit offset, R non-indirect
    int8_t offset = READ_RAM(address + 1);
    sprintf(instruction, "%s %d,%s", table->instr, offset, name[reg]);
    ADD_CYCLES(1);
    return 1;
  }
    else
  if ((post_byte & 0x9f) == 0x98)
  {
    // [8 bit offset, R] indirect
    int8_t offset = READ_RAM(address + 1);
    sprintf(instruction, "%s [%d,%s]", table->instr, offset, name[reg]);
    ADD_CYCLES(4);
    return 1;
  }
    else
  if ((post_byte & 0x9f) == 0x89)
  {
    // 16 bit offset, R non-indirect
    int16_t offset = READ_RAM16(address + 1);
    sprintf(instruction, "%s %d,%s", table->instr, offset, name[reg]);
    ADD_CYCLES(4);
    return 2;
  }
    else
  if ((post_byte & 0x9f) == 0x99)
  {
    // [16 bit offset, R] indirect
    int16_t offset = READ_RAM16(address + 1);
    sprintf(instruction, "%s [%d,%s]", table->instr, offset, name[reg]);
    ADD_CYCLES(7);
    return 2;
  }
    else
  if ((post_byte & 0x9f) == 0x86)
  {
    // A,R non-indirect
    sprintf(instruction, "%s a,%s", table->instr, name[reg]);
    ADD_CYCLES(1);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x96)
  {
    // [A,R] non-indirect
    sprintf(instruction, "%s [a,%s]", table->instr, name[reg]);
    ADD_CYCLES(4);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x85)
  {
    // B,R non-indirect
    sprintf(instruction, "%s b,%s", table->instr, name[reg]);
    ADD_CYCLES(1);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x95)
  {
    // [B,R] indirect
    sprintf(instruction, "%s [b,%s]", table->instr, name[reg]);
    ADD_CYCLES(4);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x8b)
  {
    // D,R non-indirect
    sprintf(instruction, "%s d,%s", table->instr, name[reg]);
    ADD_CYCLES(4);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x9b)
  {
    // [D,R] non-indirect
    sprintf(instruction, "%s [d,%s]", table->instr, name[reg]);
    ADD_CYCLES(7);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x80)
  {
    // ,R+ non-indirect
    sprintf(instruction, "%s ,%s+", table->instr, name[reg]);
    ADD_CYCLES(2);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x81)
  {
    // ,R++ non-indirect
    sprintf(instruction, "%s ,%s++", table->instr, name[reg]);
    ADD_CYCLES(3);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x91)
  {
    // [,R++] indirect
    sprintf(instruction, "%s [,%s++]", table->instr, name[reg]);
    ADD_CYCLES(6);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x82)
  {
    // ,-R non-indirect
    sprintf(instruction, "%s ,-%s", table->instr, name[reg]);
    ADD_CYCLES(2);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x83)
  {
    // ,--R non-indirect
    sprintf(instruction, "%s ,--%s", table->instr, name[reg]);
    ADD_CYCLES(3);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x93)
  {
    // [,--R] indirect
    sprintf(instruction, "%s [,--%s]", table->instr, name[reg]);
    ADD_CYCLES(6);
    return 0;
  }
    else
  if ((post_byte & 0x9f) == 0x8c)
  {
    // 8 bit offset, PCR non-indirect
    int8_t offset = READ_RAM(address + 1);
    sprintf(instruction, "%s %d,pc", table->instr, offset);
    ADD_CYCLES(1);
    return 1;
  }
    else
  if ((post_byte & 0x9f) == 0x9c)
  {
    // [8 bit offset, PCR] indirect
    int8_t offset = READ_RAM(address + 1);
    sprintf(instruction, "%s [%d,pc]", table->instr, offset);
    ADD_CYCLES(4);
    return 1;
  }
    else
  if ((post_byte & 0x9f) == 0x8d)
  {
    // 16 bit offset, PCR non-indirect
    int16_t offset = READ_RAM16(address + 1);
    sprintf(instruction, "%s %d,pc", table->instr, offset);
    ADD_CYCLES(5);
    return 2;
  }
    else
  if ((post_byte & 0x9f) == 0x9d)
  {
    // [16 bit offset, PCR] non-indirect
    int16_t offset = READ_RAM16(address + 1);
    sprintf(instruction, "%s [%d,pc]", table->instr, offset);
    ADD_CYCLES(8);
    return 2;
  }
    else
  if ((post_byte & 0x9f) == 0x9f)
  {
    // [16 bit offset] non-indirect
    int16_t offset = READ_RAM16(address + 1);
    sprintf(instruction, "%s [0x%04x]", table->instr, offset);
    ADD_CYCLES(5);
    return 2;
  }

  strcpy(instruction, "???");

  return 0;
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
    while(table_6809_16[n].instr != NULL)
    {
      if (table_6809_16[n].opcode == opcode)
      {
        strcpy(instruction, table_6809_16[n].instr);
        *cycles_min = table_6809_16[n].cycles_min;
        *cycles_max = table_6809_16[n].cycles_min;

        switch(table_6809_16[n].operand_type)
        {
          case M6809_OP_INHERENT:
          {
            return 2;
          }
          case M6809_OP_IMMEDIATE:
          {
            if (table_6809_16[n].bytes == 4)
            {
              sprintf(instruction, "%s #0x%02x", table_6809_16[n].instr, READ_RAM16(address + 2));
              return 4;
            }

            break;
          }
          case M6809_OP_EXTENDED:
          {
            if (table_6809_16[n].bytes == 4)
            {
              sprintf(instruction, "%s 0x%04x", table_6809_16[n].instr, READ_RAM16(address + 2));
              return 4;
            }

            break;
          }
          case M6809_OP_RELATIVE:
          {
            if (table_6809_16[n].bytes == 4)
            {
              int16_t offset = READ_RAM16(address + 2);

              sprintf(instruction, "%s 0x%04x (%d)", table_6809_16[n].instr, (address + 4 + offset) & 0xffff, offset);
              return 4;
            }

            break;
          }
          case M6809_OP_DIRECT:
          {
            if (table_6809_16[n].bytes == 3)
            {
              sprintf(instruction, "%s >0x%02x", table_6809_16[n].instr, READ_RAM(address + 2));
              return 3;
            }

            break;
          }
          case M6809_OP_INDEXED:
          {
            return get_indexed(memory, &table_6809_16[n], instruction, address + 2, cycles_min, cycles_max) + 3;

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
  else
  {
    n = 0;
    while(table_6809[n].instr != NULL)
    {
      if (table_6809[n].opcode == opcode)
      {
        *cycles_min = table_6809[n].cycles_min;
        *cycles_max = table_6809[n].cycles_min;

        switch(table_6809[n].operand_type)
        {
          case M6809_OP_INHERENT:
          {
            strcpy(instruction, table_6809[n].instr);
            return 1;
          }
          case M6809_OP_IMMEDIATE:
          {
            if (table_6809[n].bytes == 2)
            {
              sprintf(instruction, "%s #0x%02x", table_6809[n].instr, READ_RAM(address + 1));
              return 2;
            }
              else
            if (table_6809[n].bytes == 3)
            {
              sprintf(instruction, "%s #0x%02x", table_6809[n].instr, READ_RAM16(address + 1));
              return 3;
            }

            break;
          }
          case M6809_OP_EXTENDED:
          {
            if (table_6809[n].bytes == 3)
            {
              sprintf(instruction, "%s 0x%04x", table_6809[n].instr, READ_RAM16(address + 1));
              return 3;
            }

            break;
          }
          case M6809_OP_RELATIVE:
          {
            if (table_6809[n].bytes == 2)
            {
              int8_t offset = READ_RAM(address + 1);

              sprintf(instruction, "%s 0x%04x (%d)", table_6809[n].instr, (address + 2 + offset) & 0xffff, offset);
              return 2;
            }

            break;
          }
          case M6809_OP_LONG_RELATIVE:
          {
            if (table_6809[n].bytes == 3)
            {
              int16_t offset = (READ_RAM(address + 1) << 8) | READ_RAM(address + 2);

              sprintf(instruction, "%s 0x%04x (%d)", table_6809[n].instr, (address + 3 + offset) & 0xffff, offset);
              return 2;
            }

            break;
          }
          case M6809_OP_DIRECT:
          {
            if (table_6809[n].bytes == 2)
            {
              sprintf(instruction, "%s >0x%02x", table_6809[n].instr, READ_RAM(address + 1));
              return 2;
            }

            break;
          }
          case M6809_OP_STACK:
          {
            if (table_6809[n].bytes == 2)
            {
              uint8_t reg_list = READ_RAM(address + 1);
              const char *reg_names[] = { "pc","u","y","x","dp","b","a","cc" };
              uint8_t index = 0x80;
              uint8_t count = 0;

              sprintf(instruction, "%s", table_6809[n].instr);
              for (n = 0; n < 8; n++)
              {
                if ((reg_list & index) != 0)
                {
                  if (count != 0) { strcat(instruction, ", "); }
                  else { strcat(instruction, " "); }
                  strcat(instruction, reg_names[n]);
                  count++;
                  // Each byte pushed adds 1 cycle to cycle counts
                  if (n >= 4) { *cycles_min += 1; *cycles_max += 1; }
                  else { *cycles_min += 2; *cycles_max += 2; }
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

            sprintf(instruction, "%s %s, %s", table_6809[n].instr, src, dst);

            return 2;
          }
          case M6809_OP_INDEXED:
          {
            return get_indexed(memory, &table_6809[n], instruction, address + 1, cycles_min, cycles_max) + 2;

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

void disasm_range_6809(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
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


