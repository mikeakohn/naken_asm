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

#include "disasm/z80.h"
#include "table/z80.h"

#define READ_RAM(a) memory_read_m(memory, a)
//#define READ_RAM16(a) memory_read_m(memory, a)|(memory_read_m(memory, a+1)<<8)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

static char *reg8[] = { "b","c","d","e","h","l","(hl)","a" };
static char *reg_ihalf[] = { "ixh","ixl","iyh","iyl" };
static char *reg16[] = { "bc","de","hl","sp" };
static char *reg16_p[] = { "bc","de","hl","af" };
static char *reg_xy[] = { "ix","iy" };
static char *cond[] = { "nz","z","nc","c", "po","pe","p","m" };

int get_cycle_count_z80(unsigned short int opcode)
{
  return -1;
}

static const char *get_instruction(int instr_enum)
{
  int n;

  n = 0;
  while(table_instr_z80[n].instr != NULL)
  {
    if (table_instr_z80[n].instr_enum == instr_enum)
    {
      return table_instr_z80[n].instr;
    }
    n++;
  }

  return "";
}

static void get_disp(char *disp, int reg, int offset)
{
  if (offset == 0)
  {
    sprintf(disp, "(%s)", reg_xy[reg]);
  }
    else
  if (offset > 0)
  {
    sprintf(disp, "(%s+%d)", reg_xy[reg], offset);
  }
    else
  {
    sprintf(disp, "(%s%d)", reg_xy[reg], offset);
  }
}

