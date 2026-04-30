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

#ifndef UTIL_CONTEXT_H
#define UTIL_CONTEXT_H

#include "common/Range.h"
#include "common/String.h"
#include "core/cpu_list.h"
#include "core/Memory.h"
#include "core/Symbols.h"
#include "simulate/msp430.h"

class UtilContext
{
public:
  UtilContext();
  ~UtilContext();

  static int is_supported_cpu(const char *name);

  int set_cpu_by_type(uint8_t cpu_type);
  int set_cpu_by_name(const char *name);

  void disasm(const char *token);
  void disasm(uint32_t start, uint32_t end);

  void sim_show_info();
  int sim_set_register(String &arg);
  int sim_clear_flag(String &arg);
  int sim_set_speed(String &arg);
  int sim_stack_push(String &arg);
  int sim_set_breakpoint(String &arg);

  void print8(const char *token);
  void print16(const char *token);
  void print32(const char *token);

  void write8(const char *token);
  void write16(const char *token);
  void write32(const char *token);

  const char *get_address(const char *token, uint32_t *address);

  static const char *get_num(const char *token, uint32_t *num);

  bool get_range(const char *text, Range &range);

  // Converts text passed in on the command line to a start / end int.
  int get_range(const char *text, uint32_t *start, uint32_t *end);

  Memory memory;
  Symbols symbols;
  Simulate *simulate;
  const char *cpu_name;
  uint32_t flags;
  uint8_t bytes_per_address;
  uint8_t alignment;
  bool allow_unknown_cpu : 1;
  disasm_range_t disasm_range;

private:
  void copy_cpu_info(CpuList *cpu_info);
  static const char *get_hex(const char *token, uint32_t *num);
  static const char *get_token(String &value, const char *source);
};

#endif

