/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "assembler.h"
#include "disasm_65xx.h"
#include "disasm_680x.h"
#include "disasm_68hc08.h"
#include "disasm_680x0.h"
#include "disasm_805x.h"
#include "disasm_arm.h"
#include "disasm_dspic.h"
#include "disasm_mips.h"
#include "disasm_msp430.h"
#include "disasm_stm8.h"
#include "disasm_tms1000.h"
#include "disasm_tms9900.h"
#include "disasm_z80.h"
#include "naken_util.h"
#include "parse_elf.h"
#include "parse_hex.h"
#include "simulate_65xx.h"
#include "simulate_msp430.h"
#include "version.h"

#define READ_RAM(a) memory_read_m(&util_context->memory, a)
#define WRITE_RAM(a,b) memory_write_m(&util_context->memory, a, b)

static char *mode_stopped="stopped";
static char *mode_running="running";

#if 0
static char *flags[] = { "C", "Z", "N", "GIE", "CPUOFF", "OSCOFF", "SCG0",
                       "SCG1", "V" };

static int update_flag(struct _util_context *util_context, char *token, int value)
{
int n;

  while (*token==' ' && *token!=0) token++;

  for (n=0; n<=8; n++)
  {
    if (strcasecmp(token,flags[n])==0)
    {
      if (value==1)
      { util_context->simulate->reg[2]|=(1<<n); }
        else
      { util_context->simulate->reg[2]&=0xffff^(1<<n); }

      printf("%s flag %s.\n", flags[n], value==1?"set":"cleared");

      return 0;
    }
  }

  return -1;
}
#endif

static char *get_hex(char *token, int *num)
{
int s=0;
int n=0;

  while(token[s]!=0 && token[s]!=' ' && token[s]!='-' && token[s]!='h')
  {
    if (token[s]>='0' && token[s]<='9')
    {
      n=(n*16)+(token[s]-'0');
    }
      else
    if (token[s]>='a' && token[s]<='f')
    {
      n=(n*16)+((token[s]-'a')+10);
    }
      else
    if (token[s]>='A' && token[s]<='F')
    {
      n=(n*16)+((token[s]-'A')+10);
    }
      else
    {
      printf("Illegal number '%s'\n", token);
      return NULL;
    }

    s++;
  }

  *num=n;

  if (token[s]!='-') s++;

  return token+s;
}

static char *get_num(char *token, int *num)
{
int s;
int n;

  *num=0;

  while (*token==' ' && *token!=0) token++;
  if (*token==0) return NULL;

  if (token[0]=='0' && token[1]=='x')
  {
    return get_hex(token+2, num);
  }

  // Look for end so we can see if there is an h there
  s=0;
  while(token[s]!=0) s++;
  if (s==0) return NULL;
  if (token[s-1]=='h')
  {
    return get_hex(token, num);
  }

  s=0;
  n=0;
  int sign=1;
  if (token[s]=='-') { s++; sign=-1; }

  while(token[s]!=0 && token[s]!='-')
  {
    if (token[s]>='0' && token[s]<='9')
    {
      n=(n*10)+(token[s]-'0');
    }
      else
    if (token[s]==' ')
    {
      break;
    }
      else
    {
      printf("Illegal number '%s'\n", token);
      return NULL;
    }

    s++;
  }

  *num=(n*sign)&0xffff;

  return token+s;
}

static void load_debug_offsets(struct _util_context *util_context)
{
int line_count=0;
int ch,n;

  fseek(util_context->src_fp, 0, SEEK_SET);
  while(1)
  {
    ch=getc(util_context->src_fp);
    if (ch==EOF || ch=='\n')
    {
      line_count++;
      if (ch==EOF) break;
    }
  }

  util_context->debug_line_offset=(long *)malloc(sizeof(long)*line_count);
#ifdef DEBUG
  printf("Source File: line_count=%d\n", line_count);
#endif
  n=0;
  util_context->debug_line_offset[n++]=0;

  fseek(util_context->src_fp, 0, SEEK_SET);
  while(1)
  {
    ch=getc(util_context->src_fp);
    if (ch==EOF || ch=='\n')
    {
      util_context->debug_line_offset[n++]=ftell(util_context->src_fp);
      if (ch==EOF) break;
    }
    if (n==line_count) break;
  }
}