int disasm_z80(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int opcode16;
  uint8_t extra_opcode;
  int n,r,i;
  char offset;
  char disp[64];

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);
  opcode16 = READ_RAM16(address);

  n = 0;
  while(table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].mask > 0xff) { n++; continue; }
    if (table_z80[n].opcode == (opcode & table_z80[n].mask))
    {
      *cycles_min = table_z80[n].cycles_min;
      *cycles_max = table_z80[n].cycles_max;
      const char *instr = get_instruction(table_z80[n].instr_enum);

      switch(table_z80[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", instr);
          return 1;
        case OP_A_REG8:
          sprintf(instruction, "%s a,%s", instr, reg8[opcode & 0x7]);
          return 1;
        case OP_REG8:
          sprintf(instruction, "%s %s", instr, reg8[opcode & 0x7]);
          return 1;
        case OP_A_NUMBER8:
          sprintf(instruction, "%s a,%d", instr, READ_RAM(address + 1));
          return 2;
        case OP_HL_REG16_1:
          sprintf(instruction, "%s hl,%s", instr, reg16[(opcode >> 4) & 0x3]);
          return 1;
        case OP_A_INDEX_HL:
          sprintf(instruction, "%s a,(hl)", instr);
          return 1;
        case OP_INDEX_HL:
          sprintf(instruction, "%s (hl)", instr);
          return 1;
        case OP_NUMBER8:
          sprintf(instruction, "%s %d", instr, READ_RAM(address + 1));
          return 2;
        case OP_ADDRESS:
          sprintf(instruction, "%s 0x%04x", instr, READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_COND_ADDRESS:
          r = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %s,0x%04x", instr, cond[r], READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_REG8_V2:
          r = (opcode>>3) & 0x7;
          sprintf(instruction, "%s %s", instr, reg8[r]);
          return 1;
        case OP_REG16:
          r = (opcode >> 4) & 0x3;
          sprintf(instruction, "%s %s", instr, reg16[r]);
          return 1;
        case OP_INDEX_SP_HL:
          sprintf(instruction, "%s (sp),hl", instr);
          return 1;
        case OP_AF_AF_TICK:
          sprintf(instruction, "%s af,af'", instr);
          return 1;
        case OP_DE_HL:
          sprintf(instruction, "%s de,hl", instr);
          return 1;
        case OP_A_INDEX_N:
          sprintf(instruction, "%s a,(%d)", instr, READ_RAM(address + 1));
          return 2;
        case OP_OFFSET8:
          i = (int8_t)READ_RAM(address + 1);
          sprintf(instruction, "%s %d  (%d)", instr, (address + 2) + i, i);
          return 2;
        case OP_JR_COND_ADDRESS:
          r = (opcode >> 3) & 0x3;
          i = (int8_t)READ_RAM(address + 1);
          sprintf(instruction, "%s %s,%d  (%d)", instr, cond[r], (address + 2) + i, i);
          return 2;
        case OP_REG8_REG8:
          r = (opcode >> 3) & 0x7;
          i = opcode & 0x7;
          sprintf(instruction, "%s %s,%s", instr, reg8[r],reg8[i]);
          return 1;
        case OP_REG8_NUMBER8:
          r = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %s,%d", instr, reg8[r],READ_RAM(address + 1));
          return 2;
        case OP_REG8_INDEX_HL:
          r = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %s,(hl)", instr, reg8[r]);
          return 1;
        case OP_INDEX_HL_REG8:
          r = opcode & 0x7;
          sprintf(instruction, "%s (hl),%s", instr, reg8[r]);
          return 1;
        case OP_INDEX_HL_NUMBER8:
          sprintf(instruction, "%s (hl),%d", instr, READ_RAM(address + 1));
          return 2;
        case OP_A_INDEX_BC:
          sprintf(instruction, "%s a,(bc)", instr);
          return 1;
        case OP_A_INDEX_DE:
          sprintf(instruction, "%s a,(de)", instr);
          return 1;
        case OP_A_INDEX_ADDRESS:
          sprintf(instruction, "%s a,(0x%04x)", instr, READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_INDEX_BC_A:
          sprintf(instruction, "%s (bc),a", instr);
          return 1;
        case OP_INDEX_DE_A:
          sprintf(instruction, "%s (de),a", instr);
          return 1;
        case OP_INDEX_ADDRESS_A:
          sprintf(instruction, "%s (0x%04x),a", instr, READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_REG16_ADDRESS:
          r = (opcode >> 4) & 0x3;
          sprintf(instruction, "%s %s,0x%04x", instr, reg16[r], READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_HL_INDEX_ADDRESS:
          sprintf(instruction, "%s hl,(0x%04x)", instr, READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_INDEX_ADDRESS_HL:
          sprintf(instruction, "%s (0x%04x),hl", instr, READ_RAM(address + 1) | (READ_RAM(address + 2) << 8));
          return 3;
        case OP_SP_HL:
          sprintf(instruction, "%s sp,hl", instr);
          return 1;
        case OP_INDEX_ADDRESS8_A:
          sprintf(instruction, "%s (%d),a", instr, READ_RAM(address + 1));
          return 2;
        case OP_REG16P:
          r = (opcode >> 4) & 0x3;
          sprintf(instruction, "%s %s", instr, reg16_p[r]);
          return 1;
        case OP_COND:
          r = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %s", instr, cond[r]);
          return 1;
        case OP_RESTART_ADDRESS:
          r = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s 0x%x", instr, r * 8);
          return 1;
      }
    }
      else

    n++;
  }

  n = 0;
  while(table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].mask <= 0xff) { n++; continue; }
    if (table_z80[n].opcode == (opcode16 & table_z80[n].mask))
    {
      *cycles_min = table_z80[n].cycles_min;
      *cycles_max = table_z80[n].cycles_max;
      const char *instr = get_instruction(table_z80[n].instr_enum);

      switch(table_z80[n].type)
      {
        case OP_NONE16:
          sprintf(instruction, "%s", instr);
          return 2;
        case OP_NONE24:
          if (READ_RAM(address + 2) == 0)
          {
            sprintf(instruction, "%s", instr);
            return 3;
          }
          break;
        case OP_A_REG_IHALF:
          r = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          sprintf(instruction, "%s a,%s", instr, reg_ihalf[r]);
          return 2;
        case OP_B_REG_IHALF:
          r = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          sprintf(instruction, "%s b,%s", instr, reg_ihalf[r]);
          return 2;
        case OP_C_REG_IHALF:
          r = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          sprintf(instruction, "%s c,%s", instr, reg_ihalf[r]);
          return 2;
        case OP_D_REG_IHALF:
          r = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          sprintf(instruction, "%s d,%s", instr, reg_ihalf[r]);
          return 2;
        case OP_E_REG_IHALF:
          r = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          sprintf(instruction, "%s e,%s", instr, reg_ihalf[r]);
          return 2;
        case OP_A_INDEX:
          r = ((opcode16 & 0x2000) >> 13) & 0x1;
          offset = READ_RAM(address + 2);
          get_disp(disp, r, offset);
          sprintf(instruction, "%s a,%s", instr, disp);
          return 3;
        case OP_HL_REG16_2:
          sprintf(instruction, "%s hl,%s", instr, reg16[(opcode16 >> 4) & 0x3]);
          return 2;
        case OP_XY_REG16:
        {
          char *xy = ((opcode16 >> 13) & 0x1) == 0 ? "ix" : "iy";
          r = (opcode16 >> 4) & 0x3;
          if (r == 2)
          {
            sprintf(instruction, "%s %s,%s", instr, xy, xy);
          }
            else
          {
            sprintf(instruction, "%s %s,%s", instr, xy, reg16[r]);
          }
          return 2;
        }
        case OP_REG_IHALF:
          r = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          sprintf(instruction, "%s %s", instr, reg_ihalf[r]);
          return 2;
        case OP_INDEX:
          r = ((opcode16 & 0x2000) >> 13);
          offset = READ_RAM(address + 2);
          get_disp(disp, r, offset);
          sprintf(instruction, "%s %s", instr, disp);
          return 3;
        case OP_INDEX_LONG:
          extra_opcode = READ_RAM(address + 3);
          if (extra_opcode != table_z80[n].extra_opcode) { break; }
          r = ((opcode16 & 0x2000) >> 13);
          offset = READ_RAM(address + 2);
          get_disp(disp, r, offset);
          sprintf(instruction, "%s %s", instr, disp);
          return 4;
        case OP_BIT_REG8:
          r = opcode16 & 0x7;
          i = (opcode16 >> 3) & 0x7;
          sprintf(instruction, "%s %d,%s", instr, i, reg8[r]);
          return 2;
        case OP_BIT_INDEX_HL:
          i = (opcode16 >> 3) & 0x7;
          sprintf(instruction, "%s %d,(hl)", instr, i);
          return 2;
        case OP_BIT_INDEX:
          r = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(address + 2);
          i = READ_RAM(address + 3);
          //if ((i >> 6) == 1 && instr[0] == 'b')
          if ((i >> 6) == 1)
          {
            get_disp(disp, r, offset);
            i = (i >> 3) & 0x7;
            sprintf(instruction, "%s %d,%s", instr, i, disp);
            return 4;
          }
          break;
        case OP_REG_IHALF_V2:
          r = ((opcode16 & 0x2000) >> 12) | ((opcode16 >> 3) & 1);
          sprintf(instruction, "%s %s", instr, reg_ihalf[r]);
          return 2;
        case OP_XY:
          r = (opcode16 >> 13) & 0x1;
          sprintf(instruction, "%s %s", instr, reg_xy[r]);
          return 2;
        case OP_INDEX_SP_XY:
          r = (opcode16 >> 13) & 0x1;
          sprintf(instruction, "%s (sp),%s", instr, reg_xy[r]);
          return 2;
        case OP_IM_NUM:
          r = (opcode16 >> 3) & 0x3;
          if (r == 0 || r == 1) { r = 0; } else { r--; }
          sprintf(instruction, "%s %d", instr, r);
          return 2;
        case OP_REG8_INDEX_C:
          r = (opcode16 >> 3) & 0x7;
          sprintf(instruction, "%s %s,(c)", instr, reg8[r]);
          return 2;
        case OP_F_INDEX_C:
          sprintf(instruction, "%s f,(c)", instr);
          return 2;
        case OP_INDEX_XY:
          r=(opcode16>>13)&0x1;
          sprintf(instruction, "%s (%s)", instr, reg_xy[r]);
          return 2;
        case OP_REG8_REG_IHALF:
          i = ((opcode16 & 0x2000) >> 12) | (opcode16 & 1);
          r = (opcode16 >> 3) & 0x7;
          sprintf(instruction, "%s %s,%s", instr, reg8[r], reg_ihalf[i]);
          return 2;
        case OP_REG_IHALF_REG8:
          i = ((opcode16 & 0x2000) >> 12) | ((opcode16 >> 3) & 1);
          r = opcode16 & 0x7;
          sprintf(instruction, "%s %s,%s", instr, reg_ihalf[i], reg8[r]);
          return 2;
        case OP_REG_IHALF_REG_IHALF:
          r = (opcode16 & 0x2000) >> 12 | (opcode16 & 1);
          sprintf(instruction, "%s %s,%s", instr, reg_ihalf[r^1], reg_ihalf[r]);
          return 2;
        case OP_REG8_INDEX:
          r = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(address + 2);
          int dst = (opcode16 >> 3) & 0x7;
          get_disp(disp, r, offset);
          sprintf(instruction, "%s %s,%s", instr, reg8[dst], disp);
          return 3;
        case OP_INDEX_REG8:
          r = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(address + 2);
          int src = opcode16 & 0x7;
          get_disp(disp, r, offset);
          sprintf(instruction, "%s %s,%s", instr, disp, reg8[src]);
          return 3;
        case OP_INDEX_NUMBER8:
          r = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(address + 2);
          get_disp(disp, r, offset);
          sprintf(instruction, "%s %s,%d", instr, disp, READ_RAM(address + 3));
          return 4;
        case OP_IR_A:
          r = (opcode16 >> 3) & 0x1;
          sprintf(instruction, "%s %c,a", instr, (r == 0) ? 'i' : 'r');
          return 2;
        case OP_A_IR:
          r = (opcode16 >> 3) & 0x1;
          sprintf(instruction, "%s a,%c", instr, (r == 0) ? 'i' : 'r');
          return 2;
        case OP_XY_ADDRESS:
          r = (opcode16 >> 13) & 0x1;
          sprintf(instruction, "%s %s,0x%04x", instr, reg_xy[r], READ_RAM(address + 2) | (READ_RAM(address+3) << 8));
          return 4;
        case OP_REG16_INDEX_ADDRESS:
          r = (opcode16 >> 4) & 0x3;
          sprintf(instruction, "%s %s,(0x%04x)", instr, reg16[r], READ_RAM(address + 2) | (READ_RAM(address + 3) << 8));
          return 4;
        case OP_XY_INDEX_ADDRESS:
          r = (opcode16 >> 13) & 0x1;
          sprintf(instruction, "%s %s,(0x%04x)", instr, reg_xy[r], READ_RAM(address + 2) | (READ_RAM(address + 3) << 8));
          return 4;
        case OP_INDEX_ADDRESS_REG16:
          r = (opcode16 >> 4) & 0x3;
          sprintf(instruction, "%s (0x%04x),%s", instr, READ_RAM(address + 2) | (READ_RAM(address + 3) << 8), reg16[r]);
          return 4;
        case OP_INDEX_ADDRESS_XY:
          r = (opcode16 >> 13) & 0x1;
          sprintf(instruction, "%s (0x%04x),%s", instr, READ_RAM(address + 2) | (READ_RAM(address + 3) << 8), reg_xy[r]);
          return 4;
        case OP_SP_XY:
          r = (opcode16 >> 13) & 0x1;
          sprintf(instruction, "%s sp,%s", instr, reg_xy[r]);
          return 2;
        case OP_INDEX_C_REG8:
          r = (opcode16 >> 3) & 0x7;
          sprintf(instruction, "%s (c),%s", instr, reg8[r]);
          return 2;
        case OP_INDEX_C_ZERO:
          sprintf(instruction, "%s (c),0", instr);
          return 2;
        case OP_REG8_CB:
          r = opcode16 & 0x7;
          sprintf(instruction, "%s %s", instr, reg8[r]);
          return 2;
        case OP_INDEX_HL_CB:
          sprintf(instruction, "%s (hl)", instr);
          return 2;
      }
    }

    n++;
  }

  if ((opcode16 & 0xdfff) == 0xddcb)
  {
    i = READ_RAM(address + 3);

    n = 0;
    while(table_z80_4_byte[n].instr_enum != Z80_NONE)
    {

      if ((i & table_z80_4_byte[n].mask) == table_z80_4_byte[n].opcode)
      {
        offset = READ_RAM(address + 2);
        r = (opcode16 >> 13) & 0x1;
        get_disp(disp, r, offset);
        *cycles_min = table_z80_4_byte[n].cycles_min;
        *cycles_max = table_z80_4_byte[n].cycles_max;
        const char *instr = get_instruction(table_z80_4_byte[n].instr_enum);

        switch(table_z80_4_byte[n].type)
        {
          case OP_BIT_INDEX_V2:
            i = (i >> 3) & 0x7;
            sprintf(instruction, "%s %d,%s", instr, i, disp);
            return 4;
          case OP_BIT_INDEX_REG8:
            r = i & 0x7;
            i = (i >> 3) & 0x7;
            sprintf(instruction, "%s %d,%s,%s", instr, i, disp, reg8[r]);
            return 4;
        }
      }
      n++;
    }
  }

  sprintf(instruction, "???");

  return 1;
}

void list_output_z80(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  char bytes[16];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_z80(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(bytes, temp);
    }

    fprintf(asm_context->list, "0x%04x: %-9s %-40s cycles: ", start, bytes, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += count;
  }
}

void disasm_range_z80(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min=0,cycles_max=0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_z80(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", READ_RAM(start + n));
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

