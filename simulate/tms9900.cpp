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

#include "asm/tms9900.h"
#include "disasm/tms9900.h"
#include "simulate/tms9900.h"

#define SHOW_STACK sp, memory_read_m(memory, sp+1), memory_read_m(memory, sp)
#define READ_RAM(a) memory_read_m(memory, a)
#define WRITE_RAM(a,b) memory_write_m(memory, a, b)
#define READ_REG(a) \
  (memory_read_m(memory, wp + (a * 2)) << 8) | \
   memory_read_m(memory, wp + (a * 2) + 1)
#define WRITE_REG(a, b) \
   memory_write_m(memory, wp + (a * 2), b >> 8); \
   memory_write_m(memory, wp + (a * 2) + 1, b&0xff);

#define AFFECTS_NZ(a) \
  if (bw == 0) \
  { \
    if (a & 0x8000) { SET_N(); } else { CLEAR_N(); } \
  } \
    else \
  { \
    if (a & 0x80) { SET_N(); } else { CLEAR_N(); } \
  } \
  if (a == 0) { SET_Z(); } else { CLEAR_Z(); }

#define CHECK_CARRY(a) \
  if (bw == 0) \
  { \
    if ((a & 0xffff0000) == 0) { CLEAR_C(); } else { SET_C(); } \
  } \
    else \
  { \
    if ((a & 0xffffff00) == 0) { CLEAR_C(); } else { SET_C(); } \
  }

#define CHECK_OVERFLOW() \
  if (bw == 0) \
  { \
    if (((((uint16_t)dst) & 0x8000) == (((uint16_t)src) & 0x8000)) && (((((uint16_t)result) & 0x8000)) != (((uint16_t)dst) & 0x8000))) { SET_V(); } else { CLEAR_V(); } \
  } \
    else \
  { \
    if (((((uint8_t)dst) & 0x80) == (((uint8_t)src) & 0x80)) && (((((uint8_t)result) & 0x80))!=(((uint8_t)dst) & 0x80))) { SET_V(); } else { CLEAR_V(); } \
  }

#define CHECK_OVERFLOW_WITH_C() \
  if (bw==0) \
  { \
    if ((((int32_t)dst + (int32_t)src + GET_C()) & 0xffff0000) != 0) { SET_V(); } else { CLEAR_V(); } \
  } \
    else \
  { \
    if ((((int32_t)dst + (int32_t)src + GET_C()) & 0xffffff00) != 0) { SET_V(); } else { CLEAR_V(); } \
  }

#define CHECK_Z() \
  if (bw == 0) \
  { \
    if (result & 0x8000) { SET_N(); } else { CLEAR_N(); } \
  } \
    else \
  { \
    if (result & 0x80) { SET_N(); } else { CLEAR_N(); } \
  }

#define IS_LGT_SET()   (st & 0x8000)
#define IS_AGT_SET()   (st & 0x4000)
#define IS_EQ_SET()    (st & 0x2000)
#define IS_C_SET()     (st & 0x1000)
#define IS_V_SET()     (st & 0x0800)
#define IS_OP_SET()    (st & 0x0400)
#define IS_XOP_SET()   (st & 0x0200)
#define IS_PRV_SET()   (st & 0x0100)
#define IS_M_SET()     (st & 0x0080)
#define IS_AFIE_SET()  (st & 0x0020)
#define IS_EM_SET()    (st & 0x0010)
#define GET_INT_MASK() (st & 0x000f)

#define SET_LGT()  st |= 0x8000;
#define SET_AGT()  st |= 0x4000;
#define SET_EQ()   st |= 0x2000;
#define SET_C()    st |= 0x1000;
#define SET_V()    st |= 0x0800;
#define SET_OP()   st |= 0x0400;
#define SET_XOP()  st |= 0x0200;
#define SET_PRV()  st |= 0x0100;
#define SET_M()    st |= 0x0080;
#define SET_AFIE() st |= 0x0020;
#define SET_EM()   st |= 0x0010;

#define SET_INT_MASK(a) st =& 0xfff0; st =| a;

#define CLR_LGT()  st &= (0x8000 ^ 0xffff);
#define CLR_AGT()  st &= (0x4000 ^ 0xffff);
#define CLR_EQ()   st &= (0x2000 ^ 0xffff);
#define CLR_C()    st &= (0x1000 ^ 0xffff);
#define CLR_V()    st &= (0x0800 ^ 0xffff);
#define CLR_OP()   st &= (0x0400 ^ 0xffff);
#define CLR_XOP()  st &= (0x0200 ^ 0xffff);
#define CLR_PRV()  st &= (0x0100 ^ 0xffff);
#define CLR_M()    st &= (0x0080 ^ 0xffff);
#define CLR_AFIE() st &= (0x0020 ^ 0xffff);
#define CLR_EM()   st &= (0x0010 ^ 0xffff);

static const char *flags[] = { "L>", "A>", "=", "C", "O", "P", "X" };

