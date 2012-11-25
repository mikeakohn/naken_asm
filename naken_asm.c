/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "assembler.h"
//#include "disasm_msp430.h"
#include "write_elf.h"
#include "write_hex.h"
#include "lookup_tables.h"
#include "macros.h"
#include "version.h"

#define FORMAT_HEX 0
#define FORMAT_BIN 1
#define FORMAT_ELF 2

static void new_extension(char *filename, char *ext, int len)
{
int i;

  i=strlen(filename)-1;
  if (i+2>len)
  {
    printf("Internal error: filename too long %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  while(i>0)
  {
    if (filename[i]=='.')
    {
      strcpy(filename+i+1,ext);
      break;
    }

    i--;
  }

  if (i==0)
  {
    strcat(filename,ext);
  }

}

static void output_hex_text(FILE *fp, char *s, int ptr)
{
  if (ptr==0) return;
  s[ptr]=0;
  int n;
  for (n=0; n<((16-ptr)*3)+2; n++) { putc(' ', fp); }
  fprintf(fp, "%s", s);
}

int main(int argc, char *argv[])
{
FILE *out;
FILE *dbg=NULL;
//FILE *list=NULL;
int i;
int format=FORMAT_HEX;
int create_list=0;
char *infile=NULL,*outfile=NULL;
struct _asm_context asm_context;
int error_flag=0;

  printf("\nnaken_asm - by Michael Kohn (base code, 805x, ARM, dsPIC, MIPS, MSP430)\n");
  printf("               Joe Davisson (65xx)\n");
  printf("    Web: http://www.mikekohn.net/\n");
  printf("  Email: mike@mikekohn.net\n\n");
  printf("Version: "VERSION"\n\n");

  if (argc<2)
  {
    printf("Usage: naken_asm [options] <infile>\n");
    printf("   -o <outfile>\n");
    printf("   -h             [output hex file]\n");
#ifndef DISABLE_ELF
    printf("   -e             [output elf file]\n");
#endif
    printf("   -b             [output binary file]\n");
    printf("   -d             [create .ndbg debug file]\n");
    printf("   -l             [create .lst listing file]\n");
    printf("   -I             [add to include path]\n");
    printf("\n");
    exit(0);
  }

  memset(&asm_context, 0, sizeof(asm_context));

  for (i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "-o")==0)
    {
      outfile=argv[++i];
    }
      else
    if (strcmp(argv[i], "-h")==0)
    {
      format=FORMAT_HEX;
    }
      else
    if (strcmp(argv[i], "-b")==0)
    {
      format=FORMAT_BIN;
    }
#ifndef DISABLE_ELF
      else
    if (strcmp(argv[i], "-e")==0)
    {
      format=FORMAT_ELF;
    }
#endif
      else
    if (strcmp(argv[i], "-d")==0)
    {
      asm_context.debug_file=1;
    }
      else
    if (strcmp(argv[i], "-l")==0)
    {
      create_list=1;
    }
      else
    if (strncmp(argv[i], "-I", 2)==0)
    {
      char *s=argv[i];
      if (s[2]==0)
      {
        if (add_to_include_path(&asm_context, argv[++i])!=0)
        {
          printf("Internal Error:  Too many include paths\n");
          exit(1);
        }
      }
        else
      {
        if (add_to_include_path(&asm_context, s+2)!=0)
        {
          printf("Internal Error:  Too many include paths\n");
          exit(1);
        }
      }
    }
      else
    {
      infile=argv[i];
    }
  }

  if (infile==NULL)
  {
    printf("No input file specified.\n");
    exit(1);
  }

  if (outfile==NULL)
  {
    if (format==FORMAT_HEX)
    {
      outfile="out.hex";
    }
      else
    if (format==FORMAT_BIN)
    {
      outfile="out.bin";
    }
#ifndef DISABLE_ELF
      else
    if (format==FORMAT_ELF)
    {
      outfile="out.elf";
    }
#endif
  }

#ifdef INCLUDE_PATH
  if (add_to_include_path(&asm_context, INCLUDE_PATH)!=0)
  {
    printf("Internal Error:  Too many include paths\n");
    exit(1);
  }
#endif

  asm_context.in=fopen(infile,"rb");
  if (asm_context.in==NULL)
  {
    printf("Couldn't open %s for reading.\n",infile);
    exit(1);
  }

  asm_context.filename=infile;

  out=fopen(outfile,"wb");
  if (out==NULL)
  {
    printf("Couldn't open %s for writing.\n",outfile);
    exit(1);
  }

  printf(" Input file: %s\n",infile);
  printf("Output file: %s\n",outfile);

  if (asm_context.debug_file==1)
  {
    char filename[1024];
    strcpy(filename,outfile);

    new_extension(filename, "ndbg", 1024);

    dbg=fopen(filename,"wb");
    if (dbg==NULL)
    {
      printf("Couldn't open %s for writing.\n",filename);
      exit(1);
    }

    printf(" Debug file: %s\n",filename);

    fprintf(dbg,"%s\n",infile);
  }

  if (create_list==1)
  {
    char filename[1024];
    strcpy(filename,outfile);

    new_extension(filename, "lst", 1024);

    asm_context.list=fopen(filename,"wb");
    if (asm_context.list==NULL)
    {
      printf("Couldn't open %s for writing.\n",filename);
      exit(1);
    }

    printf("  List file: %s\n",filename);
  }

  printf("\n");

  address_heap_init(&asm_context.address_heap);
  defines_heap_init(&asm_context.defines_heap);

  printf("Pass 1...\n");
  asm_context.pass=1;
  assemble_init(&asm_context);
  error_flag=assemble(&asm_context);
  if (error_flag!=0)
  {
    printf("** Errors... bailing out\n");
    unlink(outfile);
  }
    else
  {
    address_heap_lock(&asm_context.address_heap);
    // defines_heap_lock(&asm_context.defines_heap);

    printf("Pass 2...\n");
    asm_context.pass=2;
    assemble_init(&asm_context);
    error_flag=assemble(&asm_context);

    if (format==FORMAT_HEX)
    {
      write_hex(&asm_context, out);
    }
      else
    if (format==FORMAT_BIN)
    {
      write_bin(&asm_context, out);
    }
#ifndef DISABLE_ELF
      else
    if (format==FORMAT_ELF)
    {
      write_elf(&asm_context, out);
    }
#endif

    if (dbg!=NULL)
    {
      for (i=0; i<asm_context.memory.size; i++)
      {
        int debug_line=memory_debug_line(&asm_context, i);
        putc(debug_line>>8,dbg);
        putc(debug_line&0xff,dbg);
      }

      fclose(dbg);
    }

  }

  fclose(out);

  if (create_list==1)
  {
    int ch=0;
    char str[17];
    int ptr=0;
    fprintf(asm_context.list, "data sections:");
    for (i=asm_context.memory.low_address; i<=asm_context.memory.high_address; i++)
    {
      if (memory_debug_line(&asm_context, i)==-2)
      {
        if (ch==0)
        {
          if (ptr!=0)
          {
            output_hex_text(asm_context.list, str, ptr);
          }
          fprintf(asm_context.list, "\n%04x:", i);
          ptr=0;
        }

        unsigned char data=memory_read(&asm_context, i);
        fprintf(asm_context.list, " %02x", data);

        if (data>=' ' && data<=120)
        { str[ptr++]=data; }
          else
        { str[ptr++]='.'; }

        ch++;
        if (ch==16) { ch=0; }
      }
        else
      {
        output_hex_text(asm_context.list, str, ptr);
        ch=0;
        ptr=0;
      }
    }
    output_hex_text(asm_context.list, str, ptr);
    fprintf(asm_context.list, "\n\n");

    assemble_print_info(&asm_context, asm_context.list);
  }

  assemble_print_info(&asm_context, stdout);

  address_heap_free(&asm_context.address_heap);
  defines_heap_free(&asm_context.defines_heap);

  if (asm_context.list!=NULL) { fclose(asm_context.list); }
  fclose(asm_context.in);

  if (error_flag!=0)
  {
    printf("*** Failed ***\n\n");
    unlink(outfile);
  }

  memory_free(&asm_context.memory);

  if (error_flag!=0) { return -1; }

  return 0;
}


