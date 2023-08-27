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

#ifndef NAKEN_ASM_SIMULATE_8008_H
#define NAKEN_ASM_SIMULATE_8008_H

#include <unistd.h>

#include "simulate/Simulate.h"

class Simulate8008 : public Simulate
{
public:
  Simulate8008(Memory *memory);
  virtual ~Simulate8008();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int execute_instruction(uint8_t opcode);

  void push(uint16_t value)
  {
    stack[sp++] = value;
    sp &= 7;
  }

  uint16_t pop()
  {
    uint16_t value = stack[--sp];
    sp &= 7;
    return value;
  }

  void set_parity(uint8_t value);
  void alu(uint8_t operation, uint8_t s);

  uint16_t pc;
  uint16_t sp;

  struct
  {
    uint8_t p : 1;
    uint8_t s : 1;
    uint8_t c : 1;
    uint8_t z : 1;
  } flags;

  uint8_t reg[8];
  uint16_t stack[8];
};

#endif

