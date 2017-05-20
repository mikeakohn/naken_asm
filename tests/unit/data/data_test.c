#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/memory.h"

#define LITTLE 0
#define BIG 1

int errors = 0;

struct _tests_int
{
  char *source;
  uint8_t answer[16];
  int length;
  int endian;
};

struct _tests_float
{
  char *source;
  float answer[2];
  int length;
  int endian;
};

struct _tests_int tests_int[] =
{
  { ".db 0x01, 0x02, 0x03, 0x04", { 0x01, 0x02, 0x03, 0x04 }, 4, LITTLE },
  { ".db 0x01, 0x02, 0x03, 0x04", { 0x01, 0x02, 0x03, 0x04 }, 4, BIG },
  { ".dc.b 0x01, 0x02, 0x03, 0x04", { 0x01, 0x02, 0x03, 0x04 }, 4, LITTLE },
  { ".dc.b 0x01, 0x02, 0x03, 0x04", { 0x01, 0x02, 0x03, 0x04 }, 4, BIG },
  { ".dw 0x0102, 0x0304", { 0x02, 0x01, 0x04, 0x03 }, 4, LITTLE },
  { ".dw 0x0102, 0x0304", { 0x01, 0x02, 0x03, 0x04 }, 4, BIG },
  { ".dc16 0x0102, 0x0304", { 0x02, 0x01, 0x04, 0x03 }, 4, LITTLE },
  { ".dc16 0x0102, 0x0304", { 0x01, 0x02, 0x03, 0x04 }, 4, BIG },
  { ".dc.w 0x0102, 0x0304", { 0x02, 0x01, 0x04, 0x03 }, 4, LITTLE },
  { ".dc.w 0x0102, 0x0304", { 0x01, 0x02, 0x03, 0x04 }, 4, BIG },
  { ".dl 0x01020304, 0x0304ffaa", { 0x04, 0x03, 0x02, 0x01, 0xaa, 0xff, 0x04, 0x03 }, 8, LITTLE },
  { ".dl 0x01020304, 0x0304ffaa", { 0x01, 0x02, 0x03, 0x04, 0x03, 0x04, 0xff, 0xaa }, 8, BIG },
  { ".dc32 0x01020304, 0x0304ffaa", { 0x04, 0x03, 0x02, 0x01, 0xaa, 0xff, 0x04, 0x03 }, 8, LITTLE },
  { ".dc32 0x01020304, 0x0304ffaa", { 0x01, 0x02, 0x03, 0x04, 0x03, 0x04, 0xff, 0xaa }, 8, BIG },
  { ".dc.l 0x01020304, 0x0304ffaa", { 0x04, 0x03, 0x02, 0x01, 0xaa, 0xff, 0x04, 0x03 }, 8, LITTLE },
  { ".dc.l 0x01020304, 0x0304ffaa", { 0x01, 0x02, 0x03, 0x04, 0x03, 0x04, 0xff, 0xaa }, 8, BIG },
  { ".dc64 0x0102030405060708, 0x0304ffaa", { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0xaa, 0xff, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00 }, 16, LITTLE },
  { ".dc64 0x0102030405060708, 0x0304ffaa", { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0xff, 0xaa }, 16, BIG },
  { NULL }
};

struct _tests_float tests_float[] =
{
  { ".dq 1.2, -1.2", { 1.2, -1.2 }, 16, LITTLE },
  { ".dq 1.2, -1.2", { 1.2, -1.2 }, 16, BIG },
  { NULL }
};

void test_int(const char *source, uint8_t *answer, int length, int endian)
{
  struct _asm_context asm_context = { 0 };
  int oops = 0;
  int i;

  printf("Testing: %s (%s) ... ", source, endian == LITTLE ? "little":"big");

  tokens_open_buffer(&asm_context, source);
  tokens_reset(&asm_context);

  if (endian == BIG)
  {
    asm_context.memory.endian = 1;
  }

  assemble(&asm_context);

  for (i = 0; i < length; i++)
  {
    if (memory_read_m(&asm_context.memory, i) != answer[i])
    {
      oops++;
    }
  }

  if (oops != 0)
  {
    printf("FAIL\n");
    errors++;
  }
    else
  {
    printf("PASS\n");
  }

  tokens_close(&asm_context);
  assembler_free(&asm_context);
}

void test_float(const char *source, float *answer, int length, int endian)
{
  struct _asm_context asm_context = { 0 };
  int oops = 0;
  int i;

  printf("Testing: %s (%s) ... ", source, endian == LITTLE ? "little":"big");

  tokens_open_buffer(&asm_context, source);
  tokens_reset(&asm_context);

  if (endian == BIG)
  {
    asm_context.memory.endian = 1;
  }

  assemble(&asm_context);

  uint32_t f = 0;

  for (i = 0; i < length * 4; i++)
  {
    if ((i % 4) == 0) { f = 0; }

    // printf("%d) %02x\n", i, memory_read_m(&asm_context.memory, i));
    if (endian == LITTLE)
    {
      f = (f >> 8) | (memory_read_m(&asm_context.memory, i) << 24);
    }
      else
    {
      f = (f << 8) | memory_read_m(&asm_context.memory, i);
    }

    if ((i % 4) == 3)
    {
#if 0
      float a = f & 0x7fffff;
      uint8_t e = ((f >> 23) & 0xff);
      if ((f & 0x80000000) != 0) { a = -a; }
#endif
      union
      {
        float f;
        uint32_t i;
      } data;

      data.i = f;

      if (data.f != answer[i / 4])
      {
        printf("[%f!=%f] ", data.f, answer[i / 4]);
        oops++;
      }
    }
  }

  if (oops != 0)
  {
    printf("FAIL\n");
    errors++;
  }
    else
  {
    printf("PASS\n");
  }

  tokens_close(&asm_context);
  assembler_free(&asm_context);
}

int main(int argc, char *argv[])
{
  int n;

  printf("directives_data.o test\n");

  n = 0;
  while(1)
  {
    if (tests_int[n].source == NULL) { break; }
    test_int(tests_int[n].source, tests_int[n].answer, tests_int[n].length, tests_int[n].endian);
    n++;
  }

#if 0
  // This test is a bit invalid.  Floats can differ depending on platform.

  n = 0;
  while(1)
  {
    if (tests_float[n].source == NULL) { break; }
    test_float(tests_float[n].source, tests_float[n].answer, tests_float[n].length, tests_float[n].endian);
    n++;
  }
#endif

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


