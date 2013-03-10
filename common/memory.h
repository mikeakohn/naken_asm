/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _MEMORY_H
#define _MEMORY_H

#define PAGE_SIZE 8192

#define ENDIAN_LITTLE 0
#define ENDIAN_BIG 1

// TODO - Use this instead later
struct _memory_page
{
  int address;
  //int size;
  struct _memory_page *next;
  unsigned char bin[PAGE_SIZE];
  int debug_line[];
};

struct _memory
{
  struct _memory_page *pages;
  int low_address;
  int high_address;
  int endian;
  int size;
  int debug_flag;
};

struct _asm_context;

void memory_init(struct _memory *memory, int size, int debug_flag);
void memory_free(struct _memory *memory);
void memory_clear(struct _memory *memory);
unsigned char memory_read(struct _asm_context *asm_context, int address);
unsigned char memory_read_m(struct _memory *memory, int address);
unsigned char memory_read_inc(struct _asm_context *asm_context);
void memory_write(struct _asm_context *asm_context, int address, unsigned char data, int line);
void memory_write_inc(struct _asm_context *asm_context, unsigned char data, int line);
void memory_write_m(struct _memory *memory, int address, unsigned char data);
int memory_debug_line(struct _asm_context *asm_context, int address);
void memory_debug_line_set(struct _asm_context *asm_context, int address, int value);
int memory_debug_line_m(struct _memory *memory, int address);
void memory_debug_line_set_m(struct _memory *memory, int address, int value);

#endif

