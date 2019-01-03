/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common/memory.h"
#include "common/version.h"
#include "fileio/read_elf.h"
#include "fileio/read_hex.h"
#include "fileio/write_hex.h"
#include "prog/lpc.h"

enum
{
  PROG_UNDEF=0,
  PROG_LPC,
};

enum
{
  COMMAND_UNDEF=0,
  COMMAND_INFO,
  COMMAND_READ,
  COMMAND_WRITE,
  COMMAND_RUN,
};

int main(int argc, char *argv[])
{
struct _memory memory;
char *filename=NULL;
char *uart="/dev/ttyUSB0";
int prog_type=PROG_UNDEF;
int command=COMMAND_UNDEF;
int param1=0,param2=0;
int i;

  printf("\nnaken_prog - by Michael Kohn\n");
  printf("    Web: http://www.mikekohn.net/\n");
  printf("  Email: mike@mikekohn.net\n\n");
  printf("Version: "VERSION"\n\n");

  if (argc<2)
  {
    printf("Usage: naken_util [options]\n");
    printf("  CPU options:\n");
    printf("   -lpc\n");
    printf("  Command:\n");
    printf("   -info\n");
    printf("   -write <outfile>\n");
    printf("   -read <address> <count> <infile>\n");
    printf("   -execute <address>\n");
    printf("\n");
    exit(0);
  }

  memset(&memory, 0, sizeof(memory));

  for (i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "-lpc")==0)
    {
      prog_type=PROG_LPC;
    }
      else
    if (strcmp(argv[i], "-info")==0)
    {
      command=COMMAND_INFO;
    }
      else
    if (strcmp(argv[i], "-read")==0)
    {
      command=COMMAND_READ;
      if (i+2>=argc)
      {
        printf("Error: -read command takes 2 arguments\n");
        exit(1);
      }
      param1=atoi(argv[++i]);
      param2=atoi(argv[++i]);
    }
      else
    if (strcmp(argv[i], "-write")==0)
    {
      command=COMMAND_WRITE;
    }
      else
    if (strcmp(argv[i], "-run")==0)
    {
      command=COMMAND_RUN;
      param1=atoi(argv[++i]);
    }
      else
    {
      filename=argv[i];
    }
  }

  if (prog_type==PROG_UNDEF)
  {
    printf("Error: No chip type selected.\n");
    exit(0);
  }

  if (command==COMMAND_INFO)
  {
    if (prog_type==PROG_LPC)
    {
      lpc_info(uart);
    }
  }
    else
  if (command==COMMAND_READ)
  {
    if (filename==NULL)
    {
      printf("No output filename.  Exiting...\n");
      exit(1);
    }

    memory_init(&memory, 0xffffffff, 1);
    lpc_memory_read(uart, &memory, param1, param2);
    // FIXME - make filename selectable
    FILE *out=fopen(filename, "wb");
    write_hex(&memory, out);
    fclose(out);
    memory_free(&memory);
  }
    else
  if (command==COMMAND_WRITE)
  {
    unsigned char cpu_type;

    if (filename==NULL)
    {
      printf("No hexfile loaded.  Exiting...\n");
      exit(1);
    }

    memory_init(&memory, 0xffffffff, 1);

    if (read_elf(filename, &memory, &cpu_type, NULL)>=0)
    {
      printf("Loaded ELF %s from 0x%04x to 0x%04x\n", filename, memory.low_address, memory.high_address);
    }
      else
    if (read_hex(filename, &memory)>=0)
    {
      printf("Loaded hexfile %s from 0x%04x to 0x%04x\n", filename, memory.low_address, memory.high_address);
    }
      else
    {
      printf("Could not load hexfile\n");
      memory_free(&memory);
      exit(1);
    }

    lpc_memory_write(uart, &memory);
    memory_free(&memory);
  }
    else
  if (command==COMMAND_RUN)
  {
    if (prog_type==PROG_LPC)
    {
      lpc_run(uart, param1);
    }
  }

  return 0;
}

