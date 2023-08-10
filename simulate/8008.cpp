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

#include "disasm/8008.h"
#include "simulate/8008.h"

#define READ_RAM(a) \
  (memory_read_m(memory, a * 2) << 8) | \
   memory_read_m(memory, (a * 2) + 1)

#define WRITE_RAM(a,b) \
  memory_write_m(memory, a * 2, b >> 8); \
  memory_write_m(memory, ((a * 2) + 1), b & 0xff)

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
  memset(reg, 0, sizeof(uint16_t) * 8);
  pc = 0x0000;
}

void Simulate8008::push(uint32_t value)
{
}

int Simulate8008::set_reg(const char *reg_string, uint32_t value)
{
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
#if 0
  printf("PC=0x%04x  N=%d Z=%d P=%d   PRIV=%d  PRIORITY=%d\n",
    pc,
    GET_N(),
    GET_Z(),
    GET_P(),
    GET_PRIV(),
    GET_PRIORITY());
#endif

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

    opcode = READ_RAM(pc_current);

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

        num = READ_RAM(pc_current);

        count = disasm_8008(
          memory,
          pc_current,
          instruction,
          sizeof(instruction),
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

          num = READ_RAM(pc_current);
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

int Simulate8008::execute_instruction(uint8_t opcode)
{
  return -1;
}

