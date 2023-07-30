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

#ifndef NAKEN_ASM_SIMULATE_Z80_H
#define NAKEN_ASM_SIMULATE_Z80_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

#define REG_B 0
#define REG_C 1
#define REG_D 2
#define REG_E 3
#define REG_H 4
#define REG_L 5
#define REG_F 6
#define REG_A 7

typedef struct _simulate_z80
{
  uint8_t reg[8];
  uint16_t ix;
  uint16_t iy;
  uint16_t sp;
  uint16_t pc;
  //uint8_t status;
  uint8_t iff1;
  uint8_t iff2;
} SimulateZ80;

Simulate *simulate_init_z80(Memory *memory);
void simulate_free_z80(Simulate *simulate);
int simulate_dumpram_z80(Simulate *simulate, int start, int end);
void simulate_push_z80(Simulate *simulate, uint32_t value);
int simulate_set_reg_z80(Simulate *simulate, const char *reg_string, uint32_t value);
uint32_t simulate_get_reg_z80(Simulate *simulate, const char *reg_string);
void simulate_set_pc_z80(Simulate *simulate, uint32_t value);
void simulate_reset_z80(Simulate *simulate);
void simulate_dump_registers_z80(Simulate *simulate);
int simulate_run_z80(Simulate *simulate, int max_cycles, int step);

#endif

