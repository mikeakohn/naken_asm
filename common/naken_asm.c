/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common/assembler.h"
#include "common/directives_include.h"
#include "common/macros.h"
#include "common/symbols.h"
#include "common/tokens.h"
#include "common/version.h"
#include "fileio/write_amiga.h"
#include "fileio/write_bin.h"
#include "fileio/write_elf.h"
#include "fileio/write_hex.h"
#include "fileio/write_srec.h"
#include "fileio/write_wdc.h"

enum
{
  FORMAT_HEX,
  FORMAT_BIN,
  FORMAT_ELF,
  FORMAT_SREC,
  FORMAT_WDC,
  FORMAT_AMIGA,
};

const char *credits =
  "\n"
  "naken_asm\n\n"
  "Authors: Michael Kohn\n"
  "         Joe Davisson\n"
  "    Web: http://www.mikekohn.net/\n"
  "  Email: mike@mikekohn.net\n"
  "Version: " VERSION "\n";

static void new_extension(char *filename, char *ext, int len)
{
  int i;

  i = strlen(filename) - 1;
  if (i + 2 > len)
  {
    printf("Internal error: filename too long %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  while(i > 0)
  {
    if (filename[i] == '.')
    {
      strcpy(filename + i + 1, ext);
      break;
    }

    i--;
  }

  if (i == 0)
  {
    strcat(filename, ".");
    strcat(filename, ext);
  }
}

static void output_hex_text(FILE *fp, char *s, int ptr)
{
  if (ptr == 0) { return; }
  s[ptr] = 0;
  int n;
  for (n = 0; n < ((16 - ptr) * 3) + 2; n++) { putc(' ', fp); }
  fprintf(fp, "%s", s);
}

int main(int argc, char *argv[])
{
  FILE *out;
  FILE *dbg = NULL;
  int i;
  int format = FORMAT_HEX;
  int create_list = 0;
  char *infile = NULL, *outfile = NULL;
  struct _asm_context asm_context;
  int error_flag = 0;

  puts(credits);

  if (argc < 2)
  {
    printf("Usage: naken_asm [options] <infile>\n"
           "   -o <outfile>\n"
           "   -type <hex, elf, bin, srec, amiga, wdc>\n"
           "   -l             [create .lst listing file]\n"
           "   -I             [add to include path]\n"
           "   -q             Quiet (only output errors)\n"
           "   -dump_symbols  Dump all symbols at end of assembly\n"
           "   -dump_macros   Dump all macros at end of assembly\n"
           "   -optimize      Optimize instructions (see docs for info)\n"
           "   -cpu_list      List supported CPUs\n"
           "\n");
    exit(0);
  }

  memset(&asm_context, 0, sizeof(asm_context));

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-cpu_list") == 0)
    {
      printf(
        " Supported CPUs:\n"
        "    1802, 4004, 6502, 65C816, 68HC08, 6809, 68000, 8008, 8048, 8051,\n"
        "    86000, ARM, AVR8, Cell BE, Copper, CP1610, dsPIC, Epiphany,\n"
        "    Java, LC-3, MIPS, MSP430, PDK13, PDK14, PDK15, PIC14, PIC24,\n"
        "    PIC32, Playstation 2 EE, PowerPC, Propeller, Propeller 2, PSoC\n"
        "    M8C, RISC-V, SH-4, STM8, SuperFX, SWEET16, unSP, THUMB, TMS1000,\n"
        "    TMS1100, TMS340, TMS9900, WebAssembly, Xtensa, Z80\n");
      exit(0);
    }

    if (strcmp(argv[i], "-o") == 0)
    {
      outfile = argv[++i];
    }
      else
    if (strcmp(argv[i], "-h") == 0)
    {
      format = FORMAT_HEX;
    }
      else
    if (strcmp(argv[i], "-bin") == 0 || strcmp(argv[i], "-b") == 0)
    {
      format = FORMAT_BIN;
    }
      else
    if (strcmp(argv[i], "-srec") == 0 || strcmp(argv[i], "-s") == 0)
    {
      format = FORMAT_SREC;
    }
#ifndef DISABLE_ELF
      else
    if (strcmp(argv[i], "-elf") == 0 || strcmp(argv[i], "-e") == 0)
    {
      format = FORMAT_ELF;
    }
#endif
      else
    if (strcmp(argv[i], "-wdc") == 0)
    {
      format = FORMAT_WDC;
    }
      else
    if (strcmp(argv[i], "-amiga") == 0)
    {
      format = FORMAT_AMIGA;
    }
      else
    if (strcmp(argv[i], "-type") == 0)
    {
      if (i + 1 >= argc)
      {
        printf("Error: -type takes an option\n");
        exit(1);
      }

      i++;

      if (strcmp(argv[i], "amiga") == 0)
      {
        format = FORMAT_AMIGA;
      }
        else
      if (strcmp(argv[i], "bin") == 0)
      {
        format = FORMAT_BIN;
      }
        else
      if (strcmp(argv[i], "elf") == 0)
      {
        format = FORMAT_ELF;
      }
        else
      if (strcmp(argv[i], "hex") == 0)
      {
        format = FORMAT_HEX;
      }
        else
      if (strcmp(argv[i], "srec") == 0)
      {
        format = FORMAT_SREC;
      }
        else
      if (strcmp(argv[i], "wdc") == 0)
      {
        format = FORMAT_WDC;
      }
        else
      {
        printf("Error: Unknown output type %s\n", argv[i]);
        exit(1);
      }
    }
#if 0
      else
    if (strcmp(argv[i], "-d") == 0)
    {
      asm_context.debug_file = 1;
    }
#endif
      else
    if (strcmp(argv[i], "-l") == 0)
    {
      create_list = 1;
    }
      else
    if (strncmp(argv[i], "-I", 2) == 0)
    {
      char *s = argv[i];

      if (s[2] == 0)
      {
        if (i + 1 >= argc)
        {
          printf("Error: -I takes an option\n");
          exit(1);
        }

        if (include_add_path(&asm_context, argv[++i]) != 0)
        {
          printf("Internal Error:  Too many include paths\n");
          exit(1);
        }
      }
        else
      {
        if (include_add_path(&asm_context, s+2) != 0)
        {
          printf("Internal Error:  Too many include paths\n");
          exit(1);
        }
      }
    }
      else
    if (strcmp(argv[i], "-q") == 0)
    {
      asm_context.quiet_output = 1;
    }
      else
    if (strcmp(argv[i], "-dump_symbols") == 0)
    {
      asm_context.dump_symbols = 1;
    }
      else
    if (strcmp(argv[i], "-dump_macros") == 0)
    {
      asm_context.dump_macros = 1;
    }
      else
    if (strcmp(argv[i], "-optimize") == 0)
    {
      asm_context.optimize = 1;
    }
      else
    {
      if (argv[i][0] == '-')
      {
        printf("Error: Unknown command line argument '%s'\n", argv[i]);
        exit(1);
      }

      int n = assembler_link_file(&asm_context, argv[i]);

      if (n == 0) { continue; }

      if (n != -1)
      {
        error_flag = 1;
        continue;
      }

      if (infile != NULL)
      {
        printf("Error: Cannot use %s as input file since %s was already chosen.\n", argv[i], infile);
        exit(1);
      }

      infile = argv[i];
    }
  }

  if (error_flag != 0)
  {
    exit(1);
  }

  if (infile == NULL)
  {
    printf("No input file specified.\n");
    exit(1);
  }

  if (outfile == NULL)
  {
    switch(format)
    {
      case FORMAT_HEX: outfile = "out.hex"; break;
      case FORMAT_BIN: outfile = "out.bin"; break;
      case FORMAT_ELF: outfile = "out.elf"; break;
      case FORMAT_SREC: outfile = "out.srec"; break;
      case FORMAT_WDC: outfile = "out.wdc"; break;
      case FORMAT_AMIGA: outfile = "out"; break;
      default: outfile = "out.err"; break;
    }
  }

#ifdef INCLUDE_PATH
  if (include_add_path(&asm_context, INCLUDE_PATH) != 0)
  {
    printf("Internal Error:  Too many include paths\n");
    exit(1);
  }
#endif

  if (include_add_path(&asm_context, "include") != 0)
  {
    printf("Internal Error:  Too many include paths\n");
    exit(1);
  }

  if (tokens_open_file(&asm_context, infile) != 0)
  {
    printf("Couldn't open %s for reading.\n", infile);
    exit(1);
  }

  out = fopen(outfile, "wb");
  if (out == NULL)
  {
    printf("Couldn't open %s for writing.\n", outfile);
    exit(1);
  }

  if (asm_context.quiet_output == 0)
  {
    printf(" Input file: %s\n", infile);
    printf("Output file: %s\n", outfile);
  }

#if 0
  if (asm_context.debug_file == 1)
  {
    char filename[1024];
    strcpy(filename, outfile);

    new_extension(filename, "ndbg", 1024);

    dbg = fopen(filename,"wb");
    if (dbg == NULL)
    {
      printf("Couldn't open %s for writing.\n",filename);
      exit(1);
    }

    printf(" Debug file: %s\n",filename);

    fprintf(dbg, "%s\n", infile);
  }
#endif

  if (create_list == 1)
  {
    char filename[1024];
    strcpy(filename, outfile);

    new_extension(filename, "lst", 1024);

    asm_context.list = fopen(filename, "wb");
    if (asm_context.list == NULL)
    {
      printf("Couldn't open %s for writing.\n", filename);
      exit(1);
    }

    if (asm_context.quiet_output == 0)
    {
      printf("  List file: %s\n", filename);
    }
  }

  if (asm_context.quiet_output == 0)
  {
    printf("\nPass 1...\n");
  }

  symbols_init(&asm_context.symbols);
  macros_init(&asm_context.macros);

  asm_context.pass = 1;
  assembler_init(&asm_context);

  error_flag = assemble(&asm_context);

  do
  {
    if (error_flag == 0 && assembler_link(&asm_context) != 0)
    {
      error_flag = 1;
    }

    if (error_flag != 0)
    {
      printf("** Errors... bailing out\n");
      unlink(outfile);
      break;
    }

    symbols_lock(&asm_context.symbols);
    symbols_scope_reset(&asm_context.symbols);
    // macros_lock(&asm_context.defines_heap);

    if (asm_context.quiet_output == 0) { printf("Pass 2...\n"); }
    asm_context.pass = 2;
    assembler_init(&asm_context);

    if (create_list == 1) { asm_context.write_list_file = 1; }

    error_flag = assemble(&asm_context);

    if (error_flag != 0) { break; }

    if (assembler_link(&asm_context) != 0)
    {
      error_flag = 1;
      break;
    }

    if (format == FORMAT_HEX)
    {
      write_hex(&asm_context.memory, out);
    }
      else
    if (format == FORMAT_BIN)
    {
      write_bin(&asm_context.memory, out);
    }
      else
    if (format == FORMAT_SREC)
    {
      write_srec(&asm_context.memory, out, cpu_list[asm_context.cpu_list_index].srec_size);
    }
      else
    if (format == FORMAT_ELF)
    {
      write_elf(&asm_context.memory, out, &asm_context.symbols, asm_context.tokens.filename, asm_context.cpu_type, cpu_list[asm_context.cpu_list_index].alignment);
    }
      else
    if (format == FORMAT_WDC)
    {
      write_wdc(&asm_context.memory, out);
    }
      else
    if (format == FORMAT_AMIGA)
    {
      write_amiga(&asm_context.memory, out);
    }

    if (dbg != NULL)
    {
      for (i = 0; i < asm_context.memory.size; i++)
      {
        int debug_line = memory_debug_line(&asm_context, i);
        putc(debug_line >> 8, dbg);
        putc(debug_line & 0xff, dbg);
      }

      fclose(dbg);
    }
  } while(0);

  fclose(out);

  if (create_list == 1)
  {
    int ch = 0;
    char str[17];
    int ptr = 0;

    fprintf(asm_context.list, "data sections:");

    for (i = asm_context.memory.low_address; i <= asm_context.memory.high_address; i++)
    {
      if (memory_debug_line(&asm_context, i) == -2)
      {
        if (ch == 0)
        {
          if (ptr != 0)
          {
            output_hex_text(asm_context.list, str, ptr);
          }
          fprintf(asm_context.list, "\n%04x:", i/asm_context.bytes_per_address);
          ptr = 0;
        }

        uint8_t data = memory_read(&asm_context, i);
        fprintf(asm_context.list, " %02x", data);

        if (data >= ' ' && data <= 120)
        { str[ptr++] = data; }
          else
        { str[ptr++] = '.'; }

        ch++;
        if (ch == 16) { ch = 0; }
      }
        else
      {
        output_hex_text(asm_context.list, str, ptr);
        ch = 0;
        ptr = 0;
      }
    }
    output_hex_text(asm_context.list, str, ptr);
    fprintf(asm_context.list, "\n\n");

    assembler_print_info(&asm_context, asm_context.list);
  }

  assembler_print_info(&asm_context, stdout);

  //symbols_free(&asm_context.symbols);
  //macros_free(&asm_context.macros);

  if (asm_context.list != NULL) { fclose(asm_context.list); }
  fclose(asm_context.tokens.in);

  if (error_flag != 0)
  {
    printf("*** Failed ***\n\n");
    unlink(outfile);
  }

  //memory_free(&asm_context.memory);
  assembler_free(&asm_context);

  return error_flag == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