static int get_range(char *token, int *start, int *end)
{
  token=get_num(token, start);
  if (*start<0) *start=0;
  //if (*start>65535) *start=65535;

  if (token==NULL) return -1;
  while(*token==' ') token++;
  if (*token=='-')
  {
    token++;
    while(*token==' ') token++;
    token=get_num(token, end);
    if (*end<0) *end=0;
    // FIXME - this will need to change for extended memory
    //if (*end>65535) *end=65535;
    if (token==NULL) return -1;
    return 0;
  }

  *end=*start;

  return 0;
}

static void bprint(struct _util_context *util_context, char *token)
{
char chars[17];
int start,end;
int ptr=0;

  // FIXME - is this right?
  if (get_range(token, &start, &end)==-1) return;
  if (start>util_context->memory.size) start=util_context->memory.size;
  if (start==end) end=start+128;
  if (end>util_context->memory.size) end=util_context->memory.size;

  while(start<end)
  {
    if ((ptr&0x0f)==0)
    {
      chars[ptr]=0;
      if (ptr!=0) printf(" %s\n", chars);
      ptr=0;
      printf("0x%04x:", start);
    }

    unsigned char data=READ_RAM(start);
    printf(" %02x", data);

    if (data>=' ' && data<=126)
    { chars[ptr++]=data; }
      else
    { chars[ptr++]='.'; }

    start++;
  }

  chars[ptr]=0;

  if (ptr!=0)
  {
    int n;
    for (n=ptr; n<16; n++) printf("   ");
    printf(" %s\n", chars);
  }
}

static void wprint(struct _util_context *util_context, char *token)
{
char chars[17];
int start,end;
int ptr=0;

  if (get_range(token, &start, &end)==-1) return;
  if (start>util_context->memory.size) start=util_context->memory.size;
  if (start==end) end=start+128;
  if (end>util_context->memory.size) end=util_context->memory.size;

  if ((start&0x01)!=0)
  {
    printf("Address range 0x%04x to 0x%04x must start on a 2 byte boundary.\n", start, end);
    return;
  }

  while(start<end)
  {
    if ((ptr&0x0f)==0)
    {
      chars[ptr]=0;
      if (ptr!=0) printf(" %s\n", chars);
      ptr=0;
      printf("0x%04x:", start);
    }

    unsigned char data0=READ_RAM(start);
    unsigned char data1=READ_RAM(start+1);

    int num=data0|(data1<<8);
    if (data0>=' ' && data0<=126)
    { chars[ptr++]=data0; }
      else
    { chars[ptr++]='.'; }
    if (data1>=' ' && data1<=126)
    { chars[ptr++]=data1; }
      else
    { chars[ptr++]='.'; }

    printf(" %04x", num);

    start=start+2;
  }

  chars[ptr]=0;

  if (ptr!=0)
  {
    int n;
    for (n=ptr; n<16; n+=2) printf("     ");
    printf(" %s\n", chars);
  }
}

static void bwrite(struct _util_context *util_context, char *token)
{
int address=0;
int count=0;
int num;

  while(*token==' ' && *token!=0) token++;
  if (token==0) { printf("Syntax error: no address given.\n"); }
  token=get_num(token,&address);
  if (token==NULL) { printf("Syntax error: bad address\n"); }

  int n=address;
  while(1)
  {
    if (address>=util_context->memory.size) break;
    while(*token==' ' && *token!=0) token++;
    token=get_num(token,&num);
    if (token==0) break;
    WRITE_RAM(address++, num);
    count++;
  }

  printf("Wrote %d bytes starting at address 0x%04x\n", count, n);
}

static void wwrite(struct _util_context *util_context, char *token)
{
int address=0;
int count=0;
int num;

  while(*token==' ' && *token!=0) token++;
  if (token==0) { printf("Syntax error: no address given.\n"); }
  token=get_num(token,&address);
  if (token==NULL) { printf("Syntax error: bad address\n"); }

  if ((address&0x01)!=0)
  {
    printf("Error: wwrite address is not 16 bit aligned\n");
    return;
  }

  int n=address;
  while(1)
  {
    if (address>=util_context->memory.size) break;
    while(*token==' ' && *token!=0) token++;
    token=get_num(token,&num);
    if (token==0) break;
    WRITE_RAM(address++, num&0xff);
    WRITE_RAM(address++, num>>8);
    count++;
  }

  printf("Wrote %d words starting at address %04x\n", count, n);
}

