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

#ifndef _SIMULATE_MSP430_H
#define _SIMULATE_MSP430_H

#include <unistd.h>

#include "memory.h"
#include "simulate_common.h"

struct _simulate_msp430
{
  uint16_t reg[16];
};

struct _simulate *simulate_init_msp430();
void simulate_free_msp430(struct _simulate *simulate);
void simulate_push_msp430(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_msp430(struct _simulate *simulate, char *reg_string, uint32_t value);
unsigned int simulate_get_reg_msp430(struct _simulate *simulate, char *reg_string);
void simulate_reset_msp430(struct _simulate *simulate);
void simulate_dump_registers_msp430(struct _simulate *simulate);
int simulate_run_msp430(struct _simulate *simulate, int max_cycles, int step);

#endif

