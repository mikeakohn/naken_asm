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

#include "asm/msp430.h"
#include "disasm/msp430.h"
#include "simulate/msp430.h"

/*

        8   7     6     5       4   3 2 1 0
Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C

*/

#define SHOW_STACK sp, memory_read_m(memory, sp+1), memory_read_m(memory, sp)
#define READ_RAM(a) memory_read_m(memory, a)
#define WRITE_RAM(a,b) \
  if (a == break_io) \
  { \
    exit(b); \
  } \
  memory_write_m(memory, a, b);

#define GET_V()      ((reg[2] >>  8) & 1)
#define GET_SCG1()   ((reg[2] >> 7) & 1)
#define GET_SCG0()   ((reg[2] >> 6) & 1)
#define GET_OSCOFF() ((reg[2] >> 5) & 1)
#define GET_CPUOFF() ((reg[2] >> 4) & 1)
#define GET_GIE()    ((reg[2] >> 3) & 1)
#define GET_N()      ((reg[2] >> 2) & 1)
#define GET_Z()      ((reg[2] >> 1) & 1)
#define GET_C()      ((reg[2]) & 1)

#define SET_V()      reg[2] |= 256;
#define SET_SCG1()   reg[2] |= 128;
#define SET_SCG0()   reg[2] |= 64;
#define SET_OSCOFF() reg[2] |= 32;
#define SET_CPUOFF() reg[2] |= 16;
#define SET_GIE()    reg[2] |= 8;
#define SET_N()      reg[2] |= 4;
#define SET_Z()      reg[2] |= 2;
#define SET_C()      reg[2] |= 1;

#define CLEAR_V()      reg[2] &= (0xffff^256);
#define CLEAR_SCG1()   reg[2] &= (0xffff^128);
#define CLEAR_SCG0()   reg[2] &= (0xffff^64);
#define CLEAR_OSCOFF() reg[2] &= (0xffff^32);
#define CLEAR_CPUOFF() reg[2] &= (0xffff^16);
#define CLEAR_GIE()    reg[2] &= (0xffff^8);
#define CLEAR_N()      reg[2] &= (0xffff^4);
#define CLEAR_Z()      reg[2] &= (0xffff^2);
#define CLEAR_C()      reg[2] &= (0xffff^1);

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

static const char *flags[] =
{
  "C",
  "Z",
  "N",
  "GIE",
  "CPUOFF",
  "OSCOFF",
  "SCG0",
  "SCG1",
  "V"
};

SimulateMsp430::SimulateMsp430(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateMsp430::~SimulateMsp430()
{
}

Simulate *SimulateMsp430::init(Memory *memory)
{
  return new SimulateMsp430(memory);
}

void SimulateMsp430::reset()
{
  cycle_count = 0;
  nested_call_count = 0;
  memset(reg, 0, sizeof(reg));

  // Set PC to reset vector.
  reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);

  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  reg[1] = 0x800;
  break_point = -1;
}

void SimulateMsp430::push(uint32_t value)
{
  reg[1] -= 2;
  WRITE_RAM(reg[1], value & 0xff);
  WRITE_RAM(reg[1] + 1, value >> 8);
}

int SimulateMsp430::set_reg(const char *reg_string, uint32_t value)
{
  while (*reg_string == ' ') { reg_string++; }

  int index = get_register_msp430(reg_string);

  if (index == -1)
  {
    for (int n = 0; n < 9; n++)
    {
      if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value == 1)
        {
          reg[2] |= (1 << n);
        }
          else
        {
          reg[2] &= 0xffff ^ (1 << n);
        }

        return 0;
      }
    }
    return -1;
  }

  reg[index] = value;

  return 0;
}

