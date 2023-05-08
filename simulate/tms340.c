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
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "asm/tms340.h"
#include "disasm/tms340.h"
#include "simulate/tms340.h"

#define SET_V(s) {(s)->st &= ~(1 << 28);}
#define CLR_V(s) {(s)->st |= 1 << 18;}
#define SET_Z(s) {(s)->st &= ~(1 << 29);}
#define CLR_Z(s) {(s)->st |= 1 << 29;}
#define SET_C(s) {(s)->st &= ~(1 << 30);}
#define CLR_C(s) {(s)->st |= 1 << 30;}
#define SET_N(s) {(s)->st &= ~(1 << 31);}
#define CLR_N(s) {(s)->st |= 1 << 31;}

#define TST_V(s) ((s->st >> 28) & 1)
#define TST_Z(s) ((s->st >> 29) & 1)
#define TST_C(s) ((s->st >> 30) & 1)
#define TST_N(s) ((s->st >> 31) & 1)

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static int get_hexdigit(char *str)
{
  if (str[0] >= '0' && str[0] <= '9') {
    return str[0] - '0';
  } else if (str[0] >= 'A' && str[0] <= 'F') {
    return str[0] - 'A';
  } else if (str[0] >= 'a' && str[0] <= 'f') {
    return str[0] - 'a';
  }
  return -1;
}


static int get_register_tms340(char *token)
{
  if (token[0]=='a' || token[0]=='A') {
    int h = get_hexdigit(str+1);
    if (token[2] == 0 && h >= 0 && h < 15) {
      return h;
    }
  } else if (token[0] == 'b' || token[0] == 'B') {
    int h = get_hexdigit(str+1);
    if (token[2] == 0 && h >= 0 && h < 15) {
      return h + 16;
    }
  }
  return -1;
}

struct _simulate *simulate_init_tms340(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_tms340) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_tms340;
  simulate->simulate_free = simulate_free_tms340;
  simulate->simulate_dumpram = simulate_dumpram_tms340;
  simulate->simulate_push = simulate_push_tms340;
  simulate->simulate_set_reg = simulate_set_reg_tms340;
  simulate->simulate_get_reg = simulate_get_reg_tms340;
  simulate->simulate_set_pc = simulate_set_pc_tms340;
  simulate->simulate_reset = simulate_reset_tms340;
  simulate->simulate_dump_registers = simulate_dump_registers_tms340;
  simulate->simulate_run = simulate_run_tms340;

  simulate->memory    = memory;
  simulate_reset_tms340(simulate);
  simulate->usec      = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show      = 1;   // Show simulation
  simulate->auto_run  = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_tms340(struct _simulate *simulate, uint32_t value)
{
//struct _simulate_tms9900 *simulate_tms9900 = (struct _simulate_tms9900 *)simulate->context;

  printf("tms340::push - I have no idea what this should do...\n");
}

int simulate_set_reg_tms340(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int reg,n;

  while(*reg_string == ' ') { reg_string++; }
 
  reg = get_register_tms340(reg_string);
  if (reg >= 0 && reg < 15) {
    simulate_tms340->a[reg]      = value;
    return 0;
  } else if (reg >= 16 && reg < 31) {
    simulate_tms340->b[reg - 16] = value;
    return 0;
  } else if (!strcasecmp(reg_string,"pc")) {
    simulate_tms340->pc          = value;
    return 0;
  } else if (!strcasecmp(reg_string,"sp")) {
    simulate_tms340->sp          = value;
    return 0;
  } else if (!strcasecmp(reg_string,"st")) {
    simulate_tms340->st          = value;
    return 0;
  }

  return -1;
}

uint32_t simulate_get_reg_tms340(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int reg;

  reg = get_register_tms340(reg_string);
  if (reg >= 0 && reg < 15) {
    return simulate_tms340->a[reg];
  } else if (reg >= 16 && reg < 31) {
    return simulate_tms340->b[reg - 16];
  } else if (!strcasecmp(reg_string,"pc")) {
    return simulate_tms340->pc;
  } else if (!strcasecmp(reg_string,"sp")) {
    return simulate_tms340->sp;
  } else if (!strcasecmp(reg_string,"st")) {
    return simulate_tms340->st;
  } else {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }
}

void simulate_set_pc_tms340(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;

  simulate_tms340->pc = value; // thor: why did this set the PC to 0?
}

void simulate_reset_tms340(struct _simulate *simulate)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int i;
  
  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  //memset(simulate_tms9900->reg, 0, sizeof(simulate_tms9900->reg));
  //memory_clear(&simulate->memory);
  //simulate_tms9900->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  //simulate_tms9900->reg[1] = 0x800;
  for(i = 0;i < 15;i++) {
    simulate_tms340->a[i] = 0;
    simulate_tms340->b[i] = 0;
  }
  simulate_tms340->pc   = 0;
  simulate_tms340->st   = 0;
  simulate_tms340->sp   = 0;
  simulate->break_point = -1;
}

