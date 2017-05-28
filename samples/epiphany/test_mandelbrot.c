#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <e-hal.h>
#include <e-loader.h>

#define WIDTH 1024
#define HEIGHT 768

static int colors[] =
{
  0x000000,  // 0
  0x000066,  // 1
  0x000099,  // 2
  0x0000aa,  // 3
  0x0033aa,  // 4
  0x0066aa,  // 5
  0x0099aa,  // 6
  0x339900,  // 7
  0x669900,  // 8
  0x999900,  // 9
  0x666600,  // a
  0xaa3300,  // b
  0xaa5500,  // c
  0xcc5500,  // d
  0xee3300,  // e
  0xff0000,  // f
};

struct _mandel_params
{
  float real_start;
  float imaginary_start;
  float real_inc;
  int count;
  int done;
  int debug;
};

int write_int32(FILE *out, int n)
{
  putc((n & 0xff), out);
  putc(((n >> 8) & 0xff), out);
  putc(((n >> 16) & 0xff), out);
  putc(((n >> 24) & 0xff), out);

  return 0;
}

int write_int16(FILE *out, int n)
{
  putc((n & 0xff), out);
  putc(((n >> 8) & 0xff), out);

  return 0;
}

void write_bmp(uint8_t *picture, int width, int height)
{
  FILE *out;
  int bmp_width;
  int bmp_size;
  int padding;
  int offset;
  int color;
  int x,y;

  out = fopen("out.bmp", "wb");

  if (out == NULL)
  {
    printf("Can't open file for writing.");
    return;
  }

  bmp_width = width * 3;
  bmp_width = (bmp_width + 3) & (~0x3);
  bmp_size = (bmp_width * height) + 14 + 40;
  padding = bmp_width - (width * 3);

  //printf("width=%d (%d)\n", width, width*3);
  //printf("bmp_width=%d\n", bmp_width);
  //printf("bmp_size=%d\n", bmp_size);

  /* size: 14 bytes */

  putc('B', out);
  putc('M', out);
  write_int32(out, bmp_size);
  write_int16(out, 0);
  write_int16(out, 0);
  write_int32(out, 54);

  /* head1: 14  head2: 40 */

  write_int32(out, 40);         /* biSize */
  write_int32(out, width);
  write_int32(out, height);
  write_int16(out, 1);
  write_int16(out, 24);
  write_int32(out, 0);          /* compression */
  write_int32(out, bmp_width*height);
  write_int32(out, 0);          /* biXPelsperMetre */
  write_int32(out, 0);          /* biYPelsperMetre */
  write_int32(out, 0);
  write_int32(out, 0);

  for (y = 0; y < height; y++)
  {
    offset = y * width;

    for (x = 0; x < width; x++)
    {
      color = colors[picture[offset++]];

      putc(color & 0xff, out);
      putc((color >> 8) & 0xff, out);
      putc((color >> 16) & 0xff, out);
    }
    for (x = 0; x < padding; x++) { putc(0, out); }
  }

  fclose(out);
}

int main(int argc, char *argv[])
{
  e_platform_t platform;
  e_epiphany_t dev;
  e_mem_t ext_mem;
  struct _mandel_params mandel_params;
  uint32_t done;
  uint8_t picture[WIDTH * HEIGHT];
  float real_start = 0.37 - 0.00;
  float real_end = 0.37 + 0.04;
  float imaginary_start = -0.2166 - 0.02;
  float imaginary_end = -0.2166 + 0.02;
  float real_inc = (real_end - real_start) / WIDTH;
  float imaginary_inc = (imaginary_end - imaginary_start) / HEIGHT;
  struct timeval tv_start, tv_end;
  int n, y;

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

  // Open a link to the Epiphany and load it with mandelbrot.asm and
  // signal the core to start running.
  n = e_open(&dev, 0, 0, 1, 1);
  printf("e_open() return value is %d\n", n);

  n = e_load("mandelbrot.srec", &dev, 0, 0, E_FALSE);
  printf("e_load() return value is %d\n", n);

  // Get a pointer to external (shared) memory.
  n = e_alloc(&ext_mem, 0, 2048);
  printf("e_alloc() return value is %d\n", n);

  n = e_start(&dev, 0, 0);
  printf("e_start() return value is %d\n", n);

  // Start timer.
  gettimeofday(&tv_start, NULL);

  // Tell a core on the Epiphany to calculate 1 line of the mandelbrot image.
  for (y = 0; y < HEIGHT; y++)
  {
    mandel_params.real_start = real_start;
    mandel_params.imaginary_start = imaginary_start;
    mandel_params.real_inc = real_inc;
    mandel_params.count = WIDTH;
    mandel_params.done = 0;
    mandel_params.debug = 10;

    n = e_write(&ext_mem, 0, 0, 0x0000, &mandel_params, sizeof(mandel_params));
    n = e_signal(&dev, 0, 0);

#if 0
    printf("y=%d real_start=%f imaginary_start=%f r_inc=%f i_inc=%f\n",
      y,
      mandel_params.real_start,
      mandel_params.imaginary_start,
      mandel_params.real_inc,
      imaginary_inc);
#endif

    while(1)
    {
      n = e_read(&ext_mem, 0, 0, 0x0010, &done, sizeof(done));
      if (done != 0) { break; }

#if 0
      uint32_t debug;
      n = e_read(&ext_mem, 0, 0, 0x0014, &debug, sizeof(debug));
      printf("debug=%d 0x%04x n=%d\n", debug, debug, n);
#endif
    }

    // Copy image data from shared memory to process's memory.
    n = e_read(&ext_mem, 0, 0, 0x0400, picture + (y * WIDTH), WIDTH);

    imaginary_start += imaginary_inc;
  }

  // Print how long it took to render.
  gettimeofday(&tv_end, NULL);

  printf("%ld %ld\n", tv_end.tv_sec, tv_end.tv_usec);
  printf("%ld %ld\n", tv_start.tv_sec, tv_start.tv_usec);
  long time_diff = tv_end.tv_usec - tv_start.tv_usec;
  while(time_diff < 0) { tv_end.tv_sec--; time_diff += 1000000; }
  time_diff += (tv_end.tv_sec - tv_start.tv_sec) * 1000000;
  printf("time=%f\n", (float)time_diff / 1000000);

  // Save image data into a BMP on the local filesystem.
  write_bmp(picture, WIDTH, HEIGHT);

  // Close everything.
  e_free(&ext_mem);
  e_close(&dev);
  e_finalize();

  return 0;
}

