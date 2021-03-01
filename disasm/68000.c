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
#include <string.h>

#include "disasm/68000.h"
#include "table/68000.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)
#define READ_RAM32(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a+1)<<16)|(memory_read_m(memory, a+2)<<8)|memory_read_m(memory, a+3)

#define SIZE(a,b) ((a>>b)&0x3)

enum
{
  SIZE_B=0,
  SIZE_W,
  SIZE_L,
};

static char sizes[] = { 'b', 'w', 'l', '?' };

int get_cycle_count_68000(uint16_t opcode)
{
  return -1;
}

static int is_illegal_ea(int16_t opcode, int omit_mode)
{
  int reg = opcode & 0x7;
  int mode = (opcode >> 3) & 0x7;

  switch (mode)
  {
    case 0:
      if ((omit_mode & MODE_DN) != 0) { return 1; }
      break;
    case 1:
      if ((omit_mode & MODE_AN) != 0) { return 1; }
      break;
    case 2:
      break;
    case 3:
      if ((omit_mode & MODE_AN_P) != 0) { return 1; }
      break;
    case 4:
      if ((omit_mode & MODE_AN_N) != 0) { return 1; }
      break;
    case 5:
       break;
    case 6:
       break;
    case 7:
       switch (reg)
       {
         case 2:
           if ((omit_mode & MODE_D16_PC) != 0) { return 1; }
           break;
         case 3:
           if ((omit_mode & MODE_D8_PC_XN) != 0) { return 1; }
           break;
         case 4:
           if ((omit_mode & MODE_IMM) != 0) { return 1; }
           break;
         default:
           break;
       }
    default:
      break;

  }

  return 0;
}

static int get_ea_68000(
  struct _memory *memory,
  uint32_t address,
  char *ea,
  uint16_t opcode,
  int skip,
  int size)
{
  int reg = opcode & 0x7;
  int mode = (opcode >> 3) & 0x7;
  int xn,xn_reg;
  char xn_ad,xn_size;

  switch (mode)
  {
    case 0:
      sprintf(ea, "d%d", reg);
      return 2;
    case 1:
      sprintf(ea, "a%d", reg);
      return 2;
    case 2:
      sprintf(ea, "(a%d)", reg);
      return 2;
    case 3:
      sprintf(ea, "(a%d)+", reg);
      return 2;
    case 4:
      sprintf(ea, "-(a%d)", reg);
      return 2;
    case 5:
      sprintf(ea, "(%d,a%d)", (int16_t)READ_RAM16(address + 2 + skip), reg);
      return 4;
    case 6:
      xn = READ_RAM16(address + 2 + skip);
      xn_ad = (xn & 0x8000) == 0 ? 'd' : 'a';
      xn_reg = (xn >> 12) & 0x7;
      xn_size = (xn & 0x0800) == 0 ? 'w' : 'l';
      sprintf(ea, "(%d,a%d,%c%d.%c)", (int8_t)(xn & 0xff), reg, xn_ad, xn_reg, xn_size);
      return 4;
    case 7:
      if (reg==0)
      {
        int16_t value = READ_RAM16(address + 2 + skip);
        if (value > 0) { sprintf(ea, "(0x%x)", value); }
        else { sprintf(ea, "(0x%x)", ((uint32_t)value) & 0xffffff); }
        return 4;
      }
      else if (reg == 1)
      {
        sprintf(ea, "(0x%x)", READ_RAM32(address + 2 + skip));
        return 6;
      }
      else if (reg == 2)
      {
        sprintf(ea, "(%d,PC)", (int16_t)READ_RAM16(address + 2 + skip));
        return 4;
      }
      else if (reg == 4)
      {
        if (size == SIZE_B)
        {
          sprintf(ea, "#0x%x", READ_RAM(address + 3 + skip));
          return 4;
        }
          else
        if (size == SIZE_W)
        {
          sprintf(ea, "#0x%x", READ_RAM16(address + 2 + skip));
          return 4;
        }
          else
        if (size == SIZE_L)
        {
          sprintf(ea, "#0x%x", READ_RAM32(address + 2 + skip));
          return 6;
        }
          else
        {
          sprintf(ea, "???");
        }
      }
        else
      {
        sprintf(ea, "???");
      }
      return 2;
    default:
      break;

  }

  strcpy(ea, "???");

  return 2;
}

