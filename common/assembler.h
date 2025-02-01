/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_ASSEMBLER_H
#define NAKEN_ASM_ASSEMBLER_H

#include <stdio.h>

#include "common/cpu_list.h"
#include "common/Linker.h"
#include "common/Macros.h"
#include "common/Memory.h"
#include "common/print_error.h"
#include "common/Symbols.h"
#include "common/tokens.h"

//#define TOKENLEN 512
#define PARAM_STACK_LEN 4096
#define INCLUDE_PATH_LEN 4096

//#define DL_EMPTY -1
//#define DL_DATA -2
//#define DL_NO_CG -3

#define SEGMENT_CODE 0
#define SEGMENT_BSS 1

class AsmContext
{
public:
  AsmContext();
  ~AsmContext();

  void init();
  void print_info(FILE *out);
  void set_cpu(int index);
  int set_cpu(const char *name);

  void set_org(uint32_t value) { address = value * bytes_per_address; }
  //uint32_t get_low_address()  { return memory.low_address / bytes_per_address; }
  //uint32_t get_high_address() { return memory.high_address / bytes_per_address; }

  int read_debug(uint32_t address)
  {
    return memory.read_debug(address);
  }

  void write_debug(uint32_t address, int data)
  {
    memory.write_debug(address, data);
  }

  uint8_t memory_read(uint32_t address) { return memory.read8(address); }

  void memory_write(uint32_t address, uint8_t data)
  {
    memory.write8(address, data);
  }

  void memory_write(uint32_t address, uint8_t data, int line)
  {
    memory.write(address, data, line);
  }

  void memory_write_inc(uint8_t data, int line)
  {
    memory.write(address++, data, line);
  }

  Memory memory;
  Tokens tokens;
  Symbols symbols;
  Macros macros;
  parse_instruction_t parse_instruction;
  parse_directive_t parse_directive;
  link_function_t link_function;
  list_output_t list_output;
  FILE *list;
  int address;
  int segment;
  int pass;
  int instruction_count;
  int data_count;
  int code_count;
  int error_count;
  int ifdef_count;
  int parsing_ifdef;
  Linker *linker;
  char def_param_stack_data[PARAM_STACK_LEN];
  int def_param_stack_ptr[MAX_NESTED_MACROS + 1];
  int def_param_stack_count;
  char include_path[INCLUDE_PATH_LEN];
  int cpu_list_index;
  uint8_t cpu_type;
  uint8_t bytes_per_address;
  bool is_dollar_hex          : 1;
  bool strings_have_dots      : 1;
  bool strings_have_slashes   : 1;
  bool can_tick_end_string    : 1;
  bool numbers_dont_have_dots : 1;
  bool quiet_output           : 1;
  bool error                  : 1;
  bool msp430_cpu4            : 1;
  bool ignore_symbols         : 1;
  bool pass_1_write_disable   : 1;
  bool write_list_file        : 1;
  bool dump_symbols           : 1;
  bool dump_macros            : 1;
  bool optimize               : 1;
  bool ignore_number_postfix  : 1;
  bool in_repeat              : 1;
  uint32_t flags;
  uint32_t extra_context;
};

int assembler_directive(AsmContext *asm_context, char *token);
int assembler_link_file(AsmContext *asm_context, const char *filename);
int assembler_link(AsmContext *asm_context);
int assemble(AsmContext *asm_context);

#endif

