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

#define READ_RAM(a) (memory_read_m(simulate->memory, a * 2) << 8) | \
                    memory_read_m(simulate->memory, (a * 2) + 1)
#define WRITE_RAM(a,b) memory_write_m(simulate->memory, a * 2, b >> 8); \
                       memory_write_m(simulate->memory, ((a * 2) + 1), b & 0xff)

#define GET_N() ((simulate_lc3->psr >> 2) & 1)
#define GET_Z() ((simulate_lc3->psr >> 1) & 1)
#define GET_P() ((simulate_lc3->psr) & 1)
#define GET_PRIV() ((simulate_lc3->psr >> 15) & 1)
#define GET_PRIORITY() ((simulate_lc3->psr >> 8) & 7)

#define CHECK_FLAGS() \
    simulate_lc3->psr &= 0xfff8; \
    if (result < 0) { simulate_lc3->psr |= 0x4; } \
    if (result == 0) {  simulate_lc3->psr |= 0x2; } \
    if (result > 0) {  simulate_lc3->psr |= 0x1; }

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static int execute_instruction(struct _simulate *simulate, uint16_t opcode)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;
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
    result = (int16_t)simulate_lc3->reg[r1] + (int16_t)simulate_lc3->reg[r2];

    CHECK_FLAGS();

    simulate_lc3->reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf020) == 0x1020)
  {
    // add
    simm = opcode & 0x1f;
    if ((simm & 0x10) != 0) { simm |= 0xf0; }

    result = (int16_t)simulate_lc3->reg[r1] + simm;

    CHECK_FLAGS();

    simulate_lc3->reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf038) == 0x5000)
  {
    // and
    result = simulate_lc3->reg[r1] & simulate_lc3->reg[r2];

    CHECK_FLAGS();

    simulate_lc3->reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xf020) == 0x5020)
  {
    // and
    simm = opcode & 0x1f;
    if ((simm & 0x10) != 0) { simm |= 0xf0; }

    result = simulate_lc3->reg[r1] & simm;

    CHECK_FLAGS();

    simulate_lc3->reg[r0] = (uint16_t)result;

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
      simulate_lc3->pc += offset9;
    }

    return 0;
  }
    else
  if ((opcode & 0xfe3f) == 0xc000)
  {
    // jmp
    simulate_lc3->pc += simulate_lc3->reg[r1];

    return 0;
  }
    else
  if ((opcode & 0xf800) == 0x4800)
  {
    // jsr
    simulate_lc3->reg[7] = simulate_lc3->pc;
    offset11 = opcode & 0x7ff;
    if ((offset11 & 0x400) != 0) { offset11 |= 0xf800; }
    simulate_lc3->pc += simulate_lc3->pc + offset11;

    return 0;
  }
    else
  if ((opcode & 0xfe3f) == 0x4000)
  {
    // jsrr
    result = simulate_lc3->reg[r1];
    simulate_lc3->reg[7] = simulate_lc3->pc;
    simulate_lc3->pc = result;

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x2000)
  {
    // ld
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = simulate_lc3->pc + offset9;
    simulate_lc3->reg[r0] = READ_RAM(address);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0xa000)
  {
    // ldi
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = simulate_lc3->pc + offset9;
    address = READ_RAM(address);
    simulate_lc3->reg[r0] = READ_RAM(address);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x6000)
  {
    // ldr
    offset6 = opcode & 0x3ff;
    if ((offset6 & 0x200) != 0) { offset6 |= 0xfe00; }

    address = simulate_lc3->reg[r1] + offset6;
    simulate_lc3->reg[r0] = READ_RAM(address);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0xe000)
  {
    // lea
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = simulate_lc3->pc + offset9;
    simulate_lc3->reg[r0] = address;

    return 0;
  }
    else
  if ((opcode & 0xf03f) == 0x903f)
  {
    // not
    result = simulate_lc3->reg[r1] ^ 0xffff;

    CHECK_FLAGS();

    simulate_lc3->reg[r0] = (uint16_t)result;

    return 0;
  }
    else
  if ((opcode & 0xffff) == 0xc1c0)
  {
    // ret
    simulate_lc3->pc = simulate_lc3->reg[7];
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

    simulate_lc3->pc = simulate_lc3->reg[6];
    simulate_lc3->reg[6]--;
    simulate_lc3->psr = simulate_lc3->reg[6];
    simulate_lc3->reg[6]--;

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x3000)
  {
    // st
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = simulate_lc3->pc + offset9;
    WRITE_RAM(address, simulate_lc3->reg[r0]);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0xb000)
  {
    // sti
    offset9 = opcode & 0x1ff;
    if ((offset9 & 0x100) != 0) { offset9 |= 0xff00; }

    address = simulate_lc3->pc + offset9;
    address = READ_RAM(address);
    WRITE_RAM(address, simulate_lc3->reg[r0]);

    return 0;
  }
    else
  if ((opcode & 0xf000) == 0x7000)
  {
    // str
    offset6 = opcode & 0x3ff;
    if ((offset6 & 0x200) != 0) { offset6 |= 0xfe00; }

    address = simulate_lc3->reg[r1] + offset6;
    WRITE_RAM(address, simulate_lc3->reg[r0]);

    return 0;
  }
    else
  if ((opcode & 0xff00) == 0xf000)
  {
    // trap
    simm = opcode & 0xff;

    simulate_lc3->reg[7] = simulate_lc3->pc;
    simulate_lc3->pc = READ_RAM(simm);

    return 0;
  }

  return -1;
}

