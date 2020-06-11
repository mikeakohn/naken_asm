/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/tms1000.h"
#include "table/tms1000.h"

#define READ_RAM(a) \
  memory_read_m(memory, (a & 0xfc0) | tms1000_address_to_lsfr[a & 0x3f])

int get_cycle_count_tms1000(uint16_t opcode)
{
  return 6;
}

int get_cycle_count_tms1100(uint16_t opcode)
{
  return 6;
}

static void compute_address(int address, int *chapter, int *page, int *pc)
{
  *chapter = (address >> 10) & 3;
  *page = (address >> 6) & 0xf;
  *pc = address & 0x3f;
}

int disasm_tms1000(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int bit_instr;
  int opcode;
  int n, c;

  *cycles_min = 6;
  *cycles_max = 6;

  opcode = READ_RAM(address);

  n = 0;
  while(table_tms1000[n].instr != NULL)
  {
    if (opcode == table_tms1000[n].op1000)
    {
      strcpy(instruction, table_tms1000[n].instr);
      return 1;
    }
    n++;
  }

  bit_instr = opcode >> 2;
  c = tms1000_reverse_bit_address[opcode & 0x3];

  if (bit_instr == 0xc) { sprintf(instruction, "sbit %d", c); return 1; }
    else
  if (bit_instr == 0xd) { sprintf(instruction, "rbit %d", c); return 1; }
    else
  if (bit_instr == 0xe) { sprintf(instruction, "tbit1 %d", c); return 1;}
    else
  if (bit_instr == 0xf) { sprintf(instruction, "ldx %d", c); return 1; }

  bit_instr = opcode >> 4;
  c = tms1000_reverse_constant[opcode & 0xf];

  if (bit_instr == 0x4) { sprintf(instruction, "tcy %d", c); return 1; }
    else
  if (bit_instr == 0x6) { sprintf(instruction, "tcmiy %d", c); return 1;}
    else
  if (bit_instr == 0x1) { sprintf(instruction, "ldp %d", c); return 1; }
    else
  if (bit_instr == 0x7) { sprintf(instruction, "alec %d", c); return 1; }
    else
  if (bit_instr == 0x5) { sprintf(instruction, "ynec %d", c); return 1; }

  bit_instr = opcode >> 6;
  uint8_t branch_address = opcode & 0x3f;

  if (bit_instr == 0x2)
  {
    sprintf(instruction, "br 0x%02x  (linear_address=0x%02x)",
      branch_address,
      tms1000_lsfr_to_address[branch_address]);
    return 1;
  }
    else
  if (bit_instr == 0x3)
  {
    sprintf(instruction, "call 0x%02x  (linear_address=0x%02x)",
      branch_address,
      tms1000_lsfr_to_address[branch_address]);
    return 1;
  }

  strcpy(instruction, "???");

  return 1;
}

int disasm_tms1100(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int bit_instr;
  int opcode;
  int n, c;

  *cycles_min = 6;
  *cycles_max = 6;

  opcode = READ_RAM(address);

  n = 0;
  while(table_tms1000[n].instr != NULL)
  {
    if (opcode == table_tms1000[n].op1100)
    {
      strcpy(instruction, table_tms1000[n].instr);
      return 1;
    }
    n++;
  }

  bit_instr = opcode >> 2;
  c = tms1000_reverse_bit_address[opcode & 0x3];

  if (bit_instr == 0xc) { sprintf(instruction, "sbit %d", c); return 1; }
    else
  if (bit_instr == 0xd) { sprintf(instruction, "rbit %d", c); return 1; }
    else
  if (bit_instr == 0xe) { sprintf(instruction, "tbit1 %d", c); return 1; }

  bit_instr = opcode >> 3;
  c = tms1000_reverse_constant[opcode & 0x7] >> 1;

  if (bit_instr == 0x5) { sprintf(instruction, "ldx %d", c); return 1; }

  bit_instr = opcode >> 4;
  c = tms1000_reverse_constant[opcode & 0xf];

  if (bit_instr == 0x4) { sprintf(instruction, "tcy %d", c); return 1; }
    else
  if (bit_instr == 0x6) { sprintf(instruction, "tcmiy %d", c); return 1; }
    else
  if (bit_instr == 0x1) { sprintf(instruction, "ldp %d", c); return 1; }
    else
  if (bit_instr == 0x5) { sprintf(instruction, "ynec %d", c); return 1; }

  bit_instr = opcode >> 6;
  uint8_t branch_address = opcode & 0x3f;
  //if ((offset & 0x20) != 0) { offset |= 0xc0; }
  //int branch_address = (address + 1) + offset;

  if (bit_instr == 0x2)
  {
     sprintf(instruction, "br 0x%02x (linear_address=0x%02x)",
       branch_address,
       tms1000_lsfr_to_address[branch_address]);
     return 1;
  }
    else
  if (bit_instr == 0x3)
  {
    sprintf(instruction, "call 0x%02x (linear_address=0x%02x)",
      branch_address,
      tms1000_lsfr_to_address[branch_address]);
    return 1;
  }

  strcpy(instruction, "???");

  return 1;
}

