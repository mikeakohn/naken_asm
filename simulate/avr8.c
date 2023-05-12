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

#include "asm/avr8.h"
#include "disasm/avr8.h"
#include "simulate/avr8.h"
#include "table/avr8.h"

#define READ_OPCODE(a)(memory_read_m(simulate->memory, (a) * 2) | \
                       (memory_read_m(simulate->memory, ((a) * 2) + 1) << 8))
//#define WRITE_RAM(a,b) memory_write_m(simulate->memory, a, b)

#define SREG_SET(bit) (simulate_avr8->sreg |= (1 << bit))
#define SREG_CLR(bit) (simulate_avr8->sreg &= (0xff ^ (1<<bit)))
#define GET_SREG(bit) ((simulate_avr8->sreg & (1 << bit)) == 0 ? 0 : 1)

#define GET_X() (simulate_avr8->reg[26] | (simulate_avr8->reg[27] << 8))
#define GET_Y() (simulate_avr8->reg[28] | (simulate_avr8->reg[29] << 8))
#define GET_Z() (simulate_avr8->reg[30] | (simulate_avr8->reg[31] << 8))

#define INC_X() { int a = GET_X(); a++; \
                  simulate_avr8->reg[26] = a & 0xff; \
                  simulate_avr8->reg[27] = (a >> 8) & 0xff; }
#define INC_Y() { int a = GET_Y(); a++; \
                  simulate_avr8->reg[28] = a & 0xff; \
                  simulate_avr8->reg[29] = (a >> 8) & 0xff; }
#define INC_Z() { int a = GET_Z(); a++; \
                  simulate_avr8->reg[30] = a & 0xff; \
                  simulate_avr8->reg[31] = (a >> 8) & 0xff; }

#define DEC_X() { int a = GET_X(); a--; \
                  simulate_avr8->reg[26] = a & 0xff; \
                  simulate_avr8->reg[27] = (a >> 8) & 0xff; }
#define DEC_Y() { int a = GET_Y(); a--; \
                  simulate_avr8->reg[28] = a & 0xff; \
                  simulate_avr8->reg[29] = (a >> 8) & 0xff; }
#define DEC_Z() { int a = GET_Z(); a--; \
                  simulate_avr8->reg[30] = a & 0xff; \
                  simulate_avr8->reg[31] = (a >> 8) & 0xff; }

#define PUSH_STACK(n) \
  simulate_avr8->ram[simulate_avr8->sp--] = (n) & 0xff;

#define POP_STACK(n) \
  simulate_avr8->ram[++simulate_avr8->sp];

#define PUSH_STACK16(n) \
  simulate_avr8->ram[simulate_avr8->sp--] = (n) & 0xff; \
  simulate_avr8->ram[simulate_avr8->sp--] = (n) >> 8;

#define POP_STACK16() \
  (simulate_avr8->ram[simulate_avr8->sp+2] | \
  (simulate_avr8->ram[simulate_avr8->sp+1] << 8)); simulate_avr8->sp += 2;

#define READ_FLASH(n) memory_read_m(simulate->memory, n)
#define WRITE_FLASH(n,data) memory_write_m(simulate->memory, n, data)

#define READ_RAM(a) simulate_avr8->ram[a & RAM_MASK];
#define WRITE_RAM(a,v) simulate_avr8->ram[a & RAM_MASK] = v;

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

static int simulate_avr8_word_count(struct _simulate *simulate)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  uint16_t opcode = READ_OPCODE(simulate_avr8->pc);
  int n;

  n = 0;
  while(table_avr8[n].instr != NULL)
  {
    if ((opcode & table_avr8[n].mask) == table_avr8[n].opcode)
    {
      switch(table_avr8[n].type)
      {
        case OP_JUMP:
        case OP_REG_SRAM:
        case OP_SRAM_REG:
          return 2;
        default:
          return 1;
      }
    }
    n++;
  }

  return 0;
}

struct _simulate *simulate_init_avr8(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_avr8) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_avr8;
  simulate->simulate_free = simulate_free_avr8;
  simulate->simulate_dumpram = simulate_dumpram_avr8;
  simulate->simulate_push = simulate_push_avr8;
  simulate->simulate_set_reg = simulate_set_reg_avr8;
  simulate->simulate_get_reg = simulate_get_reg_avr8;
  simulate->simulate_set_pc = simulate_set_pc_avr8;
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
  PUSH_STACK(value);
}