void simulate_free_tms340(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

int simulate_dumpram_tms340(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_tms340(struct _simulate *simulate)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  int n;
  uint32_t st = simulate_tms340->st;

  printf("\nSimulation Register Dump\n");
  printf("-------------------------------------------------------------------\n");
  printf("Status:  Flags  PBX  IE  FE1,FS1  FE0,FS1\n");
  printf("         %c%c%c%c   %d   %d   %c %2d    %c %2d\n",
	 ((st >> 31) & 1)?'N':' ',
	 ((st >> 30) & 1)?'C':' ',
	 ((st >> 29) & 1)?'Z':' ',
	 ((st >> 28) & 1)?'V':' ',
	 ((st >> 26) & 1),
	 ((st >> 21) & 1),
	 ((st >> 11) & 1)?'S':'U',
	 ((st >>  6) & 7),
	 ((st >>  5) & 1)?'S':'U',
	 ((st >>  0) & 7));

  printf(" PC: 0x%08x,  SP: 0x%08x",
         simulate_tms340->pc,
         simulate_tms340->sp);

  for (n = 0; n < 15; n++) {
    printf("a[%x] : 0x%08x   b[%x] : 0x%08x\n",
	   simulate_tms340->a[n],
	   simulate_tms340->b[n]);
  }
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

static uint32_t get_register(struct _simulate_tms340 *s,int r,int reg)
{
  if (reg == 15) {
    return s->sp;
  } else if (r == 0) {
    return s->a[reg];
  } else {
    return s->b[reg];
  }
  return -1;
}

static void set_register(struct _simulate_tms340 *s,int r,int reg,uint32_t v)
{
  if (reg == 15) {
    s->sp = v;
  } else if (r == 0) {
    s->a[reg] = v;
  } else {
    s->b[reg] = v;
  }
}

static uint32_t get_field(struct _simulate *simulate,uint32_t address,int field)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int fsize,fext;
  uint32_t end;
  uint8_t  v = 0;
  int lshift = 0;
  int rshift;
  
  if (field == 0) {
    fsize = (s->st >> 0 ) & 0x1f;
    fext  = (s->st >> 5 ) & 1;
  } else {
    fsize = (s->st >> 6 ) & 0x1f;
    fext  = (s->st >> 11) & 1;
  }
  if (fsize == 0)
    fsize = 32;
  
  end    = address + fsize;
  rshift = address & 0x7; // The bit within the byte at address
  lshift = 0;
  do {
    v       |= (memory_read_m(simulate->memory,address >> 3) >> rshift) << lshift;
    address += 8 - rshift;
    lshift  += 8 - rshift;
    rshift   = 0;
  } while(address < end);

  if (fsize < 32) {
    v &= (1U << fsize) - 1;
    /* Include sign extension */
    if (fext) {
      if (v & (1UL << (fsize - 1))) {
	v |= ~0UL << fsize;
      }
    }
  }

  return v;
}


static uint32_t get_byte(struct _simulate *simulate,uint32_t address)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t end;
  uint8_t  v = 0;
  int lshift = 0;
  int rshift;
  
  end    = address + 8;
  rshift = address & 0x7; // The bit within the byte at address
  lshift = 0;
  do {
    v       |= (memory_read_m(simulate->memory,address >> 3) >> rshift) << lshift;
    address += 8 - rshift;
    lshift  += 8 - rshift;
    rshift   = 0;
  } while(address < end);

  v &= 0xff;
  if (v & 0x80) {
    v |= 0xffffff00;
  }

  return v;
}

static void set_field(struct _simulate *simulate,uint32_t address,int field,uint32_t v)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int fsize;
  uint32_t end;
  uint8_t  fmw,lwm;
  uint8_t  w = 0;
  int lshift;
  
  if (field == 0) {
    fsize = (s->st >> 0 ) & 0x1f;
  } else {
    fsize = (s->st >> 6 ) & 0x1f;
  }
  if (fsize == 0)
    fsize = 32;
  
  end    = address + fsize - 1;

  /* Compute the first and last word mask. This is
  ** the mask that contains the bits to be modified
  */
  fmw    = ~0UL << (address & 0x07);
  lwm    = ~0UL >> (7 - (end     & 0x07));
  /*
  ** Check whether the first and last bit are in the same 
  ** byte. If so, combine the masks
  */
  if ((address >> 3) == (end >> 3)) {
    fwm &= lwm;
    lwm  = fwm;
  }
  lshift = address & 0x07;
  
  do {
    w        = memory_read_m(simulate->memory,address >> 3);
    /* Mask in the modificatons */
    w        = ((v << lshift) & fwm) | (w & ~fwm);
    memory_write_m(simulate->memory,address >> 3,w);
    v      >>= 8 - lshift;
    address += 8 - lshift;
    lshift   = 0;
    /* Compute the next mask */
    if ((address >> 3) == (end >> 3)) {
      fwm = lwm;
    } else {
      fwm = 0xff;
    }
  } while(address <= end);
}


static void set_byte(struct _simulate *simulate,uint32_t address,uint32_t v)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t end;
  uint8_t  fmw,lwm;
  uint8_t  w = 0;
  int lshift;
  
  end    = address + 7;

  /* Compute the first and last word mask. This is
  ** the mask that contains the bits to be modified
  */
  fmw    = ~0UL << (address & 0x07);
  lwm    = ~0UL >> (7 - (end     & 0x07));
  /*
  ** Check whether the first and last bit are in the same 
  ** byte. If so, combine the masks
  */
  if ((address >> 3) == (end >> 3)) {
    fwm &= lwm;
    lwm  = fwm;
  }
  lshift = address & 0x07;
  
  do {
    w        = memory_read_m(simulate->memory,address >> 3);
    /* Mask in the modificatons */
    w        = ((v << lshift) & fwm) | (w & ~fwm);
    memory_write_m(simulate->memory,address >> 3,w);
    v      >>= 8 - lshift;
    address += 8 - lshift;
    lshift   = 0;
    /* Compute the next mask */
    if ((address >> 3) == (end >> 3)) {
      fwm = lwm;
    } else {
      fwm = 0xff;
    }
  } while(address <= end);
}

static void set_long(struct _simulate *simulate,uint32_t adr,uint32_t v)
{
  set_byte(simulate,adr,reg >> 0);
  adr += 8;
  set_byte(simulate,adr,reg >> 8);
  adr += 8;
  set_byte(simulate,adr,reg >> 16);
  adr += 8;
  set_byte(simulate,adr,reg >> 24);
}

