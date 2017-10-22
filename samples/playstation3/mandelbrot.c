#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <libspe2.h>

#define WIDTH 1024
#define HEIGHT 768

enum
{
  STATE_IDLE,
  STATE_RENDERING,
  STATE_DMA,
};

static int colors[] =
{
  0xff0000,  // f
  0xee3300,  // e
  0xcc5500,  // d
  0xaa5500,  // c
  0xaa3300,  // b
  0x666600,  // a
  0x999900,  // 9
  0x669900,  // 8
  0x339900,  // 7
  0x0099aa,  // 6
  0x0066aa,  // 5
  0x0033aa,  // 4
  0x0000aa,  // 3
  0x000099,  // 2
  0x000066,  // 1
  0x000000,  // 0
};

struct _mandel_info
{
  float r_step4;         // 0
  float i_step;          // 4
  float imaginary_start; // 8
  float real_start;      // 12
  float r_step;          // 16
  int width;             // 20
  int height;            // 24
  int reserved_1;        // 28
  float real_start4[4];  // 32
  float constant_4_0[4]; // 48
  float constant_2_0[4]; // 64
  float temp[4];         // 80
  int *colors;           // 96
};

struct _spe_info
{
  spe_context_ptr_t spe;
  int state;
  int y;
  spe_program_handle_t *prog;
  pthread_t pid;
  int count;
};

void render_mandelbrot_altivec(int *picture, struct _mandel_info *mandel_info);

void *spe_thread(void *context)
{
  struct _spe_info *spe_info = (struct _spe_info *)context;
  spe_stop_info_t stop_info;
  uint32_t entry;

  //printf("Running SPE\n");

  // Seems default entry picks up the entry point from the ELF file.
  // If I set this to 0 with sync, stop as the first two instructions
  // before main:, it hangs.
  entry = SPE_DEFAULT_ENTRY;

  if (spe_context_run(spe_info->spe, &entry, 0, NULL, NULL, &stop_info) < 0)
  {
    printf("Context run failed.\n");
    exit(1);
  }

  return NULL;
}

int load_spus(struct _spe_info *spe_info, int max_spus)
{
  int spus = spe_cpu_info_get(SPE_COUNT_USABLE_SPES, 0);
  int n;

  printf("spus=%d\n", spus);

  if (spus > max_spus) { spus = max_spus; }

  for (n = 0; n < max_spus; n++)
  {
    spe_info[n].prog = spe_image_open("mandelbrot_spe.elf");
    spe_info[n].state = STATE_IDLE;
    spe_info[n].count = 0;

    if (spe_info[n].prog == NULL)
    {
      perror("Can't load spe module.");
      exit(1);
    }

    spe_info[n].spe = spe_context_create(0, NULL);

    if (spe_info[n].spe == NULL)
    {
      perror("Can't create context.");
      exit(1);
    }

    if (spe_program_load(spe_info[n].spe, spe_info[n].prog) != 0)
    {
      perror("Program load failed.");
      exit(1);
    }

    // Need to fork out a thread since the function that starts code running
    // on an SPE will block until it finishes.
    pthread_create(&spe_info[n].pid, NULL, &spe_thread, &spe_info[n]);
  }

  return max_spus;
}

