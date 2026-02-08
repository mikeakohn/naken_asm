/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_SIMULATE_MSP430_H
#define NAKEN_ASM_SIMULATE_MSP430_H

#include <unistd.h>

#include "simulate/Simulate.h"

class SimulateMsp430 : public Simulate
{
public:
  SimulateMsp430(Memory *memory);
  virtual ~SimulateMsp430();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  void sp_inc(int *sp);
  uint16_t get_data(int reg_index, int As, int bw, int &ea);
  void update_reg(int reg_index, int mode, int bw);
  int put_data(int ea, int reg_index, int mode, int bw, uint32_t data);
  int one_operand_exe(uint16_t opcode);
  int relative_jump_exe(uint16_t opcode);
  int two_operand_exe(uint16_t opcode);

  //         8   7     6     5       4   3 2 1 0
  // Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C

  enum Flag
  {
    FLAG_V      = 0x100,
    FLAG_SCG1   = 0x80,
    FLAG_SCG0   = 0x40,
    FLAG_OSCOFF = 0x20,
    FLAG_CPUOFF = 0x10,
    FLAG_GIE    = 0x08,
    FLAG_N      = 0x04,
    FLAG_Z      = 0x02,
    FLAG_C      = 0x01
  };

  enum Bw
  {
    BW_WORD = 0,
    BW_BYTE = 1
  };

#if 0
  void set_flag(Flag flag)
  {
    reg[2] |= flag;
  }

  void clear_flag(Flag flag)
  {
    reg[2] &= flag ^ 0xffff;
  }
#endif

  int get_v()      { return (reg[2] & FLAG_V)      == 0 ? 0 : 1; }
  int get_scg1()   { return (reg[2] & FLAG_SCG1)   == 0 ? 0 : 1; }
  int get_scg0()   { return (reg[2] & FLAG_SCG0)   == 0 ? 0 : 1; }
  int get_oscoff() { return (reg[2] & FLAG_OSCOFF) == 0 ? 0 : 1; }
  int get_cpuoff() { return (reg[2] & FLAG_CPUOFF) == 0 ? 0 : 1; }
  int get_gie()    { return (reg[2] & FLAG_GIE)    == 0 ? 0 : 1; }
  int get_n()      { return (reg[2] & FLAG_N)      == 0 ? 0 : 1; }
  int get_z()      { return (reg[2] & FLAG_Z)      == 0 ? 0 : 1; }
  int get_c()      { return (reg[2] & FLAG_C)      == 0 ? 0 : 1; }

  void clear_v()      { reg[2] &= (0xffff ^ FLAG_V);      }
  void clear_scg1()   { reg[2] &= (0xffff ^ FLAG_SCG1);   }
  void clear_scg0()   { reg[2] &= (0xffff ^ FLAG_SCG0);   }
  void clear_oscoff() { reg[2] &= (0xffff ^ FLAG_OSCOFF); }
  void clear_cpuoff() { reg[2] &= (0xffff ^ FLAG_CPUOFF); }
  void clear_gie()    { reg[2] &= (0xffff ^ FLAG_GIE);    }
  void clear_n()      { reg[2] &= (0xffff ^ FLAG_N);      }
  void clear_z()      { reg[2] &= (0xffff ^ FLAG_Z);      }
  void clear_c()      { reg[2] &= (0xffff ^ FLAG_C);      }

  void set_v()      { reg[2] |= FLAG_V;      }
  void set_scg1()   { reg[2] |= FLAG_SCG1;   }
  void set_scg0()   { reg[2] |= FLAG_SCG0;   }
  void set_oscoff() { reg[2] |= FLAG_OSCOFF; }
  void set_cpuoff() { reg[2] |= FLAG_CPUOFF; }
  void set_gie()    { reg[2] |= FLAG_GIE;    }
  void set_n()      { reg[2] |= FLAG_N;      }
  void set_z()      { reg[2] |= FLAG_Z;      }
  void set_c()      { reg[2] |= FLAG_C;      }

  void update_nz(int value, int bw)
  {
    if (bw == BW_WORD)
    {
      if (value & 0x8000) { set_n(); } else { clear_n(); }
      if ((value & 0xffff) == 0) { set_z(); } else { clear_z(); }
    }
      else
    {
      if (value & 0x80) { set_n(); } else { clear_n(); }
      if ((value & 0xff) == 0) { set_z(); } else { clear_z(); }
    }
  }

  void update_c(int value, int bw)
  {
    if (bw == BW_WORD)
    {
      if ((value & 0xffff0000) == 0) { clear_c(); } else { set_c(); }
    }
      else
    {
      if ((value & 0xffffff00) == 0) { clear_c(); } else { set_c(); }
    }
  }

  void update_v(int dst, int src, int result, int bw)
  {
    if (bw == BW_WORD)
    {
      uint16_t d = ((uint16_t)dst) & 0x8000;
      uint16_t s = ((uint16_t)src) & 0x8000;
      uint16_t r = ((uint16_t)result) & 0x8000;

      if (d == s && r != d) { set_v(); } else { clear_v(); }
    }
      else
    {
      uint8_t d = ((uint8_t)dst) & 0x80;
      uint8_t s = ((uint8_t)src) & 0x80;
      uint8_t r = ((uint8_t)result) & 0x80;

      if (d == s && r != d) { set_v(); } else { clear_v(); }
    }
  }

#if 0
  void update_z(int result, int bw)
  {
    if (bw == BW_WORD)
    {
      if (result & 0x8000) { set_n(); } else { clear_n(); }
    }
      else
    {
      if (result & 0x80) { set_n(); } else { clear_n(); }
    }
  }
#endif

  uint16_t reg[16];
  static const char *flags[];
};

#endif

