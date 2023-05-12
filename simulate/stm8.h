/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, D.L. Karmann
 *
 * stm8 file by D.L. Karmann
 *
 */

#ifndef NAKEN_ASM_SIMULATE_STM8_H
#define NAKEN_ASM_SIMULATE_STM8_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_stm8
{
  uint8_t reg_a;
  uint16_t reg_x;
  uint16_t reg_y;
  uint16_t reg_sp;
  uint32_t reg_pc;
  uint8_t reg_cc;
};

struct _simulate * simulate_init_stm8();
void simulate_free_stm8(struct _simulate * simulate);
int simulate_dumpram_stm8(struct _simulate * simulate, int start, int end);
void simulate_push_stm8(struct _simulate * simulate, uint32_t value);
int simulate_set_reg_stm8(struct _simulate * simulate, char * reg_string, uint32_t value);
uint32_t simulate_get_reg_stm8(struct _simulate * simulate, char * reg_string);
void simulate_set_pc_stm8(struct _simulate * simulate, uint32_t value);
void simulate_reset_stm8(struct _simulate * simulate);
void simulate_dump_registers_stm8(struct _simulate * simulate);
int simulate_run_stm8(struct _simulate * simulate, int max_cycles, int step);

#endif