static void disasm(struct _util_context *util_context, char *token, int dbg_flag)
{
int start,end;

  if (get_range(token, &start, &end)==-1) return;

  if ((start%2)!=0 || (end%2)!=0)
  {
    printf("Address range 0x%04x to 0x%04x must be on a 2 byte boundary.\n", start, end);
    return;
  }

  util_context->disasm_range(&util_context->memory, start, end);
}

static void show_info(struct _util_context *util_context)
{
  struct _simulate *simulate=util_context->simulate;

  printf("Start address: 0x%04x (%d)\n",util_context->memory.low_address,util_context->memory.low_address);
  printf("  End address: 0x%04x (%d)\n",util_context->memory.high_address,util_context->memory.high_address);
  printf("  Break Point: ");
  if (simulate->break_point==-1) { printf("<not set>\n"); }
  else { printf("0x%04x (%d)\n", simulate->break_point, simulate->break_point); }

  printf("  Instr Delay: ");
  if (simulate->usec==0) { printf("<step mode>\n"); }
  else { printf("%d us\n", simulate->usec); }
  printf("      Display: %s\n", simulate->show==1?"On":"Off");
}

static void print_help()
{
  printf("Commands:\n");
  printf("  bprint <start>-<end>     [ print bytes at start address (opt. to end) ]\n");
  printf("  wprint <start>-<end>     [ print words at start address (opt. to end) ]\n");
  printf("  bwrite <address> <data>..[ write multiple bytes to RAM starting at address]\n");
  printf("  wwrite <address> <data>..[ write multiple words to RAM starting at address]\n");
  printf("  registers                [ dump registers ]\n");
  printf("  run, stop, step          [ simulation run, stop, step ]\n");
  printf("  call <address>           [ call function at address ]\n");
  printf("  push <value>             [ push value on stack ]\n");
  printf("  set <reg>=<value>        [ set register to value ]\n");
  printf("  set,clear <status flag>  [ set or clear a bit in the status register]\n");
  printf("  reset                    [ reset program ]\n");
  printf("  display                  [ toggle display cpu info while simulating ]\n");
  printf("  speed <speed in Hz>      [ simulation speed or 0 for single step ]\n");
  printf("  break <address>          [ break at address ]\n");
  //printf("  flash                    [ flash device ]\n");
  printf("  info                     [ general info ]\n");
  printf("  disasm                   [ disassemble at address ]\n");
  printf("  disasm <start>-<end>     [ disassemble range of addresses ]\n");
  //printf("  list <start>-<end>       [ disassemble wth debug listing ]\n");
}

static int load_debug(FILE **srcfile, char *filename, struct _util_context *util_context)
{
char src_filename[1024];
FILE *in;
int ch;
int i;

  // Reset debug memory on all loads.
  //memset(debug_line, 0, 65536*sizeof(int));

  printf("Opening %s\n", filename);

  in=fopen(filename,"rb");

  if (in==NULL)
  {
    printf("Could not open debug file %s\n",filename);
    return -1;
  }

  i=0;
  while(1)
  {
    ch=getc(in);
    if (ch==EOF) break;
    if (ch=='\n') break;
    src_filename[i++]=ch;

    if (i>=1022)
    {
      printf("Unknown debug file format.  Aborting.\n");
      fclose(in);
      return -1;
    }
  }

  if (i==0)
  {
    printf("Unknown debug file format.  Aborting.\n");
    fclose(in);
    return -1;
  }

  src_filename[i]=0;

  if (*srcfile==NULL)
  {
    printf("Source filename %s\n",src_filename);
    *srcfile=fopen(src_filename,"rb");
    if (*srcfile==NULL)
    {
      printf("Could not open source file %s\n",src_filename);
    }
    util_context->src_fp=*srcfile;
    load_debug_offsets(util_context);
  }

  int size=util_context->memory.size;
  for (i=0; i<size; i++)
  {
    ch=getc(in);
    if (ch==EOF) break;
    //debug_line[i]=ch<<8;
    ch=(ch<<8)|getc(in);
    //debug_line[i]|=ch;
    // FIXME - <0 isn't possible in this case.. why is this line here
    if (ch<0) ch=-1;
    memory_debug_line_set_m(&util_context->memory, i, ch);

    //if (debug_line[i]<0) debug_line[i]=-1;
  }

  fclose(in);

  return 0;
}

