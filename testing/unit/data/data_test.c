#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "memory.h"

int errors = 0;

void test(const char *data, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
  struct _asm_context asm_context = { 0 };
  int i;

  printf("Testing: %s ... ", data);

  tokens_open_buffer(&asm_context, data);
  tokens_reset(&asm_context);

  assemble(&asm_context);

  for (i = 0; i < 4; i++)
  {
    printf("%u\n", memory_read_m(&asm_context.memory, i));
  }

  tokens_close(&asm_context);
}

int main(int argc, char *argv[])
{
  printf("directives_data.o test\n");

  test(".db 0x40, 0x60, 0x70, 0x80\n", 0x40, 0x60, 0x70, 0x80);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