uint32_t SimulateMsp430::get_reg(const char *reg_string)
{
  int index;

  index = get_register_msp430(reg_string);

  if (index == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return reg[index];
}

void SimulateMsp430::set_pc(uint32_t value)
{
  reg[0] = value;
}

void SimulateMsp430::dump_registers()
{
  int n, sp = reg[1];

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("        8    7    6             4   3 2 1 0              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("        %d    %d    %d      %d      %d   %d %d %d %d              0x%04x: 0x%02x%02x\n",
         (reg[2] >> 8) & 1,
         (reg[2] >> 7) & 1,
         (reg[2] >> 6) & 1,
         (reg[2] >> 5) & 1,
         (reg[2] >> 4) & 1,
         (reg[2] >> 3) & 1,
         (reg[2] >> 2) & 1,
         (reg[2] >> 1) & 1,
         (reg[2]) & 1,
         SHOW_STACK);
  sp_inc(&sp);
  printf("                                                         0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);

  printf(" PC: 0x%04x,  SP: 0x%04x,  SR: 0x%04x,  CG: 0x%04x,",
         reg[0],
         reg[1],
         reg[2],
         reg[3]);

  for (n = 4; n < 16; n++)
  {
    if ((n % 4) == 0)
    {
      printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
      printf("\n");
      sp_inc(&sp);
    }
      else
    { printf(" "); }

    char reg_string[4];
    snprintf(reg_string, sizeof(reg_string), "r%d",n);
    printf("%3s: 0x%04x,", reg_string, reg[n]);
  }
  printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int SimulateMsp430::run(int max_cycles, int step)
{
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int c;
  int n;

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    pc = reg[0];
    opcode = READ_RAM(pc) | (READ_RAM(pc + 1) << 8);
    c = get_cycle_count(opcode);
    if (c > 0) { cycle_count += c; }
    reg[0] += 2;

    if (show == true) printf("\x1b[1J\x1b[1;1H");

    if ((opcode & 0xfc00) == 0x1000)
    {
      ret = one_operand_exe(opcode);
    }
      else
    if ((opcode & 0xe000) == 0x2000)
    {
      ret = relative_jump_exe(opcode);
    }
      else
    {
      if (opcode == 0x4130) { nested_call_count--; }
      ret = two_operand_exe(opcode);
    }

    if (c > 0) { cycles += c; }

    if (show == true)
    {
      dump_registers();

      n = 0;
      while (n < 12)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);

        int count = disasm_msp430(
          memory,
          pc,
          instruction,
          sizeof(instruction),
          &cycles_min,
          &cycles_max);

        if (cycles_min == -1) { break; }

        printf("%s", pc == break_point ? "*" : " ");
/*
        if (pc == break_point) { printf("*"); }
        else { printf(" "); }
*/

        if (n == 0)
        {
          printf("! ");
        }
          else
        if (pc == reg[0])
        {
          printf("> ");
        }
          else
        {
          printf("  ");
        }

        if (cycles_min < 1)
        {
          printf("0x%04x: 0x%04x %-40s ?\n", pc, num, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: 0x%04x %-40s %d\n", pc, num, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: 0x%04x %-40s %d-%d\n", pc, num, instruction, cycles_min, cycles_max);
        }

        n = n + count;
        pc += 2;
        count -= 2;
        while (count > 0)
        {
          printf("%s", pc == break_point ? "*" : " ");
/*
          if (pc == break_point)
          {
            printf("*");
          }
          else
          {
            printf(" ");
          }
*/

          num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
          printf("  0x%04x: 0x%04x\n", pc, num);
          pc += 2;
          count -= 2;
        }
      }
    }

    if (auto_run == true && nested_call_count < 0)
    {
      return 0;
    }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    if (break_point == reg[0])
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == true)
    {
      //step_mode=0;
      disable_signal_handler();
      return 0;
    }

    if (reg[0] == 0xffff)
    {
      printf("Function ended. Total cycles: %d\n", cycle_count);
      step_mode = false;
      reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      disable_signal_handler();
      return 0;
    }

    usleep(usec);
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", reg[0]);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

void SimulateMsp430::sp_inc(int *sp)
{
  (*sp) += 2;
  if (*sp > 0xffff) { *sp = 0; }
}

uint16_t SimulateMsp430::get_data(int reg_index, int As, int bw)
{
  if (reg_index == 3) // CG
  {
    if (As == 0)
    {
      return 0;
    }
      else
    if (As == 1)
    {
      return 1;
    }
      else
    if (As == 2)
    {
      return 2;
    }
      else
    if (As == 3)
    {
      return (bw == 0) ? 0xffff : 0xff;
    }
  }

  if (As == 0) // Rn
  {
    return (bw == 0) ? reg[reg_index] : reg[reg_index] & 0xff;
  }

  if (reg_index == 2)
  {
    if (As == 1) // &LABEL
    {
      int PC = reg[0];
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC+1) << 8);

      reg[0] += 2;

      if (bw == 0)
      {
        return READ_RAM(a) | (READ_RAM(a+1) << 8);
      }
        else
      {
        return READ_RAM(a);
      }
    }
      else
    if (As == 2)
    {
      return 4;
    }
      else
    if (As == 3)
    {
      return 8;
    }
  }

  if (reg_index == 0) // PC
  {
    // This is probably worthless.. some other condition should pick this up
    if (As == 3) // #immediate
    {
      uint16_t a = READ_RAM(reg[0]) | (READ_RAM(reg[0] + 1) << 8);

      reg[0] += 2;

      return (bw == 0) ? a : a & 0xff;
    }
  }

  if (As == 1) // x(Rn)
  {
    uint16_t a = READ_RAM(reg[0]) | (READ_RAM(reg[0] + 1) << 8);
    uint16_t index = reg[reg_index] + ((int16_t)a);

    reg[0] += 2;

    if (bw == 0)
    {
      return READ_RAM(index) | (READ_RAM(index+1) << 8);
    }
      else
    {
      return READ_RAM(index);
    }
  }
    else
  if (As == 2 || As == 3) // @Rn (mode 2) or @Rn+ (mode 3)
  {
    uint16_t index = reg[reg_index];

    if (bw == 0)
    {
      return READ_RAM(index) | (READ_RAM(index + 1) << 8);
    }
      else
    {
      return READ_RAM(reg[reg_index]);
    }
  }

  printf("Error: Unrecognized source addressing mode %d\n", As);

  return 0;
}

