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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "memory.h"

void memory_init(struct _memory *memory, int size, int debug_flag)
{
  memory->low_address=size-1;
  memory->high_address=0;
  memory->endian=ENDIAN_LITTLE;
  memory->size=size;
  memory->debug_flag=debug_flag;
  memory->pages=NULL;
  //memset(memory->debug_line, 0xff, sizeof(int)*memory->size);
}

void memory_free(struct _memory *memory)
{
struct _memory_page *page;
struct _memory_page *next;

  page=memory->pages;
  while(page!=NULL)
  {
    next=page->next;
    free(page);
    page=next; 
  }

  memory->pages=NULL;
}

void memory_clear(struct _memory *memory)
{
struct _memory_page *page;
struct _memory_page *next;

  page=memory->pages;
  while(page!=NULL)
  {
    next=page->next;
    memset(page->bin, 0, PAGE_SIZE);
    //if (memory->debug==1)
    //{
    //  memset(page->debug, 0, PAGE_SIZE*sizeof(int));
    //}
    page=next; 
  }
}

static unsigned char read_byte(struct _memory *memory, int address)
{
struct _memory_page *page;

  page=memory->pages;
  while(page!=NULL)
  {
    if (address>=page->address && address<page->address+PAGE_SIZE)
    {
      return page->bin[address-page->address];
    }
    page=page->next;
  }

  return 0;
}

static int read_debug(struct _memory *memory, int address)
{
struct _memory_page *page;

  if (memory->debug_flag==0) return -1;

  page=memory->pages;
  while(page!=NULL)
  {
    if (address>=page->address && address<page->address+PAGE_SIZE)
    {
      return page->debug_line[address-page->address];
    }
    page=page->next;
  }

  return -1;
}

static struct _memory_page *alloc_page(struct _memory *memory, int address)
{
struct _memory_page *page;

//printf("allocating page %d\n", address);

  page=malloc(sizeof(struct _memory_page)+(memory->debug_flag==1?PAGE_SIZE*sizeof(int):0));
  page->address=(address/PAGE_SIZE)*PAGE_SIZE;
  page->next=0;

  memset(page->bin, 0, PAGE_SIZE);

  if (memory->debug_flag==1)
  {
    memset(page->debug_line, 0xff, PAGE_SIZE*sizeof(int));
  }

  return page;
}

static void write_byte(struct _memory *memory, int address, unsigned char data)
{
struct _memory_page *page;

  if (memory->pages==NULL) { memory->pages=alloc_page(memory, address); }

  page=memory->pages;
  while(page!=NULL)
  {
    if (address>=page->address && address<page->address+PAGE_SIZE)
    {
      break;
    }

    if (page->next==NULL)
    {
      page->next=alloc_page(memory, address);
    }

    page=page->next;
  }

  if (memory->low_address>address) memory->low_address=address;
  if (memory->high_address<address) memory->high_address=address;

  page->bin[address-page->address]=data;
}

static void write_debug(struct _memory *memory, int address, int data)
{
struct _memory_page *page;

  if (memory->pages==NULL) { memory->pages=alloc_page(memory, address); }

  page=memory->pages;
  while(page!=NULL)
  {
    if (address>=page->address && address<page->address+PAGE_SIZE)
    {
      break;
    }

    if (page->next==NULL)
    {
      page->next=alloc_page(memory, address);
    }

    page=page->next;
  }

  page->debug_line[address-page->address]=data;
}

unsigned char memory_read(struct _asm_context *asm_context, int address)
{
  if (address>=asm_context->memory.size)
  {
    printf("Warning: Data read address %d overran %d byte boundary at %s:%d\n", address, asm_context->memory.size, asm_context->filename, asm_context->line);
    return 0;
  }

  return read_byte(&asm_context->memory, address);
}

unsigned char memory_read_inc(struct _asm_context *asm_context)
{
  return read_byte(&asm_context->memory, asm_context->address++);
}

unsigned char memory_read_m(struct _memory *memory, int address)
{
  return read_byte(memory, address);
}

void memory_write(struct _asm_context *asm_context, int address, unsigned char data, int line)
{
  if (address>=asm_context->memory.size)
  {
    printf("Warning: Data write address %d overran %d byte boundary at %s:%d\n", address, asm_context->memory.size, asm_context->filename, asm_context->line);
    return;
  }

  write_byte(&asm_context->memory, address, data);
  write_debug(&asm_context->memory, address, line);
}

void memory_write_inc(struct _asm_context *asm_context, unsigned char data, int line)
{
  write_byte(&asm_context->memory, asm_context->address, data);
  write_debug(&asm_context->memory, asm_context->address, line);
  asm_context->address++;
}

void memory_write_m(struct _memory *memory, int address, unsigned char data)
{
  write_byte(memory, address, data);
}

int memory_debug_line(struct _asm_context *asm_context, int address)
{
  return read_debug(&asm_context->memory, address);
}

void memory_debug_line_set(struct _asm_context *asm_context, int address, int value)
{
  write_debug(&asm_context->memory, address, value);
}

int memory_debug_line_m(struct _memory *memory, int address)
{
  return read_debug(memory, address);
}

void memory_debug_line_set_m(struct _memory *memory, int address, int value)
{
  write_debug(memory, address, value);
}



