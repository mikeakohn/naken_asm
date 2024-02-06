/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FileIo.h"

FileIo::FileIo() : fp(NULL)
{
  set_endian(false);
}

FileIo::~FileIo()
{
  close_file();
}

int FileIo::open_for_writing(const char *filename)
{
  fp = fopen(filename, "wb");
  if (fp == NULL) { return -1; }

  return 0;
}

int FileIo::open_for_reading(const char *filename)
{
  fp = fopen(filename, "rb");
  if (fp == NULL) { return -1; }

  return 0;
}

void FileIo::close_file()
{
  if (fp != NULL) { fclose(fp); }
  fp = NULL;
}

void FileIo::set_endian(int value)
{
  if (value == FILE_ENDIAN_BIG)
  {
    get_int16_func = get_int16_be;
    get_int32_func = get_int32_be;
    get_int64_func = get_int64_be;

    write_int16_func = write_int16_be;
    write_int32_func = write_int32_be;
    write_int64_func = write_int64_be;
  }
    else
  {
    get_int16_func = get_int16_le;
    get_int32_func = get_int32_le;
    get_int64_func = get_int64_le;

    write_int16_func = write_int16_le;
    write_int32_func = write_int32_le;
    write_int64_func = write_int64_le;
  }
}

int FileIo::get_string_at_offset(char *data, int length, uint64_t offset)
{
  long marker = ftell(fp);
  int ptr = 0;

  fseek(fp, offset, SEEK_SET);

  while (true)
  {
    int ch = getc(fp);
    if (ch == 0) { break; }
    data[ptr++] = ch;
    if (ptr == length - 1) { break; }
  }

  data[ptr] = 0;

  fseek(fp, marker, SEEK_SET);

  return 0;
} 

int FileIo::get_bytes_at_offset(uint8_t *data, int length, uint64_t offset)
{
  long marker = ftell(fp);

  fseek(fp, offset, SEEK_SET);
  int n = fread(data, 1, length, fp);
  fseek(fp, marker, SEEK_SET);

  return n == length ? 0 : 1;
} 

uint32_t FileIo::get_int16_le(FILE *in)
{
  uint32_t i;

  i  = getc(in);
  i |= (getc(in) << 8);

  return i;
}

uint32_t FileIo::get_int32_le(FILE *in)
{
  uint32_t i;

  i  =  getc(in);
  i |= (getc(in) << 8);
  i |= (getc(in) << 16);
  i |= (getc(in) << 24);

  return i;
}

uint64_t FileIo::get_int64_le(FILE *in)
{
  uint64_t i;

  i  =  (uint64_t)getc(in);
  i |= ((uint64_t)getc(in) << 8);
  i |= ((uint64_t)getc(in) << 16);
  i |= ((uint64_t)getc(in) << 24);
  i |= ((uint64_t)getc(in) << 32);
  i |= ((uint64_t)getc(in) << 40);
  i |= ((uint64_t)getc(in) << 48);
  i |= ((uint64_t)getc(in) << 56);

  return i;
}

uint32_t FileIo::get_int16_be(FILE *in)
{
  uint32_t i;

  i = (getc(in) << 8);
  i |= getc(in);

  return i;
}

uint32_t FileIo::get_int32_be(FILE *in)
{
  uint32_t i;

  i =  (getc(in) << 24);
  i |= (getc(in) << 16);
  i |= (getc(in) << 8);
  i |=  getc(in);

  return i;
}

uint64_t FileIo::get_int64_be(FILE *in)
{
  uint32_t i;

  i =  ((uint64_t)getc(in) << 56);
  i |= ((uint64_t)getc(in) << 48);
  i |= ((uint64_t)getc(in) << 40);
  i |= ((uint64_t)getc(in) << 32);
  i |= ((uint64_t)getc(in) << 24);
  i |= ((uint64_t)getc(in) << 16);
  i |= ((uint64_t)getc(in) << 8);
  i |=  (uint64_t)getc(in);

  return i;
}

void FileIo::write_int16_le(FILE *out, uint32_t n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
}

void FileIo::write_int32_le(FILE *out, uint32_t n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 24) & 0xff, out);
}

void FileIo::write_int64_le(FILE *out, uint64_t n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 24) & 0xff, out);
  putc((n >> 32) & 0xff, out);
  putc((n >> 40) & 0xff, out);
  putc((n >> 48) & 0xff, out);
  putc((n >> 56) & 0xff, out);
}

void FileIo::write_int16_be(FILE *out, uint32_t n)
{
  putc((n >> 8) & 0xff, out);
  putc(n & 0xff, out);
}

void FileIo::write_int32_be(FILE *out, uint32_t n)
{
  putc((n >> 24) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc(n & 0xff, out);
}

void FileIo::write_int64_be(FILE *out, uint64_t n)
{
  putc((n >> 56) & 0xff, out);
  putc((n >> 48) & 0xff, out);
  putc((n >> 40) & 0xff, out);
  putc((n >> 32) & 0xff, out);
  putc((n >> 24) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc(n & 0xff, out);
}