SimulateTms9900::SimulateTms9900(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateTms9900::~SimulateTms9900()
{
}

Simulate *SimulateTms9900::init(Memory *memory)
{
  return new SimulateTms9900(memory);
}

void SimulateTms9900::reset()
{
  cycle_count = 0;
  nested_call_count = 0;

  //memset(reg, 0, sizeof(reg));
  //memory_clear(&memory);
  //reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);

  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  //reg[1] = 0x800;
  pc = 0;
  wp = 0;
  st = 0;
  break_point = -1;
}

void SimulateTms9900::push(uint32_t value)
{
#if 0
  reg[1] -= 2;
  WRITE_RAM(reg[1], value & 0xff);
  WRITE_RAM(reg[1] + 1, value >> 8);
#endif
}

int SimulateTms9900::set_reg(const char *reg_string, uint32_t value)
{
  while (*reg_string == ' ') { reg_string++; }

  int index = get_register(reg_string);

  if (index == -1)
  {
    for (int n = 0; n < 9; n++)
    {
      if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value == 1)
        {
          st |= (1 << n);
        }
          else
        {
          st &= 0xffff ^ (1 << n);
        }

        return 0;
      }
    }
    return -1;
  }

  WRITE_REG(index, value);

  return 0;
}

uint32_t SimulateTms9900::get_reg(const char *reg_string)
{
  int index;

  index = get_register(reg_string);
  if (index == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return READ_REG(index);
}

void SimulateTms9900::set_pc(uint32_t value)
{
  pc = value;
}

void SimulateTms9900::dump_registers()
{
  int n;

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("          0  1 2 3 4 5 6     12-15\n");
  printf("Status:  L> A> EQ C O OP XOP PRV M AFIE EM  INT_MASK\n");
  printf("         %d  %d  %d %d %d %d %d %d %d %d %d %d\n",
         IS_LGT_SET(),
         IS_AGT_SET(),
         IS_EQ_SET(),
         IS_C_SET(),
         IS_V_SET(),
         IS_OP_SET(),
         IS_XOP_SET(),
         IS_PRV_SET(),
         IS_M_SET(),
         IS_AFIE_SET(),
         IS_EM_SET(),
         GET_INT_MASK());

  printf(" PC: 0x%04x,  WP: 0x%04x,  ST: 0x%04x", pc, wp, st);

  for (n = 0; n < 16; n++)
  {
#if 0
    if ((n % 4) == 0)
    {
      printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
      printf("\n");
      sp_inc(&sp);
    }
      else
    { printf(" "); }
#endif

    char reg[4];
    snprintf(reg, sizeof(reg), "r%d",n);
    printf("%3s: 0x%04x,", reg, READ_REG(n));
  }
  //printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int SimulateTms9900::run(int max_cycles, int step)
{
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc_current;
  int c = 0; // FIXME - broken
  int n;

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    pc_current = pc;
    opcode = (READ_RAM(pc_current) << 8) | READ_RAM(pc_current);
    //c = get_cycle_count(opcode);
    //if (c > 0) cycle_count += c;
    pc += 2;

    if (show == true) { printf("\x1b[1J\x1b[1;1H"); }

    ///////
    if (opcode == 0) { break; } // FIXME
    ret = -1;
    ///////

    if (c > 0) cycles += c;

    if (show == true)
    {
      dump_registers();

      n = 0;
      while (n < 6)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc_current) << 8) | READ_RAM(pc_current + 1);

        int count = disasm_tms9900(
          memory,
          pc_current,
          instruction,
          sizeof(instruction),
          &cycles_min,
          &cycles_max);

        if (cycles_min == -1) { break; }

        if (pc_current == break_point)
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
        if (pc_current == pc)
        {
          printf("> ");
        }
          else
        {
          printf("  ");
        }

        if (cycles_min < 1)
        {
          printf("0x%04x: 0x%04x %-40s ?\n", pc_current, num, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: 0x%04x %-40s %d\n", pc_current, num, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: 0x%04x %-40s %d-%d\n", pc_current, num, instruction, cycles_min, cycles_max);
        }

        n = n + count;
        pc_current += 2;
        count--;
        while (count > 0)
        {
          if (pc_current == break_point) { printf("*"); }
          else { printf(" "); }
          num = (READ_RAM(pc_current + 1) << 8) | READ_RAM(pc_current);
          printf("  0x%04x: 0x%04x\n", pc_current, num);
          pc_current += 2;
          count--;
        }
      }
    }

    if (auto_run == true && nested_call_count < 0) { return 0; }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc_current);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) break;
    if (break_point == pc)
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
      pc = 0;
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

int SimulateTms9900::get_register(const char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
    {
      return token[1]-'0';
    }
      else
    if (token[3] == 0 && token[1] == '1' &&
       (token[2] >= '0' && token[2] <= '5'))
    {
      return 10 + (token[2] - '0');
    }
  }

  return -1;
}