int mandel_calc_cell(int *picture, int width, int height, float real_start, float real_end, float imaginary_start, float imaginary_end, struct _spe_info *spe_info, int spus)
{
  uint32_t data;
  int finished;
  uint32_t tag_status;
  int count;
  int y;
  int n = 0;

  float r_step4 = (real_end - real_start) * 4 / (float)width;
  float r_step = (real_end - real_start) / (float)width;
  float i_step = (imaginary_end - imaginary_start) / (float)height;

  for (n = 0; n < spus; n++)
  {
    // These things can be sent at start-up time to speed things up
    if (spe_in_mbox_write(spe_info[n].spe, (void *)&r_step4, 1, SPE_MBOX_ANY_BLOCKING) < 0) { return -1; }
    if (spe_in_mbox_write(spe_info[n].spe, (void *)&r_step, 1, SPE_MBOX_ANY_BLOCKING) < 0) { return -1; }
  }

  y = 0;
  finished = 0;

  while(finished < 768)
  {
    // Is there any more of the picture left to generate?
    if (y < 768)
    {
      // Find any free SPUs and send them coordinates to start
      for (n = 0; n < spus; n++)
      {
        if (spe_info[n].state != STATE_IDLE) { continue; }

        // Send 32 bits of data to the SPE: real_start, imaginary_start
        if (spe_in_mbox_write(spe_info[n].spe, (void *)&real_start, 1, SPE_MBOX_ANY_BLOCKING) < 0) { return -1; }
        if (spe_in_mbox_write(spe_info[n].spe, (void *)&imaginary_start, 1, SPE_MBOX_ANY_BLOCKING) < 0) { return -1; }

        // printf("n=%d y=%d count=%d\n", n, y, spe_info[n].count);
        spe_info[n].y = y;
        spe_info[n].state = STATE_RENDERING;
        spe_info[n].count++;

        imaginary_start += i_step;
        y++;
        if (y >= 768) { break; }
      }
    }

    // Check for a finished line on all SPUs
    for (n = 0; n < spus; n++)
    {
      if (spe_info[n].state != STATE_RENDERING) { continue; }

      count = spe_out_mbox_read(spe_info[n].spe, &data, 1);

      if (count != 0)
      {
        // printf("local storage address: 0x%x  (%d)\n", data, y);
        uint32_t tag = n;

        if (spe_mfcio_put(spe_info[n].spe, data, picture + (spe_info[n].y * 1024), 4096, tag, 0, 0) != 0)
        {
          perror("put problem");
        }

#if 0
        if (spe_mssync_start(spe_info[n].spe) != 0) { perror("sync start issue"); }
        if (spe_mssync_status(spe_info[n].spe) != 0) { perror("sync status issue"); }
#endif
        //spe_info[n].state = STATE_DMA;
        spe_info[n].state = STATE_IDLE;
        finished++;
      }
    }

    // Check for finished DMA transfer
#if 0
    for (n = 0; n < spus; n++)
    {
      if (spe_info[n].state != STATE_DMA) { continue; }

      if (spe_mfcio_tag_status_read(spe_info[n].spe, 0xff, SPE_TAG_IMMEDIATE, &tag_status) != 0) { perror("tag status"); }

      if (tag_status != 0)
      {
        //printf("tag_status=%d\n", tag_status);

        finished++;
        spe_info[n].state = STATE_IDLE;
      }
    }
#endif
  }

  for (n = 0; n < spus; n++)
  {
    while(1)
    {
      if (spe_mfcio_tag_status_read(spe_info[n].spe, 0xff, SPE_TAG_IMMEDIATE, &tag_status) != 0) { perror("tag status"); }

      if (tag_status != 0)
      {
        //printf("tag_status=%d\n", tag_status);
        break;
      }
    }
  }

  return 0;
}

int mandel_calc_altivec(int *picture, int width, int height, float real_start, float real_end, float imaginary_start, float imaginary_end)
{
  struct _mandel_info mandel_info __attribute__ ((aligned (16)));

  mandel_info.r_step4 = (real_end - real_start) * 4 / (float)width;
  mandel_info.r_step = (real_end - real_start) / (float)width;
  mandel_info.i_step = (imaginary_end - imaginary_start) / (float)height;
  mandel_info.real_start = real_start;
  mandel_info.imaginary_start = imaginary_start;
  mandel_info.width = width;
  mandel_info.height = height;
  mandel_info.real_start4[0] = real_start;
  mandel_info.real_start4[1] = mandel_info.real_start4[0] + mandel_info.r_step;
  mandel_info.real_start4[2] = mandel_info.real_start4[1] + mandel_info.r_step;
  mandel_info.real_start4[3] = mandel_info.real_start4[2] + mandel_info.r_step;

  // Load constants
  mandel_info.constant_2_0[0] = 2.0;
  mandel_info.constant_4_0[0] = 4.0;

  mandel_info.colors = colors;

  //printf("offsetof=%d\n", offsetof(struct _mandel_info, constant_2_0));
  //printf("offsetof=%d\n", offsetof(struct _mandel_info, colors));

  render_mandelbrot_altivec(picture, &mandel_info);

#if 0
printf("%f %f %f %f  %f\n",
  mandel_info.temp[0],
  mandel_info.temp[1],
  mandel_info.temp[2],
  mandel_info.temp[3],
  mandel_info.r_step);
#endif

  return 0;
}

