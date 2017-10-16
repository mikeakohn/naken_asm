#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libspe2.h>

struct _spe_info
{
  spe_context_ptr_t spe;
};

void *spe_thread(void *context)
{
  struct _spe_info *spe_info= (struct _spe_info *)context;
  spe_stop_info_t stop_info;
  uint32_t entry;

  printf("Running SPE\n");

  // Seems default entry picks up the entry point from the ELF file.
  // If I set this to 0 with sync, stop as the first two instructions
  // before main:, it hangs.
  entry = SPE_DEFAULT_ENTRY;

  if (spe_context_run(spe_info->spe, &entry, 0, NULL, NULL, &stop_info) < 0)
  {
    printf("Context run failed.\n");
    exit(1);
  }

  printf("Program done running -- reason=%d spu_status=%d result=%d\n",
    stop_info.stop_reason,
    stop_info.spu_status,
    stop_info.result);
}

int main(int argc, char *argv[])
{
  struct _spe_info spe_info;
  spe_context_ptr_t spe;
  spe_program_handle_t *prog;
  uint32_t data;
  pthread_t pid;
  int count;

  prog = spe_image_open("spe_test.elf");

  if (prog == NULL)
  {
    perror("Can't load spe module.");
    exit(1);
  }

  printf("Image opened.\n");

  spe = spe_context_create(0, NULL);

  if (spe == NULL)
  {
    printf("Can't create context.\n");
    exit(1);
  }

  printf("Context created.\n");

  if (spe_program_load(spe, prog) != 0)
  {
    printf("Program load failed.\n");
    exit(1);
  }

  printf("Program loaded.\n");

  // Need to fork out a thread since the function that starts code running
  // on an SPE will block until it finishes.

  spe_info.spe = spe;
  pthread_create(&pid, NULL, &spe_thread, &spe_info);

  //sleep(1);

  printf("Sending data through mbox.\n");

  // Send 32 bits of data to the SPE
  data = 7;

  if (spe_in_mbox_write(spe, &data, 1, SPE_MBOX_ANY_BLOCKING) < 0)
  {
    perror("Could not write mbox data.");
    exit(1);
  }

  printf("Data sent, now waiting for a response.\n");

  // Wait for data to come back from the SPE
  //spe_event_wait(handler, events_generated, NUM_THREADS, -1);

  //count = spe_out_intr_mbox_read(spe, (uint32_t *)&data, 1, SPE_MBOX_ANY_BLOCKING);

  while(1)
  {
    count = spe_out_mbox_read(spe, &data, 1);
    if (count != 0) { break; }
  }

  if (count < 0)
  {
    perror("Could not read mbox data.");
    exit(1);
  }

  printf("Received an event from SPE %d  (count=%d)\n", data, count);

  if (spe_context_destroy(spe) != 0)
  {
    printf("Destroy failed.\n");
    exit(1);
  }

  spe_image_close(prog);

  return 0;
}

