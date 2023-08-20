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
#include <stdint.h>

#include "disasm/msp430.h"
#include "table/msp430.h"

#define READ_RAM(a) memory->read8(a)

#define READ_RAM16(a) \
  (memory->read8(a + 1) << 8) | \
   memory->read8(a + 0)

static const char *regs[] =
{
  "PC", "SP",  "SR",  "CG",  "r4",  "r5",  "r6",  "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};
//static char *rpt[] = { "rptc", "rptz" };

// FIXME - Move this somewhere else
int get_register_msp430(const char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
    {
      return token[1] - '0';
    }
      else
    if (token[3] == 0 && token[1] == '1' &&
        token[2] >= '0' && token[2] <= '5')
    {
      return 10 + (token[2] - '0');
    }
  }

  if (strcasecmp(token, "pc") == 0) { return 0; }
  if (strcasecmp(token, "sp") == 0) { return 1; }
  if (strcasecmp(token, "sr") == 0) { return 2; }
  if (strcasecmp(token, "cg") == 0) { return 3; }

  return -1;
}

static int get_source_reg(
  Memory *memory,
  uint32_t address,
  int reg,
  int As,
  int bw,
  char *reg_str,
  int length,
  uint16_t prefix,
  int memory_ext)
{
  int count = 0;
  int extra = 0;

  if (memory_ext == 1) { extra = (prefix & 0x0780) << 9; }

  reg_str[0] = 0;

  if (reg == 0)
  {
    if (As == 0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (As == 1)
    {
      int32_t a = (READ_RAM(address + 3) << 8) | READ_RAM(address + 2);
      count += 2;

      if (prefix == 0xffff)
      {
        if ((a & 0x8000) != 0) { a |= 0xffff0000; }
      }
        else
      {
        a |= extra;
        if ((a & 0x80000) != 0) { a |= 0xfff00000; }
      }

      a = a + (address + count);
      snprintf(reg_str, length, "0x%04x", a);
    }
      else
    if (As == 2)
    {
      strcpy(reg_str, "@PC");
    }
      else
    if (As == 3)
    {
      uint16_t a = (READ_RAM(address + 3) << 8) | READ_RAM(address + 2);
      count += 2;
      if (bw == 0)
      {
        snprintf(reg_str, length, "#0x%04x", a | extra);
      }
        else
      {
        snprintf(reg_str, length, "#0x%02x", a | extra);
      }
    }
  }
    else
  if (reg == 2)
  {
    if (As == 0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (As == 1)
    {
      uint16_t a = (READ_RAM(address + 3) << 8) | READ_RAM(address + 2);
      count += 2;
      snprintf(reg_str, length, "&0x%04x", a | extra);
    }
      else
    if (As == 2)
    {
      strcat(reg_str, "#4");
    }
      else
    if (As == 3)
    {
      strcat(reg_str, "#8");
    }
  }
    else
  if (reg == 3)
  {
    if (As == 0)
    { strcat(reg_str, "#0"); }
      else
    if (As == 1)
    { strcat(reg_str, "#1"); }
      else
    if (As == 2)
    { strcat(reg_str, "#2"); }
      else
    if (As == 3)
    { strcat(reg_str, "#-1"); }
  }
    else
  {
    if (As == 0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (As == 1)
    {
      int32_t a = (READ_RAM(address + 3) << 8) | READ_RAM(address + 2);
      count += 2;

      if (prefix == 0xffff)
      {
        if ((a & 0x8000) != 0) { a |= 0xffff0000; }
      }
        else
      {
        a |= extra;
        if ((a & 0x80000) != 0) { a |= 0xfff00000; }
      }

      snprintf(reg_str, length, "%d(%s)", a, regs[reg]);
    }
      else
    if (As == 2)
    {
      snprintf(reg_str, length, "@%s", regs[reg]);
    }
      else
    if (As == 3)
    {
      snprintf(reg_str, length, "@%s+", regs[reg]);
    }
  }

  return count;
}

static int get_dest_reg(
  Memory *memory,
  uint32_t address,
  int reg,
  int Ad,
  char *reg_str,
  int length,
  int count,
  uint16_t prefix,
  int memory_ext)
{
  int extra = 0;

  if (memory_ext == 1) { extra = (prefix & 0x000f) << 16; }

  reg_str[0] = 0;

  if (reg == 0)
  {
    if (Ad == 0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (Ad == 1)
    {
      int32_t a = (READ_RAM(address + count + 3) << 8) | READ_RAM(address + (count + 2));
      count += 2;

      if (prefix == 0xffff)
      {
        if ((a & 0x8000) != 0) { a |= 0xffff0000; }
      }
        else
      {
        a |= extra;
        if ((a & 0x80000) != 0) { a |= 0xfff00000; }
      }

      a = a + (address + count);
      snprintf(reg_str, length, "0x%04x", a);
    }
  }
    else
  if (reg == 2)
  {
    if (Ad == 0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (Ad == 1)
    {
      uint16_t a = (READ_RAM(address + count + 3) << 8) | READ_RAM(address + count + 2);
      count += 2;
      snprintf(reg_str, length, "&0x%04x", a|extra);
    }
  }
    else
  if (reg == 3)
  {
    strcat(reg_str, regs[reg]);
  }
    else
  {
    if (Ad == 0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (Ad == 1)
    {
      int32_t a = (READ_RAM(address + count + 3) << 8)|READ_RAM(address + count + 2);
      count += 2;

      if (prefix == 0xffff)
      {
        if ((a & 0x8000) != 0) { a |= 0xffff0000; }
      }
        else
      {
        a |= extra;
        if ((a & 0x80000) != 0) { a |= 0xfff00000; }
      }

      snprintf(reg_str, length, "%d(%s)", a, regs[reg]);
    }
  }

  return count;
}

static int one_operand(
  Memory *memory,
  uint32_t address,
  char *instruction,
  uint16_t opcode,
  uint16_t prefix)
{
  char ext[3] = { 0 };
  int o;
  int reg;
  int As;
  int count = 2;
  int bw = 0;
  int memory_ext = 0;

  As = (opcode & 0x0030) >> 4;
  reg = opcode & 0x000f;
  o = (opcode & 0x0380) >> 7;

  if (prefix != 0xffff)
  {
    if (As == 0)
    {
#if 0
      char temp[64];
      int r = (prefix >> 8) & 1;

      if ((prefix & 0x0080) == 0)
      {
        snprintf(temp, sizeof(temp), "%s #%d %s", rpt[r], (prefix & 0xf) + 1, instruction);
      }
        else
      {
        snprintf(temp, sizeof(temp), "%s r%d %s", rpt[r], prefix & 0xf, instruction);
      }

      strcpy(instruction, temp);
#endif
    }
      else
    {
      memory_ext = 1;
    }

    strcat(instruction, "x");
  }

  if ((opcode & 0x0040) == 0)
  {
    if ((o & 1) == 0)
    {
      strcpy(ext, ".w");
    }
  }
    else
  {
    strcpy(ext, ".b");
    bw = 1;
  }

  if (prefix != 0xffff && o <= 5)
  {
    int al = ((prefix >> 5) & 2) | bw;

    if ((o & 1) ==1)
    {
      if (al == 0) { strcpy(ext, ".a"); }
      else if (al == 1) { strcpy(ext, ".?"); }
      else if (al == 2) { strcpy(ext, ".w"); }
      else if (al == 3) { strcpy(ext, ".?"); }
    }
      else
    {
      if (al == 0) { strcpy(ext, ".?"); }
      else if (al == 1) { strcpy(ext, ".a"); }
      else if (al == 2) { strcpy(ext, ".w"); }
      else if (al == 3) { strcpy(ext, ".b"); }
    }
  }

  strcat(instruction, ext);
  strcat(instruction, " ");

  char reg_str[128];
  count += get_source_reg(memory, address, reg, As, bw, reg_str, sizeof(reg_str), prefix, memory_ext);
  strcat(instruction, reg_str);

  return count;
}

static int relative_jump(
  Memory *memory,
  uint32_t address,
  char *instruction,
  uint16_t opcode,
  uint16_t prefix)
{
  int count = 2;
  int o;

  o = (opcode & 0x1c00) >> 10;
  if (o > 7)
  {
    printf("WTF JMP?\n");
    return 1;
  }

  if (prefix != 0xffff) { strcat(instruction, "x"); }

  int offset = opcode & 0x03ff;
  if ((offset & 0x0200) != 0)
  {
    offset = -((offset ^ 0x03ff) + 1);
  }

  offset *= 2;

  char token[128];
  snprintf(token, sizeof(token), " 0x%04x  (offset: %d)", ((address + 2) + offset) & 0xffff, offset);
  strcat(instruction, token);

  return count;
}

static int two_operand(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  uint16_t opcode,
  uint16_t prefix)
{
  char ext[3] = { 0 };
  int o;
  int Ad,As;
  int count = 0;
  int bw = 0;
  int memory_ext = 0;
  int src,dst;

  Ad = (opcode & 0x0080) >> 7;
  As = (opcode & 0x0030) >> 4;
  src = (opcode >> 8) & 0xf;
  dst = opcode & 0xf;
  o = opcode >> 12;
  o = o - 4;

  if (prefix != 0xffff)
  {
    if (Ad == 0 && (As == 0 || src == 3 || (src == 2 && As != 1)) )
    {
#if 0
      char temp[64];
      int r = (prefix >> 8) & 1;

      if ((prefix & 0x0080) == 0)
      {
        snprintf(temp, sizeof(temp), "%s #%d %s",
          rpt[r], (prefix & 0xf) + 1, instruction);
      }
        else
      {
        snprintf(temp, sizeof(temp), "%s r%d %s",
          rpt[r], prefix & 0xf, instruction);
      }

      strcpy(instruction, temp);
#endif
    }
      else
    {
      memory_ext = 1;
    }

    strcat(instruction, "x");
  }

  if ((opcode & 0x0040) == 0)
  {
    strcpy(ext, ".w");
    bw = 0;
  }
    else
  {
    strcpy(ext, ".b");
    bw = 1;
  }

  if (prefix == 0xffff)
  {
    char instr[32];
    strcpy(instr, instruction);
    if ((opcode & 0x00ff) == 0x0003)
    { snprintf(instruction, length, "nop   --  %s", instr); }
      else
    if (opcode == 0x4130)
    { snprintf(instruction, length, "ret   --  %s", instr); }
      else
    if ((opcode & 0xffb0) == 0x4130)
    { snprintf(instruction, length, "pop.%c r%d   --  %s", bw == 0 ? 'w':'b', opcode & 0x000f, instr); }
      else
    if ((opcode & 0xffb0) == 0x41b0)
    { snprintf(instruction, length, "pop.%c %d(r%d)   --  %s", bw == 0 ? 'w':'b', (int16_t)READ_RAM16(address + 2), opcode & 0x000f, instr); }
      else
    if (opcode == 0xc312)
    { snprintf(instruction, length, "clrc  --  %s", instr); }
      else
    if (opcode == 0xc222)
    { snprintf(instruction, length, "clrn  --  %s", instr); }
      else
    if (opcode == 0xc322)
    { snprintf(instruction, length, "clrz  --  %s", instr); }
      else
    if (opcode == 0xc232)
    { snprintf(instruction, length, "dint  --  %s", instr); }
      else
    if (opcode == 0xd312)
    { snprintf(instruction, length, "setc  --  %s", instr); }
      else
    if (opcode == 0xd222)
    { snprintf(instruction, length, "setn  --  %s", instr); }
      else
    if (opcode == 0xd322)
    { snprintf(instruction, length, "setz  --  %s", instr); }
      else
    if (opcode == 0xd232)
    { snprintf(instruction, length, "eint  --  %s", instr); }
  }
    else
  {
    //strcat(instruction, "x");

    int al = ((prefix >> 5) & 2) | bw;

    if (al == 0) { strcpy(ext, ".?"); }
    else if (al == 1) { strcpy(ext, ".a"); }
    else if (al == 2) { strcpy(ext, ".w"); }
    else if (al == 3) { strcpy(ext, ".b"); }
  }

  strcat(instruction, ext);
  strcat(instruction, " ");

  char reg_str[128];
  count = get_source_reg(memory, address, src, As, bw, reg_str, sizeof(reg_str), prefix, memory_ext);
  strcat(instruction, reg_str);

  strcat(instruction, ", ");
  count = get_dest_reg(memory, address, dst, Ad, reg_str, sizeof(reg_str), count, prefix, memory_ext);
  strcat(instruction, reg_str);

  return count + 2;
}

int get_cycle_count(uint16_t opcode)
{
  int src_reg,dst_reg;
  int Ad,As;

  if ((opcode & 0xfc00) == 0x1000)
  {
    // One operand
    int o = (opcode & 0x0380) >> 7;
    As = (opcode & 0x0030) >> 4;
    src_reg = opcode & 0x000f;

    if (((opcode & 0x0040) != 0) && (o == 1 || o == 3 || o == 5 || o == 6)) { return -1; }

    if (src_reg == 3 || (src_reg == 2 && (As & 2) == 2)) { As = 0; }

    if (o == 6) { return 5; }    // RETI
    if (o == 7) { return -1; }   // Illegal

    if (o == 5) // CALL
    {
      if (As == 1) { return 5; }    // x(Rn), EDE, &EDE
      if (As == 2) { return 4; }    // @Rn
      if (As == 3)
      {
        if (src_reg == 0) return 5; // #value
        return 5;                   // @Rn+
      }

      return 4;                     // Rn
    }

    if (o == 4) // PUSH
    {
      if (As == 1) { return 5; }    // x(Rn), EDE, &EDE
      if (As == 2) { return 4; }    // @Rn
      if (As == 3)
      {
        if (src_reg == 0) return 4; // #value
        return 5;                   // @Rn+
      }

      return 3;                     // Rn
    }

    // RRA, RRC, SWPB, SXT
    if (As == 1) { return 4; }      // x(Rn), EDE, &EDE
    if (As == 2) { return 3; }      // @Rn
    if (As == 3)
    {
      if (src_reg == 0) { return -1; } // #value
      return 3;                    // @Rn+
    }

    return 1;                   // Rn
  }
    else
  if ((opcode & 0xe000) == 0x2000)
  {
    // Jump
    return 2;
  }
    else
  {
    // Two operand
    Ad = (opcode & 0x0080) >> 7;
    As = (opcode & 0x0030) >> 4;
    src_reg = (opcode >> 8) & 0x000f;
    dst_reg = opcode & 0x000f;

    if (src_reg == 3 || (src_reg == 2 && (As & 2) == 2)) { As = 0; }
    if (dst_reg == 3) { Ad = 0; }

    if ((opcode >> 12) < 4) { return -1; }

    // Cycle counts
    if (As == 1) //Src EDE, &EDE, x(Rn)
    {
      if (Ad == 1) { return 6; } // Dest x(Rn) and TONI and &TONI
      return 3;  // Dest
    }
      else
    if (As == 3)  // #value and @Rn+
    {
      if (dst_reg == 0) { return 3; } // Dest PC
      if (Ad == 0) { return 2; }      // Dest Rm
      return 5;                       // Dest x(Rm), EDE, &EDE
    }
      else
    if (As  ==  2)  // @Rn
    {
      if (dst_reg  ==  0) { return 2; } // Dest PC
      if (Ad  ==  0) { return 2; }    // Dest Rm
      return 5;                       // Dest x(Rm), EDE, &EDE
    }
      else     // Rn
    {
      if (dst_reg == 0) { return 2; } // Dest PC
      if (Ad == 0) { return 1; }    // Dest Rm
      return 4;                     // Dest x(Rm), EDE, &EDE
    }
  }

  return -1;
}

int disasm_msp430(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  uint16_t opcode;
  uint16_t prefix = -1;
  int dst,src,num,wa;
  int count = 0;
  int n;

  instruction[0] = 0;
  opcode = READ_RAM16(address);
  // FIXME - this doesn't work for MSP430X
  *cycles_min = get_cycle_count(opcode);
  *cycles_max = *cycles_min;

  if (opcode == 0x0110)
  {
    snprintf(instruction, length, "reta  --  mova @SP+, PC");
    return 2;
  }

  // 20 bit prefix to 16 bit instructions
  if ((opcode & 0xf830) == 0x1800)
  {
    prefix = opcode;
    opcode = READ_RAM16(address + 2);
    address += 2;
    count = 2;
  }

  n = 0;
  while (table_msp430[n].instr != NULL)
  {
    if (table_msp430[n].version == VERSION_MSP430X_EXT) { n++; continue; }

    char mode[] = { 'a','w' };

    if ((opcode & table_msp430[n].mask) == table_msp430[n].opcode)
    {
      switch (table_msp430[n].type)
      {
        case OP_NONE:
          strcpy(instruction, table_msp430[n].instr);
          break;
        case OP_ONE_OPERAND:
        case OP_ONE_OPERAND_W:
        case OP_ONE_OPERAND_X:
          strcpy(instruction, table_msp430[n].instr);
          count += one_operand(memory, address, instruction, opcode, prefix);
          prefix = 0xffff;
          break;
        case OP_JUMP:
          strcpy(instruction, table_msp430[n].instr);
          count += relative_jump(memory, address, instruction, opcode, prefix);
          break;
        case OP_TWO_OPERAND:
          strcpy(instruction, table_msp430[n].instr);
          count += two_operand(memory, address, instruction,length,  opcode, prefix);
          prefix = 0xffff;
          break;
        case OP_MOVA_AT_REG_REG:
          src = (opcode >> 8) & 0xf;
          dst = opcode & 0xf;
          snprintf(instruction, length, "mova @%s, %s", regs[src], regs[dst]);
          count += 2;
          break;
        case OP_MOVA_AT_REG_PLUS_REG:
          src = (opcode>>8) & 0xf;
          dst = opcode&0xf;
          snprintf(instruction, length, "mova @%s+, %s", regs[src], regs[dst]);
          count += 2;
          break;
        case OP_MOVA_ABS20_REG:
          num = (((opcode >> 8) & 0xf) << 16)|(READ_RAM16(address + 2));
          dst = opcode & 0xf;
          snprintf(instruction, length, "mova &0x%x, %s", num, regs[dst]);
          count += 4;
          break;
        case OP_MOVA_INDEXED_REG:
          num = READ_RAM16(address + 2);
          src = (opcode >> 8) & 0xf;
          dst = opcode & 0xf;

          if (src != 0)
          {
            snprintf(instruction, length, "mova %d(%s), %s",
              (int16_t)num, regs[src], regs[dst]);
          }
            else
          {
            int symbolic = (address + 2) + (int16_t)num;
            snprintf(instruction, length, "mova 0x%04x, %s",
              symbolic, regs[dst]);
          }
          count += 4;
          break;
        case OP_SHIFT20:
          num = ((opcode >> 10) & 0x3) + 1;
          wa = (opcode >> 4) & 0x1;
          dst = opcode & 0xf;
          *cycles_min = num;
          *cycles_max = num;
          snprintf(instruction, length, "%s.%c #%d, %s",
            table_msp430[n].instr, mode[wa], num, regs[dst]);
          count += 2;
          break;
        case OP_MOVA_REG_ABS:
          num = ((opcode & 0xf) << 16) | READ_RAM16(address + 2);
          src = (opcode >> 8) & 0xf;
          snprintf(instruction, length, "mova %s, &0x%x", regs[src], num);
          count += 4;
          break;
        case OP_MOVA_REG_INDEXED:
          num = READ_RAM16(address+2);
          src = (opcode >> 8) & 0xf;
          dst = opcode & 0xf;
          snprintf(instruction, length, "mova %s, %d(%s)",
            regs[src], (int16_t)num, regs[dst]);
          count += 4;
          break;
        case OP_IMMEDIATE_REG:
          num = ((opcode&0x0f00)<<8)|READ_RAM16(address+2);
          dst = opcode & 0xf;
          snprintf(instruction, length, "%s #0x%x, %s",
            table_msp430[n].instr, num, regs[dst]);
          count += 4;
          break;
        case OP_REG_REG:
          src = (opcode >> 8) & 0xf;
          dst = opcode & 0xf;
          snprintf(instruction, length, "%s %s, %s",
            table_msp430[n].instr, regs[src], regs[dst]);
          count += 2;
          break;
        case OP_CALLA_SOURCE:
        {
          char temp[32];
          int as = (opcode >> 4) & 0x3;
          dst = opcode & 0xf;
          if (as == 0)
          {
            snprintf(temp, sizeof(temp), "%s", regs[dst]);
            *cycles_min = 4;
          }
          else if (as == 1)
          {
            if (dst == 0)
            {
              int16_t offset = READ_RAM16(address + 2);
              snprintf(temp, sizeof(temp), "%d(%s) -- 0x%x",
                (int16_t)(READ_RAM16(address + 2)),
                regs[dst],
                (address + 4) + offset);
            }
              else
            {
              snprintf(temp, sizeof(temp), "%d(%s)",
                (int16_t)(READ_RAM16(address + 2)),
                regs[dst]);
            }
            *cycles_min = 6;
            if (dst == 1) (*cycles_min)++; // if Rn=SP increment by 1
          }
          else if (as == 2)
          {
            snprintf(temp, sizeof(temp), "@%s", regs[dst]); *cycles_min = 5;
          }
          else if (as == 3)
          {
            snprintf(temp, sizeof(temp), "@%s+", regs[dst]); *cycles_min = 5;
          }

          snprintf(instruction, length, "%s %s", table_msp430[n].instr, temp);
          *cycles_max = *cycles_min;
          count += (as == 1) ? 4 : 2;
          break;
        }
        case OP_CALLA_ABS20:
          num = ((opcode & 0xf) << 16) | READ_RAM16(address + 2);
          snprintf(instruction, length, "%s &0x%x", table_msp430[n].instr, num);
          *cycles_min = 6;
          *cycles_max = *cycles_min;
          count += 4;
          break;
        case OP_CALLA_INDIRECT_PC:
          num = ((opcode & 0xf) << 16) | READ_RAM16(address + 2);
          if ((num & 0x80000) != 0) { num |= 0xfff0000; }
          snprintf(instruction, length, "%s 0x%x(%d)",
            table_msp430[n].instr, address + 4 + num, num);
          *cycles_min = 6;
          *cycles_max = *cycles_min;
          count += 4;
          break;
        case OP_CALLA_IMMEDIATE:
          num = ((opcode & 0xf) << 16) | READ_RAM16(address + 2);
          snprintf(instruction, length, "%s #0x%x", table_msp430[n].instr, num);
          *cycles_min = 4;
          *cycles_max = *cycles_min;
          count += 4;
          break;
        case OP_PUSH:
          src = opcode & 0xf;
          num = (opcode >> 4) & 0xf;
          wa = (opcode >> 8) & 0x1;
          snprintf(instruction, length, "pushm.%c #%d, %s",
            mode[wa], num+1, regs[src]);
          *cycles_min = 2 + (num + 1) * (wa + 1);
          *cycles_max = *cycles_min;
          count += 2;
          break;
        case OP_POP:
          dst = opcode & 0xf;
          num = (opcode >> 4) & 0xf;
          wa = (opcode >> 8) & 0x1;
          snprintf(instruction, length, "popm.%c #%d, %s",
            mode[wa], num+1, regs[dst]);
          *cycles_min = 2 + (num + 1) * (wa + 1);
          *cycles_max = *cycles_min;
          count += 2;
          break;
        default:
          snprintf(instruction, length, "%s << wtf", table_msp430[n].instr);
          break;
      }

      break;
    }

    n++;
  }

  if (table_msp430[n].instr == NULL) { strcpy(instruction, "???"); }

  if (prefix != 0xffff)
  {
    char rpt[128];
    char zc = (((prefix >> 8) & 1) == 1) ? 'z' : 'c';
    int n = prefix & 0xf;

    if ((prefix & 0xfeb0) == 0x1800)
    {
      snprintf(rpt, sizeof(rpt), "rpt%c #%d, %s", zc, n + 1, instruction);
      strcpy(instruction, rpt);
    }
      else
    if ((prefix & 0xfeb0) == 0x1880)
    {
      snprintf(rpt, sizeof(rpt), "rpt%c r%d, %s", zc, n, instruction);
      strcpy(instruction, rpt);
    }

    *cycles_min = -1;
    *cycles_max = -1;
  }

  return count;
}

int disasm_msp430x(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  uint16_t opcode = READ_RAM16(address);

  if (opcode == 0x1300)
  {
    snprintf(instruction, length, "reti");
    *cycles_min = 3;
    *cycles_max = *cycles_min;
    return 2;
  }

  return disasm_msp430(
    memory,
    address,
    instruction,
    length,
    0,
    cycles_min,
    cycles_max);
}

static void list_output_msp430_both(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end,
  bool is_msp430x)
{
  int cycles_min,cycles_max,count;
  int num;
  char instruction[128];

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    if (is_msp430x == false)
    {
      count = disasm_msp430(
        &asm_context->memory,
        start,
        instruction,
        sizeof(instruction),
        asm_context->flags,
        &cycles_min,
        &cycles_max);
    }
      else
    {
      count = disasm_msp430x(
        &asm_context->memory,
        start,
        instruction,
        sizeof(instruction),
        asm_context->flags,
        &cycles_min,
        &cycles_max);
    }

    num = asm_context->memory_read(start) |
          asm_context->memory_read(start + 1) << 8;

    if (cycles_min < 0)
    {
      fprintf(asm_context->list, "0x%04x: 0x%04x %-40s cycles: ?\n", start, num, instruction);
    }
      else
    {
      fprintf(asm_context->list, "0x%04x: 0x%04x %-40s cycles: %d\n", start, num, instruction, cycles_min);
    }

    count -= 2;
    start += 2;

    while (count > 0)
    {
      num = asm_context->memory_read(start) |
            asm_context->memory_read(start + 1) << 8;
      fprintf(asm_context->list, "0x%04x: 0x%04x\n", start, num);
      count -= 2;
      start += 2;
    }
  }
}

static void disasm_range_msp430_both(
  Memory *memory,
  int start,
  int end,
  int msp430x)
{
  // Are these correct and the same for all MSP430's?
  const char *vectors[16] =
  {
    "", "", "", "", "", "",
    "", "", "", "", "", "",
    "", "", "", "Reset/Watchdog/Flash"
  };
  char instruction[128];
  int vectors_flag = 0;
  int cycles_min = 0, cycles_max = 0;
  int num, count;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    if (start >= 0xffe0 && vectors_flag == 0)
    {
      printf("\nVectors:\n");
      vectors_flag = 1;
    }

    num = READ_RAM(start) | (READ_RAM(start + 1) << 8);

    if (vectors_flag == 1)
    {
      if (start > 0xffff)
      {
        printf("\n");
        vectors_flag = 0;
      }
        else
      {
        printf("0x%04x: 0x%04x  Vector %2d {%s}\n", start, num, (start - 0xffe0) / 2, vectors[(start - 0xffe0) / 2]);

        start += 2;
        continue;
      }
    }

    if (msp430x == 0)
    {
      count = disasm_msp430(
        memory,
        start,
        instruction,
        sizeof(instruction),
        0,
        &cycles_min,
        &cycles_max);
    }
      else
    {
      count = disasm_msp430x(
        memory,
        start,
        instruction,
        sizeof(instruction),
        0,
        &cycles_min,
        &cycles_max);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%04x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: 0x%04x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%04x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

    count -= 2;

    while (count > 0)
    {
      start = start + 2;
      num = READ_RAM(start) | (READ_RAM(start + 1) << 8);
      printf("0x%04x: 0x%04x\n", start, num);
      count -= 2;
    }

    start = start + 2;
  }
}

void list_output_msp430(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  list_output_msp430_both(asm_context, start, end, false);
}

void list_output_msp430x(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  list_output_msp430_both(asm_context, start, end, true);
}

void disasm_range_msp430(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  disasm_range_msp430_both(memory, start, end, 0);
}

void disasm_range_msp430x(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  disasm_range_msp430_both(memory, start, end, 1);
}