void SimulateMsp430::update_reg(int reg_index, int mode, int bw)
{
  if (reg_index == 0) { return; }
  if (reg_index == 2) { return; }
  if (reg_index == 3) { return; }

  if (mode == 3) // @Rn+
  {
    if (bw == 0)
    {
      reg[reg_index] += 2;
    }
      else
    {
      reg[reg_index] += 1;
    }
  }
}

int SimulateMsp430::put_data(
  int PC,
  int reg_index,
  int mode,
  int bw,
  uint32_t data)
{
  if (mode == 0) // Rn
  {
    if (bw == 0)
    {
      reg[reg_index] = data;
    }
      else
    {
      reg[reg_index] = data & 0xff;
    }
    return 0;
  }

  if (reg_index == 2)
  {
    if (mode == 1) // &LABEL
    {
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC+1) << 8);

      if (bw == 0)
      {
        WRITE_RAM(a, data & 0xff);
        WRITE_RAM(a + 1, data >> 8);
      }
        else
      { WRITE_RAM(a, data & 0xff); }

      return 0;
    }
  }

  if (reg_index == 0) // PC
  {
    if (mode == 1) // LABEL
    {
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);

      if (bw == 0)
      {
        WRITE_RAM(PC + a, data & 0xff);
        WRITE_RAM(PC + a + 1, data >> 8);
      }
        else
      {
        WRITE_RAM(PC + a, data & 0xff);
      }

      return 0;
    }
  }

  if (mode == 1) // x(Rn)
  {
    uint16_t a = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);
    int address = reg[reg_index] + ((int16_t)a);

    if (bw == 0)
    {
      WRITE_RAM(address, data & 0xff);
      WRITE_RAM(address + 1, data >> 8);
    }
      else
    {
      WRITE_RAM(address, data & 0xff);
    }

    return 0;
  }
    else
  if (mode == 2 || mode == 3) // @Rn (mode 2) or @Rn+ (mode 3)
  {
    uint16_t index = reg[reg_index];

    if (bw == 0)
    {
      WRITE_RAM(index, data & 0xff);
      WRITE_RAM(index + 1, data >> 8);
    }
      else
    {
      WRITE_RAM(index, data & 0xff);
    }

    return 0;
  }

  printf("Error: Unrecognized addressing mode for destination %d\n", mode);

  return -1;
}

