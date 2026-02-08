/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <string>

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "common/assembler.h"
#include "common/String.h"
#include "common/UtilContext.h"
#include "common/util_disasm.h"
#include "common/util_sim.h"
#include "common/version.h"
#include "fileio/file.h"

enum
{
  MODE_INTERACTIVE,
  MODE_DISASM,
  MODE_RUN,
};

static const char *state_stopped = "stopped";
static const char *state_running = "running";
static const char *reading_code = "asm";

static void print_usage()
{
  printf(
    "Usage: naken_util [options] <infile>\n"
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
    "   -dspic                       (dsPIC / PIC24)\n"
    "   -epiphany                    (Epiphany III/IV)\n"
    "   -f100_l                      (Ferranti F100-L)\n"
    "   -f8                          (Fairchild F8)\n"
    "   -java                        (Java)\n"
    "   -lc3                         (LC-3)\n"
    "   -m8c                         (PSoC M8C)\n"
    "   -mips32 / mips               (MIPS / PIC32)\n"
    "   -msp430                      (MSP430/MSP430X) DEFAULT\n"
    "   -pdp8                        (PDP-8)\n"
    "   -pdp11                       (PDP-11)\n"
    "   -pic14                       (PIC14 8 bit PIC / 14 bit opcode)\n"
    "   -pic18                       (PIC18 8 bit PIC / 16 bit opcode)\n"
    "   -powerpc                     (PowerPC)\n"
    "   -propeller                   (Parallax Propeller)\n"
    "   -propeller2                  (Parallax Propeller2)\n"
    "   -ps2_ee                      (Playstation 2 EE)\n"
    "   -ps2_ee_vu0                  (Playstation 2 VU0)\n"
    "   -ps2_ee_vu1                  (Playstation 2 VU1)\n"
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
    "   -sim_serial <address> <infile> <outfile>\n"
    "\n");
}

struct CommandInfo
{
  const char *name;
  bool has_arg;
  bool is_optional;
};

static CommandInfo command_names[] =
{
  { "asm",       true,  true  },
  { "break",     true,  true  },
  { "call",      true,  false },
  { "clear",     true,  false },
  { "disasm",    true,  true  },
  { "display",   false, false },
  { "dumpram",   true,  false },
  { "dump_ram",  true,  false },
  { "exit",      false, false },
  { "help",      false, false },
  { "info",      false, false },
  { "print",     true,  false },
  { "print16",   true,  false },
  { "print32",   true,  false },
  { "push",      true,  false },
  { "quit",      false, false },
  { "registers", false, false },
  { "reg",       false, false },
  { "reset",     false, false },
  { "run",       false, false },
  { "set",       true,  false },
  { "speed",     true,  true  },
  { "step",      false, false },
  { "stop",      false, false },
  { "symbols",   false, false },
  { "write",     true,  false },
  { "write16",   true,  false },
  { "write32",   true,  false },
  { NULL,        false, false }
};