int main(int argc, char *argv[])
{
FILE *src=NULL;
struct _util_context util_context;
char *mode=mode_stopped;
char command[1024];
#ifdef READLINE
char *line=NULL;
#endif
int loaded_debug=0;
int i;
char *hexfile=NULL;
int interactive=1;
//FIXME
//int chip=0;

  printf("\nnaken_util - by Michael Kohn\n");
  printf("    Web: http://www.mikekohn.net/\n");
  printf("  Email: mike@mikekohn.net\n\n");
  printf("Version: "VERSION"\n\n");

  if (argc<2)
  {
    printf("Usage: naken_util [options] <infile>\n");
    printf("   -s      <source file>\n");
    printf("   -d      <debug file>\n");
    printf("    // The following options turn off interactive mode\n");
    printf("   -f      <flash serial port>\n");
    printf("   -disasm                      (disassemble all or part of program)\n");
    printf("ELF files can auto-pick a CPU, if a hex file use:\n");
    printf("   -65xx                        (65xx)\n");
    printf("   -680x                        (680x)\n");
    printf("   -68hc08                      (68hc08)\n");
    printf("   -8051 / -8052                (8051 / 8052)\n");
    printf("   -arm                         (ARM)\n");
    printf("   -dspic                       (dsPIC)\n");
    printf("   -mips                        (MIPS)\n");
    printf("   -msp430                      (MSP430/MSP430X) DEFAULT\n");
    printf("   -stm8                        (STM8)\n");
    printf("   -tms1000                     (TMS1000)\n");
    printf("   -tms1100                     (TMS1100)\n");
    printf("   -tms9900                     (TMS9900)\n");
    printf("   -z80                         (z80)\n");
    //printf("   -list                        (like -disasm, but adds source code)\n");
    printf("\n");
    exit(0);
  }

  memset(&util_context, 0, sizeof(struct _util_context));
  memory_init(&util_context.memory, 1<<20, 1);

  util_context.disasm_range=disasm_range_msp430;
  util_context.simulate=simulate_init_msp430(&util_context.memory);
  //util_context.instr_bytes=2;

  for (i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "-d")==0)
    {
      //if (load_debug(&src, argv[++i], util_context.debug_line, &util_context)==0)
      if (load_debug(&src, argv[++i], &util_context)==0)
      {
        loaded_debug=1;
      }
    }
      else
    if (strcmp(argv[i], "-s")==0)
    {
      if (src!=NULL)
      {
        fclose(src);
        src=fopen(argv[++i],"rb");
        if (src==NULL)
        {
          printf("Could not open source file %s\n",argv[i]);
        }
#if 0
          else
        {
          util_context.src_fp=src;
          load_debug_offsets(&util_context);
        }
#endif
      }
    }
      else
    if (strcmp(argv[i], "-disasm")==0)
    {
       strcpy(command,"disasm");
       interactive=0;
    }
      else
#if 0
    if (strcmp(argv[i], "-list")==0)
    {
       strcpy(command,"list");
       interactive=0;
    }
      else
#endif
#ifdef ENABLE_MSP430
    if (strcmp(argv[i], "-msp430")==0)
    {
      // Default.. probably dumb to have as an option
      util_context.disasm_range=disasm_range_msp430;
      //util_context.instr_bytes=2;
    }
      else
#endif
#ifdef ENABLE_DSPIC
    if (strcmp(argv[i], "-dspic")==0)
    {
      util_context.disasm_range=disasm_range_dspic;
      //util_context.instr_bytes=4;
    }
      else
#endif
#ifdef ENABLE_65XX
    if (strcmp(argv[i], "-65xx")==0)
    {
      util_context.disasm_range=disasm_range_65xx;
      util_context.simulate=simulate_init_65xx(&util_context.memory);
      //util_context.instr_bytes=1;
      //chip=1;
    }
      else
#endif
#ifdef ENABLE_680X
    if (strcmp(argv[i], "-680x")==0)
    {
      util_context.disasm_range=disasm_range_680x;
    }
      else
#endif
#ifdef ENABLE_68HC08
    if (strcmp(argv[i], "-68hc08")==0)
    {
      util_context.disasm_range=disasm_range_68hc08;
    }
      else
