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

#include "asm_avr8.h"
#include "disasm_avr8.h"
#include "simulate_avr8.h"
#include "table_avr8.h"

#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define WRITE_RAM(a,b) memory_write_m(simulate->memory, a, b)

#define SREG_SET(bit) (simulate_avr8->sreg |= (1<<bit))
#define SREG_CLR(bit) (simulate_avr8->sreg &= (0xff ^ (1<<bit)))
#define GET_SREG(bit) ((simulate_avr8->sreg & (1<<bit)) == 0 ? 0 : 1)

#define GET_X() (simulate_avr8->reg[26] | (simulate_avr8->reg[27] << 8))
#define GET_Y() (simulate_avr8->reg[28] | (simulate_avr8->reg[29] << 8))
#define GET_Z() (simulate_avr8->reg[30] | (simulate_avr8->reg[31] << 8))

#define PUSH_STACK16(n) \
  simulate_avr8->ram[simulate_avr8->sp--] = (n) >> 8; \
  simulate_avr8->ram[simulate_avr8->sp--] = (n) & 0xff;

#define POP_STACK16() \
  (simulate_avr8->ram[simulate_avr8->sp] | \
  (simulate_avr8->ram[simulate_avr8->sp+1] << 8)); simulate_avr8->sp += 2;

#define READ_FLASH(n) memory_read_m(simulate->memory, n)
#define WRITE_FLASH(n,data) memory_write_m(simulate->memory, n, data)

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

struct _simulate *simulate_init_avr8(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_avr8) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_avr8;
  simulate->simulate_free = simulate_free_avr8;
  simulate->simulate_push = simulate_push_avr8;
  simulate->simulate_set_reg = simulate_set_reg_avr8;
  simulate->simulate_get_reg = simulate_get_reg_avr8;
  simulate->simulate_reset = simulate_reset_avr8;
  simulate->simulate_dump_registers = simulate_dump_registers_avr8;
  simulate->simulate_run = simulate_run_avr8;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_avr8(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main

  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  simulate_avr8->pc = 0;
  simulate_avr8->sp = 0;
  return simulate;
}

void simulate_push_avr8(struct _simulate *simulate, uint32_t value)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;

  simulate_avr8->sp -= 1;
  WRITE_RAM(simulate_avr8->sp, value & 0xff);
  //WRITE_RAM(simulate_avr8->sp + 1, value >> 8);
}

int simulate_set_reg_avr8(struct _simulate *simulate, char *reg_string, uint32_t value)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
int reg;

  while(*reg_string == ' ') { reg_string++; }
  reg = get_register_avr8(reg_string);
  if (reg == -1)
  {
    // Add flags here
    return -1;
  }

  simulate_avr8->reg[reg] = value;

  return 0;
}

uint32_t simulate_get_reg_avr8(struct _simulate *simulate, char *reg_string)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
int reg;

  reg = get_register_avr8(reg_string);
  if (reg == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return simulate_avr8->reg[reg];
}

void simulate_reset_avr8(struct _simulate *simulate)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  memset(simulate_avr8->reg, 0, sizeof(simulate_avr8->reg));
  //memory_clear(&simulate->memory);
  simulate_avr8->pc = 0; 
  simulate_avr8->sp = 0; 
  simulate_avr8->sreg = 0; 
  simulate->break_point = -1;
}