int simulate_set_reg_avr8(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int reg;

  while(*reg_string == ' ') { reg_string++; }

  // joe needed these
  if(strcasecmp(reg_string, "sp") == 0)
  {
    simulate_avr8->sp = value & 0xffff;
    return 0;
  }

  if(strcasecmp(reg_string, "pc") == 0)
  {
    simulate_avr8->pc = value & 0xffff;
    return 0;
  }

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

void simulate_set_pc_avr8(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;

  simulate_avr8->pc = value; 
}

void simulate_reset_avr8(struct _simulate *simulate)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  memset(simulate_avr8->reg, 0, sizeof(simulate_avr8->reg));
  memset(simulate_avr8->io, 0, sizeof(simulate_avr8->io));
  memset(simulate_avr8->ram, 0, sizeof(simulate_avr8->ram));
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

int simulate_dumpram_avr8(struct _simulate *simulate, int start, int end)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int n,count;

  count = 0;
  for (n = start; n < end; n++)
  {
    if ((count % 16) == 0) { printf("\n0x%04x: ", n); }
    printf(" %02x", simulate_avr8->ram[n]);
    count++;
  }

  printf("\n\n");

  return 0;
}

// cat table/table_avr8.c | grep OP_NONE | sed 's/^.*AVR8_/AVR8_/' | sed 's/ .*$/:/'
static int simulate_execute_avr8_op_none(struct _simulate *simulate, struct _table_avr8 *table_avr8)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;

  switch(table_avr8->id)
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
      PUSH_STACK16(simulate_avr8->pc)
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

static void simulate_execute_avr8_set_sreg_arith(struct _simulate *simulate, uint8_t rd_prev, uint8_t rd, int k)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int Rd7 = (rd_prev & 0x80) >> 7;
  int R7 = (rd & 0x80) >> 7;
  int K7 = (k & 0x80) >> 7;
  int C = (Rd7 & K7) | (K7 & (R7 ^ 1)) | ((R7 ^ 1) & Rd7);
  int V = (Rd7 & (K7 ^ 1) & (R7 ^ 1)) | ((Rd7 ^ 1) & K7 & R7);
  int N = R7; 
  int S = N ^ V;
  int Rd3 = (rd_prev & 0x08) >> 3;
  int R3 = (rd & 0x08) >> 3;
  int K3 = (k & 0x08) >> 3;
  int H = ((Rd3 ^ 1) & K3) | (K3 & R3) | (R3 & (Rd3 ^ 1));

  if (N == 1) { SREG_SET(SREG_N); } else { SREG_CLR(SREG_N); }
  if (C == 1) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
  if (rd == 0) { SREG_SET(SREG_Z); } else { SREG_CLR(SREG_Z); }
  if (S == 1) { SREG_SET(SREG_S); } else { SREG_CLR(SREG_S); }
  if (V == 1) { SREG_SET(SREG_V); } else { SREG_CLR(SREG_V); }
  if (H == 1) { SREG_SET(SREG_H); } else { SREG_CLR(SREG_H); }
}

static void simulate_execute_avr8_set_sreg_arith_sub(struct _simulate *simulate, uint8_t rd_prev, uint8_t rd, int k)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int Rd7 = (rd_prev & 0x80) >> 7;
  int R7 = (rd & 0x80) >> 7;
  int K7 = (k & 0x80) >> 7;
  int C = ((Rd7 ^ 1) & (K7)) | (K7 & R7) | (R7 & (Rd7 ^ 1));
  int V = (Rd7 & (K7 ^ 1) & (R7 ^ 1)) | ((Rd7 ^ 1) & K7 & R7);
  int N = R7; 
  int S = N ^ V;
  int Rd3 = (rd_prev & 0x08) >> 3;
  int R3 = (rd & 0x08) >> 3;
  int K3 = (k & 0x08) >> 3;
  int H = ((Rd3 ^ 1) & K3) | (K3 & R3) | (R3 & (Rd3 ^ 1));

  if (N == 1) { SREG_SET(SREG_N); } else { SREG_CLR(SREG_N); }
  if (C == 1) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
  if (rd == 0) { SREG_SET(SREG_Z); } else { SREG_CLR(SREG_Z); }
  if (S == 1) { SREG_SET(SREG_S); } else { SREG_CLR(SREG_S); }
  if (V == 1) { SREG_SET(SREG_V); } else { SREG_CLR(SREG_V); }
  if (H == 1) { SREG_SET(SREG_H); } else { SREG_CLR(SREG_H); }
}

