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
#include <signal.h>

#include "asm/z80.h"
#include "disasm/z80.h"
#include "simulate/z80.h"
#include "table/z80.h"

#define VFLAG_OVERFLOW 0
#define VFLAG_CLEAR 1
#define VFLAG_PARITY 2

#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define READ_RAM16(a) (memory_read_m(simulate->memory, a + 1) << 8) | \
                       memory_read_m(simulate->memory, a)
#define READ_OPCODE16(a) (memory_read_m(simulate->memory, a) << 8) | \
                          memory_read_m(simulate->memory, a + 1)
#define WRITE_RAM(a,b) memory_write_m(simulate->memory, a, b)

#define GET_S() ((simulate_z80->reg[REG_F] >> 7) & 1)
#define GET_Z() ((simulate_z80->reg[REG_F] >> 6) & 1)
#define GET_X() ((simulate_z80->reg[REG_F] >> 5) & 1)
#define GET_H() ((simulate_z80->reg[REG_F] >> 4) & 1)
#define GET_Y() ((simulate_z80->reg[REG_F] >> 3) & 1)
#define GET_V() ((simulate_z80->reg[REG_F] >> 2) & 1)
#define GET_N() ((simulate_z80->reg[REG_F] >> 1) & 1)
#define GET_C() ((simulate_z80->reg[REG_F] >> 0) & 1)

#define SET_S() simulate_z80->reg[REG_F] |= (1 << 7);
#define SET_Z() simulate_z80->reg[REG_F] |= (1 << 6);
#define SET_X() simulate_z80->reg[REG_F] |= (1 << 5);
#define SET_H() simulate_z80->reg[REG_F] |= (1 << 4);
#define SET_Y() simulate_z80->reg[REG_F] |= (1 << 3);
#define SET_V() simulate_z80->reg[REG_F] |= (1 << 2);
#define SET_N() simulate_z80->reg[REG_F] |= (1 << 1);
#define SET_C() simulate_z80->reg[REG_F] |= (1 << 0);

#define CLR_S() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 7);
#define CLR_Z() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 6);
#define CLR_X() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 5);
#define CLR_H() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 4);
#define CLR_Y() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 3);
#define CLR_V() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 2);
#define CLR_N() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 1);
#define CLR_C() simulate_z80->reg[REG_F] &= 0xff ^ (1 << 0);

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static int get_q(struct _simulate *simulate, int reg16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int value = 0;

  if (reg16 == 0)
  {
    value = (simulate_z80->reg[REG_B] << 8) | simulate_z80->reg[REG_C];
  }
    else
  if (reg16 == 1)
  {
    value = (simulate_z80->reg[REG_D] << 8) | simulate_z80->reg[REG_E];
  }
    else
  if (reg16 == 2)
  {
    value = (simulate_z80->reg[REG_H] << 8) | simulate_z80->reg[REG_L];
  }
    else
  if (reg16 == 3)
  {
    value = simulate_z80->sp;
  }

  return value;
}

static int get_p(struct _simulate *simulate, int reg16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int value = 0;

  if (reg16 == 0)
  {
    value = (simulate_z80->reg[REG_B] << 8) | simulate_z80->reg[REG_C];
  }
    else
  if (reg16 == 1)
  {
    value = (simulate_z80->reg[REG_D] << 8) | simulate_z80->reg[REG_E];
  }
    else
  if (reg16 == 2)
  {
    value = (simulate_z80->reg[REG_H] << 8) | simulate_z80->reg[REG_L];
  }
    else
  if (reg16 == 3)
  {
    value = (simulate_z80->reg[REG_A] << 8) | simulate_z80->reg[REG_F];
  }

  return value;
}

static int set_p(struct _simulate *simulate, int reg16, int value)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  
  if (reg16 == 0)
  {
    simulate_z80->reg[REG_B] = value >> 8;
    simulate_z80->reg[REG_C] = value & 0xff;
  }
    else
  if (reg16 == 1)
  {
    simulate_z80->reg[REG_D] = value >> 8;
    simulate_z80->reg[REG_E] = value & 0xff;
  }
    else
  if (reg16 == 2)
  {
    simulate_z80->reg[REG_H] = value >> 8;
    simulate_z80->reg[REG_L] = value & 0xff;
  }
    else
  if (reg16 == 3)
  {
    simulate_z80->reg[REG_A] = value >> 8;
    simulate_z80->reg[REG_F] = value & 0xff;
  }

  return value;
}