static uint32_t get_long(struct _simulate *simulate,uint32_t adr)
{
  uint32_t reg;

  reg  = get_byte(simulate,adr);
  adr += 8;
  reg |= get_byte(simulate,adr) << 8;
  adr += 8;
  reg |= get_byte(simulate,adr) << 16;
  adr += 8;
  reg |= get_byte(simulate,adr) << 24;

  return reg;
}

static uint32_t get_field_size(_simulate_tms340 *s,int field)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int fsize;
  
  if (field == 0) {
    fsize = (s->st >> 0) & 0x1f;
  } else {
    fsize = (s->st >> 6) & 0x1f;
  }

  if (fsize == 0)
    fsize = 32;

  return fsize;
}

typedef int execute(struct _simulate_tms340 *s,struct _table_tms340 *t,uint16_t opcode);

/*
** Instruction executer
*/
static int abs(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int32_t src;

  src = get_register(s,r,rd);
  if (src == (1L << 31)) {
    CLR_N(s);
    SET_V(s);
    CLR_V(s);
  } else if (src < 0) {
    src = -src;
    CLR_N(s);
    CLR_V(s);
    CLR_Z(s);
  } else if (src > 0) {
    SET_N(s);
    CLR_V(s);
    CLR_Z(s);
  } else {
    CLR_N(s);
    CLR_V(s);
    SET_V(s);
  }

  set_register(s,r,rd,src);

  return 0;
}

static int add(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;

  src = get_register(s,r,rs);
  dst = get_register(s,r,rd);

  fin = src + dst;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((~(dst ^ src)) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(src) + (uint64_t)(dst)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);

  return 0;
}

static int addi(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;

  switch(t->operand_types[0]) {
  case OP_IW:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    /* sign extend */
    if (src & 0x8000)
      src |= 0xffff << 16;
    break;
  case OP_IL:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    src   |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }
  dst = get_register(s,r,rd);

  fin = src + dst;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((~(dst ^ src)) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(src) + (uint64_t)(dst)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);

  return 0;
}

static int addk(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;

  src = (opcode >> 5) & 0x1f;
  if (src == 0)
    src = 32;

  dst = get_register(s,r,rd);

  fin = src + dst;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((~(dst ^ src)) & (dst ^ fin) & (1L << 31)) SET_V(s);
    else CLR_V(s);
  if (((uint64_t)(src) + (uint64_t)(dst)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);

  return 0;
}

static int addc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t c = (s->st >> 30) & 1;
  uint32_t src,dst,fin;

  src = get_register(s,r,rs);
  dst = get_register(s,r,rd);

  fin = src + dst + c;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((~(src ^ dst) & (src ^ fin)) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(src) + (uint64_t)(dst) + (uint64_t)c) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);

  return 0;
}

static int addxy(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,finx,finy;

  src = get_register(s,r,rs);
  dst = get_register(s,r,rd);

  finx = ((src & 0xffff) + (dst & 0xffff)) & 0xffff;
  finy = ((src >> 16)    + (dst >> 16)   ) & 0xffff;

  if (finx == 0) SET_N(s);
  else CLR_N(s);

  if (finx & 0x8000) SET_V(s);
  else CLR_V(s);

  if (finy == 0) SET_Z(s);
  else CLR_Z(s);

  if (finy & 0x8000) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,finx | (finy << 16));

  return 0;
}


static int and(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst;

  src  = get_register(s,r,rs);
  dst  = get_register(s,r,rd);

  src &= dst;

  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int or(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst;

  src  = get_register(s,r,rs);
  dst  = get_register(s,r,rd);

  src |= dst;

  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int andn(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst;

  src  = ~get_register(s,r,rs);
  dst  =  get_register(s,r,rd);

  src &= dst;

  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int andni(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t iwl,src;
  
  ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;
  ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
  s->pc += 16;
  src    = get_register(s,r,rd) & ~ilw;
    
  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int ori(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t iwl,src;
  
  ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;
  ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
  s->pc += 16;
  src    = get_register(s,r,rd) | ilw;
    
  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int btst(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst;

  src = (~(opcode >> 5)) & 0x1f;
  dst = get_register(s,r,rd);

  if (dst & (1L << src)) CLR_Z(s);
  else SET_Z(s);

  return 0;
}

static int call(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  s->sp -= 32;
  put_long(simulate,s->pc,s->sp);

  s->pc  = get_register(s,r,rd) & ~0x0f;

  return 0;
}
  
static int jump(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  s->pc  = get_register(s,r,rd) & ~0x0f;

  return 0;
}
  
static int calla(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t ilw;
  
  ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;
  ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
  s->pc += 16;

  s->sp -= 32;
  put_long(simulate,s->pc,s->sp);

  s->pc  = ilw & ~0x0f;

  return 0;
}

static int callr(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int16_t ilw;
  
  ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;

  s->sp -= 32;
  put_long(simulate,s->pc,s->sp);

  s->pc += ilw << 4;

  return 0;
}

static int clrc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  CLR_C(s);

  return 0;
}

static int setc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  SET_C(s);
  
  return 0;
}

static int cmp(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;

  src  = get_register(s,r,rs);
  dst  = get_register(s,r,rd);
  fin  = dst - src;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src)) >> 32) SET_C(s);
  else CLR_C(s);
  
  return 0;
}

static int cmpi(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;
  
  switch(t->operand_types[0]) {
  case OP_IW:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    /* sign extend */
    if (src & 0x8000)
      src |= 0xffff << 16;
    break;
  case OP_IL:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    src   |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  src = ~src;
  dst = get_register(s,r,rd);
  fin = dst - src;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src)) >> 32) SET_C(s);
  else CLR_C(s);
  
  return 0;
}

