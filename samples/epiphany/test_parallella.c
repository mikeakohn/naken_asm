#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <e-hal.h>
#include <e-loader.h>

int main(int argc, char *argv[])
{
  e_platform_t platform;
  e_epiphany_t dev;
  e_mem_t ext_mem;
  uint32_t data, address;
  int n;

  e_init(NULL);
  e_reset_system();
  e_get_platform_info(&platform);

  // Dump information about the Parallella board.
  printf("\n------ info --------\n");
  printf("platform.version:  %s\n", platform.version);
  printf("platform.row:      %d\n", platform.row);
  printf("platform.col:      %d\n", platform.col);
  printf("platform.rows:     %d\n", platform.rows);
  printf("platform.cols:     %d\n", platform.cols);
  printf("platform.num_chips:%d\n", platform.num_chips);
  printf("platform.num_emems:%d\n", platform.num_emems);
  printf("\n");

  // Open a link to the Epiphany and load it with epiphany.asm and
  // signal the core to start running.
  n = e_open(&dev, 0, 0, 1, 1);
  printf("e_open() return value is %d\n", n);

  n = e_load("epiphany.srec", &dev, 0, 0, E_FALSE);
  printf("e_load() return value is %d\n", n);

  n = e_start(&dev, 0, 0);
  printf("e_start() return value is %d\n", n);

  // Get a pointer to the first 4 bytes in external (shared) memory.
  n = e_alloc(&ext_mem, 0, 4);
  printf("e_alloc() return value is %d\n", n);

  // Read 4 bytes from external memory.
  n = e_read(&ext_mem, 0, 0, 0, &address, 4);
  printf("e_read(ext_mem) return value is %d address is 0x%04x\n", n, address);

  // Read 1 word from a memory address that the Epiphany is constantly
  // updating, wait a second, and read again.
  n = e_read(&dev, 0, 0, 0x6000, &data, 4);
  printf("data=0x%x n=%d\n", data, n);

  sleep(1);

  n = e_read(&dev, 0, 0, 0x6000, &data, 4);
  printf("data=0x%x n=%d\n", data, n);

  n = e_read(&dev, 0, 0, 0x6004, &address, 4);
  printf("address=0x%x n=%d\n", address, n);

  n = e_read(&dev, 0, 0, 0x6004, &address, 4);
  printf("external=0x%x n=%d\n", address, n);

  // Close everything.
  e_free(&ext_mem);
  e_close(&dev);
  e_finalize();

  return 0;
}