#ifdef READLINE
static const char *find_partial_command(const char *text, int *index)
{
  int len = strlen(text);

  //while (*index < (int)(sizeof(command_names) / sizeof(char *)))

  for (int i = *index; command_names[i].name != NULL; i++)
  {
    const char *name = command_names[i].name;
    *index = i + 1;

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

  for (int i = 0; command_names[i].name != NULL; i++)
  {
    name = command_names[i].name;

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

  // Suppress default completion.
  rl_attempted_completion_over = 1;

  char *s = rl_line_buffer;
  while (*s != 0 && *s == ' ') { s++; }

  // Check if buffer already contains a command.
  n = 0;

  while (n < (int)sizeof(command) - 1)
  {
    command[n] = s[n];

    if (s[n] == 0 || s[n] == ' ') { break; }
    n++;
  }

  command[n] = 0;

  if (n >= (int)sizeof(command) - 1) { return NULL; }

  name = find_command(command);

  if (name != NULL) { return NULL; }

  return rl_completion_matches(text, command_name_generator);
}
#endif

static void print_help()
{
  printf("Commands:\n"
    "  asm                       [ start interactive asm at last address ]\n"
    "  asm <org>                 [ start interactive asm at address ]\n"
    "  break <address>           [ break at address ]\n"
    "  call <address>            [ call function at address ]\n"
    "  clear <status flag>       [ clear a bit in the status register]\n"
    "  disasm                    [ disassemble at address ]\n"
    "  disasm <start>-<end>      [ disassemble range of addresses ]\n"
    "  display                   [ toggle display cpu info while simulating ]\n"
    "  print <start>-<end>       [ print bytes at start address (opt. to end) ]\n"
    "  print16 <start>-<end>     [ print int16's at start address (opt. to end) ]\n"
    "  print32 <start>-<end>     [ print int32's at start address (opt. to end) ]\n"
    "  dump_ram <start>-<end>    [ Dump RAM of AVR8 during simulation]\n"
    "  info                      [ general info ]\n"
    "  push <value>              [ push value on stack ]\n"
    "  reset                     [ reset program ]\n"
    "  registers                 [ dump registers ]\n"
    "  run, stop, step           [ simulation run, stop, step ]\n"
    "  set <reg>=<value>         [ set register to value ]\n"
    //"  set <status flag>         [ set a bit in the status register]\n"
    "  speed <speed in Hz>       [ simulation speed or 0 for single step ]\n"
    "  symbols                   [ show symbols ]\n"
    "  write <address> <data>..  [ write multiple bytes to RAM starting at address]\n"
    "  write16 <address> <data>..[ write multiple int16's to RAM starting at address]\n"
    "  write32 <address> <data>..[ write multiple int32's to RAM starting at address]\n");
}

int assemble_code(
  UtilContext &util_context,
  const char *cpu_name,
  const char *code,
  uint32_t &org)
{
  AsmContext asm_context;
  int i;

  asm_context.init();
  asm_context.set_cpu(cpu_name);
  asm_context.set_org(org);
  asm_context.pass = 1;

  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);

  i = assemble(&asm_context);

  if (i != 0)
  {
    printf("Error assembling in pass 1...\n");
    tokens_close(&asm_context);
    return -1;
  }

  asm_context.pass = 2;
  asm_context.init();
  asm_context.set_cpu(cpu_name);
  asm_context.set_org(org);

  i = assemble(&asm_context);

  if (i != 0)
  {
    printf("Error assembling in pass 2...\n");
    tokens_close(&asm_context);
    return -1;
  }

  uint32_t address;

  for (address = asm_context.memory.low_address;
       address <= asm_context.memory.high_address;
       address++)
  {
    uint8_t value = asm_context.memory.read8(address);
    util_context.memory.write8(address, value);
  }

  org = asm_context.memory.high_address + 1;

  tokens_close(&asm_context);

  return 0;
}

bool is_command_valid(String &command, String &arg)
{
  bool has_arg = arg.len() != 0;

  for (int i = 0; command_names[i].name != NULL; i++)
  {
    if (command.equals(command_names[i].name))
    {
      if (command_names[i].has_arg == false) 
      {
        if (has_arg)
        {
          printf("Error: %s doesn't take an argument.\n", command.value());
          return false;
        }
      }
        else
      {
        if (command_names[i].is_optional == false && has_arg == false) 
        {
          printf("Syntax error: %s requires argument(s)\n", command.value());
          return false;
        }
      }

      return true;
    }
  }

  if (command.equals("")) { return true; }

  printf("Unknown command: %s\n", command.value());

  return false;
}

int main(int argc, char *argv[])
{
  FILE *src = NULL;
  UtilContext util_context;
  const char *state = state_stopped;
  String command;
#ifdef READLINE
  char *line = NULL;
#endif
  uint32_t start_address = 0;
  uint32_t set_pc = 0xffffffff;
  int i;
  int mode = MODE_INTERACTIVE;
  int break_io = -1;
  int error_flag = 0;
  const char *filename = NULL;
  const char *cpu_name = NULL;
  int file_type = FILE_TYPE_AUTO;
  String code;
  bool in_code = false;
  bool was_pc_set = false;
  uint32_t org = 0;

  printf(
    "\n"
    "naken_util - by Michael Kohn\n"
    "                Joe Davisson\n"
    "    Web: https://www.mikekohn.net/\n"
    "  Email: mike@mikekohn.net\n\n"
    "Version: " VERSION "\n\n");

  if (argc < 2)
  {
    print_usage();
    exit(0);
  }

  util_init(&util_context);

  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      if (util_is_supported_cpu(argv[i] + 1) == 1)
      {
        cpu_name = argv[i] + 1;
        continue;
      }
    }

    if (strcmp(argv[i], "-disasm") == 0)
    {
       command = "disasm";
       mode = MODE_DISASM;
    }
      else
    if (strcmp(argv[i], "-disasm_range") == 0)
    {
       command = "disasm";
       command += ' ';
       command += argv[++i];
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
      file_type = FILE_TYPE_BIN;
    }
      else
    if (strcmp(argv[i], "-run") == 0)
    {
       command = "run";
       mode = MODE_RUN;
    }
      else
    if (strcmp(argv[i], "-sim_serial") == 0)
    {
      i++;

      if (i + 3 > argc)
      {
        printf("Error: -sim_serial needs 3 arguments\n");
        exit(1);
      }

      int address = strtol(argv[i], NULL, 0);
      util_context.simulate->init_serial(address, argv[i + 1], argv[i + 2]);
      i += 3;
    }
      else
    if (argv[i][0] == '-')
    {
      printf("Unknown option %s\n", argv[i]);
      exit(1);
    }
      else
    {
      filename = argv[i];
    }
  }

  if (filename == NULL && mode != MODE_INTERACTIVE)
  {
    printf("Error: No file selected to load.  Exiting...\n");
    exit(1);
  }

  if (filename != NULL)
  {
    int ret = file_read(
      filename,
      &util_context,
      &file_type,
      cpu_name,
      start_address);

    if (ret != 0)
    {
      printf("Error: Cannot load %s.\n", filename);
      exit(1);
    }

     const char *file_type_name = file_get_file_type_name(file_type);

     printf("Loaded %s of type %s / %s from 0x%04x to 0x%04x\n",
       filename,
       file_type_name,
       util_context.cpu_name,
       util_context.memory.low_address,
       util_context.memory.high_address);
  }
    else
  {
    util_set_cpu_by_name(&util_context, cpu_name);
  }