int mandel_calc(int *picture, int width, int height, float real_start, float real_end, float imaginary_start, float imaginary_end)
{
  const int max_count = 127;
  int x,y;
  float r,i,r_step,i_step;
  float tr,ti,zr,zi;
  int ptr,count;

  r_step = (real_end - real_start) / (float)width;
  i_step = (imaginary_end - imaginary_start) / (float)height;
  ptr = 0;

  //printf("step = %f %f\n", r_step, i_step);

  i = imaginary_start;

  for (y = 0; y < height; y++)
  {
    r = real_start;

    for (x = 0; x < width; x++)
    {
      zr = 0;
      zi = 0;

      for (count = 0; count < max_count; count++)
      {
        tr = ((zr * zr) - (zi * zi));
        ti = (2 * zr * zi);
        zr = tr + r;
        zi = ti + i;
        if ((zr * zr) + (zi * zi) > 4) break;
      }

      picture[ptr] = colors[count >> 3];
      //picture[ptr] = count;

      ptr++;
      r = r + r_step;
    }

    i = i + i_step;
  }

  return 0;
}

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

void write_bmp(int *picture, int width, int height)
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
      color = picture[offset++];

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
  struct timeval tv_start, tv_end;
  struct _spe_info spe_info[8];
  int spus = 0;
  int *picture;

  posix_memalign((void *)&picture, 16, WIDTH * HEIGHT * 4);

#if 0
  float real_start = -0.1592 - 0.01;
  float real_end = -0.1592 + 0.01;
  float imaginary_start = -1.0317 - 0.01;
  float imaginary_end = -1.0317 + 0.01;
#endif

  float real_start = 0.37 - 0.00;
  float real_end = 0.37 + 0.04;
  float imaginary_start = -0.2166 - 0.02;
  float imaginary_end = -0.2166 + 0.02;

#if 0
  float real_start = -2.00;
  float real_end = 1.00;
  float imaginary_start = -1.00;
  float imaginary_end = 1.00;
#endif
  int arch = 0;

  if (argc != 2)
  {
    printf("Usage: %s <altivec/cell/normal>\n", argv[0]);
    exit(0);
  }

  if (strcmp(argv[1], "normal") == 0) { arch = 0; }
  if (strcmp(argv[1], "altivec") == 0) { arch = 1; }
  if (strcmp(argv[1], "cell") == 0) { arch = 2; }

  printf("colors=%p\n", colors);
  printf("picture=%p\n", picture);

  if (arch == 2)
  {
    spus = load_spus(spe_info, 6);
  }

  gettimeofday(&tv_start, NULL);

  if (arch == 0)
  {
    mandel_calc(picture, WIDTH, HEIGHT, real_start, real_end, imaginary_start, imaginary_end);
  }
    else
  if (arch == 1)
  {
    mandel_calc_altivec(picture, WIDTH, HEIGHT, real_start, real_end, imaginary_start, imaginary_end);
  }
    else
  {
    mandel_calc_cell(picture, WIDTH, HEIGHT, real_start, real_end, imaginary_start, imaginary_end, spe_info, spus);
  }

  gettimeofday(&tv_end, NULL);

  printf("done\n");
  fflush(stdout);

  if (arch == 2)
  {
    int n;
    int total = 0;

    for (n = 0; n < spus; n++)
    {
      //pthread_join(pid, NULL);

      printf("%d) count=%d  y=%d\n", n, spe_info[n].count, spe_info[n].y);
      total += spe_info[n].count;

      if (spe_context_destroy(spe_info[n].spe) != 0)
      {
        perror("Destroy failed.");
        exit(1);
      }

      spe_image_close(spe_info[n].prog);
    }

    printf("total=%d\n", total);
  }

#if 0
  int picture2[WIDTH * HEIGHT];
  mandel_calc(picture2, WIDTH, HEIGHT, real_start, real_end, imaginary_start, imaginary_end);

  int n;
  for (n = 0; n < WIDTH * HEIGHT; n++)
  {
    if (picture[n] != picture2[n])
    {
      printf("error %d  %8x %8x\n", n, picture[n], picture2[n]);
    }
  }
#endif

  printf("%ld %ld\n", tv_end.tv_sec, tv_end.tv_usec);
  printf("%ld %ld\n", tv_start.tv_sec, tv_start.tv_usec);
  long time_diff = tv_end.tv_usec - tv_start.tv_usec;
  while(time_diff < 0) { tv_end.tv_sec--; time_diff += 1000000; }
  time_diff += (tv_end.tv_sec - tv_start.tv_sec) * 1000000;
  printf("time=%f\n", (float)time_diff / 1000000);

  fflush(stdout);

  write_bmp(picture, WIDTH, HEIGHT);

  free(picture);

  return 0;
}

