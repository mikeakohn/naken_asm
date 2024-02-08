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

#ifndef NAKEN_ASM_FILE_IO_H
#define NAKEN_ASM_FILE_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

class FileIo
{
public:
  FileIo();
  ~FileIo();

  int open_for_writing(const char *filename);
  int open_for_reading(const char *filename);

  void set_fp(FILE *fp) { this->fp = fp; }
  void close_file();

  void set_endian(int value);

  int get_int8() { return getc(fp); }
  uint32_t get_int16() { return get_int16_func(fp); }
  uint32_t get_int32() { return get_int32_func(fp); }
  uint64_t get_int64() { return get_int64_func(fp); }

  void write_int8(uint32_t n)  { putc(n, fp); }
  void write_int16(uint32_t n) { write_int16_func(fp, n); }
  void write_int32(uint32_t n) { write_int32_func(fp, n); }
  void write_int64(uint64_t n) { write_int64_func(fp, n); }

  int get_bytes(uint8_t *data, int length)
  {
    return fread(data, 1, length, fp);
  }

  int get_chars(char *data, int length)
  {
    return fread(data, 1, length, fp);
  }

  int write_bytes(const uint8_t *data, int length)
  {
    return fwrite(data, 1, length, fp);
  }

  int write_chars(const char *data, int length)
  {
    return fwrite(data, 1, length, fp);
  }

  long tell() { return ftell(fp); }
  void set(long offset) { fseek(fp, offset, SEEK_SET); }
  void seek(long offset, int whence) { fseek(fp, offset, whence); }
  void skip(long offset) { fseek(fp, offset, SEEK_CUR); }

  int get_string_at_offset(char *data, int length, uint64_t offset);
  int get_bytes_at_offset(uint8_t *data, int length, uint64_t offset);

  int write_string(const char *data, bool null_terminate = true);

  enum
  {
    FILE_ENDIAN_LITTLE = 0,
    FILE_ENDIAN_BIG = 1
  };

private:
  FILE *fp;

  typedef uint32_t (*get_int16_t)(FILE *);
  typedef uint32_t (*get_int32_t)(FILE *);
  typedef uint64_t (*get_int64_t)(FILE *);

  typedef void (*write_int16_t)(FILE *, uint32_t);
  typedef void (*write_int32_t)(FILE *, uint32_t);
  typedef void (*write_int64_t)(FILE *, uint64_t);

  get_int16_t get_int16_func;
  get_int32_t get_int32_func;
  get_int64_t get_int64_func;

  write_int16_t write_int16_func;
  write_int32_t write_int32_func;
  write_int64_t write_int64_func;

  static uint32_t get_int16_le(FILE *in);
  static uint32_t get_int32_le(FILE *in);
  static uint64_t get_int64_le(FILE *in);

  static uint32_t get_int16_be(FILE *in);
  static uint32_t get_int32_be(FILE *in);
  static uint64_t get_int64_be(FILE *in);

  static void write_int16_le(FILE *out, uint32_t n);
  static void write_int32_le(FILE *out, uint32_t n);
  static void write_int64_le(FILE *out, uint64_t n);

  static void write_int16_be(FILE *out, uint32_t n);
  static void write_int32_be(FILE *out, uint32_t n);
  static void write_int64_be(FILE *out, uint64_t n);
};

#endif

