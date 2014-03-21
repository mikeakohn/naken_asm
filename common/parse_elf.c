/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "parse_elf.h"
//#include "naken_util.h"

#ifndef DISABLE_ELF

typedef unsigned int (*get_int16_t)(FILE *);
typedef unsigned int (*get_int32_t)(FILE *);

static unsigned int get_int16_le(FILE *in)
{
unsigned int i;

  i = getc(in);
  i |= (getc(in)<<8);

  return i;
}

static unsigned int get_int32_le(FILE *in)
{
unsigned int i;

  i = getc(in);
  i |= (getc(in)<<8);
  i |= (getc(in)<<16);
  i |= (getc(in)<<24);

  return i;
}

static unsigned int get_int16_be(FILE *in)
{
unsigned int i;

  i = (getc(in)<<8);
  i |= getc(in);

  return i;
}

static unsigned int get_int32_be(FILE *in)
{
unsigned int i;

  i = (getc(in)<<24);
  i |= (getc(in)<<16);
  i |= (getc(in)<<8);
  i |= getc(in);

  return i;
}

int parse_elf(char *filename, struct _memory *memory, uint8_t *cpu_type)
{
FILE *in;
uint8_t e_ident[16];
int e_shoff;
int e_shentsize;
int e_shnum;
int e_shstrndx;
int n;
int start, end;
get_int16_t get_int16;
get_int32_t get_int32;

  memory_clear(memory);
  //memset(dirty, 0, memory->size);

  start = -1;
  end = -1;

  in = fopen(filename, "rb");
  if (in == 0)
  {
    return -1;
  }

  memset(e_ident, 0, 16);
  n = fread(e_ident, 1, 16, in);

  if (e_ident[0] != 0x7f || e_ident[1] != 'E' ||
      e_ident[2] != 'L' || e_ident[3] != 'F')
  {
    //printf("Not an ELF file.\n");
    fclose(in);
    return -2;
  }

  #define EI_CLASS 4   // 1=32 bit, 2=64 bit
  #define EI_DATA 5    // 1=little endian, 2=big endian
  #define EI_OSABI 7   // 0=SysV, 255=Embedded

  if (e_ident[EI_CLASS] != 1) // let's let other stuff in || e_ident[7]!=0xff)
  {
    printf("ELF Error: e_ident shows incorrect type\n");
    fclose(in);
    return -1;
  }

  // EI_DATA
  if (e_ident[EI_DATA] == 1)
  {
    memory->endian = ENDIAN_LITTLE;
    get_int16 = get_int16_le; 
    get_int32 = get_int32_le; 
  }
    else
  if (e_ident[EI_DATA] == 2)
  {
    memory->endian = ENDIAN_BIG;
    get_int16 = get_int16_be; 
    get_int32 = get_int32_be; 
  }
    else
  {
    printf("ELF Error: EI_DATA incorrect data encoding\n");
    fclose(in);
    return -1;
  }

  get_int16(in);
  n = get_int16(in);
/*
  if (n != 0x69)
  {
    printf("ELF Error: e_machine isn't set for MSP430\n");
    fclose(in);
    return -1;
  }
*/
  switch(n)
  {
    case 8:
    case 10:
      *cpu_type = CPU_TYPE_MIPS;
      break;
    case 40:
      *cpu_type = CPU_TYPE_ARM;
      break;
    case 105:
      *cpu_type = CPU_TYPE_MSP430;
      break;
    case 118:
      *cpu_type = CPU_TYPE_DSPIC;
      break;
    default:
      printf("ELF Error: e_machine unknown\n");
      fclose(in);
      return -1;
  }

  fseek(in, 32, SEEK_SET);
  e_shoff = get_int32(in);
  fseek(in, 46, SEEK_SET);
  e_shentsize = get_int16(in);
  e_shnum = get_int16(in);
  e_shstrndx = get_int16(in);

  //printf("e_shoff=%d\n", e_shoff);
  //printf("e_shentsize=%d\n", e_shentsize);
  //printf("e_shnum=%d\n", e_shnum);
  //printf("e_shstrndx=%d\n", e_shstrndx);

  fseek(in, e_shoff + (e_shstrndx * e_shentsize) + 16, SEEK_SET);
  int stroffset = get_int32(in);
  int sh_name;
  int sh_addr;
  int sh_offset;
  int sh_size;
  char name[20];
/*
  char text[20];
  char data[20];
  int text_count=0;
  int data_count=0;

  strcpy(text, ".text0");
  strcpy(data, ".data0");
*/

  for (n = 0; n < e_shnum; n++)
  {
    // FIXME - a little inefficient eh?
    fseek(in, e_shoff + (n * e_shentsize), SEEK_SET);
    sh_name = get_int32(in);
    get_int32(in);
    get_int32(in);
    sh_addr = get_int32(in);
    sh_offset = get_int32(in);
    sh_size = get_int32(in);

    fseek(in, stroffset + sh_name, SEEK_SET);
    int ptr = 0;
    while(ptr < 19)
    {
      int ch = getc(in);
      if (ch == 0 || ch == EOF) { break; }
      name[ptr++] = ch;
    }
    name[ptr] = 0;
    //printf("name=%s\n", name);
    // FIXME - Should check the flags instead
    int is_text = strncmp(name, ".text", 5) == 0 ? 1 : 0;
    if (is_text ||
        strncmp(name, ".data", 5) == 0 || strcmp(name, ".vectors") == 0)
    {
      if (is_text)
      {
        if (start == -1) { start = sh_addr; }
        else if (start > sh_addr) { start = sh_addr; }

        if (end == -1) { end = sh_addr + sh_size - 1; }
        else if (end < sh_addr + sh_size) { end = sh_addr+sh_size - 1; }
      }

      long marker = ftell(in);
      fseek(in, sh_offset, SEEK_SET);

      int n;
      for (n = 0; n < sh_size; n++)
      {
        if (sh_addr + n >= memory->size) break;
        memory_write_m(memory, sh_addr + n, getc(in)); 
        //dirty[sh_addr+n] = 1; 
      }

      fseek(in, marker, SEEK_SET);

      printf("Loaded %d %s bytes from 0x%04x\n", n, name, sh_addr);
    }
  }

  memory->low_address = start;
  memory->high_address = end;

  fclose(in);

  return start;
}

#endif