static int cmpxy(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,finx,finy;

  src = get_register(s,r,rs);
  dst = get_register(s,r,rd);

  finx = ((dst & 0xffff) - (src & 0xffff)) & 0xffff;
  finy = ((dst >> 16)    - (src >> 16)   ) & 0xffff;

  if (finx == 0) SET_N(s);
  else CLR_N(s);

  if (finx & 0x8000) SET_V(s);
  else CLR_V(s);

  if (finy == 0) SET_Z(s);
  else CLR_Z(s);

  if (finy & 0x8000) SET_C(s);
  else CLR_C(s);

  return 0;
}

static int cpw(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src;
  int16_t x,y;
  int16_t x0,y0,x1,y1;

  src = get_register(s,r,rs);
  x   = src & 0xffff;
  y   = src >> 16;

  src = get_register(s,1,5);
  x0  = src & 0xffff;
  y0  = src >> 16;

  src = get_register(s,1,6);
  x1  = src & 0xffff;
  y1  = src >> 16;

  src = 0;
  
  if (x < x0) src |= 0x1;
  if (x > x1) src |= 0x2;
  if (y < y0) src |= 0x4;
  if (y > y1) src |= 0x8;

  if (src == 0) CLR_V(s);
  else SET_V(s);

  set_register(s,r,rd,src << 5);
  
  return 0;
}

static int cvxyl(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  /*
  ** this instruction requires the knowledge of certain I/O registers and is
  ** thus currently not supported...
  */
  printf("cvxyl is currently not supported.\n");
  return -1;
}

static int sub(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;

  src  = get_register(s,r,rs);
  dst  = get_register(s,r,rd);
  fin  = dst - src;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);
  
  return 0;
}

static int subb(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t c = (s->st >> 30) & 1;
  uint32_t src,dst,fin;

  src  = get_register(s,r,rs);
  dst  = get_register(s,r,rd);
  fin  = dst - src - c;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src) - (uint64_t)(c)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);
  
  return 0;
}

static int subi(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;
  
  switch(t->operand_types[0]) {
  case OP_IW:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    /* sign extend */
    if (src & 0x8000)
      src |= 0xffff << 16;
    break;
  case OP_IL:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    src   |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  src = ~src;
  dst = get_register(s,r,rd);
  fin = dst - src;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);
  
  return 0;
}

static int dint(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  s->st &= ~(1 << 21);

  return 0;
}

static int divs(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  int32_t src;

  src = get_register(s,r,rs);

  if (src == 0) {
    SET_V(s);
    /*
    ** Unclear what the destination should be then
    */
    return 0;
  }
  
  if (rd & 1) {
    int32_t  dst = get_register(r,s,rd);
    int32_t  div = dst / src;
    
    set_register(s,r,rd,div);

    if (div == 0) SET_Z(s);
    else CLR_Z(s);

    if (div < 0) SET_N(s);
    else CLR_N(s);
    
  } else {
    uint32_t t   = get_register(s,r,rd + 1);
    int64_t  dst = (((int64_t)get_register(s,r,rd)) << 32) | t;
    int64_t  div = dst / src;
    int64_t  rem = dst % src;
    int32_t  dis = div;

    if (dis != div) SET_V(s);
    else CLR_V(s);
    
    if (div == 0) SET_Z(s);
    else CLR_Z(s);
    
    if (div < 0) SET_N(s);
    else CLR_N(s);

    set_register(s,r,rd    ,dis);
    set_register(s,r,rd + 1,rem);
  }

  return 0;
}

static int mods(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  int32_t src,dst,rem;

  src = get_register(s,r,rs);

  if (src == 0) {
    SET_V(s);
    /*
    ** Unclear what the destination should be then
    */
    return 0;
  }
  
  dst = get_register(r,s,rd);
  rem = dst % src;
  
  if (rem == 0) SET_Z(s);
  else CLR_Z(s);
    
  if (rem < 0) SET_N(s);
  else CLR_N(s);

  if (rem == (1L << 31) && src >= 0) SET_V(s);
  CLR_V(s);
  
  set_register(s,r,rd,rem);

  return 0;
}

static int mod(struct u_simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,rem;

  src = get_register(s,r,rs);

  if (src == 0) {
    SET_V(s);
    /*
    ** Unclear what the destination should be then
    */
    return 0;
  }
  
  dst = get_register(r,s,rd);
  rem = dst % src;
  
  if (rem == 0) SET_Z(s);
  else CLR_Z(s);
    
  if (rem < 0) SET_N(s);
  else CLR_N(s);

  CLR_V(s);
  
  set_register(s,r,rd,rem);

  return 0;
}


static int divu(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src;

  src = get_register(s,r,rs);

  if (src == 0) {
    SET_V(s);
    /*
    ** Unclear what the destination should be then
    */
    return 0;
  }
  
  if (rd & 1) {
    uint32_t  dst = get_register(r,s,rd);
    uint32_t  div = dst / src;
    
    set_register(s,r,rd,div);

    if (div == 0) SET_Z(s);
    else CLR_Z(s);

  } else {
    uint32_t t   = get_register(s,r,rd + 1);
    uint64_t dst = (((uint64_t)get_register(s,r,rd)) << 32) | t;
    uint64_t div = dst / src;
    uint64_t rem = dst % src;
    uint32_t dis = div;

    if (dis != div) SET_V(s);
    else CLR_V(s);
    
    if (div == 0) SET_Z(s);
    else CLR_Z(s);

    set_register(s,r,rd    ,dis);
    set_register(s,r,rd + 1,rem);
  }

  return 0;
}

