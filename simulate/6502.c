/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Joe Davisson
 *
 * 6502 file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "asm/6502.h"
#include "disasm/6502.h"
#include "simulate/6502.h"
#include "table/6502.h"

extern struct _table_6502_opcodes table_6502_opcodes[];

#define SHOW_STACK 0x100 + sp, memory_read_m(simulate->memory, 0x100 + sp)
#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define WRITE_RAM(a, b) \
  if (a == simulate->break_io) \
  { \
    exit(b); \
  } \
  memory_write_m(simulate->memory, a, b)

#define REG_A simulate_6502->reg_a
#define REG_X simulate_6502->reg_x
#define REG_Y simulate_6502->reg_y
#define REG_SR simulate_6502->reg_sr
#define REG_PC simulate_6502->reg_pc
#define REG_SP simulate_6502->reg_sp
#define CYCLE_COUNT simulate->cycle_count

// status register flags
#define flag_c 0
#define flag_z 1
#define flag_i 2
#define flag_d 3
#define flag_b 4
#define flag_g 5
#define flag_v 6
#define flag_n 7

#define READ_BIT(dst, a) ((dst & (1 << a)) ? 1 : 0)
#define SET_BIT(dst, a) (dst |= (1 << a))
#define CLEAR_BIT(dst, a) (dst &= ~(1 << a))

#define READ_FLAG(a) ((REG_SR & (1 << a)) ? 1 : 0)
#define SET_FLAG(a) (REG_SR |= (1 << a))
#define CLEAR_FLAG(a) (REG_SR &= ~(1 << a))

#define FLAG(condition, flag) if (condition) SET_FLAG(flag); else CLEAR_FLAG(flag)

static int stop_running = 0;

// return calculated address for each mode
static int calc_address(struct _simulate *simulate, int address, int mode)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  int lo = READ_RAM(address);
  int hi = READ_RAM((address + 1) & 0xFFFF);
  int indirect;

  switch(mode)
  {
    case OP_NONE:
      return address;
    case OP_IMMEDIATE:
      return address;
    case OP_ADDRESS8:
      return lo & 0xFF;
    case OP_ADDRESS16:
      return lo | (hi << 8);
    case OP_INDEXED8_X:
      return (lo + REG_X) & 0xFF;
    case OP_INDEXED8_Y:
      return (lo + REG_Y) & 0xFFFF;
    case OP_INDEXED16_X:
      return ((lo | (hi << 8)) + REG_X) & 0xFFFF;
    case OP_INDEXED16_Y:
      return ((lo | (hi << 8)) + REG_Y) & 0xFFFF;
    case OP_INDIRECT16:
      indirect = (lo | (hi << 8)) & 0xFFFF;
      return (READ_RAM(indirect) | ((READ_RAM((indirect + 1) & 0xFFFF) << 8) & 0xFFFF));
    case OP_X_INDIRECT8:
      indirect = ((READ_RAM(lo + REG_X)) & 0xFF) | (READ_RAM((lo + 1 + REG_X) & 0xFF) << 8);
      return (indirect) & 0xFFFF;
    case OP_INDIRECT8_Y:
      indirect = READ_RAM(lo) | (READ_RAM((lo + 1) & 0xFF) << 8);
      return (indirect + REG_Y) & 0xFFFF;
    case OP_RELATIVE:
      return (address + ((signed char)READ_RAM(address) + 1)) & 0xFFFF;
    default:
      return -1;
  }
}

