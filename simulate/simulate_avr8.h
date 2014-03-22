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

struct _simulate_avr8
{
  uint8_t reg[32];
  int sp;
  int sp_start;
  int pc;
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

