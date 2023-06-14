/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "common/assembler.h"
//#include "common/naken_util.h"
#include "common/util_context.h"
#include "common/util_disasm.h"
#include "common/util_sim.h"
#include "common/version.h"
#include "disasm/1802.h"
#include "disasm/4004.h"
#include "disasm/6502.h"
#include "disasm/65816.h"
#include "disasm/6800.h"
#include "disasm/6809.h"
#include "disasm/68hc08.h"
#include "disasm/68000.h"
#include "disasm/8008.h"
#include "disasm/8048.h"
#include "disasm/8051.h"
#include "disasm/86000.h"
#include "disasm/arc.h"
#include "disasm/arm.h"
#include "disasm/arm64.h"
#include "disasm/avr8.h"
#include "disasm/cell.h"
#include "disasm/copper.h"
#include "disasm/cp1610.h"
#include "disasm/dotnet.h"
#include "disasm/dspic.h"
#include "disasm/epiphany.h"
#include "disasm/java.h"
#include "disasm/lc3.h"
#include "disasm/m8c.h"
#include "disasm/mips.h"
#include "disasm/msp430.h"
#include "disasm/pdp8.h"
#include "disasm/pic14.h"
#include "disasm/powerpc.h"
#include "disasm/propeller.h"
#include "disasm/propeller2.h"
#include "disasm/ps2_ee_vu.h"
#include "disasm/riscv.h"
#include "disasm/sh4.h"
#include "disasm/sparc.h"
#include "disasm/stm8.h"
#include "disasm/super_fx.h"
#include "disasm/sweet16.h"
#include "disasm/tms340.h"
#include "disasm/tms1000.h"
#include "disasm/tms9900.h"
#include "disasm/unsp.h"
#include "disasm/webasm.h"
#include "disasm/xtensa.h"
#include "disasm/z80.h"
#include "fileio/read_amiga.h"
#include "fileio/read_bin.h"
#include "fileio/read_elf.h"
#include "fileio/read_hex.h"
#include "fileio/read_srec.h"
#include "fileio/read_ti_txt.h"
#include "fileio/read_wdc.h"
#include "simulate/1802.h"
#include "simulate/8008.h"
#include "simulate/avr8.h"
#include "simulate/6502.h"
#include "simulate/65816.h"
#include "simulate/lc3.h"
#include "simulate/mips.h"
#include "simulate/msp430.h"
#include "simulate/tms9900.h"
#include "simulate/z80.h"
#include "simulate/null.h"

enum
{
  MODE_INTERACTIVE,
  MODE_DISASM,
  MODE_RUN,
};

static char *state_stopped = "stopped";
static char *state_running = "running";

#ifdef READLINE
static const char *command_names[] =
{
  "help",
  "quit",
  "exit",
  "run",
  "step",
  "call",
  "stop",
  "reset",
  "break",
  "push",
  "set",
  "clear",
  "speed",
  "print",
  "print",
  "print16",
  "print32",
  "write",
  "write16",
  "write32",
  "print",
  "disasm",
  "symbols",
  "dumpram",
  "info",
  "registers",
  "display",
  "read",
};

static const char *find_partial_command(const char *text, int *index)
{
  int len = strlen(text);
  const char *name;

  while (*index < sizeof(command_names) / sizeof(char*))
  {
    name = command_names[*index];
    ++*index;

    if (strncmp(name, text, len) == 0)
    {
      return name;
    }
  }

  return NULL;
}

static const char *find_command(const char *text)
{
  const char *name;
  int index = 0;

  while (index < sizeof(command_names) / sizeof(char*))
  {
    name = command_names[index];
    ++index;

    if (strcmp(name, text) == 0)
    {
      return name;
    }
  }

  return NULL;
}

static char *command_name_generator(const char *text, int state)
{
  static int index;
  const char *name;

  if (!state)
  {
    index = 0;
  }

  name = find_partial_command(text, &index);

  if (name != NULL)
  {
    return strdup(name);
  }

  return NULL;
}

static char **command_name_completion(const char *text, int start, int end)
{
  const char *name;
  char command[128];
  int n;

  // suppress default completion
  rl_attempted_completion_over = 1;

  char *s = rl_line_buffer;
  while (*s != 0 && *s == ' ') { s++; }

  // check if buffer already contains a command
  n = 0;

  while (n < sizeof(command) - 1)
  {
    command[n] = s[n];

    if (s[n] == 0 || s[n] == ' ') { break; }
    n++;
  }

  command[n] = 0;

  if (n >= sizeof(command) - 1) { return NULL; }

  name = find_command(command);

  if (name != NULL) { return NULL; }

  return rl_completion_matches(text, command_name_generator);
}
#endif

