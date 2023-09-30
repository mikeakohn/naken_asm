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
#include <stdint.h>
#include <string.h>

#include "asm/z80.h"
#include "disasm/z80.h"
#include "simulate/z80.h"
#include "table/z80.h"

#define VFLAG_OVERFLOW 0
#define VFLAG_CLEAR 1
#define VFLAG_PARITY 2

#define READ_RAM(a) memory->read8(a)
#define READ_RAM16(a) (memory->read8(a + 1) << 8) | memory->read8(a)

#define READ_OPCODE16(a) (memory->read8(a) << 8) | memory->read8(a + 1)
#define WRITE_RAM(a,b) memory->write8(a, b)

#define GET_S() ((reg[REG_F] >> 7) & 1)
#define GET_Z() ((reg[REG_F] >> 6) & 1)
#define GET_X() ((reg[REG_F] >> 5) & 1)
#define GET_H() ((reg[REG_F] >> 4) & 1)
#define GET_Y() ((reg[REG_F] >> 3) & 1)
#define GET_V() ((reg[REG_F] >> 2) & 1)
#define GET_N() ((reg[REG_F] >> 1) & 1)
#define GET_C() ((reg[REG_F] >> 0) & 1)

#define SET_S() reg[REG_F] |= (1 << 7);
#define SET_Z() reg[REG_F] |= (1 << 6);
#define SET_X() reg[REG_F] |= (1 << 5);
#define SET_H() reg[REG_F] |= (1 << 4);
#define SET_Y() reg[REG_F] |= (1 << 3);
#define SET_V() reg[REG_F] |= (1 << 2);
#define SET_N() reg[REG_F] |= (1 << 1);
#define SET_C() reg[REG_F] |= (1 << 0);

#define CLR_S() reg[REG_F] &= 0xff ^ (1 << 7);
#define CLR_Z() reg[REG_F] &= 0xff ^ (1 << 6);
#define CLR_X() reg[REG_F] &= 0xff ^ (1 << 5);
#define CLR_H() reg[REG_F] &= 0xff ^ (1 << 4);
#define CLR_Y() reg[REG_F] &= 0xff ^ (1 << 3);
#define CLR_V() reg[REG_F] &= 0xff ^ (1 << 2);
#define CLR_N() reg[REG_F] &= 0xff ^ (1 << 1);
#define CLR_C() reg[REG_F] &= 0xff ^ (1 << 0);

SimulateZ80::SimulateZ80(Memory *memory) : Simulate(memory)
{
}

SimulateZ80::~SimulateZ80()
{
  reset();
}

Simulate *SimulateZ80::init(Memory *memory)
{
  return new SimulateZ80(memory);
}

void SimulateZ80::reset()
{
  memset(reg, 0, sizeof(reg));
  ix = 0;
  iy = 0;
  sp = 0;
  pc = org;
  //status = 0;
  iff1 = 0;
  iff2 = 0;
}

void SimulateZ80::push(uint32_t value)
{
  reg[1] -= 2;
  WRITE_RAM(reg[1], value & 0xff);
  WRITE_RAM(reg[1] + 1, value >> 8);
}

int SimulateZ80::set_reg(const char *reg_string, uint32_t value)
{
  while (*reg_string == ' ') { reg_string++; }
  return -1;
}

uint32_t SimulateZ80::get_reg(const char *reg_string)
{
  return 0;
}

void SimulateZ80::set_pc(uint32_t value)
{
  pc = value;
}