#endif
#ifdef ENABLE_680X0
    if (strcmp(argv[i], "-680x0")==0)
    {
      util_context.disasm_range=disasm_range_680x0;
    }
      else
#endif
#ifdef ENABLE_8051
    if (strcmp(argv[i], "-8051")==0 || strcmp(argv[i], "-8052")==0)
    {
      util_context.disasm_range=disasm_range_805x;
    }
      else
#endif
#ifdef ENABLE_ARM
    if (strcmp(argv[i], "-arm")==0)
    {
      util_context.disasm_range=disasm_range_arm;
      //util_context.instr_bytes=4;
    }
      else
#endif
#ifdef ENABLE_MIPS
    if (strcmp(argv[i], "-mips")==0)
    {
      util_context.disasm_range=disasm_range_mips;
    }
      else
#endif
#ifdef ENABLE_STM8
    if (strcmp(argv[i], "-stm8")==0)
    {
      util_context.disasm_range=disasm_range_stm8;
    }
      else
#endif
#ifdef ENABLE_TMS1000
    if (strcmp(argv[i], "-tms1000")==0)
    {
      util_context.disasm_range=disasm_range_tms1000;
    }
      else
    if (strcmp(argv[i], "-tms1100")==0)
    {
      util_context.disasm_range=disasm_range_tms1100;
    }
      else
#endif
#ifdef ENABLE_TMS9900
    if (strcmp(argv[i], "-tms9900")==0)
    {
      util_context.disasm_range=disasm_range_tms9900;
    }
      else
#endif
#ifdef ENABLE_Z80
    if (strcmp(argv[i], "-z80")==0)
    {
      util_context.disasm_range=disasm_range_z80;
    }
      else
#endif
    {
      unsigned char cpu_type;
      if (parse_elf(argv[i], &util_context.memory, &cpu_type)>=0)
      {
        switch(cpu_type)
        {
#ifdef ENABLE_MSP430
          case CPU_TYPE_MSP430:
            util_context.disasm_range=disasm_range_msp430;
            util_context.simulate=simulate_init_msp430(&util_context.memory);
            break;
#endif
#ifdef ENABLE_65XX
          case CPU_TYPE_65XX:
            util_context.disasm_range=disasm_range_65xx;
            util_context.simulate=simulate_init_65xx(&util_context.memory);
            break;
#endif
#ifdef ENABLE_ARM
          case CPU_TYPE_ARM:
            util_context.disasm_range=disasm_range_arm;
            break;
#endif
#ifdef ENABLE_DSPIC
          case CPU_TYPE_DSPIC:
            util_context.disasm_range=disasm_range_dspic;
            break;
#endif
#ifdef ENABLE_MIPS
          case CPU_TYPE_MIPS:
            util_context.disasm_range=disasm_range_mips;
            break;
#endif
          default:
            printf("Internal error %s:%d\n", __FILE__, __LINE__);
            exit(1);
        }
        hexfile=argv[i];
        printf("Loaded elf %s from 0x%04x to 0x%04x\n", argv[i], util_context.memory.low_address, util_context.memory.high_address);
      }
        else
      if (parse_hex(argv[i], &util_context.memory)>=0)
      {
        hexfile=argv[i];
        printf("Loaded hexfile %s from 0x%04x to 0x%04x\n", argv[i], util_context.memory.low_address, util_context.memory.high_address);
      }
        else
      {
        printf("Could not load hexfile\n");
      }
    }
  }

  if (hexfile==NULL)
  {
    printf("No hexfile loaded.  Exiting...\n");
    exit(1);
  }

  util_context.simulate->simulate_reset(util_context.simulate);

  if (loaded_debug==0)
  {
    char filename[1024];
    strcpy(filename,hexfile);
    i=strlen(filename)-1;
    while(i>0)
    {
      if (filename[i]=='.')
      {
        strcpy(filename+i,".ndbg");
        break;
      }

      i--;
    }

    if (i==0)
    {
      strcat(filename,".ndbg");
    }

    printf("Attempting to load debug file %s\n", filename);

    //if (load_debug(&src, filename, util_context.debug_line, &util_context)==0)
    if (load_debug(&src, filename, &util_context)==0)
    {
      printf("Loaded.\n");
      loaded_debug=1;
    }
      else
    {
      printf("Debug file not found.\n");
    }
  }