static void simulate_execute_avr8_set_sreg_logic(struct _simulate *simulate, uint8_t rd_prev, uint8_t rd, int k)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int R7 = (rd & 0x80) >> 7;
  int N = R7; 
  int S = N ^ 0;

  SREG_CLR(SREG_V);
  if (N == 1) { SREG_SET(SREG_N); } else { SREG_CLR(SREG_N); }
  if (rd == 0) { SREG_SET(SREG_Z); } else { SREG_CLR(SREG_Z); }
  if (S == 1) { SREG_SET(SREG_S); } else { SREG_CLR(SREG_S); }
}

static void simulate_execute_avr8_set_sreg_reg16(struct _simulate *simulate, int rd_prev, int rd)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int R15 = (rd & 0x8000) >> 15;
  int Rdh7 = (rd_prev & 0x0080) >> 7;
  int N = R15; 
  int V = (Rdh7 ^ 1) & R15;
  int S = N ^ V;
  int C = (R15 ^ 1) & Rdh7;

  if (S == 1) { SREG_SET(SREG_S); } else { SREG_CLR(SREG_S); }
  if (V == 1) { SREG_SET(SREG_V); } else { SREG_CLR(SREG_V); }
  if (N == 1) { SREG_SET(SREG_N); } else { SREG_CLR(SREG_N); }
  if (rd == 0) { SREG_SET(SREG_Z); } else { SREG_CLR(SREG_Z); }
  if (C == 1) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
}

static void simulate_execute_avr8_set_sreg_common(struct _simulate *simulate, uint8_t value)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int N = (value & 0x80) >> 7;

  if (N == 1) { SREG_SET(SREG_N); } else { SREG_CLR(SREG_N); }
  if (value == 0) { SREG_SET(SREG_Z); } else { SREG_CLR(SREG_Z); }
}

static void simulate_execute_avr8_set_sreg_sign(struct _simulate *simulate)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int S = GET_SREG(SREG_N) ^ GET_SREG(SREG_V);

  if (S == 1) { SREG_SET(SREG_S); } else { SREG_CLR(SREG_S); }
}

