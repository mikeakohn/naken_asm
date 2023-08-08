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

#ifndef NAKEN_ASM_SIMULATE_SIMULATE_H
#define NAKEN_ASM_SIMULATE_SIMULATE_H

#include <signal.h>
#include <unistd.h>

#include "common/memory.h"

class Simulate
{
public:
  Simulate(Memory *memory) :
    memory            (memory),
    cycle_count       (0),
    nested_call_count (0),
    usec              (1000000),
    break_point       (0xffffffff),
    break_io          (0),
    step_mode         (false),
    show              (true),
    auto_run          (true)
  {
    enable_signal_handler();
  }

  virtual ~Simulate()
  {
    disable_signal_handler();
  }

  //static Simulate *init(Memory *memory);

  virtual int dump_ram(int start, int end) = 0;
  virtual void push(uint32_t value) = 0;
  virtual int set_reg(const char *reg_string, uint32_t value) = 0;
  virtual uint32_t get_reg(const char *reg_string) = 0;
  virtual void set_pc(uint32_t value) = 0;
  virtual void reset() = 0;
  virtual void dump_registers() = 0;
  virtual int run(int max_cycles, int step) = 0;

  int get_break_point() { return break_point; }
  int get_delay() { return usec; }
  bool get_show() { return show; }

  void set_break_point(int value) { break_point = value; }
  void set_delay(useconds_t value) { usec = value; }
  void set_break_io(int value) { break_io = value; }

  void remove_break_point() { break_point = -1; }
  bool is_break_point_set() { return break_point == -1; }
  bool in_step_mode() { return usec == 0; }
  bool in_auto_run() { return auto_run == 0; }

  void disable_show() { show = false; }
  void enable_show() { show = true; }
  void enable_auto_run() { auto_run = true; }

  void disable_step_mode()
  {
    step_mode = false;
    //usec = 1;
  }

  void enable_step_mode()
  {
    step_mode = true;
    //usec = 0;
  }

protected:
  static bool stop_running;

  static void handle_signal(int sig);
  void enable_signal_handler();
  void disable_signal_handler();

  Memory *memory;
  int cycle_count;
  int nested_call_count;
  useconds_t usec;
  int break_point;
  int break_io;
  bool step_mode : 1;
  bool show : 1;
  bool auto_run : 1;
};

#endif

