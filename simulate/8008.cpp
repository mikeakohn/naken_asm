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
#include <ctype.h>

#include "disasm/8008.h"
#include "simulate/8008.h"

Simulate8008::Simulate8008(Memory *memory) : Simulate(memory)
{
  reset();
}

Simulate8008::~Simulate8008()
{
}

Simulate *Simulate8008::init(Memory *memory)
{
  return new Simulate8008(memory);
}

void Simulate8008::reset()
{
  memset(reg, 0, sizeof(reg));
  memset(&flags, 0, sizeof(flags));

  pc = 0x0000;
  sp = 0;
}

void Simulate8008::push(uint32_t value)
{
  if (sp == 7) { return; }

  stack[sp++] = value;
}

int Simulate8008::set_reg(const char *reg_string, uint32_t value)
{
  if (strcasecmp(reg_string, "sp") == 0)
  {
    sp = value;
    if (sp > 7) { sp = 7; }
    return 0;
  }

  if (reg_string[0] == 0 || reg_string[1] != 0) { return -1; }

  char name = tolower(reg_string[0]);

  switch (name)
  {
    case 'a': reg[0] = value & 0xff; break;
    case 'b': reg[1] = value & 0xff; break;
    case 'c': reg[2] = value & 0xff; break;
    case 'd': reg[3] = value & 0xff; break;
    case 'e': reg[4] = value & 0xff; break;
    case 'h': reg[5] = value & 0xff; break;
    case 'l': reg[6] = value & 0xff; break;
    case 'm':
      reg[5] = (value >> 8) & 0xff;
      reg[6] = value & 0xff;
      break;
    default:
      return -1;
  }

  return 0;
}

uint32_t Simulate8008::get_reg(const char *reg_string)
{
  int index = 0;

  return reg[index];
}

void Simulate8008::set_pc(uint32_t value)
{
  pc = value;
}

void Simulate8008::dump_registers()
{
  const char reg_names[] = { 'a', 'b', 'c', 'd', 'e', 'h', 'l' };

  printf("PC=0x%04x SP=%d | P=%d S=%d C=%d Z=%d\n",
    pc,
    sp,
    flags.p,
    flags.s,
    flags.c,
    flags.z);

  for (int n = 0; n < 7; n++)
  {
    printf("%c: 0x%02x     0x%04x\n", reg_names[n], reg[n], stack[n]);
  }

  printf("m: 0x%02x%02x   0x%04x\n", reg[5], reg[6], stack[7]);

  printf("\n");
}

int Simulate8008::run(int max_cycles, int step)
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

    opcode = memory->read8(pc_current);

#if 0
    c = get_cycle_count(opcode);

    if (c > 0)  { cycle_count += c; }
