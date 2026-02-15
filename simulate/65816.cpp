/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 * 65816 file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/65816.h"
#include "disasm/65816.h"
#include "simulate/65816.h"
#include "table/65816.h"

#define SHOW_STACK sp, memory->read8(sp)
#define READ_RAM(a) memory->read8(a)
#define WRITE_RAM(a, b) memory->write8(a, b)

// status register flags
#define flag_c 0
#define flag_z 1
#define flag_i 2
#define flag_d 3
#define flag_x 4
#define flag_m 5
#define flag_v 6
#define flag_n 7

#define READ_BIT(dst, a) ((dst & (1 << a)) ? 1 : 0)
#define SET_BIT(dst, a) (dst |= (1 << a))
#define CLEAR_BIT(dst, a) (dst &= ~(1 << a))

#define READ_FLAG(a) ((reg_sr & (1 << a)) ? 1 : 0)
#define SET_FLAG(a) (reg_sr |= (1 << a))
#define CLEAR_FLAG(a) (reg_sr &= ~(1 << a))

#define FLAG(condition, flag) if (condition) SET_FLAG(flag); else CLEAR_FLAG(flag)

Simulate65816::Simulate65816(Memory *memory) : Simulate(memory)
{
  reset();
}

Simulate65816::~Simulate65816()
{
}

Simulate *Simulate65816::init(Memory *memory)
{
  return new Simulate65816(memory);
}

void Simulate65816::reset()
{
  cycle_count = 0;
  nested_call_count = 0;
  break_point = -1;

  reg_a = 0;
  reg_x = 0;
  reg_y = 0;
  reg_sr = 0;
  reg_pc = org;
  reg_sp = 0x1FF;
  reg_db = 0;
  reg_pb = 0;
}

void Simulate65816::push(uint32_t value)
{
  WRITE_RAM(reg_sp, value & 0xFF);
  reg_sp--;
  reg_sp &= 0xFF;
}

int Simulate65816::set_reg(const char *reg_string, uint32_t value)
{
  while (*reg_string==' ') { reg_string++; }

  if (strcasecmp(reg_string, "a") == 0)
    reg_a = value & 0xFFFF;
  else if (strcasecmp(reg_string, "x") == 0)
    reg_x = value & 0xFFFF;
  else if (strcasecmp(reg_string, "y") == 0)
    reg_y = value & 0xFFFF;
  else if (strcasecmp(reg_string, "sr") == 0)
    reg_sr = value & 0xFF;
  else if (strcasecmp(reg_string, "pc") == 0)
    reg_pc = value & 0xFFFF;
  else if (strcasecmp(reg_string, "sp") == 0)
    reg_sp = value & 0xFFFF;
  else if (strcasecmp(reg_string, "db") == 0)
    reg_db = value & 0xFF;
  else if (strcasecmp(reg_string, "pb") == 0)
    reg_pb = value & 0xFF;
  else
    return -1;

  return 0;
}

uint32_t Simulate65816::get_reg(const char *reg_string)
{
  if (strcasecmp(reg_string, "a") == 0)
    return reg_a;
  else if (strcasecmp(reg_string, "x") == 0)
    return reg_x;
  else if (strcasecmp(reg_string, "y") == 0)
    return reg_y;
  else if (strcasecmp(reg_string, "sr") == 0)
    return reg_sr;
  else if (strcasecmp(reg_string, "pc") == 0)
    return reg_pc;
  else if (strcasecmp(reg_string, "sp") == 0)
    return reg_sp;
  else if (strcasecmp(reg_string, "db") == 0)
    return reg_db;
  else if (strcasecmp(reg_string, "pb") == 0)
    return reg_pb;

  return -1;
}

void Simulate65816::set_pc(uint32_t value)
{
  reg_pc = value;
}