void simulate_free_avr8(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

// cat table/table_avr8.c | grep OP_NONE | sed 's/^.*AVR8_/AVR8_/' | sed 's/ .*$/:/'
static int simulate_execute_avr8_op_none(struct _simulate *simulate, struct _table_avr8 *table_avr8)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;

  switch(table_avr8->opcode)
  {
    case AVR8_SEC:
      SREG_SET(SREG_C);
      return table_avr8->cycles_min;
    case AVR8_CLC:
      SREG_CLR(SREG_C);
      return table_avr8->cycles_min;
    case AVR8_SEN:
      SREG_SET(SREG_N);
      return table_avr8->cycles_min;
    case AVR8_CLN:
      SREG_CLR(SREG_N);
      return table_avr8->cycles_min;
    case AVR8_SEZ:
      SREG_SET(SREG_Z);
      return table_avr8->cycles_min;
    case AVR8_CLZ:
      SREG_CLR(SREG_Z);
      return table_avr8->cycles_min;
    case AVR8_SEI:
      SREG_SET(SREG_I);
      return table_avr8->cycles_min;
    case AVR8_CLI:
      SREG_CLR(SREG_I);
      return table_avr8->cycles_min;
    case AVR8_SES:
      SREG_SET(SREG_S);
      return table_avr8->cycles_min;
    case AVR8_CLS:
      SREG_CLR(SREG_S);
      return table_avr8->cycles_min;
    case AVR8_SEV:
      SREG_SET(SREG_V);
      return table_avr8->cycles_min;
    case AVR8_CLV:
      SREG_CLR(SREG_V);
      return table_avr8->cycles_min;
    case AVR8_SET:
      SREG_SET(SREG_T);
      return table_avr8->cycles_min;
    case AVR8_CLT:
      SREG_CLR(SREG_T);
      return table_avr8->cycles_min;
    case AVR8_SEH:
      SREG_SET(SREG_H);
      return table_avr8->cycles_min;
    case AVR8_CLH:
      SREG_CLR(SREG_H);
      return table_avr8->cycles_min;
    case AVR8_BREAK:
      // Should we do something here?
      return -1;
    case AVR8_NOP:
      return table_avr8->cycles_min;
    case AVR8_SLEEP:
      // Should we do something here?
      return table_avr8->cycles_min;
    case AVR8_WDR:
      // Should we do something here?
      return -1;
    case AVR8_IJMP:
      simulate_avr8->pc = GET_Z();
      return table_avr8->cycles_min;
    case AVR8_EIJMP:
      return -1;
    case AVR8_ICALL:
      PUSH_STACK16(simulate_avr8->pc + 1)
      simulate_avr8->pc = GET_Z();
      simulate->nested_call_count++;
      return table_avr8->cycles_min;
    case AVR8_EICALL:
      return -1;
    case AVR8_RET:
      simulate_avr8->pc = POP_STACK16();
      simulate->nested_call_count--;
      return table_avr8->cycles_min;
    case AVR8_RETI:
      return -1;
    case AVR8_LPM:
      simulate_avr8->reg[0] = READ_FLASH(GET_Z());
      return table_avr8->cycles_min;
    case AVR8_ELPM:
      return -1;
    case AVR8_SPM:
      WRITE_FLASH(GET_Z(), simulate_avr8->reg[0]);
      return table_avr8->cycles_min;
  }

  return -1;
}

static int simulate_execute_avr8(struct _simulate *simulate)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
uint16_t opcode;
int pc;

  pc = simulate_avr8->pc * 2;
  opcode = READ_RAM(pc) | (READ_RAM(pc + 1) << 8);
  //c = get_cycle_count(opcode);
  //if (c > 0) simulate->cycle_count += c;
  simulate_avr8->pc += 1;

  int n = 0;
  while(table_avr8[n].instr != NULL)
  {
    if ((opcode & table_avr8[n].mask) == table_avr8[n].opcode)
    {
      //*cycles_min = table_avr8[n].cycles_min;
      //*cycles_max = table_avr8[n].cycles_max;

      switch(table_avr8[n].type)
      {
        case OP_NONE:
          simulate_execute_avr8_op_none(simulate, table_avr8);
          break;
        default:
          break;
      }
    }

    n++;
  }

  return -1;
}

void simulate_dump_registers_avr8(struct _simulate *simulate)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
int n;
//int sp = simulate_avr8->sp;

  printf("\nSimulation Register Dump\n");
  printf("-------------------------------------------------------------------\n");
  printf(" PC: 0x%04x,  SP: 0x%04x, SREG: I T H S V N Z C = 0x%02x\n"
         "                                %d %d %d %d %d %d %d %d\n",
         simulate_avr8->pc,
         simulate_avr8->sp,
         GET_SREG(SREG_I),
         GET_SREG(SREG_T),
         GET_SREG(SREG_H),
         GET_SREG(SREG_S),
         GET_SREG(SREG_V),
         GET_SREG(SREG_N),
         GET_SREG(SREG_Z),
         GET_SREG(SREG_C),
         simulate_avr8->sreg);

#if 0
  printf("        8    7    6             4   3 2 1 0              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  sp_inc(&sp);
  printf("                                                         0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
#endif

  for (n = 0; n < 32; n++)
  {
    if ((n % 8) == 0)
    {
      //printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
      printf("\n");
      //sp_inc(&sp);
    }
      else
    { printf(" "); }

    char reg[4];
    sprintf(reg, "r%d",n);
    printf("%3s: 0x%02x", reg, simulate_avr8->reg[n]);
  }
  //printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_avr8(struct _simulate *simulate, int max_cycles, int step)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
char instruction[128];
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
    pc = simulate_avr8->pc * 2;
    ret = simulate_execute_avr8(simulate);

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    //if (c > 0) cycles += c;

    if (simulate->show == 1)
    {
      simulate_dump_registers_avr8(simulate);

      n = 0;
      while(n < 6)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
        int count = disasm_avr8(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == simulate_avr8->reg[0]) { printf("> "); }
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
    if (simulate->break_point == simulate_avr8->reg[0])
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

    if (simulate_avr8->reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_avr8->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_avr8->reg[0]);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}


