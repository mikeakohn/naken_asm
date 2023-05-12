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
  (memory_read_m(simulate->memory, a * 2) << 8) | \
   memory_read_m(simulate->memory, (a * 2) + 1)

#define WRITE_RAM(a,b) \
  memory_write_m(simulate->memory, a * 2, b >> 8); \
  memory_write_m(simulate->memory, ((a * 2) + 1), b & 0xff)

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static int execute_instruction(struct _simulate *simulate, uint16_t opcode)
{
  //struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

  return -1;
}

struct _simulate *simulate_init_8008(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_8008) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_8008;
  simulate->simulate_free = simulate_free_8008;
  simulate->simulate_dumpram = simulate_dumpram_8008;
  simulate->simulate_push = simulate_push_8008;
  simulate->simulate_set_reg = simulate_set_reg_8008;
  simulate->simulate_get_reg = simulate_get_reg_8008;
  simulate->simulate_set_pc = simulate_set_pc_8008;
  simulate->simulate_reset = simulate_reset_8008;
  simulate->simulate_dump_registers = simulate_dump_registers_8008;
  simulate->simulate_run = simulate_run_8008;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_8008(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main

  return simulate;
}

void simulate_push_8008(struct _simulate *simulate, uint32_t value)
{
  //struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

}

int simulate_set_reg_8008(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  //struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

  return 0;
}

uint32_t simulate_get_reg_8008(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

  return simulate_8008->reg[0];
}

void simulate_set_pc_8008(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

  simulate_8008->pc = value; 
}

void simulate_reset_8008(struct _simulate *simulate)
{
  struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

  memset(simulate_8008->reg, 0, sizeof(uint16_t) * 8);

  simulate_8008->pc = 0x0000;
}

void simulate_free_8008(struct _simulate *simulate)
{
  free(simulate);
}

int simulate_dumpram_8008(struct _simulate *simulate, int start, int end)
{
  //struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

  return -1;
}

void simulate_dump_registers_8008(struct _simulate *simulate)
{
  //struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;

#if 0
  printf("PC=0x%04x  N=%d Z=%d P=%d   PRIV=%d  PRIORITY=%d\n",
    simulate_8008->pc,
    GET_N(),
    GET_Z(),
    GET_P(),
    GET_PRIV(),
    GET_PRIORITY());
#endif

  printf("\n");
}

int simulate_run_8008(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_8008 *simulate_8008 = (struct _simulate_8008 *)simulate->context;
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int n;

  stop_running = 0;

  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == 0)
  {
    pc = simulate_8008->pc;

    opcode = READ_RAM(pc);

#if 0
    c = get_cycle_count(opcode);

    if (c > 0)  { simulate->cycle_count += c; }
#endif

    simulate_8008->pc += 1;

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    ret = execute_instruction(simulate, opcode);

    if (simulate->show == 1)
    {
      simulate_dump_registers_8008(simulate);

      n = 0;

      while (n < 12)
      {
        int cycles_min,cycles_max;
        int num, count;

        num = READ_RAM(pc);

        count = disasm_8008(
          simulate->memory,
          pc,
          instruction,
          &cycles_min,
          &cycles_max);

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0) { printf("! "); }
        else if (pc == simulate_8008->reg[0]) { printf("> "); }
        else { printf("  "); }

        printf("0x%04x: 0x%04x %-40s\n", pc, num, instruction);

        n = n + count;
        pc += 1;
        count -= 2;

        while (count > 0)
        {
          if (pc == simulate->break_point) { printf("*"); }
          else { printf(" "); }

          num = READ_RAM(pc);
          printf("  0x%04x: 0x%02x\n", pc, num);
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

    if (simulate->break_point == simulate_8008->pc)
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

    if (simulate_8008->reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_8008->pc = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_8008->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