void Simulate65816::dump_registers()
{
  int sp = reg_sp;

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("---------------------------------------------------------------\n");
  printf("        7 6 5 4 3 2 1 0                          0x%04x: 0x%04x\n", SHOW_STACK);
  sp = (sp - 2) & 0xFFFF;

  printf("Status: N V M X D I Z C                          0x%04x: 0x%04x\n", SHOW_STACK);
  sp = (sp - 2) & 0xFFFF;
  printf("        %d %d %d %d %d %d %d %d                          0x%04x: 0x%04x\n",
    READ_FLAG(flag_n),
    READ_FLAG(flag_v),
    READ_FLAG(flag_m),
    READ_FLAG(flag_x),
    READ_FLAG(flag_d),
    READ_FLAG(flag_i),
    READ_FLAG(flag_z),
    READ_FLAG(flag_c),
    SHOW_STACK);

  sp = (sp - 2) & 0xFFFF;
  printf("                                                 0x%04x: 0x%04x\n", SHOW_STACK);

  sp = (sp - 2) & 0xFFFF;
  printf("  A=0x%04x    X=0x%04x    Y=0x%04x               0x%04x: 0x%04x\n", reg_a, reg_x, reg_y, SHOW_STACK);

  sp = (sp - 2) & 0xFFFF;
  printf(" SR=0x%02x     SP=0x%04x   PC=0x%04x               0x%04x: 0x%04x\n", reg_sr, reg_sp, reg_pc, SHOW_STACK);

  sp = (sp - 2) & 0xFFFF;
  printf(" DB=0x%02x     PB=0x%02x                             0x%04x: 0x%04x\n", reg_db, reg_pb, SHOW_STACK);

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int Simulate65816::run(int max_cycles, int step)
{
  char instruction[128];

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    int pc = reg_pc;
    int cycles_min, cycles_max;
    int opcode = READ_RAM(pc);

    int ret = operand_exe(opcode);

    // stop simulation on BRK instruction
    if (ret == -1)
      break;

    // only increment if reg_pc not touched
    if (ret == 0)
    {
      reg_pc += disasm_65816(
        memory,
        pc,
        instruction,
        sizeof(instruction),
        0,
        &cycles_min,
        &cycles_max);
    }

    if (show == true)
    {
      clear_screen();
      dump_registers();

      int n = 0;
      while (n < 6)
      {
        int count = disasm_65816(
          memory,
          pc,
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        if (cycles_min == -1) break;

        if (pc == break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == reg_pc) { printf("> "); }
          else
        { printf("  "); }

        printf("0x%04x: %-40s %d-%d\n", pc, instruction, cycles_min, cycles_max);
        n += count;
        pc += count;

        count--;
        while (count > 0)
        {
          if (pc == break_point) { printf("*"); }
          else { printf(" "); }
          printf("  0x%04x: 0x%04x\n", pc, READ_RAM(pc));
          pc += count;
          count--;
        }
      }
    }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (break_point == reg_pc)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == 1)
    {
      disable_signal_handler();
      return 0;
    }

    if (reg_pc == 0xFFFF)
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      reg_pc = READ_RAM(0xFFFC) + READ_RAM(0xFFFD) * 256;

      disable_signal_handler();
      return 0;
    }

    usleep(usec > 999999 ? 999999 : usec);
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", reg_pc);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

// Return calculated address for each mode.
int Simulate65816::calc_address(int address, int mode)
{
  int lo =   READ_RAM((address + 0) & 0xFFFF);
  int hi =   READ_RAM((address + 1) & 0xFFFF);
  // int bank = READ_RAM((address + 2) & 0xFFFF);
  int indirect;

  switch(mode)
  {
    case OP_NONE:
      return address;
    case OP_IMMEDIATE8:
      return address;
    case OP_IMMEDIATE16:
      return address;
    case OP_ADDRESS8:
      return lo & 0xFF;
    case OP_ADDRESS16:
      return lo + 256 * hi;
    case OP_ADDRESS24:
      return 0;
    case OP_INDEXED8_X:
      return (lo + reg_x) & 0xFF;
    case OP_INDEXED8_Y:
      return (lo + reg_y) & 0xFFFF;
    case OP_INDEXED16_X:
      return ((lo + 256 * hi) + reg_x) & 0xFFFF;
    case OP_INDEXED16_Y:
      return ((lo + 256 * hi) + reg_y) & 0xFFFF;
    case OP_INDEXED24_X:
      return 0;
    case OP_INDIRECT8:
      return 0;
    case OP_INDIRECT8_LONG:
      return 0;
    case OP_INDIRECT16:
      indirect = (lo + 256 * hi) & 0xFFFF;
      return (READ_RAM(indirect) + 256 * READ_RAM((indirect + 1) & 0xFFFF)) & 0xFFFF;
      return 0;
    case OP_INDIRECT16_LONG:
      return 0;
    case OP_X_INDIRECT8:
      indirect = ((READ_RAM(lo) + reg_x) & 0xFF) + 256 * READ_RAM((lo + 1) & 0xFF);
      return (indirect) & 0xFFFF;
    case OP_X_INDIRECT16:
      return 0;
    case OP_INDIRECT8_Y:
      indirect = READ_RAM(lo) + 256 * READ_RAM((lo + 1) & 0xFF);
      return (indirect + reg_y) & 0xFFFF;
    case OP_INDIRECT8_Y_LONG:
      return 0;
    case OP_BLOCK_MOVE:
      return 0;
    case OP_RELATIVE:
      return (address + ((signed char)READ_RAM(address) + 1)) & 0xFFFF;
    case OP_RELATIVE_LONG:
      return 0;
    case OP_SP_RELATIVE:
      return 0;
    case OP_SP_INDIRECT_Y:
      return 0;
    default:
      return -1;
  }
}

int Simulate65816::operand_exe(int opcode)
{
  if (opcode < 0 || opcode > 0xFF)
  {
    return -1;
  }

  int mode = table_65816_opcodes[opcode].op;

  int address = calc_address(reg_pc + 1, mode);

  if (address == -1)
  {
    return -1;
  }

  int m = READ_RAM(address);
  int temp;
  int pc_lo, pc_hi;
  int temp_a = reg_a;

  cycle_count += table_65816_opcodes[opcode].cycles_min;

  //FIXME add extra cycles when required below
  switch(opcode)
  {
    // ADC
    case 0x61:
    case 0x65:
    case 0x69:
    case 0x6D:
    case 0x71:
    case 0x75:
    case 0x79:
    case 0x7D:
      if (READ_FLAG(flag_d))
      {
        int bcd_a = (reg_a & 15) + 10 * (reg_a >> 4);
        int bcd_m = (m & 15) + 10 * (m >> 4);
        int result = bcd_a + bcd_m + READ_FLAG(flag_c);

        FLAG(result > 99, flag_c);
        result %= 100;

        reg_a = (result % 10) + ((result / 10) << 4);
      }
        else
      {
        reg_a += m + READ_FLAG(flag_c);

        FLAG(reg_a > 255, flag_c);
      }

      reg_a &= 0xFF;
      FLAG((temp_a ^ reg_a) & (m ^ reg_a) & 0x80, flag_v);
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);

      break;
    // AND
    case 0x21:
    case 0x25:
    case 0x29:
    case 0x2D:
    case 0x31:
    case 0x35:
    case 0x39:
    case 0x3D:
      reg_a &= m;
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);
      break;
    // ASL
    case 0x06:
    case 0x0A:
    case 0x0E:
    case 0x16:
    case 0x1E:
      if (mode == 4)
      {
        FLAG(READ_BIT(reg_a, 7), flag_c);
        reg_a <<= 1;
        reg_a &= 0xFF;
        FLAG(reg_a > 127, flag_n);
        FLAG(reg_a == 0, flag_z);
      }
        else
      {
        FLAG(READ_BIT(m, 7), flag_c);
        m <<= 1;
        m &= 0xFF;
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // BCC
    case 0x90:
      if (READ_FLAG(flag_c) == 0)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BCS
    case 0xB0:
      if (READ_FLAG(flag_c) == 1)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BEQ
    case 0xF0:
      if (READ_FLAG(flag_z) == 1)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BMI
    case 0x30:
      if (READ_FLAG(flag_n) == 1)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BNE
    case 0xD0:
      if (READ_FLAG(flag_z) == 0)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BPL
    case 0x10:
      if (READ_FLAG(flag_n) == 0)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BVC
    case 0x50:
      if (READ_FLAG(flag_v) == 0)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BVS
    case 0x70:
      if (READ_FLAG(flag_v) == 1)
      {
        reg_pc = address;
        return 1;
      }
      break;
    // BRK
    case 0x00:
      // stop simulation
      return -1;
    // BIT
    case 0x24:
    case 0x2C:
      FLAG((reg_a & m) == 0, flag_z);
      FLAG(READ_BIT(m, 6), flag_v);
      FLAG(READ_BIT(m, 7), flag_n);
      break;
    // CLC
    case 0x18:
      CLEAR_FLAG(flag_c);
      break;
    // CLD
    case 0xD8:
      CLEAR_FLAG(flag_d);
      break;
    // CLI
    case 0x58:
      CLEAR_FLAG(flag_i);
      break;
    // CLV
    case 0xB8:
      CLEAR_FLAG(flag_v);
      break;
    // CMP
    case 0xC1:
    case 0xC5:
    case 0xC9:
    case 0xCD:
    case 0xD1:
    case 0xD5:
    case 0xD9:
    case 0xDD:
      temp = (reg_a - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // CPX
    case 0xE0:
    case 0xE4:
    case 0xEC:
      temp = (reg_x - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // CPY
    case 0xC0:
    case 0xC4:
    case 0xCC:
      temp = (reg_y - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // DEC
    case 0xC6:
    case 0xCE:
    case 0xD6:
    case 0xDE:
      temp = (m - 1) & 0xFF;
      WRITE_RAM(address, temp);
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // DEX
    case 0xCA:
      reg_x = (reg_x - 1) & 0xFF;
      FLAG(reg_x > 127, flag_n);
      FLAG(reg_x == 0, flag_z);
      break;
    // DEY
    case 0x88:
      reg_y = (reg_y - 1) & 0xFF;
      FLAG(reg_y > 127, flag_n);
      FLAG(reg_y == 0, flag_z);
      break;
    // EOR
    case 0x41:
    case 0x45:
    case 0x49:
    case 0x4D:
    case 0x51:
    case 0x55:
    case 0x59:
    case 0x5D:
      reg_a ^= m;
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);
      break;
    // INC
    case 0xE6:
    case 0xEE:
    case 0xF6:
    case 0xFE:
      temp = (m + 1) & 0xFF;
      WRITE_RAM(address, temp);
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // INX
    case 0xE8:
      reg_x = (reg_x + 1) & 0xFF;
      FLAG(reg_x > 127, flag_n);
      FLAG(reg_x == 0, flag_z);
      break;
    // INY
    case 0xC8:
      reg_y = (reg_y + 1) & 0xFF;
      FLAG(reg_x > 127, flag_n);
      FLAG(reg_x == 0, flag_z);
      break;
    // JMP
    case 0x4C:
    case 0x6C:
      reg_pc = address;
      return 1;
    // JSR
    case 0x20:
      WRITE_RAM(0x100 + reg_sp, (reg_pc + 2) / 256);
      reg_sp--;
      reg_sp &= 0xFF;
      WRITE_RAM(0x100 + reg_sp, (reg_pc + 2) & 0xFF);
      reg_sp--;
      reg_sp &= 0xFF;
      reg_pc = address;
      return 1;
    // LDA
    case 0xA1:
    case 0xA5:
    case 0xA9:
    case 0xAD:
    case 0xB1:
    case 0xB5:
    case 0xB9:
    case 0xBD:
      reg_a = m;
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);
      break;
    // LDX
    case 0xA2:
    case 0xA6:
    case 0xAE:
    case 0xB6:
    case 0xBE:
      reg_x = m;
      FLAG(reg_x == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // LDY
    case 0xA0:
    case 0xA4:
    case 0xAC:
    case 0xB4:
    case 0xBC:
      reg_y = m;
      FLAG(reg_y == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // LSR
    case 0x46:
    case 0x4A:
    case 0x4E:
    case 0x56:
    case 0x5E:
      if (mode == 4)
      {
        FLAG(READ_BIT(reg_a, 0), flag_c);
        reg_a >>= 1;
        CLEAR_BIT(reg_a, 7);
        FLAG(reg_a > 127, flag_n);
        FLAG(reg_a == 0, flag_z);
      }
        else
      {
        FLAG(READ_BIT(m, 0), flag_c);
        m >>= 1;
        CLEAR_BIT(m, 7);
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // NOP
    case 0xEA:
      break;
    // ORA
    case 0x01:
    case 0x05:
    case 0x09:
    case 0x0D:
    case 0x11:
    case 0x15:
    case 0x19:
    case 0x1D:
      reg_a |= m;
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);
      break;
    // PHA
    case 0x48:
      WRITE_RAM(0x100 + reg_sp, reg_a);
      reg_sp--;
      reg_sp &= 0xFF;
      break;
    // PHP
    case 0x08:
      WRITE_RAM(0x100 + reg_sp, reg_sr);
      reg_sp--;
      reg_sp &= 0xFF;
      break;
    // PLA
    case 0x68:
      reg_sp++;
      reg_sp &= 0xFF;
      reg_a = READ_RAM(0x100 + reg_sp);
      break;
    // PLP
    case 0x28:
      reg_sp++;
      reg_sp &= 0xFF;
      reg_sr = READ_RAM(0x100 + reg_sp);
      break;
    // ROL
    case 0x26:
    case 0x2A:
    case 0x2E:
    case 0x36:
    case 0x3E:
      if (mode == 4)
      {
        FLAG(READ_BIT(reg_a, 7), flag_c);
        reg_a <<= 1;
        SET_BIT(reg_a, READ_FLAG(flag_c));
        reg_a &= 0xFF;
        FLAG(reg_a > 127, flag_n);
        FLAG(reg_a == 0, flag_z);
      }
        else
      {
        FLAG(READ_BIT(m, 7), flag_c);
        m <<= 1;
        SET_BIT(m, READ_FLAG(flag_c));
        m &= 0xFF;
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // ROR
    case 0x66:
    case 0x6A:
    case 0x6E:
    case 0x76:
    case 0x7E:
      if (mode == 4)
      {
        temp = READ_BIT(reg_a, 0);
        reg_a >>= 1;
        SET_BIT(reg_a, READ_FLAG(flag_c) << 7);
        FLAG(temp, flag_c);
        FLAG(reg_a > 127, flag_n);
        FLAG(reg_a == 0, flag_z);
      }
        else
      {
        temp = READ_BIT(m, 0);
        m >>= 1;
        SET_BIT(m, READ_FLAG(flag_c) << 7);
        FLAG(temp, flag_c);
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // RTI
    case 0x40:
      reg_sp++;
      reg_sp &= 0xFF;
      reg_sr = READ_RAM(0x100 + reg_sp);
      reg_sp++;
      reg_sp &= 0xFF;
      pc_lo = READ_RAM(0x100 + reg_sp);
      reg_sp++;
      reg_sp &= 0xFF;
      pc_hi = READ_RAM(0x100 + reg_sp);
      reg_pc = (pc_lo + 256 * pc_hi);
      reg_pc++;
      return 1;
    // RTS
    case 0x60:
      reg_sp++;
      reg_sp &= 0xFF;
      pc_lo = READ_RAM(0x100 + reg_sp);
      reg_sp++;
      reg_sp &= 0xFF;
      pc_hi = READ_RAM(0x100 + reg_sp);
      reg_pc = (pc_lo + 256 * pc_hi);
      reg_pc++;
      return 1;
    // SBC
    case 0xE1:
    case 0xE5:
    case 0xE9:
    case 0xED:
    case 0xF1:
    case 0xF5:
    case 0xF9:
    case 0xFD:
      if (READ_FLAG(flag_d))
      {
        int bcd_a = (reg_a & 15) + 10 * (reg_a >> 4);
        int bcd_m = (m & 15) + 10 * (m >> 4);
        int result = bcd_a - bcd_m - (1 - READ_FLAG(flag_c));

        // clear carry if < 0
        FLAG(result >= 0, flag_c);
        result %= 100;

        reg_a = (result % 10) + ((result / 10) << 4);
      }
        else
      {
        reg_a -= m - (1 - READ_FLAG(flag_c));

        FLAG(reg_a >= 0, flag_c);
      }

      reg_a &= 0xFF;
      FLAG((temp_a ^ reg_a) & (m ^ reg_a) & 0x80, flag_v);
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);

      break;
    // SEC
    case 0x38:
      SET_FLAG(flag_c);
      break;
    // SED
    case 0xF8:
      SET_FLAG(flag_d);
      break;
    // SEI
    case 0x78:
      SET_FLAG(flag_i);
      break;
    // STA
    case 0x81:
    case 0x85:
    case 0x8D:
    case 0x91:
    case 0x95:
    case 0x99:
    case 0x9D:
      WRITE_RAM(address, reg_a);
      break;
    // STX
    case 0x86:
    case 0x8E:
    case 0x96:
      WRITE_RAM(address, reg_x);
      break;
    // STY
    case 0x84:
    case 0x8C:
    case 0x94:
      WRITE_RAM(address, reg_y);
      break;
    // TAX
    case 0xAA:
      reg_x = reg_a;
      FLAG(reg_x == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TAY
    case 0xA8:
      reg_y = reg_a;
      FLAG(reg_y == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TSX
    case 0xBA:
      reg_x = reg_sp;
      FLAG(reg_x == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TXA
    case 0x8A:
      reg_a = reg_x;
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);
      break;
    // TXS
    case 0x9A:
      reg_sp = reg_x;
      break;
    // TYA
    case 0x98:
      reg_a = reg_y;
      FLAG(reg_a > 127, flag_n);
      FLAG(reg_a == 0, flag_z);
      break;
  }

  return 0;
}