static int get_reg(char *reg_string)
{
  while(*reg_string == ' ') { reg_string++; }

  if (reg_string[0] != 'r' && reg_string[0] != 'R') { return -1; }
  if (reg_string[2] != 0) { return -1; }
  if (reg_string[1] < '0' || reg_string[2] > '7') { return -1; }

  return reg_string[1] - '0';
}

struct _simulate *simulate_init_lc3(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_lc3) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_lc3;
  simulate->simulate_free = simulate_free_lc3;
  simulate->simulate_dumpram = simulate_dumpram_lc3;
  simulate->simulate_push = simulate_push_lc3;
  simulate->simulate_set_reg = simulate_set_reg_lc3;
  simulate->simulate_get_reg = simulate_get_reg_lc3;
  simulate->simulate_set_pc = simulate_set_pc_lc3;
  simulate->simulate_reset = simulate_reset_lc3;
  simulate->simulate_dump_registers = simulate_dump_registers_lc3;
  simulate->simulate_run = simulate_run_lc3;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_lc3(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_lc3(struct _simulate *simulate, uint32_t value)
{
  //struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;

}

int simulate_set_reg_lc3(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;

  int reg = get_reg(reg_string);

  if (reg < 0) { return -1; }

  simulate_lc3->reg[reg] = value;

  return 0;
}

uint32_t simulate_get_reg_lc3(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;
  int reg = get_reg(reg_string);

  if (reg < 0) { return -1; }

  return simulate_lc3->reg[reg];
}

void simulate_set_pc_lc3(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;

  simulate_lc3->pc = value; 
}

void simulate_reset_lc3(struct _simulate *simulate)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;

  memset(simulate_lc3->reg, 0, sizeof(uint16_t) * 8);

  simulate_lc3->pc = 0x3000;
  simulate_lc3->psr = 0;
}

void simulate_free_lc3(struct _simulate *simulate)
{
  free(simulate);
}

int simulate_dumpram_lc3(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_lc3(struct _simulate *simulate)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;
  int reg;

  printf("PC=0x%04x  N=%d Z=%d P=%d   PRIV=%d  PRIORITY=%d\n",
    simulate_lc3->pc,
    GET_N(),
    GET_Z(),
    GET_P(),
    GET_PRIV(),
    GET_PRIORITY());

  for (reg = 0; reg < 8; reg++)
  {
    printf(" r%d: 0x%04x,", reg, simulate_lc3->reg[reg]);
    if ((reg & 0x3) == 0x3) { printf("\n"); }
  }

  printf("\n");
}

int simulate_run_lc3(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_lc3 *simulate_lc3 = (struct _simulate_lc3 *)simulate->context;
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int n;

  stop_running = 0;

  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running == 0)
  {
    pc = simulate_lc3->pc;

    opcode = READ_RAM(pc);

#if 0
    c = get_cycle_count(opcode);

    if (c > 0)  { simulate->cycle_count += c; }
#endif

    simulate_lc3->pc += 1;

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    ret = execute_instruction(simulate, opcode);

    if (simulate->show == 1)
    {
      simulate_dump_registers_lc3(simulate);

      n = 0;

      while(n < 12)
      {
        int cycles_min,cycles_max;
        int num, count;

        num = READ_RAM(pc);

        count = disasm_lc3(simulate->memory, pc * 2, instruction, &cycles_min, &cycles_max);

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == simulate_lc3->reg[0]) { printf("> "); }
          else
        { printf("  "); }

        printf("0x%04x: 0x%04x %-40s\n", pc, num, instruction);

        n = n + count;
        pc += 1;
        count -= 2;

        while (count > 0)
        {
          if (pc == simulate->break_point) { printf("*"); }
          else { printf(" "); }

          num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
          printf("  0x%04x: 0x%04x\n", pc, num);
          pc += 1;
          count -= 2;
        }
      }
    }

    if (simulate->auto_run == 1 && simulate->nested_call_count < 0)
    {
      return 0;
    }

    if (ret == -1)
    {
      printf("Illegal instruction 0x%04x at address 0x%04x\n", opcode, pc);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    printf("\n");

    if (simulate->break_point == simulate_lc3->pc)
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

    if (simulate_lc3->reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_lc3->pc = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_lc3->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

