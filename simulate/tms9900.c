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

#include "asm/tms9900.h"
#include "disasm/tms9900.h"
#include "simulate/tms9900.h"

#define SHOW_STACK sp, memory_read_m(simulate->memory, sp+1), memory_read_m(simulate->memory, sp)
#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define WRITE_RAM(a,b) memory_write_m(simulate->memory, a, b)
#define READ_REG(a) \
  (memory_read_m(simulate->memory, simulate_tms9900->wp + (a * 2)) << 8) | \
   memory_read_m(simulate->memory, simulate_tms9900->wp + (a * 2) + 1)
#define WRITE_REG(a, b) \
   memory_write_m(simulate->memory, simulate_tms9900->wp + (a * 2), b >> 8); \
   memory_write_m(simulate->memory, simulate_tms9900->wp + (a * 2) + 1, b&0xff);

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

#define IS_LGT_SET() (simulate_tms9900->st & 0x8000)
#define IS_AGT_SET() (simulate_tms9900->st & 0x4000)
#define IS_EQ_SET() (simulate_tms9900->st & 0x2000)
#define IS_C_SET() (simulate_tms9900->st & 0x1000)
#define IS_V_SET() (simulate_tms9900->st & 0x0800)
#define IS_OP_SET() (simulate_tms9900->st & 0x0400)
#define IS_XOP_SET() (simulate_tms9900->st & 0x0200)
#define IS_PRV_SET() (simulate_tms9900->st & 0x0100)
#define IS_M_SET() (simulate_tms9900->st & 0x0080)
#define IS_AFIE_SET() (simulate_tms9900->st & 0x0020)
#define IS_EM_SET() (simulate_tms9900->st & 0x0010)
#define GET_INT_MASK() (simulate_tms9900->st & 0x000f)

#define SET_LGT() simulate_tms9900->st |= 0x8000;
#define SET_AGT() simulate_tms9900->st |= 0x4000;
#define SET_EQ() simulate_tms9900->st |= 0x2000;
#define SET_C() simulate_tms9900->st |= 0x1000;
#define SET_V() simulate_tms9900->st |= 0x0800;
#define SET_OP() simulate_tms9900->st |= 0x0400;
#define SET_XOP() simulate_tms9900->st |= 0x0200;
#define SET_PRV() simulate_tms9900->st |= 0x0100;
#define SET_M() simulate_tms9900->st |= 0x0080;
#define SET_AFIE() simulate_tms9900->st |= 0x0020;
#define SET_EM() simulate_tms9900->st |= 0x0010;
#define SET_INT_MASK(a) simulate_tms9900->st =& 0xfff0; \
                        simulate_tms9900->st =| a;

#define CLR_LGT() simulate_tms9900->st &= (0x8000 ^ 0xffff);
#define CLR_AGT() simulate_tms9900->st &= (0x4000 ^ 0xffff);
#define CLR_EQ() simulate_tms9900->st &= (0x2000 ^ 0xffff);
#define CLR_C() simulate_tms9900->st &= (0x1000 ^ 0xffff);
#define CLR_V() simulate_tms9900->st &= (0x0800 ^ 0xffff);
#define CLR_OP() simulate_tms9900->st &= (0x0400 ^ 0xffff);
#define CLR_XOP() simulate_tms9900->st &= (0x0200 ^ 0xffff);
#define CLR_PRV() simulate_tms9900->st &= (0x0100 ^ 0xffff);
#define CLR_M() simulate_tms9900->st &= (0x0080 ^ 0xffff);
#define CLR_AFIE() simulate_tms9900->st &= (0x0020 ^ 0xffff);
#define CLR_EM() simulate_tms9900->st &= (0x0010 ^ 0xffff);

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

static int get_register_tms9900(char *token)
{
  if (token[0]=='r' || token[0]=='R')
  {
    if (token[2]==0 && (token[1]>='0' && token[1]<='9'))
    {
      return token[1]-'0';
    }
      else
    if (token[3]==0 && token[1]=='1' && (token[2]>='0' && token[2]<='5'))
    {
      return 10+(token[2]-'0');
    }
  }

  return -1;
}