static void set_q(struct _simulate *simulate, int reg16, int value)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  if (reg16 == 3)
  {
    simulate_z80->sp = value;
  }
  else
  {
    set_p(simulate, reg16, value);
  }
}

static void set_xy(struct _simulate *simulate, int xy, int value)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  if (xy == 0) { simulate_z80->ix = value; }
  else { simulate_z80->iy = value; }
}

static uint16_t get_xy(struct _simulate *simulate, int xy)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  if (xy == 0) { return simulate_z80->ix; }
  else { return simulate_z80->iy; }
}

#if 0
static int get_reg16_half(struct _simulate *simulate, int reg16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
int value;

  if (reg16 == 0) { value = simulate_z80->ix >> 8; }
  else if (reg16 == 1) { value = simulate_z80->ix & 0xff; }
  else if (reg16 == 2) { value = simulate_z80->iy >> 8; }
  else if (reg16 == 3) { value = simulate_z80->iy & 0xff; }

  return value;
}

static void set_reg16_half(struct _simulate *simulate, int reg16, int value)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  value = value & 0xff;

  if (reg16 == 0)
  {
    simulate_z80->ix = (simulate_z80->ix & 0x00ff) | (value << 8);
  }
  else if (reg16 == 1)
  {
    simulate_z80->ix = (simulate_z80->ix & 0xff00) | value;
  }
  else if (reg16 == 2)
  {
    simulate_z80->iy = (simulate_z80->iy & 0x00ff) | (value << 8);
  }
  else if (reg16 == 3)
  {
    simulate_z80->iy = (simulate_z80->iy & 0xff00) | value;
  }
}
#endif

static void set_parity(struct _simulate *simulate, uint8_t a)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
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

static void set_flags_a(struct _simulate *simulate, int a, int number, uint8_t vflag)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  if (vflag == VFLAG_OVERFLOW)
  {
    int a0 = simulate_z80->reg[REG_A];
    int v = (((a0 >> 7) & 1) & ((number >> 7) & 1) & (((a >> 7) & 1) ^ 1)) |
           ((((a0 >> 7) & 1) ^1) & (((number >> 7) & 1) ^ 1) & ((a >> 7) & 1));
    if (v) { SET_V(); } else { CLR_V(); }
  }

  simulate_z80->reg[REG_A] = (a & 0xff);
  if (simulate_z80->reg[REG_A] == 0) { SET_Z(); } else { CLR_Z(); }
  if ((simulate_z80->reg[REG_A] & 0x80) != 0) { SET_S(); } else { CLR_S(); }
  if ((simulate_z80->reg[REG_A] & 0x10) != 0) { SET_H(); } else { CLR_H(); }
  if (a & 0x0100) { SET_C(); } else { CLR_C(); }
  CLR_C();

  if (vflag == VFLAG_CLEAR)
  {
    CLR_V();
  }
    else
  if (vflag == VFLAG_PARITY)
  {
    set_parity(simulate, simulate_z80->reg[REG_A]);
  }
}

#if 0
static void set_flags8(struct _simulate *simulate, int new, int old, int number, uint8_t vflag)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  if (vflag == VFLAG_OVERFLOW)
  {
    int a0 = old;
    int a = new;
    int v = (((a0 >> 7) & 1) & ((number >> 7) & 1) & (((a >> 7) & 1) ^ 1)) |
           ((((a0 >> 7) & 1) ^1) & (((number >> 7) & 1) ^ 1) & ((a >> 7) & 1));
    if (v) { SET_V(); } else { CLR_V(); }
  }

  if ((new & 0xffff) == 0) { SET_Z(); } else { CLR_Z(); }
  if ((new & 0x8000) != 0) { SET_S(); } else { CLR_S(); }
  //if ((new & 0x10) != 0) { SET_H(); } else { CLR_H(); }
  if (new & 0x10000) { SET_C(); } else { CLR_C(); }
  CLR_C();

  if (vflag == VFLAG_CLEAR)
  {
    CLR_V();
  }
    else
  if (vflag == VFLAG_PARITY)
  {
    set_parity(simulate, new & 0xff);
  }
}
#endif

