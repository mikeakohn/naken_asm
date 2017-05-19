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
  uint32_t data;
  int n;

  e_init(NULL);
  e_reset_system();
  e_get_platform_info(&platform);

  printf("\n------ info --------\n");
  printf("platform.version:  %s\n", platform.version);
  printf("platform.row:      %d\n", platform.row);
  printf("platform.col:      %d\n", platform.col);
  printf("platform.rows:     %d\n", platform.rows);
  printf("platform.cols:     %d\n", platform.cols);
  printf("platform.num_chips:%d\n", platform.num_chips);
  printf("platform.num_emems:%d\n", platform.num_emems);
  printf("\n");

  n = e_open(&dev, 0, 0, 1, 1);

  printf("e_open() return value is %d\n", n);

  n = e_load("epiphany.srec", &dev, 0, 0, E_FALSE);

  printf("e_load() return value is %d\n", n);

  n = e_start(&dev, 0, 0);

  printf("e_start() return value is %d\n", n);

  sleep(1);

  n = e_read(&dev, 0, 0, 0x6000, &data, 4);
  printf("data=0x%x n=%d\n", data, n);

  sleep(1);

  n = e_read(&dev, 0, 0, 0x6000, &data, 4);
  printf("data=0x%x n=%d\n", data, n);

  e_close(&dev);
  e_finalize();

  return 0;
}