#if 0
  if (interactive==1)
  {
    if (chip==0)
    {
      util_context.simulate=simulate_init_msp430();
    }
      else
    if (chip==1)
    {
      util_context.simulate=simulate_init_65xx();
    }
  }
#endif

  printf("Type help for a list of commands.\n");
  command[1023]=0;

  while(1)
  {
    if (interactive==1)
    {
#ifndef READLINE
      printf("%s> ",mode);
      fflush(stdout);
      if (fgets(command, 1023, stdin)==NULL) break;
      command[1023]=0;
#else
      char prompt[32];
      sprintf(prompt, "%s> ", mode);
      line=readline(prompt);
      if (!(line==NULL || line[0]==0))
      {
        add_history(line);
        // FIXME - this stinks.
        strncpy(command, line, 1023);
        command[1023]=0;
      }
        else
      {
        command[0]=0;
      }
#endif
    }

    i=0;
    while(command[i]!=0)
    {
      if (command[i]=='\n' || command[i]=='\r')
      { command[i]=0; break; }
      i++;
    }

    if (command[0]==0)
    {
      if (util_context.simulate->step_mode==1)
      {
        util_context.simulate->simulate_run(util_context.simulate, -1, 1);
      }
      continue;
    }

    if (strcmp(command, "help")==0 || strcmp(command, "?")==0)
    {
      print_help();
    }
      else
    if (strcmp(command, "quit")==0 || strcmp(command, "exit")==0)
    {
      break;
    }
      else
    if (strncmp(command, "run", 3)==0 && (command[3]==0 || command[3]==' '))
    {
      mode=mode_running;
      if (util_context.simulate->usec==0) { util_context.simulate->step_mode=1; }
      util_context.simulate->simulate_run(util_context.simulate, (command[3]==0)?-1:atoi(command+4), 0);
      mode=mode_stopped;
      continue;
    }
      else
    if (strcmp(command, "step")==0)
    {
      util_context.simulate->step_mode=1;
      util_context.simulate->simulate_run(util_context.simulate, -1, 1);
      continue;
    }
      else
    if (strncmp(command, "call ", 4)==0)
    {
      mode=mode_running;
      if (util_context.simulate->usec==0) { util_context.simulate->step_mode=1; }
      int a;
      get_num(command+5, &a);
      util_context.simulate->simulate_push(util_context.simulate, 0xffff);
      util_context.simulate->simulate_set_reg(util_context.simulate, "r0", a);
#if 0
      if (util_context.simulate->reg[1]==0)
      {
        util_context.simulate->simulate_set(util_context.simulate, "r1", 0x0800);
      }
#endif
      util_context.simulate->simulate_run(util_context.simulate, -1, 0);
      mode=mode_stopped;
      continue;
    }
      else
    if (strcmp(command, "stop")==0)
    {
      mode=mode_stopped;
    }
      else
    if (strcmp(command, "reset")==0)
    {
      util_context.simulate->simulate_reset(util_context.simulate);
    }
      else
    if (strcmp(command, "break")==0)
    {
      printf("Breakpoint removed.\n");
      util_context.simulate->break_point=-1;
    }
      else
    if (strncmp(command, "break ", 6)==0)
    {
      int a;
      get_num(command+6, &a);
      if ((a&1)==0)
      {
        printf("Breakpoint added at 0x%04x.\n", a);
        util_context.simulate->break_point=a;
      }
        else
      {
        printf("Address 0x%04x is not 16 bit aligned.  Breakpoint not set.\n",a);
      }
    }
      else
    if (strncmp(command, "push ", 5)==0)
    {
      int a;
      get_num(command+5, &a);
      util_context.simulate->simulate_push(util_context.simulate, a);
      printf("Pushed 0x%04x.\n", a);
    }
      else
    if (strncmp(command, "set ", 4)==0)
    {
      char *s=command+4;
      while(*s!=0)
      {
        if (*s=='=')
        {
          *s=0;
          s++;
          int a;
          get_num(s, &a);
          if (util_context.simulate->simulate_set_reg(util_context.simulate, command+4, a)==0)
          {
            printf("Register %s set to 0x%04x.\n", command+4, a);
          }
          else
          {
            printf("Syntax error.\n");
          }
          break;
        }
        s++;
      }

      if (*s==0)
      {
        if (util_context.simulate->simulate_set_reg(util_context.simulate, command+4, 1)==0)
        {
          printf("Flag %s set.\n", command+4);
        }
          else
        {
          printf("Syntax error.\n");
        }
      }
/*
      i=get_reg(command+4, &r);
      if (r==-1)
      {
        if (update_flag(&util_context, command+4, 1)!=0)
        {
          printf("Syntax error.\n");
        }
      }
        else
      {
        get_num(command+4+i, &a);
        if (util_context.simulate->simulate_set(util_context.simulate, command+4, a)==0)
        {
          printf("%s set to 0x%04x.\n", command+4, a);
        }
      }
*/
    }
      else
    if (strncmp(command, "clear ",6)==0)
    {
      if (util_context.simulate->simulate_set_reg(util_context.simulate, command+6, 0)==0)
      {
        printf("Flag %s set.\n", command+4);
      }
        else
      {
        printf("Syntax error.\n");
      }
/*
      if (update_flag(&util_context, command+6, 0)!=0)
      {
        printf("Syntax error.\n");
      }
*/
    }
      else
    if (strncmp(command, "speed ", 6)==0)
    {
      int a=atoi(command+6);
      if (a==0)
      {
        util_context.simulate->usec=0;
        printf("Simulator now in single step mode.\n");
      }
        else
      {
        util_context.simulate->usec=(1000000/a);
        printf("Instruction delay is now %dus\n", util_context.simulate->usec);
      }
    }
      else
    if (strncmp(command, "bprint ", 7)==0)
    {
       bprint(&util_context, command+7);
    }
      else
    if (strncmp(command, "wprint ", 7)==0)
    {
       wprint(&util_context, command+7);
    }
      else
    if (strncmp(command, "bwrite ", 7)==0)
    {
       bwrite(&util_context, command+7);
    }
      else
    if (strncmp(command, "wwrite ", 7)==0)
    {
       wwrite(&util_context, command+7);
    }
      else
    if (strncmp(command, "print ", 6)==0)
    {
       wprint(&util_context, command+6);
    }
      else
    if (strncmp(command, "disasm ", 7)==0)
    {
       disasm(&util_context, command+7, 0);
    }
      else
    if (strcmp(command, "disasm")==0)
    {
       util_context.disasm_range(&util_context.memory, util_context.memory.low_address, util_context.memory.high_address);
    }
#if 0
      else
    if (strncmp(command, "list ", 7)==0)
    {
       if (util_context.debug_line_offset==NULL)
       {
         printf("Error: Must load debug file first.\n");
       }
         else
       {
         disasm(&util_context, command+7, 1);
       }
    }
      else
    if (strcmp(command, "list")==0)
    {
       if (util_context.debug_line_offset==NULL)
       {
         printf("Error: Must load debug file first.\n");
       }
         else
       {
         disasm_range(&util_context, util_context.memory.low_address, util_context.memory.high_address);
       }
    }
#endif
      else
    if (strcmp(command, "info")==0)
    {
      show_info(&util_context);
    }
      else
    if (strcmp(command, "registers")==0 || strcmp(command, "reg")==0)
    {
      util_context.simulate->simulate_dump_registers(util_context.simulate);
    }
      else
    if (strcmp(command, "display")==0)
    {
      if (util_context.simulate->show==0)
      {
        printf("display now turned on\n");
        util_context.simulate->show=1;
      }
        else
      {
        printf("display now turned off\n");
        util_context.simulate->show=0;
      }
    }
      else
    if (strcmp(command, "read")==0)
    {
#ifdef JTAG
      //open_jtag(&util_context, "/dev/ttyUSB0");
      open_jtag(&util_context, "/dev/ttyACM0");
      int ret=read_memory(&util_context, 0, 255);
      if (ret!=0)
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

    if (interactive==0) break;
    util_context.simulate->step_mode=0;
  }

  if (src!=NULL) { fclose(src); }

  if (util_context.debug_line_offset!=NULL)
  { free(util_context.debug_line_offset); }

  if (util_context.simulate!=NULL)
  { util_context.simulate->simulate_free(util_context.simulate); }

  memory_free(&util_context.memory);

  return 0;
}