static void set_flags16(struct _simulate *simulate, int new, int old, int number, uint8_t vflag)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  if (vflag == VFLAG_OVERFLOW)
  {
    int a0 = old;
    int a = new;
    int v = (((a0 >> 15) & 1) & ((number >> 15) & 1) & (((a >> 15) & 1) ^ 1)) |
           ((((a0 >> 15) & 1) ^1) & (((number >> 15) & 1) ^ 1) & ((a >> 15) & 1));
    if (v) { SET_V(); } else { CLR_V(); }
  }

  if ((new & 0xffff) == 0) { SET_Z(); } else { CLR_Z(); }
  if ((new & 0x8000) != 0) { SET_S(); } else { CLR_S(); }
  //if ((new & 0x10) != 0) { SET_H(); } else { CLR_H(); }
  if (new & 0x10000) { SET_C(); } else { CLR_C(); }
  CLR_C();

  if (vflag == VFLAG_CLEAR)
  {
    CLR_V();
  }
    else
  if (vflag == VFLAG_PARITY)
  {
    //set_parity(simulate, new & 0xffff);
  }
}

static void add_reg16(struct _simulate *simulate, int xy, int reg16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int number;
  int new, old;

  number = get_q(simulate, reg16);

  if (xy == 0)
  {
    old = simulate_z80->ix;
    new = old + number;
    simulate_z80->ix = new & 0xffff;
  }
    else
  {
    old = simulate_z80->iy;
    new = old + number;
    simulate_z80->iy = new & 0xffff;
  }

  CLR_N();

  set_flags16(simulate, new, old, number, VFLAG_OVERFLOW);
}

#if 0
static void sp_inc(int *sp)
{
  (*sp) += 2;
  if (*sp > 0xffff) *sp = 0;
}
#endif

struct _simulate *simulate_init_z80(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_z80) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_z80;
  simulate->simulate_free = simulate_free_z80;
  simulate->simulate_dumpram = simulate_dumpram_z80;
  simulate->simulate_push = simulate_push_z80;
  simulate->simulate_set_reg = simulate_set_reg_z80;
  simulate->simulate_get_reg = simulate_get_reg_z80;
  simulate->simulate_set_pc = simulate_set_pc_z80;
  simulate->simulate_reset = simulate_reset_z80;
  simulate->simulate_dump_registers = simulate_dump_registers_z80;
  simulate->simulate_run = simulate_run_z80;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_z80(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_z80(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  simulate_z80->reg[1] -= 2;
  WRITE_RAM(simulate_z80->reg[1], value & 0xff);
  WRITE_RAM(simulate_z80->reg[1] + 1, value >> 8);
}

int simulate_set_reg_z80(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  //struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  //int reg,n;

  while(*reg_string == ' ') { reg_string++; }
#if 0
  reg = get_register_z80(reg_string);
  if (reg == -1)
  {
    for (n = 0; n < 9; n++)
    {
      if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value == 1)
        { simulate_z80->reg[2] |= (1 << n); }
          else
        { simulate_z80->reg[2] &= 0xffff ^ (1 << n); }
        return 0;
      }
    }
    return -1;
  }

  simulate_z80->reg[reg] = value;

  return 0;
#endif
  return -1;
}

uint32_t simulate_get_reg_z80(struct _simulate *simulate, char *reg_string)
{
  //struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  //int reg;

#if 0
  reg = get_register_z80(reg_string);
  if (reg == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return simulate_z80->reg[reg];
#endif
  return 0;
}

void simulate_set_pc_z80(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  simulate_z80->pc = value;
}

void simulate_reset_z80(struct _simulate *simulate)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  simulate->break_point = -1;
  memset(simulate_z80->reg, 0, sizeof(simulate_z80->reg));
  simulate_z80->ix = 0;
  simulate_z80->iy = 0;
  simulate_z80->sp = 0;
  simulate_z80->pc = 0;
  //simulate_z80->status = 0;
  simulate_z80->iff1 = 0;
  simulate_z80->iff2 = 0;
}