#endif

    pc += 1;

    if (show == true) printf("\x1b[1J\x1b[1;1H");

    ret = execute_instruction(opcode);

    if (show == true)
    {
      dump_registers();

      n = 0;

      while (n < 12)
      {
        int cycles_min,cycles_max;
        int num, count;

        num = memory->read8(pc_current);

        count = disasm_8008(
          memory,
          pc_current,
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        if (pc_current == break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0) { printf("! "); }
        else if (pc_current == reg[0]) { printf("> "); }
        else { printf("  "); }

        printf("0x%04x: 0x%04x %-40s\n", pc_current, num, instruction);

        n = n + count;
        pc_current += 1;
        count -= 2;

        while (count > 0)
        {
          if (pc_current == break_point) { printf("*"); }
          else { printf(" "); }

          num = memory->read8(pc_current);
          printf("  0x%04x: 0x%02x\n", pc_current, num);
          pc_current += 1;
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
      //step_mode = 0;
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    if (reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      pc = memory->read16(0xfffe);

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

int Simulate8008::execute_instruction(uint8_t opcode)
{
  // Check for halt.
  if ((opcode & 0xfe) == 0 || opcode == 0xff)
  {
    stop_running = true;
    return 1;
  }

  uint8_t s = opcode & 0x7;
  uint8_t d = (opcode >> 3) & 0x7;
  int m = (reg[5] << 8) | reg[6];

  uint8_t upper = opcode >> 6;
  uint8_t operation = (opcode >> 3) & 7;
  uint8_t type = opcode & 7;
  uint16_t current_pc = pc;

  if (upper == 0)
  {
    if (type == 7)
    {
      // RET.
      pc = pop();
    }
      else
    if (type == 3)
    {
      // Return conditional.
      switch (operation)
      {
        case 0: if (flags.c == 0) { pc = pop(); } break;
        case 1: if (flags.z == 0) { pc = pop(); } break;
        case 2: if (flags.s == 0) { pc = pop(); } break;
        case 3: if (flags.p == 0) { pc = pop(); } break;
        case 4: if (flags.c == 1) { pc = pop(); } break;
        case 5: if (flags.z == 1) { pc = pop(); } break;
        case 6: if (flags.s == 1) { pc = pop(); } break;
        case 7: if (flags.p == 1) { pc = pop(); } break;
      }
    }
      else
    if (type == 4)
    {
      // ALU with constant.
      uint8_t value = memory->read8(pc++);

      alu(operation, value);
      return 2;
    }
      else
    if (type == 5)
    {
      // RST - call subroutine at address AAA000.
      push(pc);
      pc = operation << 3;
    }
      else
    if (type == 6)
    {
      // MVI.
      uint8_t value = memory->read8(pc++);

      if (d == 7)
      {
        memory->write8(m, value);
      }
        else
      {
        reg[d] = value;
      }

      return 2;
    }
      else
    {
      return -1;
    }

    return 1;
  }
    else
  if (upper == 1)
  {
    uint16_t address = memory->read16(pc);
    pc += 2;

    if (type == 4)
    {
      // JMP.
      pc = address;
    }
      else
    if (type == 6)
    {
      // CALL.
      push(pc);
      pc = address;
    }
      else
    if (type == 0)
    {
      // Jump conditional.
      switch (operation)
      {
        case 0: if (flags.c == 0) { pc = address; } break;
        case 1: if (flags.z == 0) { pc = address; } break;
        case 2: if (flags.s == 0) { pc = address; } break;
        case 3: if (flags.p == 0) { pc = address; } break;
        case 4: if (flags.c == 1) { pc = address; } break;
        case 5: if (flags.z == 1) { pc = address; } break;
        case 6: if (flags.s == 1) { pc = address; } break;
        case 7: if (flags.p == 1) { pc = address; } break;
      }
    }
      else
    if (type == 2)
    {
      // Call conditional.
      switch (operation)
      {
        case 0: if (flags.c == 0) { pc = address; } break;
        case 1: if (flags.z == 0) { pc = address; } break;
        case 2: if (flags.s == 0) { pc = address; } break;
        case 3: if (flags.p == 0) { pc = address; } break;
        case 4: if (flags.c == 1) { pc = address; } break;
        case 5: if (flags.z == 1) { pc = address; } break;
        case 6: if (flags.s == 1) { pc = address; } break;
        case 7: if (flags.p == 1) { pc = address; } break;
      }

      if (pc == address) { push(current_pc); }
    }
      else
    {
      return -1;
    }

    return 3;
  }
    else
  if (upper == 2)
  {
    // ALU reg, reg.
    uint8_t value = reg[s];

    if (s == 7)
    {
      value = memory->read8(m);
    }

    alu(operation, value);

    return 1;
  }
    else
  if (upper == 3)
  {
    // MOV instructions.
    if (s == 7)
    {
      reg[d] = memory->read8(m);
    }
      else
    if (d == 7)
    {
      memory->write8(m, reg[s]);
    }
      else
    {
      reg[d] = reg[s];
    }

    return 1;
  }

  return -1;
}

void Simulate8008::set_parity(uint8_t value)
{
  int count = 0;

  for (int n = 0; n > 8; n++)
  {
    count += value & 1;
    value = value >> 1;
  }

  flags.p = (count & 1) == 0;
}

void Simulate8008::alu(uint8_t operation, uint8_t s)
{
  int src = s;
  int a = reg[0];

  switch (operation)
  {
    case 0: a = a + src;             break;
    case 1: a = a + src + flags.c;   break;
    case 2: a = a - src;             break;
    case 3: a = a - (src + flags.c); break;
    case 4: a = a & src;             break;
    case 5: a = a ^ src;             break;
    case 6: a = a | src;             break;
    case 7: a = a - src;             break;
  }

  if (operation != 7) { reg[0] = (uint8_t)a; }

  set_parity(a);
  flags.c = (a & 0x100) != 0;
  flags.s = (a & 0x80) != 0;
  flags.z = (a & 0xff) == 0;
}