struct _simulate *simulate_init_tms9900(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_tms9900) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_tms9900;
  simulate->simulate_free = simulate_free_tms9900;
  simulate->simulate_dumpram = simulate_dumpram_tms9900;
  simulate->simulate_push = simulate_push_tms9900;
  simulate->simulate_set_reg = simulate_set_reg_tms9900;
  simulate->simulate_get_reg = simulate_get_reg_tms9900;
  simulate->simulate_set_pc = simulate_set_pc_tms9900;
  simulate->simulate_reset = simulate_reset_tms9900;
  simulate->simulate_dump_registers = simulate_dump_registers_tms9900;
  simulate->simulate_run = simulate_run_tms9900;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_tms9900(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_tms9900(struct _simulate *simulate, uint32_t value)
{
//struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;

#if 0
  simulate_tms9900->reg[1] -= 2;
  WRITE_RAM(simulate_tms9900->reg[1], value & 0xff);
  WRITE_RAM(simulate_tms9900->reg[1] + 1, value >> 8);
#endif
}

static char *flags[] = { "L>", "A>", "=", "C", "O", "P", "X" };

int simulate_set_reg_tms9900(struct _simulate *simulate, char *reg_string, uint32_t value)
{
struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;
int reg,n;

  while(*reg_string == ' ') { reg_string++; }
  reg = get_register_tms9900(reg_string);
  if (reg == -1)
  {
    for (n = 0; n < 9; n++)
    {
      if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value == 1)
        { simulate_tms9900->st |= (1 << n); }
          else
        { simulate_tms9900->st &= 0xffff ^ (1 << n); }
        return 0;
      }
    }
    return -1;
  }

  WRITE_REG(reg, value);

  return 0;
}

uint32_t simulate_get_reg_tms9900(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;
  int reg;

  reg = get_register_tms9900(reg_string);
  if (reg == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return READ_REG(reg);
}

void simulate_set_pc_tms9900(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;

  simulate_tms9900->pc = 0;
}

void simulate_reset_tms9900(struct _simulate *simulate)
{
  struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  //memset(simulate_tms9900->reg, 0, sizeof(simulate_tms9900->reg));
  //memory_clear(&simulate->memory);
  //simulate_tms9900->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  //simulate_tms9900->reg[1] = 0x800;
  simulate_tms9900->pc = 0;
  simulate_tms9900->wp = 0;
  simulate_tms9900->st = 0;
  simulate->break_point = -1;
}

void simulate_free_tms9900(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

int simulate_dumpram_tms9900(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_tms9900(struct _simulate *simulate)
{
  struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;
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

  printf(" PC: 0x%04x,  WP: 0x%04x,  ST: 0x%04x",
         simulate_tms9900->pc,
         simulate_tms9900->wp,
         simulate_tms9900->st);

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
    sprintf(reg, "r%d",n);
    printf("%3s: 0x%04x,", reg, READ_REG(n));
  }
  //printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_tms9900(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int c = 0; // FIXME - broken
  int n;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running == 0)
  {
    pc = simulate_tms9900->pc;
    opcode = (READ_RAM(pc) << 8) | READ_RAM(pc);
    //c = get_cycle_count(opcode);
    //if (c > 0) simulate->cycle_count += c;
    simulate_tms9900->pc += 2;

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    ///////
    if (opcode == 0) { break; } // FIXME
    ret = -1;
    ///////

    if (c > 0) cycles += c;

    if (simulate->show == 1)
    {
      simulate_dump_registers_tms9900(simulate);

      n = 0;
      while(n < 6)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc) << 8) | READ_RAM(pc + 1);
        int count = disasm_tms9900(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == simulate_tms9900->pc) { printf("> "); }
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
    if (simulate->break_point == simulate_tms9900->pc)
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

    if (simulate_tms9900->pc == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_tms9900->pc = 0;
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_tms9900->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

