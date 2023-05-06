/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "asm/tms340.h"
#include "disasm/tms340.h"
#include "simulate/tms340.h"

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static int get_hexdigit(char *str)
{
  if (str[0] >= '0' && str[0] <= '9') {
    return str[0] - '0';
  } else if (str[0] >= 'A' && str[0] <= 'F') {
    return str[0] - 'A';
  } else if (str[0] >= 'a' && str[0] <= 'f') {
    return str[0] - 'a';
  }
  return -1;
}


static int get_register_tms340(char *token)
{
  if (token[0]=='a' || token[0]=='A') {
    int h = get_hexdigit(str+1);
    if (token[2] == 0 && h >= 0 && h < 15) {
      return h;
    }
  } else if (token[0] == 'b' || token[0] == 'B') {
    int h = get_hexdigit(str+1);
    if (token[2] == 0 && h >= 0 && h < 15) {
      return h + 16;
    }
  }
  return -1;
}

struct _simulate *simulate_init_tms340(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_tms340) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_tms340;
  simulate->simulate_free = simulate_free_tms340;
  simulate->simulate_dumpram = simulate_dumpram_tms340;
  simulate->simulate_push = simulate_push_tms340;
  simulate->simulate_set_reg = simulate_set_reg_tms340;
  simulate->simulate_get_reg = simulate_get_reg_tms340;
  simulate->simulate_set_pc = simulate_set_pc_tms340;
  simulate->simulate_reset = simulate_reset_tms340;
  simulate->simulate_dump_registers = simulate_dump_registers_tms340;
  simulate->simulate_run = simulate_run_tms340;

  simulate->memory    = memory;
  simulate_reset_tms340(simulate);
  simulate->usec      = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show      = 1;   // Show simulation
  simulate->auto_run  = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_tms340(struct _simulate *simulate, uint32_t value)
{
//struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;

  printf("tms340::push - I have no idea what this should do...\n");
}

int simulate_set_reg_tms340(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int reg,n;

  while(*reg_string == ' ') { reg_string++; }
 
  reg = get_register_tms340(reg_string);
  if (reg >= 0 && reg < 15) {
    simulate_tms340->a[reg]      = value;
    return 0;
  } else if (reg >= 16 && reg < 31) {
    simulate_tms340->b[reg - 16] = value;
    return 0;
  } else if (!strcasecmp(reg_string,"pc")) {
    simulate_tms340->pc          = value;
    return 0;
  } else if (!strcasecmp(reg_string,"sp")) {
    simulate_tms340->sp          = value;
    return 0;
  } else if (!strcasecmp(reg_string,"st")) {
    simulate_tms340->st          = value;
    return 0;
  }

  return -1;
}

uint32_t simulate_get_reg_tms340(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int reg;

  reg = get_register_tms340(reg_string);
  if (reg >= 0 && reg < 15) {
    return simulate_tms340->a[reg];
  } else if (reg >= 16 && reg < 31) {
    return simulate_tms340->b[reg - 16];
  } else if (!strcasecmp(reg_string,"pc")) {
    return simulate_tms340->pc;
  } else if (!strcasecmp(reg_string,"sp")) {
    return simulate_tms340->sp;
  } else if (!strcasecmp(reg_string,"st")) {
    return simulate_tms340->st;
  } else {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }
}

void simulate_set_pc_tms340(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  simulate_tms340->pc = value; // thor: why did this set the PC to 0?
}

void simulate_reset_tms340(struct _simulate *simulate)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int i;
  
  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  //memset(simulate_tms9900->reg, 0, sizeof(simulate_tms9900->reg));
  //memory_clear(&simulate->memory);
  //simulate_tms9900->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  //simulate_tms9900->reg[1] = 0x800;
  for(i = 0;i < 15;i++) {
    simulate_tms340->a[i] = 0;
    simulate_tms340->b[i] = 0;
  }
  simulate_tms340->pc   = 0;
  simulate_tms340->st   = 0;
  simulate_tms340->sp   = 0;
  simulate->break_point = -1;
}

