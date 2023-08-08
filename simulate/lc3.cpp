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

#include "disasm/lc3.h"
#include "simulate/lc3.h"

#define READ_RAM(a) \
  (memory_read_m(memory, a * 2) << 8) | \
   memory_read_m(memory, (a * 2) + 1)

#define WRITE_RAM(a,b) \
  memory_write_m(memory, a * 2, b >> 8); \
  memory_write_m(memory, ((a * 2) + 1), b & 0xff)

#define GET_N() ((psr >> 2) & 1)
#define GET_Z() ((psr >> 1) & 1)
#define GET_P() ((psr) & 1)
#define GET_PRIV() ((psr >> 15) & 1)
#define GET_PRIORITY() ((psr >> 8) & 7)

#define CHECK_FLAGS() \
  psr &= 0xfff8; \
  if (result < 0)  { psr |= 0x4; } \
  if (result == 0) { psr |= 0x2; } \
  if (result > 0)  { psr |= 0x1; }

SimulateLc3::SimulateLc3(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateLc3::~SimulateLc3()
{
}

Simulate *SimulateLc3::init(Memory *memory)
{
  return new SimulateLc3(memory);
}

void SimulateLc3::push(uint32_t value)
{
}

int SimulateLc3::set_reg(const char *reg_string, uint32_t value)
{
  int index = get_reg_index(reg_string);

  if (index < 0) { return -1; }

  reg[index] = value;

  return 0;
}

uint32_t SimulateLc3::get_reg(const char *reg_string)
{
  int index = get_reg_index(reg_string);
  if (index < 0) { return -1; }

  return reg[index];
}

void SimulateLc3::set_pc(uint32_t value)
{
  pc = value;
}

void SimulateLc3::reset()
{
  memset(reg, 0, sizeof(reg));

  pc = 0x3000;
  psr = 0;
}

int SimulateLc3::dump_ram(int start, int end)
{
  return -1;
}

void SimulateLc3::dump_registers()
{
  int index;

  printf("PC=0x%04x  N=%d Z=%d P=%d   PRIV=%d  PRIORITY=%d\n",
    pc,
    GET_N(),
    GET_Z(),
    GET_P(),
    GET_PRIV(),
    GET_PRIORITY());

  for (index = 0; index < 8; index++)
  {
    printf(" r%d: 0x%04x,", index, reg[index]);
    if ((index & 0x3) == 0x3) { printf("\n"); }
  }

  printf("\n");
}

int SimulateLc3::run(int max_cycles, int step)
{
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc_current;
  int n;

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    pc_current = pc;

    opcode = READ_RAM(pc_current);

    pc += 1;

    if (show == true) { printf("\x1b[1J\x1b[1;1H"); }

    ret = execute(opcode);

    if (show == true)
    {
      dump_registers();

      n = 0;

      while (n < 12)
      {
        int cycles_min,cycles_max;
        int num, count;

        num = READ_RAM(pc_current);

        count = disasm_lc3(
          memory,
          pc_current * 2,
          instruction,
          sizeof(instruction),
          &cycles_min,
          &cycles_max);

        if (pc_current == break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc_current == reg[0]) { printf("> "); }
          else
        { printf("  "); }

        printf("0x%04x: 0x%04x %-40s\n", pc_current, num, instruction);

        n = n + count;
        pc_current += 1;
        count -= 2;

        while (count > 0)
        {
          if (pc_current == break_point) { printf("*"); }
          else { printf(" "); }

          num = (READ_RAM(pc_current + 1) << 8) | READ_RAM(pc_current);
          printf("  0x%04x: 0x%04x\n", pc_current, num);
          pc_current += 1;
          count -= 2;
        }
      }
    }

    if (auto_run == 1 && nested_call_count < 0)
    {
      return 0;
    }

    if (ret == -1)
    {
      printf("Illegal instruction 0x%04x at address 0x%04x\n", opcode, pc_current);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    printf("\n");

    if (break_point == pc)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == 1)
    {
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    if (reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      pc = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", pc);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

int SimulateLc3::execute(uint16_t opcode)
{
  int r0, r1, r2;
  uint16_t address;
  int16_t result;
  int16_t simm;
  int n, z, p;
  int offset6;
  int offset9;
  int offset11;

  r0 = (opcode >> 9) & 0x7;
  r1 = (opcode >> 6) & 0x7;
  r2 = opcode & 0x7;

  if ((opcode & 0xf038) == 0x1000)
  {
    // add
    result = (int16_t)reg[r1] + (int16_t)reg[r2];
    CHECK_FLAGS();
    reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf020) == 0x1020)
  {
    // add
    simm = opcode & 0x1f;
    if ((simm & 0x10) != 0) { simm |= 0xf0; }

    result = (int16_t)reg[r1] + simm;
    CHECK_FLAGS();
    reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf038) == 0x5000)
  {
    // and
    result = reg[r1] & reg[r2];
    CHECK_FLAGS();
    reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf020) == 0x5020)
  {
    // and
    simm = opcode & 0x1f;
    if ((simm & 0x10) != 0) { simm |= 0xf0; }
    result = reg[r1] & simm;
    CHECK_FLAGS();

    reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x0000)
  {
    // br
    n = (opcode >> 9) & 1;
    z = (opcode >> 10) & 1;
    p = (opcode >> 11) & 1;

    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    if ((n == 1 && GET_N() == 1) ||
        (z == 1 && GET_Z() == 1) ||
        (p == 1 && GET_P() == 1))
    {
      pc += offset9;
    }

    return 0;
  }
    else
  if ((opcode & 0xfe3f) == 0xc000)
  {
    // jmp
    pc += reg[r1];

    return 0;
  }
    else
  if ((opcode & 0xf800) == 0x4800)
  {
    // jsr
    reg[7] = pc;
    offset11 = opcode & 0x7ff;
    if ((offset11 & 0x400) != 0) { offset11 |= 0xf800; }
    pc += pc + offset11;

    return 0;
  }
    else
  if ((opcode & 0xfe3f) == 0x4000)
  {
    // jsrr
    result = reg[r1];
    reg[7] = pc;
    pc = result;

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x2000)
  {
    // ld
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = pc + offset9;
    reg[r0] = READ_RAM(address);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0xa000)
  {
    // ldi
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = pc + offset9;
    address = READ_RAM(address);
    reg[r0] = READ_RAM(address);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x6000)
  {
    // ldr
    offset6 = opcode & 0x3ff;
    if ((offset6 & 0x200) != 0) { offset6 |= 0xfe00; }

    address = reg[r1] + offset6;
    reg[r0] = READ_RAM(address);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0xe000)
  {
    // lea
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = pc + offset9;
    reg[r0] = address;

    return 0;
  }
    else
  if ((opcode & 0xf03f) == 0x903f)
  {
    // not
    result = reg[r1] ^ 0xffff;

    CHECK_FLAGS();

    reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xffff) == 0xc1c0)
  {
    // ret
    pc = reg[7];
    return 0;
  }
    else
  if ((opcode & 0xffff) == 0x8000)
  {
    // rti
    if (GET_PRIV() != 0)
    {
      printf("Error: Privilege mode exception\n");
      return -1;
    }

    pc = reg[6];
    reg[6]--;
    psr = reg[6];
    reg[6]--;

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x3000)
  {
    // st
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = pc + offset9;
    WRITE_RAM(address, reg[r0]);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0xb000)
  {
    // sti
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = pc + offset9;
    address = READ_RAM(address);
    WRITE_RAM(address, reg[r0]);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x7000)
  {
    // str
    offset6 = opcode & 0x3ff;
    if ((offset6 & 0x200) != 0) { offset6 |= 0xfe00; }

    address = reg[r1] + offset6;
    WRITE_RAM(address, reg[r0]);

    return 0;
  }
    else
  if ((opcode & 0xff00) == 0xf000)
  {
    // trap
    simm = opcode & 0xff;

    reg[7] = pc;
    pc = READ_RAM(simm);

    return 0;
  }

  return -1;
}

int SimulateLc3::get_reg_index(const char *reg_string)
{
  while (*reg_string == ' ') { reg_string++; }

  if (reg_string[0] != 'r' && reg_string[0] != 'R') { return -1; }
  if (reg_string[2] != 0) { return -1; }
  if (reg_string[1] < '0' || reg_string[2] > '7') { return -1; }

  return reg_string[1] - '0';
}