void simulate_free_z80(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

int simulate_dumpram_z80(struct _simulate *simulate, int start, int end)
{
  return -1;
}

// cat table/table_z80.c | grep OP_REG8 | awk -F, '{ print "    case" $5 ":" }'
static int simulate_z80_execute_op_none(struct _simulate *simulate, struct _table_z80 *table_z80, uint16_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int tmp;
  int a4,tmp4;

  switch(table_z80->instr_enum)
  {
    case  Z80_CCF:
      if (GET_C() == 1) { SET_H(); } else { CLR_H(); }
      if (GET_C() == 0) { SET_C(); } else { CLR_C(); }
      return table_z80->cycles_min;
    case  Z80_CPL:
      simulate_z80->reg[REG_A] = ~simulate_z80->reg[REG_A];
      return table_z80->cycles_min;
    case  Z80_DAA:
      // What the fuck?
      tmp = simulate_z80->reg[REG_A];
      if (GET_N() == 1)
      {
        if (GET_H() == 1 || ((simulate_z80->reg[REG_A] & 0x0f) > 9)) { tmp -= 0x06; }
        if (GET_C() == 1 || (simulate_z80->reg[REG_A] > 0x99)) { tmp -= 0x60; }
      }
        else
      {
        if (GET_H() == 1 || ((simulate_z80->reg[REG_A] & 0x0f) > 9)) { tmp += 0x06; }
        if (GET_C() == 1 || (simulate_z80->reg[REG_A] > 0x99)) { tmp += 0x60; }
      }
      if (GET_C() == 1 || simulate_z80->reg[REG_A] > 0x099) { SET_C(); }
      a4 = simulate_z80->reg[REG_A] & 0x08;
      tmp4 = tmp & 0x08;
      a4 = (a4 == 0) ? 0 : 1;
      tmp4 = (tmp4 == 0) ? 0 : 1;
      if ((a4 ^ tmp4) == 0) { CLR_H(); } else { SET_H(); }
      simulate_z80->reg[REG_A] = tmp;
      return table_z80->cycles_min;
    case  Z80_DI:
      simulate_z80->iff1 = 0;
      simulate_z80->iff2 = 0;
      return table_z80->cycles_min;
    case  Z80_EI:
      simulate_z80->iff1 = 1;
      simulate_z80->iff2 = 1;
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
      tmp = simulate_z80->reg[REG_A] & 0x80;
      simulate_z80->reg[REG_A] <<= 1;
      simulate_z80->reg[REG_A] |= GET_C();
      if (tmp == 0) { CLR_C(); } else { SET_C(); }
      return table_z80->cycles_min;
    case  Z80_RLCA:
      if ((simulate_z80->reg[REG_A] & 0x80) == 0) { CLR_C(); } else { SET_C(); }
      simulate_z80->reg[REG_A] <<= 1;
      simulate_z80->reg[REG_A] |= GET_C();
      return table_z80->cycles_min;
    case  Z80_RRA:
      tmp = simulate_z80->reg[REG_A] & 1;
      simulate_z80->reg[REG_A] >>= 1;
      simulate_z80->reg[REG_A] |= GET_C() << 7;
      if (tmp == 0) { CLR_C(); } else { SET_C(); }
      return table_z80->cycles_min;
    case  Z80_RRCA:
      if ((simulate_z80->reg[REG_A] & 0x01) == 0) { CLR_C(); } else { SET_C(); }
      simulate_z80->reg[REG_A] >>= 1;
      simulate_z80->reg[REG_A] |= GET_C() << 7;
      return table_z80->cycles_min;
    case  Z80_SCF:
      SET_C();
      return table_z80->cycles_min;
  }

  return -1;
}

static int simulate_z80_execute_op_a_reg8(struct _simulate *simulate, struct _table_z80 *table_z80, uint8_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int a = simulate_z80->reg[REG_A];
  int rrr = opcode & 0x7;
  int number = simulate_z80->reg[rrr];

  switch(table_z80->instr_enum)
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

  set_flags_a(simulate, a, number, VFLAG_OVERFLOW);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_reg8(struct _simulate *simulate, struct _table_z80 *table_z80, uint8_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int a = simulate_z80->reg[REG_A];
  int rrr = opcode & 0x7;
  int number = simulate_z80->reg[rrr];
  int vflag = VFLAG_CLEAR;
  int tmp;

  switch(table_z80->instr_enum)
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
      set_parity(simulate, tmp);
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

  set_flags_a(simulate, a, number, vflag);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_a_number8(struct _simulate *simulate, struct _table_z80 *table_z80, uint16_t opcode16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int a = simulate_z80->reg[REG_A];
  int number = opcode16 & 0xff;

  switch(table_z80->instr_enum)
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
      a = simulate_z80->reg[REG_A] - (number + GET_C());
      SET_N();
      break;
  }

  set_flags_a(simulate, a, number, VFLAG_OVERFLOW);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_number8(struct _simulate *simulate, struct _table_z80 *table_z80, uint16_t opcode16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int a = simulate_z80->reg[REG_A];
  int number = opcode16 & 0xff;
  int vflag = VFLAG_CLEAR;
  int tmp;

  switch(table_z80->instr_enum)
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
      set_parity(simulate, tmp);
      return table_z80->cycles_min;
    case Z80_DJNZ:
      simulate_z80->reg[REG_B]--;
      if (simulate_z80->reg[REG_B] == 0)
      {
        simulate_z80->pc += (int8_t)number;
      }
      return table_z80->cycles_min;
    case Z80_JR:
      simulate_z80->pc += (int8_t)number;
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

  set_flags_a(simulate, a, number, vflag);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_reg8_v2(struct _simulate *simulate, struct _table_z80 *table_z80, uint8_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int a = simulate_z80->reg[REG_A];
  int rrr = (opcode >> 3) & 0x7;
  int number = 1;
  int vflag = VFLAG_OVERFLOW;

  switch(table_z80->instr_enum)
  {
    case Z80_DEC:
      simulate_z80->reg[rrr]--;
      SET_N();
      break;
    case Z80_INC:
      simulate_z80->reg[rrr]++;
      CLR_N();
      break;
  }

  set_flags_a(simulate, a, number, vflag);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_reg16(struct _simulate *simulate, struct _table_z80 *table_z80, uint8_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  uint16_t new;
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int number = 1;
  int vflag = VFLAG_OVERFLOW;

  //old = get_reg16_half(simulate, reg16);
  old = get_q(simulate, reg16);
  new = old;

  switch(table_z80->instr_enum)
  {
    case Z80_DEC:
      new--;
      SET_N();
      number = -1;
      break;
    case Z80_INC:
      new++;
      CLR_N();
      number = 1;
      break;
  }

  //set_reg16_half(simulate, reg16, new);
  set_q(simulate, reg16, new);

  set_flags16(simulate, new, old, number, vflag);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_reg16p(struct _simulate *simulate, struct _table_z80 *table_z80, uint8_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int reg16 = (opcode >> 4) & 0x3;
  int value;

  if (table_z80->instr_enum == Z80_PUSH)
  {
    value = get_p(simulate, reg16);

    simulate_z80->sp -= 2;
    WRITE_RAM(simulate_z80->sp, value >> 8);
    WRITE_RAM(simulate_z80->sp + 1, value & 0xff);
    return table_z80->cycles_min;
  }
    else
  if (table_z80->instr_enum == Z80_POP)
  {
    value = READ_RAM(simulate_z80->sp);
    value |= READ_RAM(simulate_z80->sp + 1) << 8;
    simulate_z80->sp += 2;

    set_p(simulate, reg16, value);
    return table_z80->cycles_min;
  }

  return -1;
}

static int simulate_z80_execute_op_hl_reg16_2(struct _simulate *simulate, struct _table_z80 *table_z80, uint8_t opcode)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  uint16_t new;
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int number;
  int value;

  number = get_p(simulate, reg16);
  old = get_p(simulate, 2);
  new = old;

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

  set_p(simulate, 2, value);

  set_flags16(simulate, new, old, number, VFLAG_OVERFLOW);

  return table_z80->cycles_min;
}

static int simulate_z80_execute_op_xy(struct _simulate *simulate, struct _table_z80 *table_z80, uint16_t opcode16)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int xy = (opcode16 >> 13) & 0x1;
  int value;

  if (table_z80->instr_enum == Z80_PUSH)
  {
    value = (xy == 0) ? simulate_z80->ix : simulate_z80->iy;

    simulate_z80->sp -= 2;
    WRITE_RAM(simulate_z80->sp, value >> 8);
    WRITE_RAM(simulate_z80->sp + 1, value & 0xff);
    return table_z80->cycles_min;
  }
    else
  if (table_z80->instr_enum == Z80_POP)
  {
    value = READ_RAM(simulate_z80->sp);
    value |= READ_RAM(simulate_z80->sp + 1) << 8;
    simulate_z80->sp += 2;

    if (xy == 0) { simulate_z80->ix = value; }
    else { simulate_z80->iy = value; }
    return table_z80->cycles_min;
  }

  return -1;
}

int simulate_z80_execute(struct _simulate *simulate)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  int reg,n;
  int reg16,xy;
  int offset;
  int address;

  uint16_t opcode = READ_RAM(simulate_z80->pc);
  uint16_t opcode16 = READ_OPCODE16(simulate_z80->pc);

  n = 0;
  while(table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].opcode == (opcode & table_z80[n].mask))
    {
      if (table_z80[n].mask > 0xff) { n++; continue; }
      switch(table_z80[n].type)
      {
        case OP_NONE:
          return simulate_z80_execute_op_none(simulate, &table_z80[n], opcode);
        case OP_A_REG8:
          return simulate_z80_execute_op_a_reg8(simulate, &table_z80[n], opcode);
        case OP_REG8:
          return simulate_z80_execute_op_reg8(simulate, &table_z80[n], opcode);
        case OP_A_NUMBER8:
          return simulate_z80_execute_op_a_number8(simulate, &table_z80[n], opcode16);
        case OP_HL_REG16_1:
          return -1;
        case OP_A_INDEX_HL:
          return -1;
        case OP_INDEX_HL:
          return -1;
        case OP_NUMBER8:
          return simulate_z80_execute_op_number8(simulate, &table_z80[n], opcode16);
        case OP_ADDRESS:
          return -1;
        case OP_COND_ADDRESS:
          return -1;
        case OP_REG8_V2:
          return simulate_z80_execute_op_reg8_v2(simulate, &table_z80[n], opcode);
        case OP_REG16:
          return simulate_z80_execute_op_reg16(simulate, &table_z80[n], opcode);
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
          reg = (opcode >> 3) & 0x7;
          simulate_z80->reg[reg] = simulate_z80->reg[opcode & 0x7];
          return table_z80->cycles_min;
        case OP_REG8_NUMBER8:
          reg = (opcode16 >> 11) & 0x7;
          simulate_z80->reg[reg] = opcode16 & 0xff;
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
          set_q(simulate, reg16, READ_RAM16(simulate_z80->pc + 1));
          return table_z80->cycles_min;
        case OP_HL_INDEX_ADDRESS:
        case OP_INDEX_ADDRESS_HL:
        case OP_SP_HL:
        case OP_INDEX_ADDRESS8_A:
          return -1;
        case OP_REG16P:
          return simulate_z80_execute_op_reg16p(simulate, &table_z80[n], opcode);
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
  while(table_z80[n].instr_enum != Z80_NONE)
  {
    if (table_z80[n].mask <= 0xff) { n++; continue; }
    if (table_z80[n].opcode == (opcode16 & table_z80[n].mask))
    {
      switch(table_z80[n].type)
      {
        case OP_NONE16:
        case OP_NONE24:
        case OP_A_REG_IHALF:
        case OP_A_INDEX:
          return -1;
        case OP_HL_REG16_2:
          return simulate_z80_execute_op_hl_reg16_2(simulate, &table_z80[n], opcode16);
        case OP_XY_REG16:
          xy = (opcode16 >> 13) & 0x1;
          reg16 = (opcode16 >> 4) & 0x3;
          add_reg16(simulate, xy, reg16);
          return table_z80->cycles_min;
        case OP_REG_IHALF:
        case OP_INDEX:
        case OP_BIT_REG8:
        case OP_BIT_INDEX_HL:
        case OP_BIT_INDEX:
        case OP_REG_IHALF_V2:
        case OP_XY:
          return simulate_z80_execute_op_xy(simulate, &table_z80[n], opcode16);
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
          offset = READ_RAM(simulate_z80->pc + 2);
          address = xy + offset;
          reg = (opcode16 >> 3) & 0x7;
          simulate_z80->reg[reg] = READ_RAM(address);
          return table_z80->cycles_min;
        case OP_INDEX_REG8:
          xy = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(simulate_z80->pc + 2);
          address = xy + offset;
          reg = opcode16 & 0x7;
          WRITE_RAM(address, simulate_z80->reg[reg]);
          return table_z80->cycles_min;
        case OP_INDEX_NUMBER8:
          xy = (opcode16 >> 13) & 0x1;
          offset = READ_RAM(simulate_z80->pc + 2);
          address = xy + offset;
          WRITE_RAM(address, READ_RAM(simulate_z80->pc + 3));
          return table_z80->cycles_min;
        case OP_IR_A:
          return -1;
        case OP_A_IR:
          return -1;
        case OP_XY_ADDRESS:
          xy = (opcode16 >> 13) & 0x1;
          set_xy(simulate, xy, READ_RAM16(simulate_z80->pc + 2));
          return table_z80->cycles_min;
        case OP_REG16_INDEX_ADDRESS:
        case OP_XY_INDEX_ADDRESS:
        case OP_INDEX_ADDRESS_REG16:
        case OP_INDEX_ADDRESS_XY:
          return -1;
        case OP_SP_XY:
          xy = (opcode16 >> 13) & 0x1;
          simulate_z80->sp = get_xy(simulate, xy);
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

void simulate_dump_registers_z80(struct _simulate *simulate)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("Status: %02x   S Z X H Y V N C\n", simulate_z80->reg[REG_F]);
  printf("             %d %d %d %d %d %d %d %d\n",
    GET_S(), GET_Z(), GET_X(), GET_H(), GET_Y(), GET_V(), GET_N(), GET_C());

  printf(" A: %02x F: %02x     B: %02x C: %02X    "
         " D: %02x E: %02x     H: %02x L: %02X\n",
         simulate_z80->reg[REG_A],
         simulate_z80->reg[REG_F],
         simulate_z80->reg[REG_B],
         simulate_z80->reg[REG_C],
         simulate_z80->reg[REG_D],
         simulate_z80->reg[REG_E],
         simulate_z80->reg[REG_H],
         simulate_z80->reg[REG_L]);
  printf("IX: %04x        IY: %04x        SP: %04x        PC: %04x\n",
         simulate_z80->ix, simulate_z80->iy,
         simulate_z80->sp, simulate_z80->pc);

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_z80(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
  char instruction[128];
  //uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  //int c;
  int n;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running == 0)
  {
    int cycles_min, cycles_max;
    pc = simulate_z80->pc;
    //opcode = READ_RAM(pc) | (READ_RAM(pc + 1) << 8);

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");
    int count = disasm_z80(simulate->memory, pc, instruction, &cycles_min, &cycles_max);

    // Insert code execution
    // FIXME
    ret = simulate_z80_execute(simulate);

    simulate_z80->pc += count;

    if (simulate->show == 1)
    {
      simulate_dump_registers_z80(simulate);
      int disasm_pc = pc;

      n = 0;
      while(n < 6)
      {
        //int num;
        int count = disasm_z80(simulate->memory, disasm_pc, instruction, &cycles_min, &cycles_max);
#if 0
        if (count == 2)
        {
          num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
        }
          else
        {
          num = READ_RAM(pc);
        }
#endif
        if (cycles_min == -1) break;

        if (disasm_pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (disasm_pc == simulate_z80->reg[0]) { printf("> "); }
          else
        { printf("  "); }

        char hex[16];
        if (count == 1) { sprintf(hex, "         %02x", READ_RAM(disasm_pc)); }
        else if (count == 2) { sprintf(hex, "      %02x %02x", READ_RAM(disasm_pc), READ_RAM(disasm_pc + 1)); }
        else if (count == 3) { sprintf(hex, "   %02x %02x %02x", READ_RAM(disasm_pc), READ_RAM(disasm_pc + 1), READ_RAM(disasm_pc + 2)); }
        else if (count == 4) { sprintf(hex, "%02x %02x %02x %02x", READ_RAM(disasm_pc), READ_RAM(disasm_pc + 1), READ_RAM(disasm_pc + 2), READ_RAM(disasm_pc + 3)); }
        else { sprintf(hex, "         ???"); }

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

    if (simulate->auto_run == 1 && simulate->nested_call_count < 0) { return 0; }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    simulate->cycle_count += ret;
    if (max_cycles != -1 && cycles > max_cycles) break;
    if (simulate->break_point == simulate_z80->reg[0])
    {
       printf("Breakpoint hit at 0x%04x\n", simulate->break_point);
      break;
    }

    if (simulate->usec == 0 || step == 1)
    {
      //simulate->step_mode=0;
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    if (simulate_z80->pc == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_z80->pc = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_z80->reg[0]);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