static int is_ea_valid(struct _table_68000 *table, uint16_t opcode, int is_dst)
{
  int omit_mode = (is_dst == 1) ? table->omit_dst : table->omit_src;
  int mode = (opcode >> 3) & 0x7;
  int reg = opcode & 0x7;

  switch (mode)
  {
    case 0: // Dn
      if (omit_mode & MODE_DN) { return 0; }
      break;
    case 1: // An
      if (omit_mode & MODE_AN) { return 0; }
      break;
    case 2: // (An)
      break;
    case 3: // (An)+
      if (omit_mode & MODE_AN_P) { return 0; }
      break;
    case 4: // (An)-
      if (omit_mode & MODE_AN_N) { return 0; }
      break;
    case 5: // (d16,An)
      break;
    case 6: // (d8,An,Xn)
      break;
    case 7:
      switch (reg)
      {
        case 0: // (xxx).w
          break;
        case 1: // (xxx).l
          break;
        case 4: // #<data> IMM
          if (is_dst == 1) { return 0; }
          if (omit_mode & MODE_IMM) { return 0; }
          break;
        case 2: // (d16,PC)
          if (omit_mode & MODE_D16_PC) { return 0; }
          break;
        case 3: // (d8,PC,Xn)
          if (omit_mode & MODE_D8_PC_XN) { return 0; }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

  return 1;
}

static void get_reglist(char *reglist, int regs)
{
  int ptr = 0;
  int n;

  for (n = 0; n < 8; n++)
  {
    if ((regs & 1) == 1)
    {
      if (ptr != 0) { reglist[ptr++] = '/'; }
      reglist[ptr++] = 'd';
      reglist[ptr++] = n + '0';
    }
    regs >>= 1;
  }

  for (n = 0; n < 8; n++)
  {
    if ((regs & 1) == 1)
    {
      if (ptr != 0) { reglist[ptr++] = '/'; }
      reglist[ptr++] = 'a';
      reglist[ptr++] = n + '0';
    }
    regs >>= 1;
  }

  reglist[ptr] = 0;
}

#if 0
static char get_size_68000(unsigned short int opcode, int pos)
{
  char size[] = { 'b','w','l','?' };
  return size[(opcode >> pos) & 0x3];
}
#endif

int disasm_68000(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  char ea[32];
  int size;
  int reg,imm;
  int mode,len;
  uint32_t immediate;
  int offset;
  //int value;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM16(address);

  n = 0;
  while (table_68000[n].instr != NULL)
  {
    if ((opcode & table_68000[n].mask) == table_68000[n].opcode)
    {
      switch (table_68000[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_68000[n].instr);
          return 2;
        case OP_SINGLE_EA:
          size = SIZE(opcode, 6);
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          if (size == 3) { break; }
          sprintf(instruction, "%s.%c %s", table_68000[n].instr, sizes[size], ea);
          return len;
        case OP_SINGLE_EA_NO_SIZE:
          if (is_illegal_ea(opcode, table_68000[n].omit_src)) { break; }
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s %s", table_68000[n].instr, ea);
          return len;
        case OP_IMMEDIATE:
          size = SIZE(opcode, 6);
          mode = (opcode >> 3) & 0x7;
          reg = (opcode) & 0x7;
          if (mode == 1 || (mode == 7 && reg == 4)) { break; }
          if (size == 3) { break; }
          len = get_ea_68000(memory, address, ea, opcode, size == SIZE_L ? 4 : 2, size);

          if (size == SIZE_B)
          {
            immediate = READ_RAM(address + 3);
            sprintf(instruction, "%s.%c #0x%02x, %s", table_68000[n].instr, sizes[size], immediate, ea);
            len += 2;
          }
            else
          if (size == SIZE_W)
          {
            immediate = READ_RAM16(address + 2);
            sprintf(instruction, "%s.%c #0x%04x, %s", table_68000[n].instr, sizes[size], immediate, ea);
            len += 2;
          }
            else
          {
            immediate = READ_RAM32(address + 2);
            sprintf(instruction, "%s.%c #0x%08x, %s", table_68000[n].instr, sizes[size], immediate, ea);
            len += 4;
          }

          return len;
        case OP_SHIFT_EA:
          if (((opcode >> 3) & 0x7) <= 1) { break; }
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s %s", table_68000[n].instr, ea);
          return len;
        case OP_SHIFT:
          size = SIZE(opcode, 6);
          if (size == 3) { break; }
          if ((opcode & 0x0020) == 0)
          {
            immediate = (opcode >> 9) & 0x7;
            immediate = (immediate == 0) ? 8 : immediate;
            sprintf(instruction, "%s.%c #%d, d%d", table_68000[n].instr, sizes[size], immediate, opcode & 0x7);
          }
            else
          {
            immediate = (opcode >> 9) & 0x7;
            sprintf(instruction, "%s.%c d%d, d%d", table_68000[n].instr, sizes[size], immediate, opcode & 0x7);
          }
          return 2;
        case OP_REG_AND_EA:
          size = SIZE(opcode, 6);
          if (size == 3) { break; }
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          reg = (opcode >> 9) & 0x7;
          mode = (opcode >> 8) & 0x1;
          if (mode == 1 && ((opcode >> 3) & 0x7) <= 1) { break; }
          if (mode == 0)
          {
            sprintf(instruction, "%s.%c %s, d%d", table_68000[n].instr, sizes[size], ea, reg);
          }
            else
          {
            sprintf(instruction, "%s.%c d%d, %s", table_68000[n].instr, sizes[size], reg, ea);
          }
          return len;
        case OP_VECTOR:
          sprintf(instruction, "%s #%d", table_68000[n].instr, opcode & 0xf);
          return 2;
        case OP_VECTOR3:
          sprintf(instruction, "%s #%d", table_68000[n].instr, opcode & 0x7);
          return 2;
        case OP_AREG:
          sprintf(instruction, "%s a%d", table_68000[n].instr, opcode & 0x7);
          return 2;
        case OP_REG:
        {
          char r = (opcode & 0x8) == 0 ? 'd' : 'a';
          sprintf(instruction, "%s %c%d", table_68000[n].instr, r, opcode & 0x7);
          return 2;
        }
        case OP_EA_AREG:
          reg = (opcode >> 9) & 0x7;
          mode = (opcode >> 6) & 0x7;
          if (mode == 3) { size = SIZE_W; }
          else if (mode == 7) { size = SIZE_L; }
          else { break; }
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c %s, a%d", table_68000[n].instr, sizes[size], ea, reg);
          return len;
        case OP_EA_DREG:
          reg = (opcode >> 9) & 0x7;
          mode = (opcode >> 6) & 0x7;
          if (mode > 2) { break; }
          size = mode;
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c %s, d%d", table_68000[n].instr, sizes[size], ea, reg);
          return len;
        case OP_LOAD_EA:
          reg = (opcode >> 9) & 0x7;
          size = 0;
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s %s, a%d", table_68000[n].instr, ea, reg);
          return len;
        case OP_QUICK:
          reg = (opcode >> 9) & 0x7;
          size = (opcode >> 6) & 0x3;
          if (size == 3) { break; }
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c #%d, %s", table_68000[n].instr, sizes[size], reg, ea);
          return len;
        case OP_MOVE_QUICK:
          reg = (opcode >> 9) & 0x7;
          sprintf(instruction, "%s #%d, d%d", table_68000[n].instr, opcode & 0xff, reg);
          return 2;
        case OP_MOVE_FROM_CCR:
          len = get_ea_68000(memory, address, ea, opcode, 0, SIZE_W);
          sprintf(instruction, "%s CCR, %s", table_68000[n].instr, ea);
          return len;
        case OP_MOVE_TO_CCR:
          len = get_ea_68000(memory, address, ea, opcode, 0, SIZE_W);
          sprintf(instruction, "%s %s, CCR", table_68000[n].instr, ea);
          return len;
        case OP_MOVE_FROM_SR:
          len = get_ea_68000(memory, address, ea, opcode, 0, SIZE_W);
          sprintf(instruction, "%s SR, %s", table_68000[n].instr, ea);
          return len;
        case OP_MOVE_TO_SR:
          len = get_ea_68000(memory, address, ea, opcode, 0, SIZE_W);
          sprintf(instruction, "%s %s, SR", table_68000[n].instr, ea);
          return len;
        case OP_MOVEA:
          size = (opcode >> 12) & 0x3;
          size = (size == 3) ? SIZE_W:SIZE_L;
          reg = (opcode >> 9) & 0x7;
          len  = get_ea_68000(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c %s, a%d", table_68000[n].instr, sizes[size], ea, reg);
          return len;
        case OP_CMPM:
          size = (opcode >> 6) & 0x3;
          reg = (opcode >> 9) & 0x7;
          sprintf(instruction, "%s.%c (a%d)+, (a%d)+", table_68000[n].instr, sizes[size], opcode & 0x7, reg);
          return 2;
        case OP_BCD:
          reg = (opcode >> 9) & 0x7;
          if ((opcode & 8) == 0)
          {
            sprintf(instruction, "%s d%d, d%d", table_68000[n].instr, opcode & 0x7, reg);
          }
            else
          {
            sprintf(instruction, "%s -(a%d), -(a%d)", table_68000[n].instr, opcode & 0x7, reg);
          }
          return 2;
        case OP_EXTENDED:
          reg = (opcode >> 9) & 0x7;
          size = (opcode >> 6) & 0x3;
          if ((opcode & 8) == 0)
          {
            sprintf(instruction, "%s.%c d%d, d%d", table_68000[n].instr, sizes[size], opcode & 0x7, reg);
          }
            else
          {
            sprintf(instruction, "%s.%c -(a%d), -(a%d)", table_68000[n].instr, sizes[size], opcode & 0x7, reg);
          }
          return 2;
        case OP_ROX_MEM:
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s %s", table_68000[n].instr, ea);
          return len;
        case OP_ROX:
          size = (opcode >> 6) & 0x3;
          reg = (opcode >> 9) & 0x7;
          if ((opcode & 0x20) == 0)
          {
            reg = (reg == 0) ? 8 : reg;
            sprintf(instruction, "%s.%c #%d, d%d", table_68000[n].instr, sizes[size], reg, opcode & 0x7);
          }
            else
          {
            sprintf(instruction, "%s.%c d%d, d%d", table_68000[n].instr, sizes[size], reg, opcode & 0x7);
          }
          return 2;
        case OP_EXCHANGE:
          reg = (opcode >> 9) & 0x7;
          mode = (opcode >> 3) & 0x1f;
          if (mode == 8)
          {
            sprintf(instruction, "%s d%d, d%d", table_68000[n].instr, reg, opcode & 0x7);
          }
            else
          if (mode == 9)
          {
            sprintf(instruction, "%s a%d, a%d", table_68000[n].instr, reg, opcode&0x7);
          }
            else
          if (mode == 0x11)
          {
            sprintf(instruction, "%s d%d, a%d", table_68000[n].instr, reg, opcode&0x7);
          }
            else
          {
            break;
          }
          return 2;
        case OP_BIT_REG_EA:
          reg = (opcode >> 9) & 0x7;
          // FIXME - should this be for all destination EA's?
          if (((opcode >> 3) & 0x7) == 1) { break; }
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s d%d, %s", table_68000[n].instr, reg, ea);
          return len;
        case OP_BIT_IMM_EA:
          imm = READ_RAM16(address+2); // Immediate
          len = get_ea_68000(memory, address, ea, opcode, 2, 0);
          sprintf(instruction, "%s #%d, %s", table_68000[n].instr, imm, ea);
          return len + 2;
        case OP_EA_DREG_WL:
          reg = (opcode >> 9) & 0x7;
          size = (opcode >> 7) & 0x3;
          if (size < 2) { break; }
          size = (size == 2) ? SIZE_L : SIZE_W;
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s.%c %s, d%d", table_68000[n].instr, sizes[size], ea, reg);
          return len;
        case OP_LOGIC_CCR:
          sprintf(instruction, "%s #0x%02x, CCR", table_68000[n].instr, READ_RAM16(address + 2));
          return 4;
        case OP_LOGIC_SR:
          sprintf(instruction, "%s #0x%02x, SR", table_68000[n].instr, READ_RAM16(address + 2));
          return 4;
        case OP_BRANCH:
          offset = (opcode & 0xff);
          if (offset == 0)
          {
            offset = (int16_t)READ_RAM16(address + 2);
            sprintf(instruction, "%s.w 0x%x (%d)", table_68000[n].instr, address + 2 + offset, offset);
            return 4;
          }
            else
          if (offset == 0xff)
          {
            offset = READ_RAM32(address + 2);
            sprintf(instruction, "%s.l 0x%x (%d)", table_68000[n].instr, address + 2 + offset, offset);
            return 6;
          }
            else
          {
            offset = (int)((char)offset);
            sprintf(instruction, "%s.s 0x%x (%d)", table_68000[n].instr, address + 2 + offset, offset);
            return 2;
          }
        case OP_EXT:
          reg = opcode & 0x7;
          mode = (opcode>>6)&0x7;
          if (mode != 2 && mode != 3) { break; }
          size = (mode == 2) ? SIZE_W : SIZE_L;
          sprintf(instruction, "%s.%c d%d", table_68000[n].instr, sizes[size], reg);
          return 2;
        case OP_LINK_W:
          reg = opcode & 0x7;
          sprintf(instruction, "%s.w a%d, #%d", table_68000[n].instr, reg, (int16_t)READ_RAM16(address + 2));
          return 4;
        case OP_LINK_L:
          reg = opcode & 0x7;
          sprintf(instruction, "%s.l a%d, #%d", table_68000[n].instr, reg, READ_RAM32(address + 2));
          return 6;
        case OP_DIV_MUL:
          reg = (opcode >> 9) & 0x7;
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s.w %s, d%d", table_68000[n].instr, ea, reg);
          return len;
        case OP_MOVEP:
          reg = (opcode >> 9) & 0x7;
          mode = (opcode >> 6) & 0x7;
          offset = READ_RAM16(address + 2);
          if (mode < 4) { break; }
          if (mode == 4 || mode == 5)
          {
            sprintf(instruction, "%s.%c (%d,a%d), d%d", table_68000[n].instr, sizes[mode - 3], (int16_t)offset, opcode & 0x7,reg);
          }
            else
          {
            sprintf(instruction, "%s.%c d%d, (%d,a%d)", table_68000[n].instr, sizes[mode - 5], reg, (short int)offset, opcode & 0x7);
          }
          return 4;
        case OP_MOVEM:
        {
          char reglist[128];
          uint16_t mask = READ_RAM16(address + 2);
          if (((opcode >> 3) & 0x7) == 4) { mask = reverse_bits16(mask); }
          get_reglist(reglist, mask);
          size = ((opcode >> 6) & 1) + 1;
          len = get_ea_68000(memory, address, ea, opcode, 2, size);
          if (((opcode >> 10) & 0x1) == 0)
          {
            sprintf(instruction, "%s.%c %s, %s", table_68000[n].instr, sizes[size], reglist, ea);
          }
            else
          {
            sprintf(instruction, "%s.%c %s, %s", table_68000[n].instr, sizes[size], ea, reglist);
          }
          return len + 2;
        }
        case OP_MOVE:
        {
          const int move_size[] = { 0, 0, 2, 1 };
          char dst_ea[128];
          int dst_len;
          uint16_t ea_dst = (opcode >> 6) & 0x3f;
          ea_dst = (ea_dst >> 3) | ((ea_dst & 0x7) << 3);

          if (is_ea_valid(&table_68000[n], opcode, 0) == 0) { break; }
          if (is_ea_valid(&table_68000[n], ea_dst, 1) == 0) { break; }

          size = (opcode >> 12) & 3;
          size = move_size[size];

          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          dst_len = get_ea_68000(memory, address, dst_ea, ea_dst, len - 2, size);
          sprintf(instruction, "%s.%c %s, %s", table_68000[n].instr, sizes[size], ea, dst_ea);
          return len + dst_len- 2;
        }
        case OP_JUMP:
          len = get_ea_68000(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s %s", table_68000[n].instr, ea);
          return len;
        case OP_DREG_EA:
          reg = (opcode >> 9) & 0x7;
          mode = (opcode >> 6) & 0x7;
          size = mode & 0x3;
          len = get_ea_68000(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c %s, d%d", table_68000[n].instr, sizes[size], ea, reg);
          return len;
        default:
          return -1;
      }
    }

    n++;
  }

  if ((opcode & 0xf0f8) == 0x50c8)
  {
    int16_t offset = READ_RAM16(address + 2);
    const char *cond_code = table_68000_condition_codes[(opcode >> 8) & 0xf];

    if (((opcode >> 8) & 0xf) == 1)
    {
      cond_code = "ra";  // if false, then always branch
    }

    sprintf(instruction, "db%s d%d, 0x%x (%d)", cond_code, opcode & 0x7, (address + 2) + offset, offset);
    return 4;
  }
    else
  if ((opcode & 0xf000) == 0x6000)
  {
    offset = (opcode & 0xff);
    if (offset == 0)
    {
      offset = (int16_t)READ_RAM16(address + 2);
      sprintf(instruction, "b%s.w 0x%x (%d)", table_68000_condition_codes[(opcode >> 8) & 0xf], address + 4 + offset, offset);
      return 4;
    }
      else
    if (offset == 0xff)
    {
      offset = READ_RAM32(address + 2);
      sprintf(instruction, "b%s.l 0x%x (%d)", table_68000_condition_codes[(opcode >> 8) & 0xf], address + 6 + offset, offset);
      return 6;
    }
      else
    {
      offset = (int)((char)offset);
      sprintf(instruction, "b%s.s 0x%x (%d)", table_68000_condition_codes[(opcode >> 8) & 0xf], address + 2 + offset, offset);
      return 2;
    }
    return 4;
  }
    else
  if ((opcode & 0xf0c0) == 0x50c0)
  {
    len = get_ea_68000(memory, address, ea, opcode, 0, 0);
    sprintf(instruction, "s%s %s", table_68000_condition_codes[(opcode >> 8) & 0xf], ea);
    return len;
  }

  strcpy(instruction, "???");
  return -1;
}

void list_output_68000(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min = -1, cycles_max = -1;
  int count;
  char instruction[128];
  int n;

  strcpy(instruction, "???");
  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_68000(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%04x: %04x %-40s\n",
      start,
      (memory_read_m(&asm_context->memory, start) << 8) |
       memory_read_m(&asm_context->memory, start + 1),
      instruction);

    if (count < 0) { count = 2; }

    for (n = 2; n < count; n += 2)
    {
      fprintf(asm_context->list, "        %04x\n",
        (memory_read_m(&asm_context->memory, start + n) << 8) |
         memory_read_m(&asm_context->memory, start + n + 1));
    }

    start += count;
  }
}

void disasm_range_68000(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  char temp2[4];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-11s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("----------- ------ ----------------------------------\n");

  while (start <= end)
  {
    count = disasm_68000(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(memory, start + n));
      strcat(temp, temp2);
    }

    printf("0x%04x: %04x   %-40s\n",
      start,
      (memory_read_m(memory, start) << 8) |
       memory_read_m(memory, start + 1), instruction);

    for (n = 2; n < count; n += 2)
    {
      printf("        %04x\n",
        (memory_read_m(memory, start + n) << 8) |
         memory_read_m(memory, start + n + 1));
    }

    start = start + count;
  }
}