void SimulateZ80::dump_registers()
{
  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("Status: %02x   S Z X H Y V N C\n", reg[REG_F]);
  printf("             %d %d %d %d %d %d %d %d\n",
    GET_S(), GET_Z(), GET_X(), GET_H(), GET_Y(), GET_V(), GET_N(), GET_C());

  printf(" A: %02x F: %02x     B: %02x C: %02X    "
         " D: %02x E: %02x     H: %02x L: %02X\n",
         reg[REG_A],
         reg[REG_F],
         reg[REG_B],
         reg[REG_C],
         reg[REG_D],
         reg[REG_E],
         reg[REG_H],
         reg[REG_L]);
  printf("IX: %04x        IY: %04x        SP: %04x        PC: %04x\n",
         ix, iy,
         sp, pc);

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int SimulateZ80::run(int max_cycles, int step)
{
  char instruction[128];
  //uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc_current;
  //int c;
  int n;

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    int cycles_min, cycles_max;
    pc_current = pc;

    if (show == 1) { printf("\x1b[1J\x1b[1;1H"); }

    int count = disasm_z80(
      memory,
      pc_current,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    // Insert code execution
    // FIXME
    ret = execute();

    pc += count;

    if (show == true)
    {
      dump_registers();
      int disasm_pc = pc_current;

      n = 0;
      while (n < 6)
      {
        int count = disasm_z80(
          memory,
          disasm_pc,
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        if (cycles_min == -1) break;

        if (disasm_pc == break_point)
        {
          printf("*");
        }
          else
        {
          printf(" ");
        }

        if (n == 0)
        {
          printf("! ");
        }
          else
        if (disasm_pc == reg[0])
        {
          printf("> ");
        }
          else
        {
          printf("  ");
        }

        char hex[32];

        if (count == 1)
        {
          snprintf(hex, sizeof(hex), "         %02x", READ_RAM(disasm_pc));
        }
          else
        if (count == 2)
        {
          snprintf(hex, sizeof(hex), "      %02x %02x",
            READ_RAM(disasm_pc),
            READ_RAM(disasm_pc + 1));
        }
          else
        if (count == 3)
        {
          snprintf(hex, sizeof(hex), "   %02x %02x %02x",
            READ_RAM(disasm_pc),
            READ_RAM(disasm_pc + 1),
            READ_RAM(disasm_pc + 2));
        }
          else
        if (count == 4)
        {
          snprintf(hex, sizeof(hex), "%02x %02x %02x %02x",
            READ_RAM(disasm_pc),
            READ_RAM(disasm_pc + 1),
            READ_RAM(disasm_pc + 2),
            READ_RAM(disasm_pc + 3));
        }
          else
        {
          snprintf(hex, sizeof(hex), "         ???");
        }

        if (cycles_min < 1)
        {
          printf("0x%04x: %s %-40s ?\n", disasm_pc, hex, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: %s %-40s %d\n", disasm_pc, hex, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: %s %-40s %d-%d\n", disasm_pc, hex, instruction, cycles_min, cycles_max);
        }

        n++;
        disasm_pc += count;
      }
    }

    if (auto_run == true && nested_call_count < 0) { return 0; }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc_current);
      return -1;
    }

    cycle_count += ret;
    if (max_cycles != -1 && cycles > max_cycles) break;
    if (break_point == reg[0])
    {
       printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == true)
    {
      //step_mode = 0;
      disable_signal_handler();
      return 0;
    }

    if (pc == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      pc = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      disable_signal_handler();
      return 0;
    }

    usleep(usec);
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", pc);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

int SimulateZ80::get_q(int reg16)
{
  int value = 0;

  if (reg16 == 0)
  {
    value = (reg[REG_B] << 8) | reg[REG_C];
  }
    else
  if (reg16 == 1)
  {
    value = (reg[REG_D] << 8) | reg[REG_E];
  }
    else
  if (reg16 == 2)
  {
    value = (reg[REG_H] << 8) | reg[REG_L];
  }
    else
  if (reg16 == 3)
  {
    value = sp;
  }

  return value;
}

int SimulateZ80::get_p(int reg16)
{
  int value = 0;

  if (reg16 == 0)
  {
    value = (reg[REG_B] << 8) | reg[REG_C];
  }
    else
  if (reg16 == 1)
  {
    value = (reg[REG_D] << 8) | reg[REG_E];
  }
    else
  if (reg16 == 2)
  {
    value = (reg[REG_H] << 8) | reg[REG_L];
  }
    else
  if (reg16 == 3)
  {
    value = (reg[REG_A] << 8) | reg[REG_F];
  }

  return value;
}

int SimulateZ80::set_p(int reg16, int value)
{
  if (reg16 == 0)
  {
    reg[REG_B] = value >> 8;
    reg[REG_C] = value & 0xff;
  }
    else
  if (reg16 == 1)
  {
    reg[REG_D] = value >> 8;
    reg[REG_E] = value & 0xff;
  }
    else
  if (reg16 == 2)
  {
    reg[REG_H] = value >> 8;
    reg[REG_L] = value & 0xff;
  }
    else
  if (reg16 == 3)
  {
    reg[REG_A] = value >> 8;
    reg[REG_F] = value & 0xff;
  }

  return value;
}

void SimulateZ80::set_q(int reg16, int value)
{
  if (reg16 == 3)
  {
    sp = value;
  }
  else
  {
    set_p(reg16, value);
  }
}

void SimulateZ80::set_xy(int xy, int value)
{
  if (xy == 0)
  {
    ix = value;
  }
    else
  {
    iy = value;
  }
}

uint16_t SimulateZ80::get_xy(int xy)
{
  return xy == 0 ? ix : iy;
}

void SimulateZ80::set_parity(uint8_t a)
{
  int parity;

  // Hmm is 4 bit look-up table better?
  parity = ((a & 0x01) >> 0) ^
           ((a & 0x02) >> 1) ^
           ((a & 0x04) >> 2) ^
           ((a & 0x08) >> 3) ^
           ((a & 0x10) >> 4) ^
           ((a & 0x20) >> 5) ^
           ((a & 0x40) >> 6) ^
           ((a & 0x80) >> 7);
  if (parity) { SET_V(); } else { CLR_V(); }
}

void SimulateZ80::set_flags_a(int a, int number, uint8_t vflag)
{
  if (vflag == VFLAG_OVERFLOW)
  {
    int a0 = reg[REG_A];
    int v = (((a0 >> 7) & 1) & ((number >> 7) & 1) & (((a >> 7) & 1) ^ 1)) |
           ((((a0 >> 7) & 1) ^1) & (((number >> 7) & 1) ^ 1) & ((a >> 7) & 1));
    if (v) { SET_V(); } else { CLR_V(); }
  }

  reg[REG_A] = (a & 0xff);
  if (reg[REG_A] == 0) { SET_Z(); } else { CLR_Z(); }
  if ((reg[REG_A] & 0x80) != 0) { SET_S(); } else { CLR_S(); }
  if ((reg[REG_A] & 0x10) != 0) { SET_H(); } else { CLR_H(); }
  if (a & 0x0100) { SET_C(); } else { CLR_C(); }
  CLR_C();

  if (vflag == VFLAG_CLEAR)
  {
    CLR_V();
  }
    else
  if (vflag == VFLAG_PARITY)
  {
    set_parity(reg[REG_A]);
  }
}

void SimulateZ80::set_flags16(int _new, int old, int number, uint8_t vflag)
{
  if (vflag == VFLAG_OVERFLOW)
  {
    int a0 = old;
    int a = _new;
    int v = (((a0 >> 15) & 1) & ((number >> 15) & 1) & (((a >> 15) & 1) ^ 1)) |
           ((((a0 >> 15) & 1) ^1) & (((number >> 15) & 1) ^ 1) & ((a >> 15) & 1));
    if (v) { SET_V(); } else { CLR_V(); }
  }

  if ((_new & 0xffff) == 0) { SET_Z(); } else { CLR_Z(); }
  if ((_new & 0x8000) != 0) { SET_S(); } else { CLR_S(); }
  //if ((_new & 0x10) != 0) { SET_H(); } else { CLR_H(); }
  if (_new & 0x10000) { SET_C(); } else { CLR_C(); }
  CLR_C();

  if (vflag == VFLAG_CLEAR)
  {
    CLR_V();
  }
    else
  if (vflag == VFLAG_PARITY)
  {
  }
}

void SimulateZ80::add_reg16(int xy, int reg16)
{
  int number;
  int _new, old;

  number = get_q(reg16);

  if (xy == 0)
  {
    old = ix;
    _new = old + number;
    ix = _new & 0xffff;
  }
    else
  {
    old = iy;
    _new = old + number;
    iy = _new & 0xffff;
  }

  CLR_N();

  set_flags16(_new, old, number, VFLAG_OVERFLOW);
}

int SimulateZ80::execute_op_none(struct _table_z80 *table_z80, uint16_t opcode)
{
  int tmp;
  int a4,tmp4;

  switch (table_z80->instr_enum)
  {
    case  Z80_CCF:
      if (GET_C() == 1) { SET_H(); } else { CLR_H(); }
      if (GET_C() == 0) { SET_C(); } else { CLR_C(); }
      return table_z80->cycles_min;
    case  Z80_CPL:
      reg[REG_A] = ~reg[REG_A];
      return table_z80->cycles_min;
    case  Z80_DAA:
      // What the fuck?
      tmp = reg[REG_A];
      if (GET_N() == 1)
      {
        if (GET_H() == 1 || ((reg[REG_A] & 0x0f) > 9)) { tmp -= 0x06; }
        if (GET_C() == 1 || (reg[REG_A] > 0x99)) { tmp -= 0x60; }
      }
        else
      {
        if (GET_H() == 1 || ((reg[REG_A] & 0x0f) > 9)) { tmp += 0x06; }
        if (GET_C() == 1 || (reg[REG_A] > 0x99)) { tmp += 0x60; }
      }
      if (GET_C() == 1 || reg[REG_A] > 0x099) { SET_C(); }
      a4 = reg[REG_A] & 0x08;
      tmp4 = tmp & 0x08;
      a4 = (a4 == 0) ? 0 : 1;
      tmp4 = (tmp4 == 0) ? 0 : 1;
      if ((a4 ^ tmp4) == 0) { CLR_H(); } else { SET_H(); }
      reg[REG_A] = tmp;
      return table_z80->cycles_min;
    case  Z80_DI:
      iff1 = 0;
      iff2 = 0;
      return table_z80->cycles_min;
    case  Z80_EI:
      iff1 = 1;
      iff2 = 1;
      return table_z80->cycles_min;
    case  Z80_EXX:
      return -1;
    case  Z80_HALT:
      return -1;
    case  Z80_NOP:
      return 1;
    case  Z80_RET:
      return -1;
    case  Z80_RLA:
      tmp = reg[REG_A] & 0x80;
      reg[REG_A] <<= 1;
      reg[REG_A] |= GET_C();
      if (tmp == 0) { CLR_C(); } else { SET_C(); }
      return table_z80->cycles_min;
    case  Z80_RLCA:
      if ((reg[REG_A] & 0x80) == 0) { CLR_C(); } else { SET_C(); }
      reg[REG_A] <<= 1;
      reg[REG_A] |= GET_C();
      return table_z80->cycles_min;
    case  Z80_RRA:
      tmp = reg[REG_A] & 1;
      reg[REG_A] >>= 1;
      reg[REG_A] |= GET_C() << 7;
      if (tmp == 0) { CLR_C(); } else { SET_C(); }
      return table_z80->cycles_min;
    case  Z80_RRCA:
      if ((reg[REG_A] & 0x01) == 0) { CLR_C(); } else { SET_C(); }
      reg[REG_A] >>= 1;
      reg[REG_A] |= GET_C() << 7;
      return table_z80->cycles_min;
    case  Z80_SCF:
      SET_C();
      return table_z80->cycles_min;
  }

  return -1;
}

int SimulateZ80::execute_op_a_reg8(struct _table_z80 *table_z80, uint8_t opcode)
{
  int a = reg[REG_A];
  int rrr = opcode & 0x7;
  int number = reg[rrr];

  switch (table_z80->instr_enum)
  {
    case Z80_ADC:
      a += number + GET_C();
      CLR_N();
      break;
    case Z80_ADD:
      a += number;
      CLR_N();
      break;
    case Z80_SBC:
      a -= number + GET_C();
      SET_N();
      break;
  }

  set_flags_a(a, number, VFLAG_OVERFLOW);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_reg8(struct _table_z80 *table_z80, uint8_t opcode)
{
  int a = reg[REG_A];
  int rrr = opcode & 0x7;
  int number = reg[rrr];
  int vflag = VFLAG_CLEAR;
  int tmp;

  switch (table_z80->instr_enum)
  {
    case Z80_AND:
      a &= number;
      CLR_N();
      break;
    case Z80_CP:
      tmp = a - number;
      if (number & 0x20) { SET_X(); } else { CLR_X(); }
      if (number & 0x08) { SET_Y(); } else { CLR_Y(); }
      SET_N();
      if (tmp & 0x80) { SET_S(); } else { CLR_S(); }
      if (tmp & 0xff) { CLR_Z(); } else { SET_Z() }
      if (tmp & 0x10) { SET_H(); } else { CLR_H(); }
      if (tmp & 0x100) { SET_C(); } else { CLR_C(); }
      set_parity(tmp);
      return table_z80->cycles_min;
    case Z80_OR:
      a &= number;
      CLR_N();
      break;
    case Z80_SUB:
      a -= number;
      vflag = VFLAG_OVERFLOW;
      SET_N();
      break;
    case Z80_XOR:
      a ^= number;
      SET_N();
      break;
  }

  set_flags_a(a, number, vflag);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_a_number8(
  struct _table_z80 *table_z80,
  uint16_t opcode16)
{
  int a = reg[REG_A];
  int number = opcode16 & 0xff;

  switch (table_z80->instr_enum)
  {
    case Z80_ADC:
      a += number + GET_C();
      CLR_N();
      break;
    case Z80_ADD:
      a += number;
      CLR_N();
      break;
    case Z80_SBC:
      a = reg[REG_A] - (number + GET_C());
      SET_N();
      break;
  }

  set_flags_a(a, number, VFLAG_OVERFLOW);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_number8(
  struct _table_z80 *table_z80,
  uint16_t opcode16)
{
  int a = reg[REG_A];
  int number = opcode16 & 0xff;
  int vflag = VFLAG_CLEAR;
  int tmp;

  switch (table_z80->instr_enum)
  {
    case Z80_AND:
      a &= number;
      CLR_N();
      break;
    case Z80_CP:
      tmp = a - number;
      SET_N();
      if (tmp & 0x80) { SET_S(); } else { CLR_S(); }
      if (tmp & 0xff) { CLR_Z(); } else { SET_Z() }
      if (tmp & 0x10) { SET_H(); } else { CLR_H(); }
      if (tmp & 0x100) { SET_C(); } else { CLR_C(); }
      set_parity(tmp);
      return table_z80->cycles_min;
    case Z80_DJNZ:
      reg[REG_B]--;
      if (reg[REG_B] == 0)
      {
        pc += (int8_t)number;
      }
      return table_z80->cycles_min;
    case Z80_JR:
      pc += (int8_t)number;
      return table_z80->cycles_min;
    case Z80_OR:
      a |= number;
      CLR_N();
      break;
    case Z80_SUB:
      SET_N();
      vflag = VFLAG_OVERFLOW;
      break;
    case Z80_XOR:
      a ^= number;
      CLR_N();
      break;
  }

  set_flags_a(a, number, vflag);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_reg8_v2(
  struct _table_z80 *table_z80,
  uint8_t opcode)
{
  int a = reg[REG_A];
  int rrr = (opcode >> 3) & 0x7;
  int number = 1;
  int vflag = VFLAG_OVERFLOW;

  switch (table_z80->instr_enum)
  {
    case Z80_DEC:
      reg[rrr]--;
      SET_N();
      break;
    case Z80_INC:
      reg[rrr]++;
      CLR_N();
      break;
  }

  set_flags_a(a, number, vflag);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_reg16(struct _table_z80 *table_z80, uint8_t opcode)
{
  uint16_t _new;
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int number = 1;
  int vflag = VFLAG_OVERFLOW;

  old = get_q(reg16);
  _new = old;

  switch (table_z80->instr_enum)
  {
    case Z80_DEC:
      _new--;
      SET_N();
      number = -1;
      break;
    case Z80_INC:
      _new++;
      CLR_N();
      number = 1;
      break;
  }

  set_q(reg16, _new);
  set_flags16(_new, old, number, vflag);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_reg16p(struct _table_z80 *table_z80, uint8_t opcode)
{
  int reg16 = (opcode >> 4) & 0x3;
  int value;

  if (table_z80->instr_enum == Z80_PUSH)
  {
    value = get_p(reg16);

    sp -= 2;
    WRITE_RAM(sp, value >> 8);
    WRITE_RAM(sp + 1, value & 0xff);
    return table_z80->cycles_min;
  }
    else
  if (table_z80->instr_enum == Z80_POP)
  {
    value = READ_RAM(sp);
    value |= READ_RAM(sp + 1) << 8;
    sp += 2;

    set_p(reg16, value);
    return table_z80->cycles_min;
  }

  return -1;
}

int SimulateZ80::execute_op_hl_reg16_2(
  struct _table_z80 *table_z80,
  uint8_t opcode)
{
  uint16_t _new;
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int number;
  int value;

  number = get_p(reg16);
  old = get_p(2);
  _new = old;

  if (table_z80->instr_enum == Z80_ADC)
  {
    value = old + (number + GET_C());
    SET_N();
  }
    else
  if (table_z80->instr_enum == Z80_SBC)
  {
    value = old - (number + GET_C());
    SET_N();
  }
    else
  {
    return -1;
  }

  set_p(2, value);

  set_flags16(_new, old, number, VFLAG_OVERFLOW);

  return table_z80->cycles_min;
}

int SimulateZ80::execute_op_xy(struct _table_z80 *table_z80, uint16_t opcode16)
{
  int xy = (opcode16 >> 13) & 0x1;
  int value;

  if (table_z80->instr_enum == Z80_PUSH)
  {
    value = (xy == 0) ? ix : iy;

    sp -= 2;
    WRITE_RAM(sp, value >> 8);
    WRITE_RAM(sp + 1, value & 0xff);
    return table_z80->cycles_min;
  }
    else
  if (table_z80->instr_enum == Z80_POP)
  {
    value = READ_RAM(sp);
    value |= READ_RAM(sp + 1) << 8;
    sp += 2;

    if (xy == 0)
    {
      ix = value;
    }
      else
    {
      iy = value;
    }

    return table_z80->cycles_min;
  }

  return -1;
}

int SimulateZ80::execute()
{
  int index, n;
  int reg16, xy;
  int offset;
  int address;

  uint16_t opcode = READ_RAM(pc);
  uint16_t opcode16 = READ_OPCODE16(pc);

  n = 0;
  while (table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].opcode == (opcode & table_z80[n].mask))
    {
      if (table_z80[n].mask > 0xff) { n++; continue; }
      switch (table_z80[n].type)
      {
        case OP_NONE:
          return execute_op_none(&table_z80[n], opcode);
        case OP_A_REG8:
          return execute_op_a_reg8(&table_z80[n], opcode);
        case OP_REG8:
          return execute_op_reg8(&table_z80[n], opcode);
        case OP_A_NUMBER8:
          return execute_op_a_number8(&table_z80[n], opcode16);
        case OP_HL_REG16_1:
          return -1;
        case OP_A_INDEX_HL:
          return -1;
        case OP_INDEX_HL:
          return -1;
        case OP_NUMBER8:
          return execute_op_number8(&table_z80[n], opcode16);
        case OP_ADDRESS:
          return -1;
        case OP_COND_ADDRESS:
          return -1;
        case OP_REG8_V2:
          return execute_op_reg8_v2(&table_z80[n], opcode);
        case OP_REG16:
          return execute_op_reg16(&table_z80[n], opcode);
        case OP_INDEX_SP_HL:
          return -1;
        case OP_AF_AF_TICK:
          return -1;
        case OP_DE_HL:
          return -1;
        case OP_A_INDEX_N:
          return -1;
        case OP_JR_COND_ADDRESS:
          return -1;
        case OP_REG8_REG8:
          index = (opcode >> 3) & 0x7;
          reg[index] = reg[opcode & 0x7];
          return table_z80->cycles_min;
        case OP_REG8_NUMBER8:
          index = (opcode16 >> 11) & 0x7;
          reg[index] = opcode16 & 0xff;
          return table_z80->cycles_min;
        case OP_REG8_INDEX_HL:
          return -1;
        case OP_INDEX_HL_REG8:
        case OP_INDEX_HL_NUMBER8:
        case OP_A_INDEX_BC:
        case OP_A_INDEX_DE:
        case OP_A_INDEX_ADDRESS:
        case OP_INDEX_BC_A:
        case OP_INDEX_DE_A:
        case OP_INDEX_ADDRESS_A:
          return -1;
        case OP_REG16_ADDRESS:
          reg16 = (opcode >> 4) & 0x3;
          set_q(reg16, READ_RAM16(pc + 1));
          return table_z80->cycles_min;
        case OP_HL_INDEX_ADDRESS:
        case OP_INDEX_ADDRESS_HL:
        case OP_SP_HL:
        case OP_INDEX_ADDRESS8_A:
          return -1;
        case OP_REG16P:
          return execute_op_reg16p(&table_z80[n], opcode);
          return 1;
        case OP_COND:
          return -1;
        case OP_RESTART_ADDRESS:
        default:
          return -1;
      }
    }

    n++;
  }

  n = 0;
  while (table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].mask <= 0xff) { n++; continue; }
    if (table_z80[n].opcode == (opcode16 & table_z80[n].mask))
    {
      switch (table_z80[n].type)
      {
        case OP_NONE16:
        case OP_NONE24:
        case OP_A_REG_IHALF:
        case OP_A_INDEX:
          return -1;
        case OP_HL_REG16_2:
          return execute_op_hl_reg16_2(&table_z80[n], opcode16);
        case OP_XY_REG16:
          xy = (opcode16 >> 13) & 0x1;
          reg16 = (opcode16 >> 4) & 0x3;
          add_reg16(xy, reg16);
          return table_z80->cycles_min;
        case OP_REG_IHALF:
        case OP_INDEX:
        case OP_BIT_REG8:
        case OP_BIT_INDEX_HL:
        case OP_BIT_INDEX:
        case OP_REG_IHALF_V2:
        case OP_XY:
          return execute_op_xy(&table_z80[n], opcode16);
        case OP_INDEX_SP_XY:
        case OP_IM_NUM:
        case OP_REG8_INDEX_C:
        case OP_F_INDEX_C:
        case OP_INDEX_XY:
        case OP_REG8_REG_IHALF:
        case OP_REG_IHALF_REG8:
        case OP_REG_IHALF_REG_IHALF:
          return -1;
        case OP_REG8_INDEX:
          xy = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(pc + 2);
          address = xy + offset;
          index = (opcode16 >> 3) & 0x7;
          reg[index] = READ_RAM(address);
          return table_z80->cycles_min;
        case OP_INDEX_REG8:
          xy = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(pc + 2);
          address = xy + offset;
          index = opcode16 & 0x7;
          WRITE_RAM(address, reg[index]);
          return table_z80->cycles_min;
        case OP_INDEX_NUMBER8:
          xy = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(pc + 2);
          address = xy + offset;
          WRITE_RAM(address, READ_RAM(pc + 3));
          return table_z80->cycles_min;
        case OP_IR_A:
          return -1;
        case OP_A_IR:
          return -1;
        case OP_XY_ADDRESS:
          xy = (opcode16 >> 13) & 0x1;
          set_xy(xy, READ_RAM16(pc + 2));
          return table_z80->cycles_min;
        case OP_REG16_INDEX_ADDRESS:
        case OP_XY_INDEX_ADDRESS:
        case OP_INDEX_ADDRESS_REG16:
        case OP_INDEX_ADDRESS_XY:
          return -1;
        case OP_SP_XY:
          xy = (opcode16 >> 13) & 0x1;
          sp = get_xy(xy);
          return table_z80->cycles_min;
        case OP_INDEX_C_REG8:
        case OP_INDEX_C_ZERO:
        case OP_REG8_CB:
        case OP_INDEX_HL_CB:
          case OP_BIT_INDEX_V2:
          case OP_BIT_INDEX_REG8:
        default:
          return -1;
      }
    }

    n++;
  }

  return -1;
}