void list_output_tms1000(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  struct _memory *memory = &asm_context->memory;
  uint32_t opcode = READ_RAM(start);
  int chapter;
  int page;
  int pc;
  uint8_t lsfr;

  compute_address(start, &chapter, &page, &pc);

  lsfr = tms1000_address_to_lsfr[pc];

  disasm_tms1000(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  fprintf(asm_context->list, "%03x %x/%02x: %02x %-40s cycles: ", start, page, lsfr, opcode, instruction);

  if (cycles_min == cycles_max)
  {
    fprintf(asm_context->list, "%d\n", cycles_min);
  }
    else
  {
    fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
  }
}

void list_output_tms1100(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  struct _memory *memory = &asm_context->memory;
  uint32_t opcode = READ_RAM(start);
  int chapter;
  int page;
  int pc;
  uint8_t lsfr;

  compute_address(start, &chapter, &page, &pc);

  lsfr = tms1000_address_to_lsfr[pc];

  disasm_tms1100(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  fprintf(asm_context->list, "%03x %d/%x/%02x: %02x %-40s cycles: ", start, chapter, page, lsfr, opcode, instruction);

  if (cycles_min == cycles_max)
  {
    fprintf(asm_context->list, "%d\n", cycles_min);
  }
    else
  {
    fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
  }
}

void disasm_range_tms1000(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int chapter;
  int page;
  int pc;
  int num;

  printf("\n");

  printf("%-4s %-4s %-5s %-40s Cycles\n", "Linr", "Addr", "Opcode", "Instruction");
  printf("---- ---- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    num = READ_RAM(start);

    disasm_tms1000(memory, start, instruction, &cycles_min, &cycles_max);

    compute_address(start, &chapter, &page, &pc);

    uint8_t lsfr = tms1000_address_to_lsfr[pc];

    if (cycles_min < 1)
    {
      printf("%03x  %x/%02x: %02x    %-40s ?\n", start, page, lsfr, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("%03x  %x/%02x: %02x    %-40s %d\n", start, page, lsfr, num, instruction, cycles_min);
    }
      else
    {
      printf("%03x  %x/%02x: %02x    %-40s %d-%d\n", start, page, lsfr, num, instruction, cycles_min, cycles_max);
    }

    start++;
  }
}

void disasm_range_tms1100(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int chapter;
  int page;
  int pc;
  int num;

  printf("\n");

  printf("%-4s %-4s   %-5s %-40s Cycles\n", "Linr", "Addr", "Opcode", "Instruction");
  printf("---- ------ ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    num = READ_RAM(start);

    disasm_tms1100(memory, start, instruction, &cycles_min, &cycles_max);

    compute_address(start, &chapter, &page, &pc);

    uint8_t lsfr = tms1000_address_to_lsfr[pc];

    if (cycles_min < 1)
    {
      printf("%03x %d/%x/%02x: %02x     %-40s ?\n", start, chapter, page, lsfr, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("%03x %d/%x/%02x: %02x     %-40s %d\n", start, chapter, page, lsfr, num, instruction, cycles_min);
    }
      else
    {
      printf("%03x %d/%x/%02x: %02x     %-40s %d-%d\n", start, chapter, page, lsfr, num, instruction, cycles_min, cycles_max);
    }

    start++;
  }
}

