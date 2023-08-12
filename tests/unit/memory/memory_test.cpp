#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/Memory.h"
#include "common/MemoryPage.h"

int test_Memory()
{
  Memory memory;
  int errors = 0;

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

  return errors;
}

int test_MemoryPage()
{
  MemoryPage memory_page(PAGE_SIZE + 100);

  int errors = 0;

  if (memory_page.address != PAGE_SIZE)
  {
    fprintf(stderr, "Error: address isn't on page boundary.\n");
    errors += 1;
  }

  memory_page.set_data(PAGE_SIZE + 5, 100);
  memory_page.set_data(PAGE_SIZE + 10, 110);

  if (memory_page.offset_min != 5)
  {
    fprintf(stderr, "Error: offset_min %s:%d\n", __FILE__, __LINE__);
    errors += 1;
  }

  if (memory_page.offset_max != 10)
  {
    fprintf(stderr, "Error: offset_max %s:%d\n", __FILE__, __LINE__);
    errors += 1;
  }

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  errors += test_Memory();
  errors += test_MemoryPage();

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}