static int drav(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  /* This requires emulating graphics operations, which is not yet done here */
  printf("drav is not yet emulated.\n");

  return -1;
}

static int dsj(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int16_t  ilw;
  uint32_t src;
  
  ilw    = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;

  src     = get_register(s,r,rd) - 1;
  set_register(s,r,rd,src);

  if (src)
    s->pc += ilw << 4;
    
  return 0;
}


static int dsjeq(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int16_t  ilw;
  uint32_t src;
  
  ilw    = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;

  if (TST_Z(s)) {
    src     = get_register(s,r,rd) - 1;
    set_register(s,r,rd,src);
    
    if (src)
      s->pc += ilw << 4;
  }
    
  return 0;
}

static int dsjne(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int16_t  ilw;
  uint32_t src;
  
  ilw    = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;

  if (!TST_Z(s)) {
    src     = get_register(s,r,rd) - 1;
    set_register(s,r,rd,src);
    
    if (src)
      s->pc += ilw << 4;
  }
    
  return 0;
}


static int dsjs(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int16_t  ilw;
  uint32_t src;
  
  ilw     = (opcode >> 5) & 0x1f;
  if ((opcode >> 10) & 1)
    ilw   = -ilw;
  src     = get_register(s,r,rd) - 1;
  set_register(s,r,rd,src);
  
  if (src)
    s->pc += ilw << 4;
    
  return 0;
}

static int eint(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  s->st |= (1 << 21);
  
  return 0;
}

static int emu(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  printf("emu is not supported\n");
  
  return -1;
}

static int exgf(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
 struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int  f = (opcode >> 9) & 1;
  int fc;
  uint32_t reg;

  reg = get_register(s,r,rd);
  
  if (f) {
    fc     = (s->st >> 6) & 0x3f;
    s->st &= ~(0x3f << 6);
    s->st |=  (reg & 0x3f) << 6;
    reg    = fc;
  } else {
    fc     = (s->st >> 0) & 0x3f;
    s->st &= ~(0x3f << 0);
    s->st |=  (reg & 0x3f) << 0;
    reg    = fc;
  }

  return 0;
}

static int exgpc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src;

  src = get_register(s,r,rd);
  set_register(s,r,rd,s->pc);
  s->pc = src & ~0x0f;

  return -1;
}

static int fill(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  printf("fill is not implemented\n");

  return -1;
}

static int getpc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  set_register(s,r,rd,s->pc);

  return 0;
}

static int getst(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  set_register(s,r,rd,s->st);

  return 0;
}

static int jauc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }
  
  s->pc  = ilw & ~0x0f;

  return 0;
}

