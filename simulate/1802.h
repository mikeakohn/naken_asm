/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 * 1802 file by Malik Enes Safak
 *
 */

#ifndef NAKEN_ASM_SIMULATE_1802_H
#define NAKEN_ASM_SIMULATE_1802_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_1802
{
  // Define registers and anything 1802 specific here
  uint8_t reg_d, reg_p, reg_x, reg_t, reg_n, reg_i, reg_b;
  uint16_t reg_r[16];
  uint8_t reg_cntr, reg_cn;
  uint8_t flag_df, flag_q, flag_mie, flag_cie, flag_xie, flag_cil, flag_etq;
};

struct _simulate *simulate_init_1802(struct _memory *memory);
void simulate_free_1802(struct _simulate *simulate);
int simulate_dumpram_1802(struct _simulate *simulate, int start, int end);
void simulate_push_1802(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_1802(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_1802(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_1802(struct _simulate *simulate, uint32_t value);
void simulate_reset_1802(struct _simulate *simulate);
void simulate_dump_registers_1802(struct _simulate *simulate);
int simulate_run_1802(struct _simulate *simulate, int max_cycles, int step);

#endif