static int simulate_execute_avr8_op_branch_s_k(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int k = (opcode >> 3) & 0x7f;
  int s = (opcode & 0x7);

  if ((k & 0x40) != 0) { k = (char)(0x80 | k); }

  switch(table_avr8->id)
  {
    case AVR8_BRBS:
      if (GET_SREG(s) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRBC:
      if (GET_SREG(s) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
  }

  return -1;
}

static int simulate_execute_avr8_op_branch_k(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int k = (opcode >> 3) & 0x7f;

  if ((k & 0x40) != 0) { k = (char)(0x80 | k); }

  switch(table_avr8->id)
  {
    case AVR8_BREQ:
      if (GET_SREG(SREG_Z) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRNE:
      if (GET_SREG(SREG_Z) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRCS:
      if (GET_SREG(SREG_C) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRCC:
      if (GET_SREG(SREG_C) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRSH:
      if ((GET_SREG(SREG_C) | GET_SREG(SREG_Z)) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRLO:
      if ((GET_SREG(SREG_C) | GET_SREG(SREG_Z)) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRMI:
      if (GET_SREG(SREG_N) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRPL:
      if (GET_SREG(SREG_N) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRGE:
      if ((GET_SREG(SREG_N) ^ GET_SREG(SREG_V)) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRLT:
      if ((GET_SREG(SREG_N) ^ GET_SREG(SREG_V)) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRHS:
      if (GET_SREG(SREG_H) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRHC:
      if (GET_SREG(SREG_H) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRTS:
      if (GET_SREG(SREG_T) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRTC:
      if (GET_SREG(SREG_T) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRVS:
      if (GET_SREG(SREG_V) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRVC:
      if (GET_SREG(SREG_V) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRIE:
      if (GET_SREG(SREG_I) == 1) { simulate_avr8->pc += k; return 2; }
      return 1;
    case AVR8_BRID:
      if (GET_SREG(SREG_I) == 0) { simulate_avr8->pc += k; return 2; }
      return 1;
  }

  return -1;
}

static int simulate_execute_avr8_op_two_reg(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int rd = (opcode >> 4) & 0x1f;
  int rr = ((opcode & 0x200) >> 5) | ((opcode) & 0xf);
  uint8_t prev = simulate_avr8->reg[rd];
  int temp;

  switch(table_avr8->id)
  {
    case AVR8_ADC:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] +
                               simulate_avr8->reg[rr] + GET_SREG(SREG_C);
      simulate_execute_avr8_set_sreg_arith(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
    case AVR8_ADD:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] + simulate_avr8->reg[rr];
      simulate_execute_avr8_set_sreg_arith(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
    case AVR8_AND:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] & simulate_avr8->reg[rr];
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
    case AVR8_CP:
      temp = simulate_avr8->reg[rd] - simulate_avr8->reg[rr];
      simulate_execute_avr8_set_sreg_arith_sub(simulate, prev, temp, simulate_avr8->reg[rr]); 
      break;
    case AVR8_CPC:
      temp = simulate_avr8->reg[rd] - simulate_avr8->reg[rr] - GET_SREG(SREG_C);
      simulate_execute_avr8_set_sreg_arith_sub(simulate, prev, temp, simulate_avr8->reg[rr]); 
      break;
    case AVR8_CPSE:
      if (simulate_avr8->reg[rd] == simulate_avr8->reg[rr])
      {
        int words = simulate_avr8_word_count(simulate);
        simulate_avr8->pc += words;
        return words;
      }
      return 1;
      break;
    case AVR8_EOR:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] ^ simulate_avr8->reg[rr];
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
    case AVR8_MOV:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rr];
      break;
    case AVR8_MUL:
      temp = simulate_avr8->reg[rd] * simulate_avr8->reg[rr];
      temp = temp & 0xffff;
      simulate_avr8->reg[0] = temp & 0xff;
      simulate_avr8->reg[1] = (temp >> 8) & 0xff;
      if (temp == 0) { SREG_SET(SREG_Z); } else { SREG_CLR(SREG_Z); }
      if ((temp & 0x8000) != 0) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
      break;
    case AVR8_OR:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] - simulate_avr8->reg[rr];
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
    case AVR8_SBC:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] -
                               simulate_avr8->reg[rr] - GET_SREG(SREG_C);
      simulate_execute_avr8_set_sreg_arith_sub(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
    case AVR8_SUB:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] - simulate_avr8->reg[rr];
      simulate_execute_avr8_set_sreg_arith_sub(simulate, prev, simulate_avr8->reg[rd], simulate_avr8->reg[rr]); 
      break;
  }

  return table_avr8->cycles_min;
}


static int simulate_execute_avr8_op_reg_imm(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int rd = ((opcode >> 4) & 0xf) + 16;
  int k = ((opcode & 0xf00) >> 4) | (opcode & 0xf);
  uint8_t prev = simulate_avr8->reg[rd];
  int temp;

  switch(table_avr8->id)
  {
    case AVR8_ANDI:
      simulate_avr8->reg[rd] &= k;
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], k); 
      break;
    case AVR8_CPI:
      temp = simulate_avr8->reg[rd] - k;
      simulate_execute_avr8_set_sreg_arith(simulate, prev, temp, k); 
      break;
    case AVR8_LDI:
      simulate_avr8->reg[rd] = k;
      break;
    case AVR8_ORI:
      simulate_avr8->reg[rd] |= k;
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], k); 
      break;
    case AVR8_SBCI:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] - k - GET_SREG(SREG_C);
      simulate_execute_avr8_set_sreg_arith_sub(simulate, prev, simulate_avr8->reg[rd], k); 
      break;
    case AVR8_SBR:
      simulate_avr8->reg[rd] &= k;
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], k); 
      break;
    case AVR8_SUBI:
      simulate_avr8->reg[rd] = simulate_avr8->reg[rd] - k;
      simulate_execute_avr8_set_sreg_arith_sub(simulate, prev, simulate_avr8->reg[rd], k); 
      break;
    case AVR8_CBR:
      simulate_avr8->reg[rd] &= k ^ 0xff;
      simulate_execute_avr8_set_sreg_logic(simulate, prev, simulate_avr8->reg[rd], k); 
      break;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_one_reg(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int rd = (opcode >> 4) & 0x1f;
  int prev = simulate_avr8->reg[rd];

  switch(table_avr8->id)
  {
    case AVR8_ASR:
      simulate_avr8->reg[rd] = ((prev >> 1) & 0x7f) | (prev & 0x80);
      if ((prev & 1) != 0) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
      if ((GET_SREG(SREG_N) ^ GET_SREG(SREG_C)) != 0) { SREG_SET(SREG_V); }
      else { SREG_CLR(SREG_V); }
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_COM:
      simulate_avr8->reg[rd] = ~simulate_avr8->reg[rd];
      SREG_CLR(SREG_V);
      SREG_SET(SREG_C);
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_DEC:
      simulate_avr8->reg[rd] -= 1;
      if ((simulate_avr8->reg[rd] ^ 0x80) == 0xff) { SREG_SET(SREG_V); }
      else { SREG_CLR(SREG_V); }
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_INC:
      simulate_avr8->reg[rd] += 1;
      if ((simulate_avr8->reg[rd] ^ 0x80) == 0xff) { SREG_SET(SREG_V); }
      else { SREG_CLR(SREG_V); }
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_LSR:
      simulate_avr8->reg[rd] = ((prev >> 1) & 0x7f);
      if ((prev & 1) != 0) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      if ((GET_SREG(SREG_N) ^ GET_SREG(SREG_C)) != 0) { SREG_SET(SREG_V); }
      else { SREG_CLR(SREG_V); }
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_NEG:
      simulate_avr8->reg[rd] = -simulate_avr8->reg[rd];
      if ((simulate_avr8->reg[rd] ^ 0x7f) == 0xff) { SREG_SET(SREG_V); }
      else { SREG_CLR(SREG_V); }
      if ((simulate_avr8->reg[rd]) != 0x00) { SREG_SET(SREG_C); }
      else { SREG_CLR(SREG_C); }
      if (((simulate_avr8->reg[rd] & 0x08) | (prev & 0x08)) != 0x00)
           { SREG_SET(SREG_H); }
      else { SREG_CLR(SREG_H); }  // FIXME - WTF? This is always H=1?
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_POP:
      simulate_avr8->reg[rd] = POP_STACK();
      break;
    case AVR8_PUSH:
      PUSH_STACK(simulate_avr8->reg[rd]);
      break;
    case AVR8_ROR:
      simulate_avr8->reg[rd] = ((prev >> 1) & 0x7f) | (GET_SREG(SREG_C) << 7);
      if ((prev & 1) != 0) { SREG_SET(SREG_C); } else { SREG_CLR(SREG_C); }
      simulate_execute_avr8_set_sreg_common(simulate, simulate_avr8->reg[rd]);
      if ((GET_SREG(SREG_N) ^ GET_SREG(SREG_C)) != 0) { SREG_SET(SREG_V); }
      else { SREG_CLR(SREG_V); }
      simulate_execute_avr8_set_sreg_sign(simulate);
      break;
    case AVR8_SWAP:
      simulate_avr8->reg[rd] = (prev >> 4) | ((prev & 0xf) << 4);
      break;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_reg_bit(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int rd = (opcode >> 4) & 0x1f;
  int k = opcode & 0x7;
  int t;

  switch(table_avr8->id)
  {
    case AVR8_BLD:
      t = GET_SREG(SREG_T);
      simulate_avr8->reg[rd] &= 0xff ^ (1 < k);
      simulate_avr8->reg[rd] |= (t < k);
      break;
    case AVR8_BST:
      t = simulate_avr8->reg[rd] & (1 << k);
      if (t != 0) { SREG_SET(SREG_T); } else { SREG_CLR(SREG_T); }
      break;
    case AVR8_SBRC:
      if ((simulate_avr8->reg[rd] & (1 << k)) == 0)
      {
        int words = simulate_avr8_word_count(simulate);
        simulate_avr8->pc += words;
        return words;
      }
      return 1;
    case AVR8_SBRS:
      if ((simulate_avr8->reg[rd] & (1 << k)) != 0)
      {
        int words = simulate_avr8_word_count(simulate);
        simulate_avr8->pc += words;
        return words;
      }
      return 1;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_reg_imm_word(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int prev_reg16,reg16;
  int rd = (((opcode >> 4) & 0x3) << 1) + 24;
  int k = ((opcode & 0xc0) >> 2) | (opcode & 0xf);

  switch(table_avr8->id)
  {
    case AVR8_ADIW:
      prev_reg16 = simulate_avr8->reg[rd] | (simulate_avr8->reg[rd + 1] << 8);
      reg16 = prev_reg16 + k;
      simulate_execute_avr8_set_sreg_reg16(simulate, prev_reg16, reg16); 
      break;
    case AVR8_SBIW:
      prev_reg16 = simulate_avr8->reg[rd] | (simulate_avr8->reg[rd + 1] << 8);
      reg16 = prev_reg16 - k;
      simulate_execute_avr8_set_sreg_reg16(simulate, prev_reg16, reg16); 
      break;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_ioreg_bit(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int a = (opcode >> 3) & 0x1f;
  int k = opcode & 0x7;

  switch(table_avr8->id)
  {
    case AVR8_CBI:
      simulate_avr8->io[a] &= 0xff ^ (1 << k); 
      break;
    case AVR8_SBI:
      simulate_avr8->io[a] |= (1 << k); 
      break;
    case AVR8_SBIC:
      if ((simulate_avr8->io[a] & (1 << k)) == 0)
      {
        int words = simulate_avr8_word_count(simulate);
        simulate_avr8->pc += words;
        return words;
      }
      return 1;
    case AVR8_SBIS:
      if ((simulate_avr8->io[a] & (1 << k)) != 0)
      {
        int words = simulate_avr8_word_count(simulate);
        simulate_avr8->pc += words;
        return words;
      }
      return 1;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_sreg_bit(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int k = (opcode >> 4) & 0x7;


  switch(table_avr8->id)
  {
    case AVR8_BSET:
      simulate_avr8->sreg |= (1 << k);
      break;
    case AVR8_BCLR:
      simulate_avr8->sreg &= 0xff ^ (1 << k);
      break;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_relative(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int k = opcode & 0xfff;

  if (k & 0x800) { k = -(((~k) & 0xfff) + 1); }

  switch(table_avr8->id)
  {
    case AVR8_RJMP:
      break;
    case AVR8_RCALL:
      PUSH_STACK16(simulate_avr8->pc)
      break;
  }

  simulate_avr8->pc += k;

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8_op_jump(struct _simulate *simulate, struct _table_avr8 *table_avr8, uint16_t opcode)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
int k = ((((opcode & 0x1f0) >> 3) | (opcode & 0x1)) << 16) | READ_OPCODE(simulate_avr8->pc);

  simulate_avr8->pc++;

  switch(table_avr8->id)
  {
    case AVR8_CALL:
      PUSH_STACK16(simulate_avr8->pc);
    case AVR8_JMP:
      simulate_avr8->pc = k;
      break;
  }

  return table_avr8->cycles_min;
}

static int simulate_execute_avr8(struct _simulate *simulate)
{
struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
uint16_t opcode;
int cycles = -1;
int rd,rr,k;
int t;

  //pc = simulate_avr8->pc * 2;
  opcode = READ_OPCODE(simulate_avr8->pc);
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
          cycles = simulate_execute_avr8_op_none(simulate, &table_avr8[n]);
          break;
        case OP_BRANCH_S_K:
          cycles = simulate_execute_avr8_op_branch_s_k(simulate, &table_avr8[n], opcode);
          break;
        case OP_BRANCH_K:
          cycles = simulate_execute_avr8_op_branch_k(simulate, &table_avr8[n], opcode);
          break;
        case OP_TWO_REG:
          cycles = simulate_execute_avr8_op_two_reg(simulate, &table_avr8[n], opcode);
          break;
        case OP_REG_IMM:
          cycles = simulate_execute_avr8_op_reg_imm(simulate, &table_avr8[n], opcode);
          break;
        case OP_ONE_REG:
          cycles = simulate_execute_avr8_op_one_reg(simulate, &table_avr8[n], opcode);
          break;
        case OP_REG_BIT:
          cycles = simulate_execute_avr8_op_reg_bit(simulate, &table_avr8[n], opcode);
          break;
        case OP_REG_IMM_WORD:
          cycles = simulate_execute_avr8_op_reg_imm_word(simulate, &table_avr8[n], opcode);
          break;
        case OP_IOREG_BIT:
          cycles = simulate_execute_avr8_op_ioreg_bit(simulate, &table_avr8[n], opcode);
          break;
        case OP_SREG_BIT:
          cycles = simulate_execute_avr8_op_sreg_bit(simulate, &table_avr8[n], opcode);
          break;
        case OP_REG_4:
          rd = ((opcode >> 4) & 0xf) + 16;
          simulate_avr8->reg[rd] = 0xff;
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_IN:
          rd = (opcode >> 4) & 0xf;
          k = ((opcode & 0x600) >> 5) | (opcode & 0xf);
          simulate_avr8->reg[rd] = simulate_avr8->io[k];
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_OUT:
          rd = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x600) >> 5) | (opcode & 0xf);
          simulate_avr8->io[k] = simulate_avr8->reg[rd];
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_MOVW:
          rd = ((opcode >> 4) & 0xf) << 1;
          rr = (opcode & 0xf) << 1;
          simulate_avr8->reg[rd] = simulate_avr8->reg[rr];
          simulate_avr8->reg[rd + 1] = simulate_avr8->reg[rr + 1];
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_RELATIVE:
          cycles = simulate_execute_avr8_op_relative(simulate, &table_avr8[n], opcode);
          break;
        case OP_JUMP:
          cycles = simulate_execute_avr8_op_jump(simulate, &table_avr8[n], opcode);
          break;
        case OP_SPM_Z_PLUS:
          WRITE_FLASH(GET_Z(), simulate_avr8->reg[0]);
          WRITE_FLASH(GET_Z() + 1, simulate_avr8->reg[1]);
          { INC_Z(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_REG_X:
        case OP_REG_X_PLUS:
        case OP_REG_MINUS_X:
          rd = (opcode >> 4) & 0x1f;
          if (table_avr8[n].type == OP_REG_MINUS_X) { DEC_X(); }
          simulate_avr8->reg[rd] = READ_RAM(GET_X());
          if (table_avr8[n].type == OP_REG_X_PLUS) { INC_X(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_REG_Y:
        case OP_REG_Y_PLUS:
        case OP_REG_MINUS_Y:
          rd = (opcode >> 4) & 0x1f;
          if (table_avr8[n].type == OP_REG_MINUS_Y) { DEC_Y(); }
          simulate_avr8->reg[rd] = READ_RAM(GET_Y());
          if (table_avr8[n].type == OP_REG_Y_PLUS) { INC_Y(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_REG_Z:
        case OP_REG_Z_PLUS:
        case OP_REG_MINUS_Z:
          rd = (opcode >> 4) & 0x1f;
          if (table_avr8[n].type == OP_REG_MINUS_Z) { DEC_Z(); }
          if(table_avr8[n].id == AVR8_LPM)
            simulate_avr8->reg[rd] = READ_FLASH(GET_Z());
          else
            simulate_avr8->reg[rd] = READ_RAM(GET_Z());
          if (table_avr8[n].type == OP_REG_Z_PLUS) { INC_Z(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_X_REG:
        case OP_X_PLUS_REG:
        case OP_MINUS_X_REG:
          rd = (opcode >> 4) & 0x1f;
          if (table_avr8[n].type == OP_MINUS_X_REG) { DEC_X(); }
          WRITE_RAM(GET_X(), simulate_avr8->reg[rd]);
          if (table_avr8[n].type == OP_X_PLUS_REG) { INC_X(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_Y_REG:
        case OP_Y_PLUS_REG:
        case OP_MINUS_Y_REG:
          rd = (opcode >> 4) & 0x1f;
          if (table_avr8[n].type == OP_MINUS_Y_REG) { DEC_Y(); }
          WRITE_RAM(GET_Y(), simulate_avr8->reg[rd]);
          if (table_avr8[n].type == OP_Y_PLUS_REG) { INC_Y(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_Z_REG:
        case OP_Z_PLUS_REG:
        case OP_MINUS_Z_REG:
          rd = (opcode >> 4) & 0x1f;
          if (table_avr8[n].type == OP_MINUS_Z_REG) { DEC_Z(); }
          WRITE_RAM(GET_Z(), simulate_avr8->reg[rd]);
          if (table_avr8[n].type == OP_Z_PLUS_REG) { INC_Z(); }
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_FMUL:
          // FIXME - implement
          return -1;
        case OP_MULS:
          rd = ((opcode >> 4) & 0xf) + 16;
          rr = (opcode & 0xf) + 16;
          t = ((uint32_t)((int8_t)simulate_avr8->reg[rd])) *
              ((uint32_t)((int8_t)simulate_avr8->reg[rd]));
          simulate_avr8->reg[0] = ((uint32_t)t) & 0xff;
          simulate_avr8->reg[1] = (((uint32_t)t) >> 8) & 0xff;
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_DATA4:
          // FIXME - implement
          return -1;
        case OP_REG_SRAM:
          rd = (opcode >> 4) & 0x1f;
          k = READ_OPCODE(simulate_avr8->pc);
          simulate_avr8->pc++;
          simulate_avr8->reg[rd] = READ_RAM(k);
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_SRAM_REG:
          rr = (opcode >> 4) & 0x1f;
          k = READ_OPCODE(simulate_avr8->pc);
          simulate_avr8->pc++;
          WRITE_RAM(k, simulate_avr8->reg[rr]);
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_REG_Y_PLUS_Q:
        case OP_REG_Z_PLUS_Q:
          rd = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x2000) >> 8) | ((opcode & 0xc00) >> 7) | (opcode & 0x7);
          if (table_avr8[n].type == OP_REG_Y_PLUS_Q) { k += GET_Y(); }
          if (table_avr8[n].type == OP_REG_Z_PLUS_Q) { k += GET_Z(); }
          simulate_avr8->reg[rd] = READ_RAM(k);
          cycles = table_avr8[n].cycles_min;
          break;
        case OP_Y_PLUS_Q_REG:
        case OP_Z_PLUS_Q_REG:
          rr = (opcode >> 4) & 0x1f;
          k = ((opcode & 0x2000) >> 8) | ((opcode & 0xc00) >> 7) | (opcode & 0x7);
          if (table_avr8[n].type == OP_Y_PLUS_Q_REG) { k += GET_Y(); }
          if (table_avr8[n].type == OP_Z_PLUS_Q_REG) { k += GET_Z(); }
          WRITE_RAM(k, simulate_avr8->reg[rr]);
          cycles = table_avr8[n].cycles_min;
          break;

        default:
          return -1;
      }

      return cycles;
    }

    n++;
  }

  return cycles;
}

void simulate_dump_registers_avr8(struct _simulate *simulate)
{
  struct _simulate_avr8 *simulate_avr8 = (struct _simulate_avr8 *)simulate->context;
  int n;

  printf("\nSimulation Register Dump\n");
  printf("-------------------------------------------------------------------\n");
  printf(" PC: 0x%04x,  SP: 0x%04x, SREG: I T H S V N Z C = 0x%02x\n"
         "                                %d %d %d %d %d %d %d %d\n",
         simulate_avr8->pc,
         simulate_avr8->sp,
         simulate_avr8->sreg,
         GET_SREG(SREG_I),
         GET_SREG(SREG_T),
         GET_SREG(SREG_H),
         GET_SREG(SREG_S),
         GET_SREG(SREG_V),
         GET_SREG(SREG_N),
         GET_SREG(SREG_Z),
         GET_SREG(SREG_C));

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

  printf(" X=0x%04x, Y=0x%04x, Z=0x%04x\n\n", GET_X(), GET_Y(), GET_Z());
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
    pc = simulate_avr8->pc;
    ret = simulate_execute_avr8(simulate);

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    if (ret > 0) simulate->cycle_count += ret;

    if (simulate->show == 1)
    {
      int disasm_pc = pc;
      simulate_dump_registers_avr8(simulate);

      n = 0;
      while(n < 6)
      {
        int cycles_min,cycles_max;
        int num;
        num = READ_OPCODE(disasm_pc);
        int count = disasm_avr8(simulate->memory, disasm_pc * 2, instruction, &cycles_min, &cycles_max) / 2;
        if (cycles_min == -1) break;

        if (disasm_pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (disasm_pc == simulate_avr8->pc) { printf("> "); }
          else
        { printf("  "); }

        if (cycles_min < 1)
        {
          printf("0x%04x: 0x%04x %-40s ?\n", disasm_pc, num, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: 0x%04x %-40s %d\n", disasm_pc, num, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: 0x%04x %-40s %d-%d\n", disasm_pc, num, instruction, cycles_min, cycles_max);
        }

        n = n + count;
        count--;
        disasm_pc++;
        while (count > 0)
        {
          if (disasm_pc == simulate->break_point) { printf("*"); }
          else { printf(" "); }
          num = READ_OPCODE(disasm_pc);
          printf("  0x%04x: 0x%04x\n", disasm_pc, num);
          disasm_pc++;
          count--;
        }
      }
    }
//if (pc == 1) exit(1);

    if (simulate->auto_run == 1 && simulate->nested_call_count < 0) { return 0; }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) break;
    if (simulate->break_point == simulate_avr8->pc)
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

#if 0
    if (simulate_avr8->pc == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      signal(SIGINT, SIG_DFL);
      return 0;
    }
#endif

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_avr8->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