#if 0
  if (read_amiga(argv[i], &util_context.memory) >= 0)
  {
    util_set_cpu_by_name(&util_context, "68000");

    printf("Loaded Amiga (hunk) exe %s from 0x%04x to 0x%04x\n",
      argv[i],
      util_context.memory.low_address,
      util_context.memory.high_address);
  }
#endif

  util_context.simulate->reset();

  if (mode == MODE_RUN)
  {
    util_context.simulate->set_delay(1);
    util_context.simulate->enable_show();
    util_context.simulate->enable_auto_run();
  }

  util_context.simulate->set_break_io(break_io);

  if (set_pc != 0xffffffff)
  {
    util_context.simulate->set_pc(set_pc);
  }

  printf("Type help for a list of commands.\n");

  while (true)
  {
    if (mode == MODE_INTERACTIVE)
    {
#ifndef READLINE
      printf("%s> ", state);
      fflush(stdout);
      char temp[1024];
      if (fgets(temp, 1023, stdin) == NULL) { break; }
      temp[1023] = 0;
      command = temp;
#else
      char prompt[32];
      snprintf(prompt, sizeof(prompt), "%s> ", state);
      rl_attempted_completion_function = command_name_completion;
      line = readline(prompt);

      if (!(line == NULL || line[0] == 0))
      {
        add_history(line);
        command = line;
      }
#if 0
        else
      {
        command.clear();
      }
#endif
#endif
    }

    command.trim();

    String arg;
    int space = command.find(' ');

    if (space != -1)
    {
      arg = command.value() + space;
      arg.trim();

      command.replace_at(space, 0);
      command.rtrim();
    }

    if (is_command_valid(command, arg) == false) { continue; }

    bool has_arg = arg.len() != 0;

    // Assembler mode.
    if (in_code)
    {
      if (command.len() == 0)
      {
        if (code.len() > 0)
        {
          printf("Assembling to 0x%04x\n", org);

          if (! was_pc_set)
          {
            if (org != 0)
            {
              util_context.simulate->set_pc(org);
              util_context.simulate->set_org(org);
            }

            was_pc_set = true;
          }

          assemble_code(util_context, cpu_name, code.value(), org);
          code.clear();
        }

        state = state_stopped;
        in_code = false;
      }
        else
      {
        code += command.value();
        code += "\n";
      }

      continue;
    }

    if (command.len() == 0)
    {
      if (util_context.simulate->in_step_mode())
      {
        util_context.simulate->run(-1, 1);
      }
      continue;
    }

    if (command == "help" || command == "?")
    {
      print_help();
    }
      else
    if (command == "quit" || command == "exit")
    {
      break;
    }
      else
    if (command == "run")
    {
      state = state_running;

      if (util_context.simulate->get_delay() == 0)
      {
        util_context.simulate->enable_step_mode();
      }

      if (has_arg)
      {
        util_context.simulate->set_pc(arg.as_int());
      }

      int ret = util_context.simulate->run(-1, 0);

      state = state_stopped;

      if (util_context.simulate->in_auto_run() && ret != 0)
      {
         error_flag = 1;
      }

      if (mode == MODE_RUN) { break; }
      continue;
    }
      else
    if (command == "step")
    {
      util_context.simulate->enable_step_mode();
      util_context.simulate->run(-1, 1);
      continue;
    }
      else
    if (command == "call")
    {
      if (util_context.simulate->in_step_mode() == false)
      {
        util_context.simulate->enable_step_mode();
      }

      // FIXME: This is MSP430 specific.
      uint32_t num;

      const char *end = util_get_address(&util_context, arg.value(), &num);

      if (end == NULL)
      {
        printf("Error: Unknown address '%s'\n", arg.value());
        continue;
      }

      state = state_running;
      util_context.simulate->push(0xffff);
      util_context.simulate->set_reg("r0", num);
      util_context.simulate->run(-1, 0);
      state = state_stopped;
      continue;
    }
      else
    if (command == "stop")
    {
      state = state_stopped;
    }
      else
    if (command == "reset")
    {
      util_context.simulate->reset();
    }
      else
    if (command == "break")
    {
      sim_set_breakpoint(&util_context, arg);
    }
      else
    if (command == "push")
    {
      sim_stack_push(&util_context, arg);
    }
      else
    if (command == "set")
    {
      sim_set_register(&util_context, arg);
    }
      else
    if (command == "clear")
    {
      sim_clear_flag(&util_context, arg);
    }
      else
    if (command == "speed")
    {
      sim_set_speed(&util_context, arg);
    }
      else
    if (command == "print")
    {
      util_print8(&util_context, arg.value());
    }
      else
    if (command == "print16")
    {
      util_print16(&util_context, arg.value());
    }
      else
    if (command == "print32")
    {
      util_print32(&util_context, arg.value());
    }
      else
    if (command == "write")
    {
      util_write8(&util_context, arg.value());
    }
      else
    if (command == "write16")
    {
      util_write16(&util_context, arg.value());
    }
      else
    if (command == "write32")
    {
      util_write32(&util_context, arg.value());
    }
      else
    if (command == "disasm")
    {
      if (has_arg)
      {
        util_disasm(&util_context, arg.value());
      }
        else
      if (util_context.memory.low_address != 0xffffffff)
      {
        util_disasm_range(
          &util_context,
          util_context.memory.low_address,
          util_context.memory.high_address);
      }
    }
      else
    if (command == "symbols")
    {
      util_context.symbols.print(stdout);
    }
      else
    if (command == "dumpram" || command == "dump_ram")
    {
      uint32_t start, end;

      if (util_get_range(&util_context, arg.value(), &start, &end) == -1)
      {
        printf("Illegal range.\n");
      }
        else
      if (util_context.simulate->dump_ram(start, end) == -1)
      {
        printf("This arch doesn't support dump_ram.  Use print / print16.\n");
      }
    }
      else
    if (command == "info")
    {
      sim_show_info(&util_context);
    }
      else
    if (command == "registers" || command == "reg")
    {
      util_context.simulate->dump_registers();
    }
      else
    if (command == "display")
    {
      if (util_context.simulate->get_show() == false)
      {
        printf("Display now turned on.\n");
        util_context.simulate->enable_show();
      }
        else
      {
        printf("Display now turned off.\n");
        util_context.simulate->disable_show();
      }
    }
      else
    if (command == "asm")
    {
      if (has_arg) { org = arg.as_int(); }
      in_code = true;
      state = reading_code;
    }
      else
    {
      printf("Unknown command: %s\n", command.value());
    }

    if (mode != MODE_INTERACTIVE) { break; }

    util_context.simulate->disable_step_mode();
  }

  if (mode == MODE_RUN)
  {
    util_context.simulate->dump_registers();
  }

  if (src != NULL) { fclose(src); }

  return error_flag == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

