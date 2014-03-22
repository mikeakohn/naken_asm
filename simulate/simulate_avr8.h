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

#ifndef _SIMULATE_AVR8_H
#define _SIMULATE_AVR8_H

#include <unistd.h>

#include "memory.h"
#include "simulate_common.h"

#define SREG_C 0
#define SREG_Z 1
#define SREG_N 2
#define SREG_V 3
#define SREG_S 4
#define SREG_H 5
#define SREG_T 6
#define SREG_I 7

struct _simulate_avr8
{
  uint8_t reg[32];
  uint8_t ram[8192];
  int sp_start;
  int pc;
  int sp;
  uint8_t sreg;
};

struct _simulate *simulate_init_avr8();
void simulate_free_avr8(struct _simulate *simulate);
void simulate_push_avr8(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_avr8(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_avr8(struct _simulate *simulate, char *reg_string);
void simulate_reset_avr8(struct _simulate *simulate);
void simulate_dump_registers_avr8(struct _simulate *simulate);
int simulate_run_avr8(struct _simulate *simulate, int max_cycles, int step);

#endif