static int jac(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
  s->pc += 16;
  ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
  s->pc += 16;

  if (TST_C(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jals(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (TST_C(s) || TST_Z(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jahi(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }
  
  if (!(TST_C(s) || TST_Z(s)))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int janc(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (!TST_C(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jaz(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (TST_Z(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int janz(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (TST_Z(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jalt(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (TST_N(s) != TST_V(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jale(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if ((TST_N(s) != TST_V(s)) || TST_Z(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jagt(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if ((TST_N(s) == TST_V(s)) && !TST_Z(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jage(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }
  
  if (TST_N(s) == TST_V(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jap(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (!TST_N(s) && !TST_Z(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jan(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }
  
  if (TST_N(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jann(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (!TST_N(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int jav(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (TST_V(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int janv(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t iwl,src;
  
  switch(t->operand_types[0]) {
  case OP_JUMP_REL:
    iwl     = (int8_t)(opcode & 0xff) + s->pc;
    break;
  case OP_ADDRESS:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }

  if (!TST_V(s))
    s->pc  = ilw & ~0x0f;

  return 0;
}

static int line(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  printf("Line is currently not supported.\n");

  return -1;
}

static int lmo(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst = 0;

  src = get_register(s,r,rs);

  if (src == 0) SET_Z(s);
  else CLR_Z(s);
  
  while(src <<= 1) {
    dst++;
  }

  set_register(s,r,rd,dst);

  return 0;
}

static int subk(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;

  src = (opcode >> 5) & 0x1f;
  if (src == 0)
    src = 32;

  dst = get_register(s,r,rd);
  fin = dst - src;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);
  
  return 0;
}

static int subxy(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst,finx,finy;

  src = get_register(s,r,rs);
  dst = get_register(s,r,rd);

  finx = ((dst & 0xffff) - (src & 0xffff)) & 0xffff;
  finy = ((dst >> 16)    - (src >> 16)   ) & 0xffff;

  if (finx == 0) SET_N(s);
  else CLR_N(s);

  if (finx & 0x8000) SET_V(s);
  else CLR_V(s);

  if (finy == 0) SET_Z(s);
  else CLR_Z(s);

  if (finy & 0x8000) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,finx | (finy << 16));

  return 0;
}

static int mpys(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  int32_t src,dst;
  int64_t prd;

  src    = get_register(s,r,rs);
  dst    = get_register(s,r,rd);

  prd    = ((int64_t)src) * ((int64_t)dst);

  if (prd < 0) SET_N(s);
  else CLR_N(s);
  if (prd == 0) SET_Z(s);
  else CLR_Z(s);

  if (rd & 1) {
    /* Target register is odd. Store only the lower end */
    set_register(s,r,rd,prd);
  } else {
    set_register(s,r,rd  ,prd);
    set_register(s,r,rd+1,prd >> 32);
  }
  
  return 0;
}

static int mpyu(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  uint32_t src,dst;
  int64_t prd;

  src    = get_register(s,r,rs);
  dst    = get_register(s,r,rd);

  prd    = ((uint64_t)src) * ((uint64_t)dst);

  if (prd == 0) SET_Z(s);
  else CLR_Z(s);

  if (rd & 1) {
    /* Target register is odd. Store only the lower end */
    set_register(s,r,rd,prd);
  } else {
    set_register(s,r,rd  ,prd);
    set_register(s,r,rd+1,prd >> 32);
  }
  
  return 0;
}

static int movi(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t src;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  switch(t->operand_types[0]) {
  case OP_IW:
    src = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    /* sign extend */
    if (src & 0x8000)
      src |= 0xffff << 16;
    break;
  case OP_IL:
    src    = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    src   |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    break;
  }
  CLR_V(s);
  if ((int32_t)(src) < 0) SET_N(s);
  else CLR_N(s);
  if (src == 0) SET_Z(s);
  else CLR_Z(s);
  set_register(s,r,rd,src);

  return 0;
}

static int movk(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t src;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  src    = (opcode >> 5) & 0x1f;
  if (src == 0)
    src = 32;

  set_register(s,r,rd,src);

  return 0;
}

static int movx(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;

  src    = get_register(s,r,rs) & 0xffff;
  src   |= get_register(s,r,rd) & (0xffff << 16);
  
  set_register(r,s,rd,src);

  return 0;
}

static int movy(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;

  src    = get_register(s,r,rs) & (0xffff << 16);
  src   |= get_register(s,r,rd) & 0xffff;
  
  set_register(r,s,rd,src);

  return 0;
}

static int movb(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t src,reg;
  int16_t displacement;
  int32_t displacement32,ilw;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;

  switch(t->operand_types[0]) {
  case OP_RS:
    src = get_register(s,r,rs) & 0xff;
    if (src & 0x80)
      src |= 0xffffff00;
    break;
  case OP_P_RS:
    reg = get_register(s,r,rs);
    src = get_byte(simulate,reg);
    break;
  case OP_P_RS_DISP:
    reg = get_register(s,r,rs);
    displacement = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    src = get_byte(simulate,reg + displacement);
    break;
  case OP_AT_ADDR:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    src    = get_byte(simulate,ilw);
    break;
  }
  switch(t->operand_types[1]) {
  case OP_RD:
    /* This is the only combo that sets the status bits */
    CLR_V(s);
    if ((int32_t)(src) < 0) SET_N(s);
    else CLR_N(s);
    if (src == 0) SET_Z(s);
    else CLR_Z(s);
    set_register(s,r,rd,src);
    break;
  case OP_P_RD:
    reg = get_register(s,r,rd);
    set_byte(s,reg,src);
    break;
  case OP_P_RD_DISP:
    reg = get_register(s,r,rs);
    displacement = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    set_byte(simulate,reg + displacement,src);
    break;
  case OP_AT_ADDR:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    set_byte(simulate,ilw,src);
    break;
  }
  return 0;
}

static int move(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t src,reg;
  int16_t displacement;
  int32_t displacement32,ilw;
  int  r = (opcode >> 4) & 1;
  int rs = (opcode >> 5) & 0xf;
  int rd = opcode & 0xf;
  int f  = (opcode >> 9) & 1;

  switch(t->operand_types[0]) {
  case OP_RS:
    src = get_register(s,r,rs);
    break;
  case OP_P_RS:
    reg = get_register(s,r,rs);
    src = get_field(simulate,reg,f);
    break;
  case OP_MP_RS:
    reg  = get_register(s,r,rs);
    reg -= get_field_size(s,f);
    set_register(s,r,rs,reg);
    src  = get_field(simulate,reg,f);
    break;
  case OP_P_RS_P:
    reg  = get_register(s,r,rs);
    src  = get_field(simulate,reg,f);
    reg += get_field_size(s,f);
    set_register(s,r,rs,reg);
    break;
  case OP_P_RS_DISP:
    reg = get_register(s,r,rs);
    displacement = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    src = get_field(simulate,reg + displacement,f);
    break;
  case OP_RD:
  case OP_RDS:
    src = get_register(s,r,rd);
    break;
  case OP_AT_ADDR:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    src    = get_field(simulate,ilw,f);
    break;
  }
  switch(t->operand_types[1]) {
  case OP_RD:
  case OP_RDS:
    /* This is the only combo that sets the status bits */
    CLR_V(s);
    if ((int32_t)(src) < 0) SET_N(s);
    else CLR_N(s);
    if (src == 0) SET_Z(s);
    else CLR_Z(s);
    if ((opcode & 0xfe00) == 0x4e00) {
      set_register(s,r^1,rd,src);
    } else {
      set_register(s,r,rd,src);
    }
    break;
  case OP_P_RD:
    reg = get_register(s,r,rd);
    set_field(simulate,reg,f,src);
    break;
  case OP_MP_RD:
    reg  = get_register(s,r,rd);
    reg -= get_field_size(s,f);
    set_register(s,r,rd,reg);
    set_field(simulate,reg,f,src);
    break;
  case OP_P_RD_P:
    reg  = get_register(s,r,rd);
    set_field(simulate,reg,f,src);
    reg += get_field_size(s,f);
    set_register(s,r,rd,reg);
    break;
  case OP_RD_DISP:
    reg = get_register(s,r,rs);
    displacement = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    set_field(simulate,reg + displacement,f,src);
    break;
  case OP_AT_ADDR:
    ilw     = memory_read16_m(simulate->memory,s->pc >> 3);
    s->pc += 16;
    ilw    |= memory_read16_m(simulate->memory,s->pc >> 3) << 16;
    s->pc += 16;
    set_field(simulate,ilw,f,src);
    break;
  }
  return 0;
}

static int mmtm(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t adr,reg;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int rs;
  int32_t mask;

  mask = memory_read16_m(simulate->memory,s->pc >> 3);
  adr  = get_register(s,r,rd);
  rs   = 0; /* Move low-registers first */
  
  while(mask) {
    if (mask & 1) {
      reg  = get_register(s,r,rs);
      adr -= 32;
      set_long(simulate,adr,reg);
      mask >>= 1;
      rs++;
    }
  }

  set_register(s,r,rd,adr);

  return 0;
}

static int mmfm(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  uint32_t adr,reg;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int rs;
  int32_t mask;

  mask = memory_read16_m(simulate->memory,s->pc >> 3);
  adr  = get_register(s,r,rd);
  rs   = 15; /* Move high-registers first */
  
  while(mask) {
    if (mask & 0x8000) {
      reg  = get_long(simulate,adr);
      adr += 32;
      mask  = (mask & 0x7fff) << 1;
      set_register(s,r,rs,reg);
      rs--;
    }
  }

  set_register(s,r,rd,adr);

  return 0;
}

static int neg(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src;

  src = get_register(s,r,rd);

  if (src = (1L << 31)) {
    SET_V(s);
    CLR_Z(s);
    SET_N(s);
  } else {
    src = -src;
    if (src == 0) SET_Z(s),CLR_C(s);
    else CLR_Z(s),SET_C(s);
    CLR_V(s);
    CLR_Z(s);
    if (src & (1L << 31)) SET_N(s);
    else CLR_N(s);
  }

  set_register(s,r,rd,src);

  return 0;
}

static int negb(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src,dst,fin;
  
  dst  = TST_C(s);
  src  = get_register(s,r,rd);
  fin  = dst - src;

  if (fin == 0) SET_Z(s);
  else CLR_Z(s);
  if (fin & (1L << 31)) SET_N(s);
  else CLR_N(s);
  if ((dst ^ src) & (dst ^ fin) & (1L << 31)) SET_V(s);
  else CLR_V(s);
  if (((uint64_t)(dst) - (uint64_t)(src)) >> 32) SET_C(s);
  else CLR_C(s);

  set_register(s,r,rd,fin);

  return 0;
}

static int nop(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  return 0;
}

static int not(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  uint32_t src;

  src = ~get_register(s,r,rd);

  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int pixblt(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  printf("pixblt is current not supported.\n");

  return -1;
}

static int pix(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  printf("pix is current not supported.\n");

  return -1;
}

static int popst(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  s->st  = get_long(simulate,s->sp);
  s->sp += 32:

  return 0;
}
 
static int pushst(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  s->sp -= 32;
  s->st  = set_long(simulate,s->sp,s->st);

  return 0;
}
 
static int putst(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
 
  s->st  = get_register(s,r,rd);

  return 0;
}
 
static int reti(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  s->st  = get_long(simulate,s->sp);
  s->sp += 32;
  s->pc  = get_long(simulate,s->sp) & ~0x0f;
  s->sp += 32;

  return 0;
}

static int rets(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;

  s->pc  = get_long(simulate,s->sp) & ~0x0f;
  s->sp += 32;
  s->pc += (opcode & 0x1f) << 4;
  
  return 0;
}

static int rev(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;

  set_register(s,r,rd,8);
  
  return 0;
}

static int rl(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int rs = (opcode >> 5) & 0xf;
  int  k;
  uint32_t src,tmp;
  
  switch(t->operand_types[0]) {
  case OP_K:
    k = (opcode >> 5) & 0x1f;
    break;
  case OP_RS:
    k = get_register(s,r,rs) & 0x1f;
    break;
  }

  src = get_register(s,r,rd);
  tmp = src >> (32 - k);
  src = (src << k) | tmp;
  
  if (tmp & 1) SET_C(s);
  else CLR_C(s);

  if (src == 0) SET_Z(s);
  else CLR_Z(s);

  set_register(s,r,rd,src);

  return 0;
}

static int setf(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int fv = opcode & 0x3f;
  int f  = (opcode >> 9) & 1;
  int m  = 0x3f;

  if (f) {
    m  <<= 6;
    fv <<= 6;
  }

  s->st = (s->st & ~m) | fv;

  return 0;
}

static int sext(struct _simulate *simulate,struct _table_tms340 *t,uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  int  r = (opcode >> 4) & 1;
  int rd = opcode & 0xf;
  int f  = (opcode >> 9) & 1;
  int fs = get_field_size(s,f);
  uint32_t src;

  if (fs < 32) {
    src = get_register(r,s,rd);
    fs--;
    
    if (src & (1 << fs)) {
      src |=   ~0L << fs;
    } else {
      src &= ~(~0L << fs);
    }

    set_register(r,s,rd,src);
  }
  
  return 0;
}


struct instruction {
  const char *instname;
  execute    *executer;
} instructions[] = {
		    "addxy", &addxy,
		    "cmpxy", &cmpxy,
		    "cpw",   &cpw,
		    "cvxyl", &cvxyl,
		    "drav",  &drav,
		    "fill",  &fill,
		    "movx",  &movx,
		    "movy",  &movy,
		    "pixblt",&pixblt,
		    "pixt",  &pixt,
		    "subxy", &subxy,
		    "line",  &line,
		    "movb",  &movb,
		    "move",  &move,
		    "abs",   &abs,
		    "add",   &add,
		    "addc",  &addc,
		    "addi",  &addi,
		    "addk",  &addk,
		    "and",   &and,
		    "andi",  &andi,
		    "andn",  &andn,
		    "andni", &andni,
		    "btst",  &btst,
		    "clrc",  &clrc,
		    "cmp",   &cmp,
		    "cmpi",  &cmpi,
		    "dec",   &dec,
		    "dint",  &dint,
		    "divs",  &divs,
		    "divu",  &divu,
		    "eint",  &eint,
		    "exgf",  &exgf,
		    "lmo",   &lmo,
		    "mmfm",  &mmfm,
		    "mmtm",  &mmtm,
		    "mods",  &mods,
		    "modu",  &modu,
		    "movi",  &movi,
		    "movk",  &movk,
		    "mpys",  &mpys,
		    "mpyu",  &mpyu,
		    "neg",   &neg,
		    "negb",  &negb,
		    "nop",   &nop,
		    "not",   &not,
		    "or",    &or,
		    "ori",   &ori,
		    "rl",    &rl,
		    "setc",  &setc,
		    "setf",  &setf,
		    "sext",  &sext,
		    "sla",   &sla,
		    "sll",   &sll,
		    "sra",   &sra,
		    "srl",   &srl,
		    "sub",   &sub,
		    "subb",  &subb,
		    "subi",  &subi,
		    "subk",  &subk,
		    "xor",   &xor,
		    "xori",  &xori,
		    "zext",  &zext,
		    "call",  &call,
		    "calla", &calla,
		    "callr", &callr,
		    "dsj",   &dsj,
		    "dsjeq", &dsjeq,
		    "dsjne", &dsjne,
		    "dsjs",  &dsjs,
		    "emu",   &emu,
		    "exgpc", &exgpc,
		    "getpc", &getpc,
		    "getst", &getst,
		    "jauc",  &jauc,
		    "jap",   &jap,
		    "jals",  &jals,
		    "jahi",  &jahi,
		    "jalt",  &jalt,
		    "jage",  &jage,
		    "jale",  &jale,
		    "jagt",  &jagt,
		    "jac",   &jac,
		    "jalo",  &jac,
		    "jab",   &jac,
		    "janc",  &janc,
		    "jahs",  &janc,
		    "janb",  &janc,
		    "jaz",   &jaz,
		    "jaeq",  &jaz,
		    "janz",  &janz,
		    "jane",  &janz,
		    "jav",   &jav,
		    "janv",  &janv,
		    "jan",   &jan,
		    "jann",  &jann,
		    "jruc",  &jauc,
		    "jrp",   &jap,
		    "jrls",  &jals,
		    "jrhi",  &jahi,
		    "jrlt",  &jalt,
		    "jrge",  &jage,
		    "jrle",  &jale,
		    "jrgt",  &jagt,
		    "jrc",   &jac,
		    "jrb",   &jac,
		    "jrlo",  &jac,
		    "jrnc",  &janc,
		    "jrhs",  &janc,
		    "jrnb",  &janc,
		    "jrz",   &jaz,
		    "jreq",  &jaz,
		    "jrnz",  &janz,
		    "jrne",  &janz,
		    "jrv",   &jav,
		    "jrnv",  &janv,
		    "jrn",   &jan,
		    "jrnn",  &jann,
		    "jump",  &jump,
		    "popst", &popst,
		    "pushst",&pushst,
		    "putst", &putst,
		    "reti",  &reti,
		    "rets",  &rets,
		    "trap",  &trap,
		    NULL,    NULL
};

static int operand_exe(struct _simulate *simulate, uint16_t opcode)
{
  struct _simulate_tms340 *s = (struct _simulate_tms340 *)simulate->context;
  const struct _table_tms340 *t = table_tms340;
 
  while(t->instr) {
    if ((opcode & t->mask) == t->opcode) {
      struct instruction *inst = instructions;
      while(inst->instname) {
	if (!strcmp(inst->instname,t->instr)) {
	  return (inst->executer)(simulate,t,opcode);
	}
	inst++;
      }
      printf("Found unsupported instruction %s\n",t->instr);
    }
    t++:
  }
  printf("Found unsupported instruction opcode 0x%04x\n",opcode);
  
  return -1;
}

int simulate_run_tms340(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_tms340 *simulate_tms340 = (struct _simulate_tms340 *)simulate->context;
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int c = 0; // FIXME - broken
  int n;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running == 0)
  {
    pc     = simulate_tms340->pc >> 3;
    opcode = (READ_RAM(pc) << 8) | READ_RAM(pc);

    operand_exe(simulate, opcode);
      
    //c = get_cycle_count(opcode);
    //if (c > 0) simulate->cycle_count += c;
    simulate_tms9900->pc += 2;

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    ///////
    if (opcode == 0) { break; } // FIXME
    ret = -1;
    ///////

    if (c > 0) cycles += c;

    if (simulate->show == 1)
    {
      simulate_dump_registers_tms9900(simulate);

      n = 0;
      while(n < 6)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
        int count = disasm_tms340(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
        { printf("! "); }
          else
        if (pc == simulate_tms340->pc >> 3) { printf("> "); }
          else
        { printf("  "); }

        if (cycles_min < 1)
        {
          printf("0x%04x: 0x%04x %-40s ?\n", pc << 3, num, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: 0x%04x %-40s %d\n", pc << 3, num, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: 0x%04x %-40s %d-%d\n", pc << 3, num, instruction, cycles_min, cycles_max);
        }

        n   = n + count;
        pc += 2;
        count--;
        while (count > 0)
        {
          if (pc << 3 == simulate->break_point) { printf("*"); }
          else { printf(" "); }
          num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
          printf("  0x%04x: 0x%04x\n", pc, num);
          pc += 2;
          count--;
        }
      }
    }

    if (simulate->auto_run == 1 && simulate->nested_call_count < 0) { return 0; }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) break;
    if (simulate->break_point == simulate_tms340->pc)
    {
       printf("Breakpoint hit at 0x%04x\n", simulate->break_point);
      break;
    }

    if (simulate->usec == 0 || step == 1)
    {
      //simulate->step_mode=0;
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%08x.\n", simulate_tms340->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

