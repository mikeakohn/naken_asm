/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "asm_z80.h"
#include "disasm_z80.h"
#include "simulate_z80.h"

#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define WRITE_RAM(a,b) memory_write_m(simulate->memory, a, b)

#define GET_S() ((simulate_z80->status >> 7) & 1)
#define GET_Z() ((simulate_z80->status >> 6) & 1)
#define GET_I() ((simulate_z80->status >> 5) & 1)
#define GET_H() ((simulate_z80->status >> 4) & 1)
#define GET_P() ((simulate_z80->status >> 2) & 1)
#define GET_C() ((simulate_z80->status >> 0) & 1)

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
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
  simulate_z80->status = 0;
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

void simulate_dump_registers_z80(struct _simulate *simulate)
{
struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("Status: %02x   S Z I H - P - C\n", simulate_z80->status);
  printf("             %d %d %d %d - %d - %d\n",
         GET_S(), GET_Z(), GET_I(), GET_H(), GET_P(), GET_C());

  printf(" A: %02x F: %02x          B: %02x C: %02X\n",
         simulate_z80->reg[REG_A],
         simulate_z80->reg[REG_F],
         simulate_z80->reg[REG_B],
         simulate_z80->reg[REG_C]);
  printf(" D: %02x E: %02x          H: %02x L: %02X\n",
         simulate_z80->reg[REG_D],
         simulate_z80->reg[REG_E],
         simulate_z80->reg[REG_H],
         simulate_z80->reg[REG_L]);
  printf("IX: %04x   IY: %04x   SP: %04X   PC: %04x\n",
         simulate_z80->ix, simulate_z80->iy,
         simulate_z80->sp, simulate_z80->pc);


  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_z80(struct _simulate *simulate, int max_cycles, int step)
{
struct _simulate_z80 *simulate_z80 = (struct _simulate_z80 *)simulate->context;
char instruction[128];
uint16_t opcode;
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
    pc = simulate_z80->reg[0];
    opcode = READ_RAM(pc) | (READ_RAM(pc + 1) << 8);
    //c = get_cycle_count(opcode);
    //if (c > 0) simulate->cycle_count += c;
    simulate_z80->reg[0] += 2;

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    // Insert code execution
    // FIXME
    ret = opcode;

    //if (c > 0) cycles += c;

    if (simulate->show == 1)
    {
      simulate_dump_registers_z80(simulate);

      n = 0;
      while(n < 6)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
        int count = disasm_z80(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == simulate_z80->reg[0]) { printf("> "); }
          else
        { printf("  "); }

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
        count--;
        while (count > 0)
        {
          if (pc == simulate->break_point) { printf("*"); }
          else { printf(" "); }
          num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
          printf("  0x%04x: 0x%04x\n", pc, num);
          pc += 2;
          count--;
        }
      }
    }

    if (simulate->auto_run == 1 && simulate->nested_call_count < 0) { return 0; }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

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

    if (simulate_z80->reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_z80->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
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


