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

#ifndef NAKEN_ASM_SIMULATE_AVR8_H
#define NAKEN_ASM_SIMULATE_AVR8_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

#define SREG_C 0
#define SREG_Z 1
#define SREG_N 2
#define SREG_V 3
#define SREG_S 4
#define SREG_H 5
#define SREG_T 6
#define SREG_I 7

#define RAM_MASK 0x1fff
#define RAM_SIZE (RAM_MASK + 1)

struct _simulate_avr8
{
  uint8_t reg[32];
  uint8_t ram[RAM_SIZE];
  uint8_t io[64];
  int sp_start;
  int pc;
  int sp;
  uint8_t sreg;
};

struct _simulate *simulate_init_avr8();
void simulate_free_avr8(struct _simulate *simulate);
int simulate_dumpram_avr8(struct _simulate *simulate, int start, int end);
void simulate_push_avr8(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_avr8(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_avr8(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_avr8(struct _simulate *simulate, uint32_t value);
void simulate_reset_avr8(struct _simulate *simulate);
void simulate_dump_registers_avr8(struct _simulate *simulate);
int simulate_run_avr8(struct _simulate *simulate, int max_cycles, int step);

#endif

