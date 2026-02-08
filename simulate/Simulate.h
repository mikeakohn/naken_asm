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

#ifndef NAKEN_ASM_SIMULATE_SIMULATE_H
#define NAKEN_ASM_SIMULATE_SIMULATE_H

#include <signal.h>
#include <unistd.h>

#include "common/Memory.h"

class Simulate
{
public:
  Simulate(Memory *memory);
  virtual ~Simulate();

  //static Simulate *init(Memory *memory);

  virtual void reset() = 0;
  virtual void push(uint32_t value) = 0;
  virtual int set_reg(const char *reg_string, uint32_t value) = 0;
  virtual uint32_t get_reg(const char *reg_string) = 0;
  virtual void set_pc(uint32_t value) = 0;
  virtual void dump_registers() = 0;
  virtual int run(int max_cycles, int step) = 0;

  // For chips that don't have RAM in the same address space as
  // instruction memory.
  virtual int dump_ram(int start, int end);

  void set_org(uint32_t value) { org = value; }
  int get_break_point() { return break_point; }
  int get_delay() { return usec; }
  bool get_show() { return show; }

  void set_break_point(int value)  { break_point = value; }
  void set_delay(useconds_t value) { usec = value; }
  void set_break_io(int value)     { break_io = value; }

  void remove_break_point() { break_point = -1; }
  bool is_break_point_set() { return break_point == -1; }
  bool in_step_mode()       { return usec == 0; }
  bool in_auto_run()        { return auto_run == 0; }

  void disable_show()    { show = false; }
  void enable_show()     { show = true; }
  void enable_auto_run() { auto_run = true; }

  void disable_step_mode()
  {
    step_mode = false;
    //usec = 1;
  }

  void enable_step_mode()
  {
    auto_run = false;
    step_mode = true;
    disable_signal_handler();
    //usec = 0;
  }

  void init_serial(
    uint32_t address,
    const char *in_name,
    const char *out_name);

protected:
  static bool stop_running;

  static void handle_signal(int sig);
  void enable_signal_handler();
  void disable_signal_handler();

  void serial_write8(uint8_t data);
  void serial_write16(uint16_t data);
  void serial_write32(uint32_t data);

  uint8_t serial_read8();
  uint16_t serial_read16();
  uint32_t serial_read32();

  void ram_write8(uint32_t address, uint8_t data)
  {
    memory->write8(address, data);

    if (serial_out != nullptr && address == serial_address)
    {
      serial_write8(data);
    }

    if (address == break_io)
    {
      exit(data);
    }
  }

  void ram_write16(uint32_t address, uint16_t data)
  {
    memory->write16(address, data);

    if (serial_out != nullptr && address == serial_address)
    {
      serial_write16(data);
    }
  }

  void ram_write32(uint32_t address, uint32_t data)
  {
    memory->write32(address, data);

    if (serial_out != nullptr && address == serial_address)
    {
      serial_write32(data);
    }
  }

  uint8_t ram_read8(uint32_t address)
  {
    uint8_t data = memory->read8(address);

    if (serial_in != nullptr && address == serial_address)
    {
      data = serial_read8();
    }

    return data;
  }

  uint16_t ram_read16(uint32_t address)
  {
    uint16_t data = memory->read16(address);

    if (serial_in != nullptr && address == serial_address)
    {
      data = serial_read16();
    }

    return data;
  }

  uint32_t ram_read32(uint32_t address)
  {
    uint32_t data = memory->read32(address);

    if (serial_in != nullptr && address == serial_address)
    {
      data = serial_read32();
    }

    return data;
  }

  Memory *memory;
  uint32_t org;
  int cycle_count;
  int nested_call_count;
  useconds_t usec;
  int break_point;
  uint32_t break_io;

  bool step_mode : 1;
  bool show      : 1;
  bool auto_run  : 1;

  FILE *serial_in;
  FILE *serial_out;
  uint32_t serial_address;
};

#endif