static int operand_exe(struct _simulate *simulate, int opcode)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  if (opcode < 0 || opcode > 0xFF)
    return -1;

  int mode = table_6502_opcodes[opcode].op;

  if (table_6502_opcodes[opcode].instr == M65XX_ERROR)
    return -1;

  int address = calc_address(simulate, REG_PC + 1, mode);
  if (address == -1)
    return -1;

  int m = READ_RAM(address);
  int temp;
  int pc_lo, pc_hi;
  int temp_a = REG_A;

  CYCLE_COUNT += table_6502_opcodes[opcode].cycles_min;
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
        int bcd_a = (REG_A & 15) + 10 * (REG_A >> 4);
        int bcd_m = (m & 15) + 10 * (m >> 4);
        int result = bcd_a + bcd_m + READ_FLAG(flag_c);

        FLAG(result > 99, flag_c);
        result %= 100;

        REG_A = (result % 10) + ((result / 10) << 4);
      }
      else
      {
        REG_A += m + READ_FLAG(flag_c);

        FLAG(REG_A > 255, flag_c);
      }

      REG_A &= 0xFF;
      FLAG((temp_a ^ REG_A) & (m ^ REG_A) & 0x80, flag_v);
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);

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
      REG_A &= m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // ASL
    case 0x06:
    case 0x0A:
    case 0x0E:
    case 0x16:
    case 0x1E:
      if (mode == OP_NONE)
      {
        FLAG(READ_BIT(REG_A, 7), flag_c);
        REG_A <<= 1;
        REG_A &= 0xFF;
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
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
        REG_PC = address;
        return 1;
      }
      break;
    // BCS
    case 0xB0:
      if (READ_FLAG(flag_c) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BEQ
    case 0xF0:
      if (READ_FLAG(flag_z) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BMI
    case 0x30:
      if (READ_FLAG(flag_n) == 1)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BNE
    case 0xD0:
      if (READ_FLAG(flag_z) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BPL
    case 0x10:
      if (READ_FLAG(flag_n) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BVC
    case 0x50:
      if (READ_FLAG(flag_v) == 0)
      {
        REG_PC = address;
        return 1;
      }
      break;
    // BVS
    case 0x70:
      if (READ_FLAG(flag_v) == 1)
      {
        REG_PC = address;
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
      FLAG((REG_A & m) == 0, flag_z);
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
      temp = (REG_A - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // CPX
    case 0xE0:
    case 0xE4:
    case 0xEC:
      temp = (REG_X - m);
      FLAG(temp >= 0, flag_c);
      temp &= 0xFF;
      FLAG(temp > 127, flag_n);
      FLAG(temp == 0, flag_z);
      break;
    // CPY
    case 0xC0:
    case 0xC4:
    case 0xCC:
      temp = (REG_Y - m);
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
      REG_X = (REG_X - 1) & 0xFF;
      FLAG(REG_X > 127, flag_n);
      FLAG(REG_X == 0, flag_z);
      break;
    // DEY
    case 0x88:
      REG_Y = (REG_Y - 1) & 0xFF;
      FLAG(REG_Y > 127, flag_n);
      FLAG(REG_Y == 0, flag_z);
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
      REG_A ^= m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
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
      REG_X = (REG_X + 1) & 0xFF;
      FLAG(REG_X > 127, flag_n);
      FLAG(REG_X == 0, flag_z);
      break;
    // INY
    case 0xC8:
      REG_Y = (REG_Y + 1) & 0xFF;
      FLAG(REG_X > 127, flag_n);
      FLAG(REG_X == 0, flag_z);
      break;
    // JMP
    case 0x4C:
    case 0x6C:
      REG_PC = address;
      return 1;
    // JSR
    case 0x20:
      WRITE_RAM(0x100 + REG_SP, (REG_PC + 2) / 256);
      REG_SP--;
      REG_SP &= 0xFF;
      WRITE_RAM(0x100 + REG_SP, (REG_PC + 2) & 0xFF);
      REG_SP--;
      REG_SP &= 0xFF;
      REG_PC = address;
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
      REG_A = m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // LDX
    case 0xA2:
    case 0xA6:
    case 0xAE:
    case 0xB6:
    case 0xBE:
      REG_X = m;
      FLAG(REG_X == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // LDY
    case 0xA0:
    case 0xA4:
    case 0xAC:
    case 0xB4:
    case 0xBC:
      REG_Y = m;
      FLAG(REG_Y == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // LSR
    case 0x46:
    case 0x4A:
    case 0x4E:
    case 0x56:
    case 0x5E:
      if (mode == OP_NONE)
      {
        FLAG(READ_BIT(REG_A, 0), flag_c);
        REG_A >>= 1;
        CLEAR_BIT(REG_A, 7);
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
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
      REG_A |= m;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // PHA
    case 0x48:
      WRITE_RAM(0x100 + REG_SP, REG_A);
      REG_SP--;
      REG_SP &= 0xFF;
      break;
    // PHP
    case 0x08:
      WRITE_RAM(0x100 + REG_SP, REG_SR);
      REG_SP--;
      REG_SP &= 0xFF;
      break;
    // PLA
    case 0x68:
      REG_SP++;
      REG_SP &= 0xFF;
      REG_A = READ_RAM(0x100 + REG_SP);
      break;
    // PLP
    case 0x28:
      REG_SP++;
      REG_SP &= 0xFF;
      REG_SR = READ_RAM(0x100 + REG_SP);
      break;
    // ROL
    case 0x26:
    case 0x2A:
    case 0x2E:
    case 0x36:
    case 0x3E:
      if (mode == OP_NONE)
      {
        temp = READ_FLAG(flag_c);
        FLAG(READ_BIT(REG_A, 7), flag_c);
        REG_A <<= 1;
        REG_A |= temp;
        REG_A &= 0xFF;
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
      }
      else
      {
        temp = READ_FLAG(flag_c);
        FLAG(READ_BIT(m, 7), flag_c);
        m <<= 1;
        m |= temp;
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
      if (mode == OP_NONE)
      {
        temp = READ_BIT(REG_A, 0);
        REG_A >>= 1;
        REG_A |= READ_FLAG(flag_c) << 7;
        FLAG(temp, flag_c);
        FLAG(REG_A > 127, flag_n);
        FLAG(REG_A == 0, flag_z);
      }
      else
      {
        temp = READ_BIT(m, 0);
        m >>= 1;
        m |= READ_FLAG(flag_c) << 7;
        FLAG(temp, flag_c);
        FLAG(m > 127, flag_n);
        FLAG(m == 0, flag_z);
        WRITE_RAM(address, m);
      }
      break;
    // RTI
    case 0x40:
      REG_SP++;
      REG_SP &= 0xFF;
      REG_SR = READ_RAM(0x100 + REG_SP);
      REG_SP++;
      REG_SP &= 0xFF;
      pc_lo = READ_RAM(0x100 + REG_SP);
      REG_SP++;
      REG_SP &= 0xFF;
      pc_hi = READ_RAM(0x100 + REG_SP);
      REG_PC = (pc_lo + 256 * pc_hi);
      REG_PC++;
      return 1;
    // RTS
    case 0x60:
      REG_SP++;
      REG_SP &= 0xFF;
      pc_lo = READ_RAM(0x100 + REG_SP);
      REG_SP++;
      REG_SP &= 0xFF;
      pc_hi = READ_RAM(0x100 + REG_SP);
      REG_PC = (pc_lo + 256 * pc_hi);
      REG_PC++;
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
        int bcd_a = (REG_A & 15) + 10 * (REG_A >> 4);
        int bcd_m = (m & 15) + 10 * (m >> 4);
        int result = bcd_a - bcd_m - (1 - READ_FLAG(flag_c));

        // clear carry if < 0
        FLAG(result >= 0, flag_c);
        result %= 100;

        REG_A = (result % 10) + ((result / 10) << 4);
      }
      else
      {
        REG_A -= m - (1 - READ_FLAG(flag_c));

        FLAG(REG_A >= 0, flag_c);
      }

      REG_A &= 0xFF;
      FLAG((temp_a ^ REG_A) & (m ^ REG_A) & 0x80, flag_v);
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);

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
      WRITE_RAM(address, REG_A);
      break;
    // STX
    case 0x86:
    case 0x8E:
    case 0x96:
      WRITE_RAM(address, REG_X);
      break;
    // STY
    case 0x84:
    case 0x8C:
    case 0x94:
      WRITE_RAM(address, REG_Y);
      break;
    // TAX
    case 0xAA:
      REG_X = REG_A;
      FLAG(REG_X == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TAY
    case 0xA8:
      REG_Y = REG_A;
      FLAG(REG_Y == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TSX
    case 0xBA:
      REG_X = REG_SP;
      FLAG(REG_X == 0, flag_z);
      FLAG(m > 127, flag_n);
      break;
    // TXA
    case 0x8A:
      REG_A = REG_X;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
    // TXS
    case 0x9A:
      REG_SP = REG_X;
      break;
    // TYA
    case 0x98:
      REG_A = REG_Y;
      FLAG(REG_A > 127, flag_n);
      FLAG(REG_A == 0, flag_z);
      break;
  }

  return 0;
}

static void handle_signal(int sig)
{
  stop_running=1;
  signal(SIGINT, SIG_DFL);
}

struct _simulate *simulate_init_6502(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_6502)+sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_6502;
  simulate->simulate_free = simulate_free_6502;
  simulate->simulate_dumpram = simulate_dumpram_6502;
  simulate->simulate_push = simulate_push_6502;
  simulate->simulate_set_reg = simulate_set_reg_6502;
  simulate->simulate_get_reg = simulate_get_reg_6502;
  simulate->simulate_set_pc = simulate_set_pc_6502;
  simulate->simulate_reset = simulate_reset_6502;
  simulate->simulate_dump_registers = simulate_dump_registers_6502;
  simulate->simulate_run = simulate_run_6502;

  simulate_reset_6502(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->show = 1; // Show simulation
  simulate->step_mode = 0;
  simulate->memory = memory;

  return simulate;
}

void simulate_push_6502(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  WRITE_RAM(0x100 + REG_SP, value & 0xFF);
  REG_SP--;
  REG_SP &= 0xFF;
}

int simulate_set_reg_6502(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  // a, x, y, sr, pc, sp

  while(*reg_string==' ') { reg_string++; }

  char *pos = reg_string;

  if (pos[0] == 'a' || pos[0] == 'A')
    REG_A = value & 0xFF;
  else if (pos[0] == 'x' || pos[0] == 'X')
    REG_X = value & 0xFF;
  else if (pos[0] == 'y' || pos[0] == 'Y')
    REG_Y = value & 0xFF;
  else if ((pos[0] == 's' || pos[0] == 'S') && (pos[1] == 'r' || pos[1] == 'R'))
    REG_SR = value & 0xFF;
  else if ((pos[0] == 'p' || pos[0] == 'P') && (pos[1] == 'c' || pos[1] == 'C'))
    REG_PC = value & 0xFFFF;
  else if ((pos[0] == 's' || pos[0] == 'S') && (pos[1] == 'p' || pos[1] == 'P'))
    REG_SP = value & 0xFF;
  else
    return -1;

  return 0;
}

uint32_t simulate_get_reg_6502(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  while(*reg_string == ' ') { reg_string++; }

  char *pos = reg_string;

  if (pos[0] == 'a' || pos[0] == 'A')
    return REG_A;
  if (pos[0] == 'x' || pos[0] == 'X')
    return REG_X;
  if (pos[0] == 'y' || pos[0] == 'Y')
    return REG_Y;
  if ((pos[0] == 's' || pos[0] == 'S') && (pos[1] == 'r' || pos[1] == 'R'))
    return REG_SR;
  if ((pos[0] == 'p' || pos[0] == 'P') && (pos[1] == 'c' || pos[1] == 'C'))
    return REG_PC;
  if ((pos[0] == 's' || pos[0] == 'S') && (pos[1] == 'p' || pos[1] == 'P'))
    return REG_SP;

  return -1;
}

void simulate_set_pc_6502(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  REG_PC = value;
}

void simulate_reset_6502(struct _simulate *simulate)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  REG_A = 0;
  REG_X = 0;
  REG_Y = 0;
  REG_SR = 0;
  REG_PC = 0;
  REG_SP = 0xFF;
  simulate->break_point = -1;
}

void simulate_free_6502(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

int simulate_dumpram_6502(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_6502(struct _simulate *simulate)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;

  int sp = REG_SP;

  printf("\nSimulation Register Dump                               Stack\n");
  printf("------------------------------------------------------------\n");
  printf("        7 6 5 4 3 2 1 0                          0x%03x: 0x%02x\n", SHOW_STACK);
  sp = (sp - 1) & 0xFF;

  printf("Status: N V - B D I Z C                          0x%03x: 0x%02x\n", SHOW_STACK);
  sp = (sp - 1) & 0xFF;
  printf("        %d %d %d %d %d %d %d %d                          0x%03x: 0x%02x\n",
    READ_FLAG(flag_n),
    READ_FLAG(flag_v),
    READ_FLAG(flag_g),
    READ_FLAG(flag_b),
    READ_FLAG(flag_d),
    READ_FLAG(flag_i),
    READ_FLAG(flag_z),
    READ_FLAG(flag_c),
    SHOW_STACK);
  sp = (sp - 1) & 0xFF;

  printf("                                                 0x%03x: 0x%02x\n", SHOW_STACK);
  sp = (sp - 1) & 0xFF;

  printf("  A=0x%02x   X=0x%02x   Y=0x%02x                       0x%03x: 0x%02x\n", REG_A, REG_X, REG_Y, SHOW_STACK);
  sp = (sp - 1) & 0xFF;
  printf(" SR=0x%02x  SP=0x%02x  PC=0x%04x                     0x%03x: 0x%02x\n", REG_SR, REG_SP, REG_PC, SHOW_STACK);

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_6502(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_6502 *simulate_6502 = (struct _simulate_6502 *)simulate->context;
  char instruction[128];
  char bytes[16];

  stop_running = 0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running==0)
  {
    int pc = REG_PC;
    int cycles_min, cycles_max;
    int opcode = READ_RAM(pc);

    int ret = operand_exe(simulate, opcode);

    // stop simulation on BRK instruction
    if (ret == -1)
      break;

    // only increment if REG_PC not touched
    if (ret == 0)
      REG_PC += disasm_6502(simulate->memory, pc, instruction, &cycles_min, &cycles_max);

    if (simulate->show == 1)
    {
      printf("\x1b[1J\x1b[1;1H");
      simulate_dump_registers_6502(simulate);

      int n = 0;
      while(n < 6)
      {
        int count = disasm_6502(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        int i;

        bytes[0] = 0;
        for (i = 0; i < count; i++)
        {
          char temp[4];
          sprintf(temp, "%02x ", READ_RAM(pc + i));
          strcat(bytes, temp);
        }

        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == REG_PC) { printf("> "); }
          else
        { printf("  "); }

        printf("0x%04x: %-10s %-40s %d-%d\n", pc, bytes, instruction, cycles_min, cycles_max);

        if (count == 0) { break; }

        n++;
        pc += count;

#if 0
        count--;
        while(count > 0)
        {
          if (pc == simulate->break_point) { printf("*"); }
          else { printf(" "); }
          printf("  0x%04x: 0x%04x\n", pc, READ_RAM(pc));
          pc += count;
          count--;
        }
#endif
      }
    }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (simulate->break_point == REG_PC)
    {
      printf("Breakpoint hit at 0x%04x\n", simulate->break_point);
      break;
    }

    if ((simulate->usec == 0) || (step == 1))
    {
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    if (REG_PC == 0xFFFF)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      REG_PC = READ_RAM(0xFFFC) + READ_RAM(0xFFFD) * 256;
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec > 999999 ? 999999 : simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", REG_PC);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}


