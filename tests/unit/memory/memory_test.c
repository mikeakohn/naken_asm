#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/memory.h"

int main(int argc, char *argv[])
{
  struct _memory memory;
  int errors = 0;

  memory_init(&memory, 100, 0);

  memory.endian = ENDIAN_LITTLE;

  memory_write32_m(&memory, 32, 0x12345678);

  if (memory_read32_m(&memory, 32) != 0x12345678) { errors++; }
  if (memory_read16_m(&memory, 32) != 0x5678) { errors++; }

  if (memory_read_m(&memory, 32) != 0x78) { errors++; }
  if (memory_read_m(&memory, 33) != 0x56) { errors++; }
  if (memory_read_m(&memory, 34) != 0x34) { errors++; }
  if (memory_read_m(&memory, 35) != 0x12) { errors++; }

  memory_write16_m(&memory, 40, 0x4567);

  if (memory_read16_m(&memory, 40) != 0x4567) { errors++; }

  if (memory_read_m(&memory, 40) != 0x67) { errors++; }
  if (memory_read_m(&memory, 41) != 0x45) { errors++; }

  memory.endian = ENDIAN_BIG;

  memory_write32_m(&memory, 32, 0x12345678);

  if (memory_read32_m(&memory, 32) != 0x12345678) { errors++; }
  if (memory_read16_m(&memory, 32) != 0x1234) { errors++; }

  if (memory_read_m(&memory, 32) != 0x12) { errors++; }
  if (memory_read_m(&memory, 33) != 0x34) { errors++; }
  if (memory_read_m(&memory, 34) != 0x56) { errors++; }
  if (memory_read_m(&memory, 35) != 0x78) { errors++; }

  memory_write16_m(&memory, 40, 0x4567);

  if (memory_read16_m(&memory, 40) != 0x4567) { errors++; }

  if (memory_read_m(&memory, 40) != 0x45) { errors++; }
  if (memory_read_m(&memory, 41) != 0x67) { errors++; }

  memory_free(&memory);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