static void print_help()
{
  printf("Commands:\n");
  printf("  print <start>-<end>       [ print bytes at start address (opt. to end) ]\n");
  printf("  print16 <start>-<end>     [ print int16's at start address (opt. to end) ]\n");
  printf("  print32 <start>-<end>     [ print int32's at start address (opt. to end) ]\n");
  printf("  write <address> <data>..  [ write multiple bytes to RAM starting at address]\n");
  printf("  write16 <address> <data>..[ write multiple int16's to RAM starting at address]\n");
  printf("  write32 <address> <data>..[ write multiple int32's to RAM starting at address]\n");
  printf("  dumpram <start>-<end>     [ Dump RAM of AVR8 during simulation]\n");
  printf("  registers                 [ dump registers ]\n");
  printf("  run, stop, step           [ simulation run, stop, step ]\n");
  printf("  call <address>            [ call function at address ]\n");
  printf("  push <value>              [ push value on stack ]\n");
  printf("  set <reg>=<value>         [ set register to value ]\n");
  printf("  set,clear <status flag>   [ set or clear a bit in the status register]\n");
  printf("  reset                     [ reset program ]\n");
  printf("  display                   [ toggle display cpu info while simulating ]\n");
  printf("  speed <speed in Hz>       [ simulation speed or 0 for single step ]\n");
  printf("  break <address>           [ break at address ]\n");
  //printf("  flash                    [ flash device ]\n");
  printf("  info                      [ general info ]\n");
  printf("  disasm                    [ disassemble at address ]\n");
  printf("  disasm <start>-<end>      [ disassemble range of addresses ]\n");
  printf("  symbols                   [ show symbols ]\n");
  //printf("  list <start>-<end>       [ disassemble wth debug listing ]\n");
}