void simulate_free_tms340(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

int simulate_dumpram_tms340(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_tms340(struct _simulate *simulate)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int n;
  uint32_t st = simulate_tms340->st;

  printf("\nSimulation Register Dump\n");
  printf("-------------------------------------------------------------------\n");
  printf("Status:  Flags  PBX  IE  FE1,FS1  FE0,FS1\n");
  printf("         %c%c%c%c   %d   %d   %c %2d    %c %2d\n",
	 ((st >> 31) & 1)?'N':' ',
	 ((st >> 30) & 1)?'C':' ',
	 ((st >> 29) & 1)?'Z':' ',
	 ((st >> 28) & 1)?'V':' ',
	 ((st >> 26) & 1),
	 ((st >> 21) & 1),
	 ((st >> 11) & 1)?'S':'U',
	 ((st >>  6) & 7),
	 ((st >>  5) & 1)?'S':'U',
	 ((st >>  0) & 7));

  printf(" PC: 0x%08x,  SP: 0x%08x",
         simulate_tms340->pc,
         simulate_tms340->sp);

  for (n = 0; n < 15; n++) {
    printf("a[%x] : 0x%08x   b[%x] : 0x%08x\n",
	   simulate_tms340->a[n],
	   simulate_tms340->b[n]);
  }
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

static int arithctrl_exe(struct _simulate *simulate,int r,int dreg)
{
  /* 0000.0011.001 = 0x032 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch((r << 5) | dreg) {
  case 0x00:
    return clrc_exe(simulate);
  }
}

static int irqctrl_exe(struct _simulate *simulate,int r,int dreg)
{
  /* 0000.0011.011 = 0x036 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch((r << 5) | dreg) {
  case 0x00:
    return dint_exe(simulate);
  }
}

static int irqnctrl_exe(struct _simulate *simulate,int r,int dreg)
{
  /* 0000.1101.011 = 0x0d6 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch((r << 5) | dreg) {
  case 0x00:
    return eint_exe(simulate);
  }
}

static int arith_exe(struct _simulate *simulate,int sreg,int r,int dreg)
{
  /* 0000.001 = 0x0200 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch(sreg) {
  case 0xc:
    return abs_exe(simulate,r,dreg);
  case 0x9:
    return arithctrl_exe(simulate,r,dreg);
  case 0xb:
    return irqctrl_exe(simulate,r,dreg);
  }
}

static int arithi_exe(struct _simulate *simulate,int sreg,int r,int dreg)
{
  /* 0000.101 = 0x0a00 */
  switch(sreg) {
  case 0x8:
    return addiw_exe(simulate,r,dreg);
  case 0x9:
    return addil_exe(simulate,r,dreg);
  case 0xa:
    return cmpiw_exe(simulate,r,dreg);
  case 0xb:
    return cmpil_exe(simulate,r,dreg);
  case 0xc:
    return andnil_exe(simulate,r,dreg);
  }
}

static int ctrlreg_exe(struct _simulate *simulate, int sreg,int r,int dreg)
{
  /* 0000.100 = 0x0800 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch(sreg) {
  case 0x09:
    return call_exe(simulate,r,dreg); /* call indirect through register */
  }
}

static int ctrl_exe(struct _simulate *simulate, int opcode)
{
  /* 0000.110 = 0x0c00 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch(opcode) {
  case 0x15f:
    return callal_exe(simulate);
  case 0x13f:
    return callr_exe(simulate);
  }
}

static int elem_exe(struct _simulate *simulate,int sreg,int r,int dreg)
{
  /* 0001.010 = 0x1400 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch(sreg) {
  case 0x1:
    return dec_exe(simulate,r,dreg);
  }
}

static int branch_exe(struct simulate *simulate,int sreg,int r,int dreg)
{
  /* 0000.110 = 0x0c00 */
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  switch(sreg) {
  case 0xb:
    return irqn_exe(simulate,r,dreg);
  case 0xc:
    return dsj_exe(simulate,r,dreg);
  case 0xd:
    return dsjeq_exe(simulate,r,dreg);
  case 0xe:
    return dsjne_exe(simulate,r,dreg);
  }
}

static int operand_exe(struct _simulate *simulate, uint16_t opcode)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int dreg = (opcode >> 0) & 0x0f;
  int sreg = (opcode >> 5) & 0x0f;
  int r    = (opcode >> 4) & 0x01;

  switch(opcode & 0xfe00) {
  case 0x0200:
    return arith_exe(simulate,sreg,r,dreg);
  case 0x4000:
    return add_exe(simulate,sreg,r,dreg);
  case 0x4200:
    return addc_exe(simulate,sreg,r,dreg);
  case 0x0a00:
    return arithi_exe(simulate,sreg,r,dreg);
  case 0x0c00:
    return branch_exe(simulate,sreg,r,dreg);
  case 0x1000:
    return addk_exe(simulate,sreg==0?32:0,r,dreg);
  case 0x1200:
    return addk_exe(simulate,sreg+16,r,dreg);
  case 0x1400:
    return elem_exe(simulate,sreg,r,dreg);
  case 0x1c00:
    return btstnk_exe(simulate,sreg,r,dreg);
  case 0x1e00:
    return btstnk_exe(simulate,sreg+16,r,dreg);
  case 0xe000:
    return addxy_exe(simulate,sreg,r,dreg);
  case 0x5000:
    return and_exe(simulate,sreg,r,dreg);
  case 0x5200:
    return andn_exe(simulate,sreg,r,dreg);
  case 0x4a00:
    return btst_exe(simulate,sreg,r,dreg);
  case 0x0800:
    return ctrlreg_exe(simulate,sreg,r,dreg);
  case 0x0c00:
    return ctrl_exe(simulate,opcode & 0x1ff);
  case 0x5600:
    return xor_exe(simulate,sreg,r,dreg);
  case 0x4800:
    return cmp_exe(simulate,sreg,r,dreg);
  case 0xe400:
    return cmpxy_exe(simulate,sreg,r,dreg);
  case 0xe600:
    return cpw_exe(simulate,sreg,r,dreg);
  case 0xe800:
    return cvxyl_exe(simulate,sreg,r,dreg);
  case 0x5800:
    return divs_exe(simulate,sreg,r,dreg);
  case 0x5a00:
    return divu_exe(simulate,sreg,r,dreg);
  case 0xf600:
    return drav_exe(simulate,sreg,r,dreg);
  case 0x3800:
    return dsjs_exe(simulate,sreg,r,dreg);
  case 0x3a00:
    return dsjs_exe(simulate,sreg+16,r,dreg);
  case 0x3c00:
    return dsjs_exe(simulate,-sreg,r,dreg);
  case 0x3e00:
    return dsjs_exe(simulate,-sreg-16,r,dreg);
  }
}

int simulate_run_tms340(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
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
    pc     = simulate_tms340->pc >> 3;
    opcode = (READ_RAM(pc) << 8) | READ_RAM(pc);

    operand_exe(simulate, opcode);
      
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
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
        int count = disasm_tms340(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == simulate_tms340->pc >> 3) { printf("> "); }
          else
        { printf("  "); }

        if (cycles_min < 1)
        {
          printf("0x%04x: 0x%04x %-40s ?\n", pc << 3, num, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: 0x%04x %-40s %d\n", pc << 3, num, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: 0x%04x %-40s %d-%d\n", pc << 3, num, instruction, cycles_min, cycles_max);
        }

        n   = n + count;
        pc += 2;
        count--;
        while (count > 0)
        {
          if (pc << 3 == simulate->break_point) { printf("*"); }
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
    if (simulate->break_point == simulate_tms340->pc)
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

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%08x.\n", simulate_tms340->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