int SimulateMsp430::one_operand_exe(uint16_t opcode)
{
  int o;
  int reg_index;
  int As,bw;
  int count = 1;
  uint32_t result;
  int src;
  int pc;

  o = (opcode & 0x0380) >> 7;

  if (o == 7) { return 1; }
  if (o == 6) { return count; }

  As = (opcode & 0x0030) >> 4;
  bw = (opcode & 0x0040) >> 6;
  reg_index = opcode & 0x000f;

  switch (o)
  {
    case 0:  // RRC
    {
      pc = reg[0];
      src = get_data(reg_index, As, bw);
      int c = GET_C();
      if ((src & 1) == 1) { SET_C(); } else { CLEAR_C(); }
      if (bw == 0)
      { result = (c << 15) | (((uint16_t)src) >> 1); }
        else
      { result = (c << 7) | (((uint8_t)src) >> 1); }
      put_data(pc, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      AFFECTS_NZ(result);
      CLEAR_V();
      break;
    }
    case 1:  // SWPB (no bw)
    {
      pc = reg[0];
      src = get_data(reg_index, As, bw);
      result = ((src & 0xff00) >> 8) | ((src & 0xff) << 8);
      put_data(pc, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      break;
    }
    case 2:  // RRA
    {
      pc = reg[0];
      src = get_data(reg_index, As, bw);
      if ((src & 1) == 1) { SET_C(); } else { CLEAR_C(); }
      if (bw == 0)
      { result = ((int16_t)src) >> 1; }
        else
      { result = ((int8_t)src) >> 1; }
      put_data(pc, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      AFFECTS_NZ(result);
      CLEAR_V();
      break;
    }
    case 3:  // SXT (no bw)
    {
      pc = reg[0];
      src = get_data(reg_index, As, bw);
      result = (int16_t)((int8_t)((uint8_t)src));
      put_data(pc, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      AFFECTS_NZ(result);
      CHECK_CARRY(result);
      CLEAR_V();
      break;
    }
    case 4:  // PUSH
    {
      reg[1] -= 2;
      src = get_data(reg_index, As, bw);
      update_reg(reg_index, As, bw);
      WRITE_RAM(reg[1], src & 0xff);
      WRITE_RAM(reg[1] + 1, src >> 8);
      break;
    }
    case 5:  // CALL (no bw)
    {
      src = get_data(reg_index, As, bw);
      update_reg(reg_index, As, bw);
      reg[1] -= 2;
      WRITE_RAM(reg[1], reg[0] & 0xff);
      WRITE_RAM(reg[1] + 1, reg[0] >> 8);
      reg[0] = src;
      nested_call_count++;
      break;
    }
    case 6:  // RETI
    {
      break;
    }
    default:
    {
      return -1;
    }
  }

  return 0;
}

int SimulateMsp430::relative_jump_exe(uint16_t opcode)
{
  int o;

  o = (opcode & 0x1c00) >> 10;

  int offset = opcode & 0x03ff;
  if ((offset & 0x0200) != 0)
  {
    offset = -((offset ^ 0x03ff) + 1);
  }

  offset *= 2;

  switch (o)
  {
    case 0:  // JNE/JNZ  Z==0
      if (GET_Z() == 0) { reg[0] += offset; }
      break;
    case 1:  // JEQ/JZ   Z==1
      if (GET_Z() == 1) { reg[0] += offset; }
      break;
    case 2:  // JNC/JLO  C==0
      if (GET_C() == 0) { reg[0] += offset; }
      break;
    case 3:  // JC/JHS   C==1
      if (GET_C() == 1) { reg[0] += offset; }
      break;
    case 4:  // JN       N==1
      if (GET_N() == 1) { reg[0] += offset; }
      break;
    case 5:  // JGE      (N^V)==0
      if ((GET_N() ^ GET_V()) == 0) { reg[0] += offset; }
      break;
    case 6:  // JL       (N^V)==1
      if ((GET_N() ^ GET_V()) == 1) { reg[0] += offset; }
      break;
    case 7:  // JMP
      reg[0] += offset;
      break;
    default:
      return -1;
  }

  return 0;
}

int SimulateMsp430::two_operand_exe(uint16_t opcode)
{
  int o;
  int src_reg,dst_reg;
  int Ad,As,bw;
  int dst,src;
  int pc;
  uint32_t result;

  o = opcode >> 12;
  Ad = (opcode & 0x0080) >> 7;
  As = (opcode & 0x0030) >> 4;
  bw = (opcode & 0x0040) >> 6;

  src_reg = (opcode >> 8) & 0x000f;
  dst_reg = opcode & 0x000f;

  switch (o)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      return -1;
    case 4:  // MOV
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      put_data(pc, dst_reg, Ad, bw, src);
      break;
    case 5:  // ADD
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = (uint16_t)dst + (uint16_t)src;
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result);
      break;
    case 6:  // ADDC
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = (uint16_t)dst + (uint16_t)src + GET_C();
      //CHECK_OVERFLOW_WITH_C();
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 7:  // SUBC
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      //src =~ ((uint16_t)src)+1;
      src = ((~((uint16_t)src)) & 0xffff);
      //result = (uint16_t)dst + (uint16_t)src + GET_C();
      // FIXME - Added GET_C().  Test it.
      result = dst + src + GET_C();
      //CHECK_OVERFLOW_WITH_C();
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 8:  // SUB
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      src = ((~((uint16_t)src)) & 0xffff) + 1;
      result = dst + src;
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 9:  // CMP
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      src = ((~((uint16_t)src)) & 0xffff) + 1;
      //result = (uint16_t)dst + (uint16_t)src;
      result = dst + src;
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      //put_data(pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 10: // DADD
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = src + dst + GET_C();
      if (bw == 0)
      {
        int a;
        a = (src & 0xf) + (dst & 0xf) + GET_C();
        a = ((((src >>  4) & 0xf) + ((dst >>  4) & 0xf) + ((a >> 0)/10)) <<  4) | (((a >> 0) % 10)<<0);
        a = ((((src >>  8) & 0xf) + ((dst >>  8) & 0xf) + ((a >> 4)/10)) <<  8) | (((a >> 4) % 10)<<4) | (a & 0xf);
        a = ((((src >> 12) & 0xf) + ((dst >> 12) & 0xf) + ((a >> 8)/10)) << 12) | (((a >> 8) % 10)<<8) | (a & 0xff);
        if( (a>>12) >= 10 ) { a = (((a >> 12) % 10)<<12) | (a&0xfff); SET_C(); } else { CLEAR_C(); }
        result = a;
      }
        else
      {
        int a;
        a = (src & 0xf) + (dst & 0xf) + GET_C();
        a = ((((src >> 4) & 0xf) + ((dst >> 4) & 0xf) + ((a >> 0)/10)) << 4) | (((a >> 0) % 10)<<0);
        if( (a>>4) >= 10 ) { a = (((a >> 4) % 10) << 4) | (a & 0x0f); SET_C(); } else {CLEAR_C(); }
        result = a;
      }
      put_data(pc, dst_reg, Ad, bw, result);
      AFFECTS_NZ(result);
      break;
    case 11: // BIT (dest & src)
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = src & dst;
      AFFECTS_NZ(result);
      if (result != 0) { SET_C(); } else { CLEAR_C(); }
      CLEAR_V();
      break;
    case 12: // BIC (dest &= ~src)
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = (~src) & dst;
      put_data(pc, dst_reg, Ad, bw, result);
      break;
    case 13: // BIS (dest |= src)
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = src | dst;
      put_data(pc, dst_reg, Ad, bw, result);
      break;
    case 14: // XOR
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = src ^ dst;
      put_data(pc, dst_reg, Ad, bw, result);
      AFFECTS_NZ(result);
      if (result != 0) { SET_C(); } else { CLEAR_C(); }
      if ((src & 0x8000) && (dst & 0x8000)) { SET_V(); } else { CLEAR_V(); }
      break;
    case 15: // AND
      src = get_data(src_reg, As, bw);
      update_reg(src_reg, As, bw);
      pc = reg[0];
      dst = get_data(dst_reg, Ad, bw);
      result = src & dst;
      put_data(pc, dst_reg, Ad, bw, result);
      AFFECTS_NZ(result);
      if (result != 0) { SET_C(); } else { CLEAR_C(); }
      CLEAR_V();
      break;
    default:
      return -1;
  }

  return 0;
}