int main(int argc, char *argv[])
{
  FILE *src = NULL;
  UtilContext util_context;
  char *state = state_stopped;
  char command[1024];
#ifdef READLINE
  char *line = NULL;
#endif
  uint32_t start_address = 0;
  uint32_t set_pc = -1;
  uint8_t force_bin = 0;
  int i;
  char *hexfile = NULL;
  int mode = MODE_INTERACTIVE;
  int break_io = -1;
  int error_flag = 0;
  uint8_t cpu_type_set = 0;

  printf("\nnaken_util - by Michael Kohn\n"
         "                Joe Davisson\n"
         "    Web: https://www.mikekohn.net/\n"
         "  Email: mike@mikekohn.net\n\n"
         "Version: " VERSION "\n\n");

  if (argc<2)
  {
    printf("Usage: naken_util [options] <infile>\n"
           "   // ELF files can auto-pick a CPU, if a hex file use:\n"
           "   -1802                        (RCA 1802)\n"
           "   -4004                        (Intel 4004 / MCS-4)\n"
           "   -6502                        (6502)\n"
           "   -65816                       (65816)\n"
           "   -6800                        (6800)\n"
           "   -6809                        (6809)\n"
           "   -68hc08                      (68hc08)\n"
           "   -68000                       (68000)\n"
           "   -8008                        (8008 / MCS-8)\n"
           "   -8048                        (8048 / MCS-48)\n"
           "   -8051 / -8052                (8051 / 8052 / MCS-51)\n"
           "   -86000                       (86000 / VMU)\n"
           "   -arc                         (ARC)\n"
           "   -arm                         (ARM)\n"
           "   -arm64                       (ARM64)\n"
           "   -avr8                        (Atmel AVR8)\n"
           "   -cell                        (IBM Cell BE)\n"
           "   -copper                      (Amiga Copper)\n"
           "   -cp1610                      (General Instruments CP1610)\n"
           "   -dotnet                      (.NET CIL)\n"
           "   -dspic                       (dsPIC)\n"
           "   -epiphany                    (Epiphany III/IV)\n"
           "   -java                        (Java)\n"
           "   -lc3                         (LC-3)\n"
           "   -m8c                         (PSoC M8C)\n"
           "   -mips32 / mips               (MIPS)\n"
           "   -msp430                      (MSP430/MSP430X) DEFAULT\n"
           "   -pdp8                        (PDP-8)\n"
           "   -pic14                       (PIC14 8 bit PIC / 14 bit opcode)\n"
           "   -powerpc                     (PowerPC)\n"
           "   -propeller                   (Parallax Propeller)\n"
           "   -propeller2                  (Parallax Propeller2)\n"
           "   -ps2ee                       (Playstation 2 EE)\n"
           "   -ps2ee_vu0                   (Playstation 2 VU0)\n"
           "   -ps2ee_vu1                   (Playstation 2 VU1)\n"
           "   -riscv                       (RISCV)\n"
           "   -sh4                         (SH4)\n"
           "   -stm8                        (STM8)\n"
           "   -super_fx                    (SuperFX)\n"
           "   -sweet16                     (sweet16)\n"
           "   -tms340                      (TMS340 / TMS34010)\n"
           "   -tms1000                     (TMS1000)\n"
           "   -tms1100                     (TMS1100)\n"
           "   -tms9900                     (TMS9900)\n"
           "   -unsp                        (SunPlus unSP)\n"
           "   -webasm                      (WebAssembly)\n"
           "   -xtensa                      (Xtensa)\n"
           "   -z80                         (z80)\n"
           "   -bin                         (file is binary)\n"
           "   // The following options turn off interactive mode\n"
           "   -disasm                      (Disassemble all of program)\n"
           "   -disasm_range <start>-<end>  (Disassemble a range of executable code)\n"
           "   -run                         (Simulate program and dump registers)\n"
           "   -address <start_address>     (For bin files: binary placed at this address)\n"
           "   -set_pc <address>            (Sets program counter after loading program)\n"
           "   -break_io <address>          (In -run mode writing to an i/o port exits sim)\n"
           "\n");
    exit(0);
  }

  util_init(&util_context);

  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      if (util_set_cpu_by_name(&util_context, argv[i] + 1) == 1) { continue; }
    }

    if (strcmp(argv[i], "-disasm") == 0)
    {
       strcpy(command, "disasm");
       mode = MODE_DISASM;
    }
      else
    if (strcmp(argv[i], "-disasm_range") == 0)
    {
       snprintf(command, sizeof(command), "disasm %s", argv[++i]);
       mode = MODE_DISASM;
    }
      else
    if (strcmp(argv[i], "-address") == 0)
    {
      i++;
      if (i >= argc)
      {
        printf("Error: -address needs an address\n");
        exit(1);
      }
      start_address = strtol(argv[i], NULL, 0);
    }
      else
    if (strcmp(argv[i], "-set_pc") == 0)
    {
      i++;
      if (i >= argc)
      {
        printf("Error: -set_pc needs an address\n");
        exit(1);
      }
      set_pc = strtol(argv[i], NULL, 0);
    }
      else
    if (strcmp(argv[i], "-break_io") == 0)
    {
      i++;
      if (i >= argc)
      {
        printf("Error: -break_io needs an address\n");
        exit(1);
      }
      break_io = strtol(argv[i], NULL, 0);
    }
      else
    if (strcmp(argv[i], "-bin") == 0)
    {
      force_bin = 1;
    }
      else
    if (strcmp(argv[i], "-run") == 0)
    {
       strcpy(command, "run");
       mode = MODE_RUN;
    }
      else
    if (argv[i][0] == '-')
    {
      printf("Unknown option %s\n", argv[i]);
      exit(1);
    }
      else
    {
      uint8_t cpu_type;
      char *extension = argv[i] + strlen(argv[i]) - 1;

      while (extension != argv[i])
      {
        if (*extension == '.') { extension++; break; }
        extension--;
      }

      if (read_elf(argv[i], &util_context.memory, &cpu_type, &util_context.symbols) >= 0)
      {
        int n = 0;

        while (cpu_list[n].name != NULL)
        {
          if (cpu_type_set == 1) { break; }

          if (cpu_type == cpu_list[n].type)
          {
            util_context.disasm_range = cpu_list[n].disasm_range;
            util_context.flags = cpu_list[n].flags;
            util_context.bytes_per_address = cpu_list[n].bytes_per_address;
            util_context.alignment = cpu_list[n].alignment;

            if (cpu_list[n].simulate_init != NULL)
            {
              // FIXME - Remove this free by allocating the memory early
              // to the size of the biggest possible struct.
              if (util_context.simulate != NULL)
              {
                free(util_context.simulate);
              }

              util_context.simulate = cpu_list[n].simulate_init(&util_context.memory);
            }

            break;
          }

          n++;
        }

        hexfile = argv[i];
        printf("Loaded elf %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      if (strcmp(extension, "txt") == 0 &&
          read_ti_txt(argv[i], &util_context.memory) >= 0)
      {
        hexfile = argv[i];
        printf("Loaded ti_txt %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      if ((strcmp(extension, "bin") == 0 || force_bin == 1) &&
          read_bin(argv[i], &util_context.memory, start_address) >= 0)
      {
        hexfile = argv[i];
        printf("Loaded bin %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      if (strcmp(extension, "srec") == 0 &&
          read_srec(argv[i], &util_context.memory) >= 0)
      {
        hexfile = argv[i];
        printf("Loaded srec %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      if ((strcmp(extension, "wdc") == 0) &&
          read_wdc(argv[i], &util_context.memory) >= 0)
      {
        hexfile = argv[i];
        printf("Loaded WDC binary %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      if (read_amiga(argv[i], &util_context.memory) >= 0)
      {
        util_set_cpu_by_name(&util_context, "68000");

        hexfile = argv[i];
        printf("Loaded Amiga (hunk) exe %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      if (read_hex(argv[i], &util_context.memory) >= 0)
      {
        hexfile = argv[i];
        printf("Loaded hexfile %s from 0x%04x to 0x%04x\n",
          argv[i],
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
        else
      {
        printf("Could not load hexfile\n");
      }
    }
  }

  if (hexfile == NULL)
  {
    printf("No hexfile loaded.  Exiting...\n");
    exit(1);
  }

  util_context.simulate->simulate_reset(util_context.simulate);

  if (mode == MODE_RUN)
  {
    util_context.simulate->usec = 1;
    util_context.simulate->show = 0;
    util_context.simulate->auto_run = 1;
  }

  util_context.simulate->break_io = break_io;

  if (set_pc != -1)
  {
    util_context.simulate->simulate_set_pc(util_context.simulate, set_pc);
  }

  printf("Type help for a list of commands.\n");
  command[1023] = 0;

  while (1)
  {
    if (mode == MODE_INTERACTIVE)
    {
#ifndef READLINE
      printf("%s> ", state);
      fflush(stdout);
      if (fgets(command, 1023, stdin) == NULL) break;
      command[1023] = 0;
#else
      char prompt[32];
      sprintf(prompt, "%s> ", state);
      rl_attempted_completion_function = command_name_completion;
      line = readline(prompt);

      if (!(line == NULL || line[0] == 0))
      {
        add_history(line);
        // FIXME - this stinks.
        strncpy(command, line, 1023);
        command[1023] = 0;
      }
        else
      {
        command[0] = 0;
      }
#endif
    }

    // Trim CR/LF and whitespace at end of line
    i = 0;

    while (command[i] != 0)
    {
      if (command[i] == '\n' || command[i] == '\r')
      {
        command[i] = 0;
        break;
      }

      i++;
    }

    // Trim trailing whitespace
    i--;

    while (i >= 0 && command[i] == ' ')
    {
      command[i] = 0;
      i--;
    }

    if (command[0] == 0)
    {
      if (util_context.simulate->step_mode == 1)
      {
        util_context.simulate->simulate_run(util_context.simulate, -1, 1);
      }
      continue;
    }

    if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0)
    {
      print_help();
    }
      else
    if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0)
    {
      break;
    }
      else
    if (strncmp(command, "run", 3) == 0 &&
        (command[3] == 0 || command[3] == ' '))
    {
      state = state_running;

      if (util_context.simulate->usec == 0)
      {
        util_context.simulate->step_mode = 1;
      }

      int ret = util_context.simulate->simulate_run(
        util_context.simulate,
        command[3] == 0 ? -1 : atoi(command + 4),
        0);

      state = state_stopped;

      if (util_context.simulate->auto_run == 1 && ret != 0)
      {
         error_flag = 1;
      }

      if (mode == MODE_RUN) { break; }
      continue;
    }
      else
    if (strcmp(command, "step") == 0)
    {
      util_context.simulate->step_mode = 1;
      util_context.simulate->simulate_run(util_context.simulate, -1, 1);
      continue;
    }
      else
    if (strncmp(command, "call", 4) == 0)
    {
      if (command[4] != ' ')
      {
        printf("Syntax error: call requires an address\n");
        continue;
      }

      if (util_context.simulate->usec == 0)
      {
        util_context.simulate->step_mode = 1;
      }

      // FIXME: This is MSP430 specific.
      uint32_t num;

      char *end = util_get_address(&util_context, command + 5, &num);

      if (end == NULL)
      {
        printf("Error: Unknown address '%s'\n", command + 5);
        continue;
      }

      state = state_running;
      util_context.simulate->simulate_push(util_context.simulate, 0xffff);
      util_context.simulate->simulate_set_reg(util_context.simulate, "r0", num);
      util_context.simulate->simulate_run(util_context.simulate, -1, 0);
      state = state_stopped;
      continue;
    }
      else
    if (strcmp(command, "stop") == 0)
    {
      state = state_stopped;
    }
      else
    if (strcmp(command, "reset") == 0)
    {
      util_context.simulate->simulate_reset(util_context.simulate);
    }
      else
    if (strncmp(command, "break", 5) == 0)
    {
      sim_set_breakpoint(&util_context, command);
    }
      else
    if (strncmp(command, "push", 4) == 0)
    {
      sim_stack_push(&util_context, command);
    }
      else
    if (strncmp(command, "set", 3) == 0)
    {
      sim_set_register(&util_context, command);
    }
      else
    if (strncmp(command, "clear", 5) == 0)
    {
      sim_clear_flag(&util_context, command);

#if 0
      if (util_context.simulate->simulate_set_reg(util_context.simulate, command+6, 0) == 0)
      {
        printf("Flag %s cleared.\n", command+6);
      }
        else
      {
        printf("Syntax error.\n");
      }
#endif
    }
      else
    if (strncmp(command, "speed", 5) == 0)
    {
      sim_set_speed(&util_context, command);
    }
      else
    if (strncmp(command, "print ", 6) == 0)
    {
       util_print8(&util_context, command + 5);
    }
      else
    if (strncmp(command, "bprint ", 7) == 0)
    {
       util_print8(&util_context, command + 6);
    }
      else
    if (strncmp(command, "print16", 7) == 0)
    {
       util_print16(&util_context, command + 7);
    }
      else
    if (strncmp(command, "wprint ", 7) == 0)
    {
       util_print16(&util_context, command + 6);
    }
      else
    if (strncmp(command, "print32", 7) == 0)
    {
       util_print32(&util_context, command + 7);
    }
      else
    if (strncmp(command, "write ", 6) == 0)
    {
       util_write8(&util_context, command + 5);
    }
      else
    if (strncmp(command, "bwrite ", 7) == 0)
    {
       util_write8(&util_context, command + 6);
    }
      else
    if (strncmp(command, "write16 ", 8) == 0)
    {
       util_write16(&util_context, command + 7);
    }
      else
    if (strncmp(command, "wwrite ", 7) == 0)
    {
       util_write16(&util_context, command + 6);
    }
      else
    if (strncmp(command, "write32 ", 8) == 0)
    {
       util_write32(&util_context, command + 7);
    }
      else
    if (strncmp(command, "disasm ", 7) == 0)
    {
       util_disasm(&util_context, command + 7);
    }
      else
    if (strcmp(command, "disasm") == 0)
    {
       util_disasm_range(
         &util_context,
         util_context.memory.low_address,
         util_context.memory.high_address);
    }
      else
    if (strcmp(command, "symbols") == 0)
    {
      symbols_print(&util_context.symbols, stdout);
    }
      else
    if (strncmp(command, "dumpram", 7) == 0)
    {
      uint32_t start, end;

      if (util_get_range(&util_context, command + 7, &start, &end) == -1)
      {
        printf("Illegal range.\n");
      }
        else
      if (util_context.simulate->simulate_dumpram(util_context.simulate, start, end) == -1)
      {
        printf("This arch doesn't support dumpram.  Use print / print16.\n");
      }
    }
      else
    if (strcmp(command, "info") == 0)
    {
      sim_show_info(&util_context);
    }
      else
    if (strcmp(command, "registers") == 0 || strcmp(command, "reg") == 0)
    {
      util_context.simulate->simulate_dump_registers(util_context.simulate);
    }
      else
    if (strcmp(command, "display") == 0)
    {
      if (util_context.simulate->show == 0)
      {
        printf("display now turned on\n");
        util_context.simulate->show = 1;
      }
        else
      {
        printf("display now turned off\n");
        util_context.simulate->show = 0;
      }
    }
      else
    if (strcmp(command, "read") == 0)
    {
#ifdef JTAG
      //open_jtag(&util_context, "/dev/ttyUSB0");
      open_jtag(&util_context, "/dev/ttyACM0");
      int ret = read_memory(&util_context, 0, 255);
      if (ret != 0)
      {
        printf("Read from device error %d.\n", ret);
      }

      close_jtag(&util_context);
#endif
    }
      else
    {
      printf("Unknown command: %s\n", command);
    }

    if (mode != MODE_INTERACTIVE) { break; }
    util_context.simulate->step_mode = 0;
  }

  if (mode == MODE_RUN)
  {
    util_context.simulate->simulate_dump_registers(util_context.simulate);
  }

  if (src != NULL) { fclose(src); }

  symbols_free(&util_context.symbols);

  if (util_context.simulate != NULL)
  {
    util_context.simulate->simulate_free(util_context.simulate);
  }

  memory_free(&util_context.memory);

  return error_flag == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

