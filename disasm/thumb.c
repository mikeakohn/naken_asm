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

#include "disasm/thumb.h"
#include "table/thumb.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a))|(memory_read_m(memory, a+1)<<8)

int get_cycle_count_thumb(unsigned short int opcode)
{
  return -1;
}

static void get_rlist(char *s, int rlist)
{
  int i, comma;
  char temp[32];

  s[0] = 0;

  for (i = 0; i < 8; i++)
  {
    if ((rlist & (1 << i)) != 0)
    {
      if (comma == 0)
      {
        sprintf(temp, " r%d", i);
        comma = 1;
      }
        else
      {
        sprintf(temp, ", r%d", i);
        comma = 1;
      }

      strcat(s, temp);
    }
  }
}

static void get_special_register(char *name, int value)
{
  int n = 0;

  while (special_reg_thumb[n].name != NULL)
  {
    if (special_reg_thumb[n].value == value)
    {
      strcpy(name, special_reg_thumb[n].name);
      return;
    }

    n++;
  }

  sprintf(name, "%d", value);
}

int disasm_thumb(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint16_t opcode;
  int rd, rs, rn, offset;
  int h1, h2;
  int immediate;
  char temp[128];
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM16(address);

  n = 0;
  while(table_thumb[n].instr != NULL)
  {
    if (table_thumb[n].opcode == (opcode & table_thumb[n].mask))
    {
      *cycles_min = table_thumb[n].cycles;
      *cycles_max = table_thumb[n].cycles;

      switch(table_thumb[n].type)
      {
        case OP_NONE:
          strcpy(instruction, table_thumb[n].instr);
          return 2;
        case OP_SHIFT:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;
          offset = (opcode >> 6) & 0x1f;
          sprintf(instruction, "%s r%d, r%d, #%d", table_thumb[n].instr, rd, rs, offset);
          return 2;
        case OP_ADD_SUB:
          immediate = (opcode >> 10) & 1;
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;
          if (immediate == 0)
          {
            rn=(opcode>>6)&7;
            sprintf(instruction, "%s r%d, r%d, r%d", table_thumb[n].instr, rd, rs, rn);
          }
            else
          {
            offset = (opcode >> 6) & 7;
            sprintf(instruction, "%s r%d, r%d, #%d", table_thumb[n].instr, rd, rs, offset);
          }
          return 2;
        case OP_REG_IMM:
          immediate = opcode & 0xff;
          rd = (opcode >> 8) & 0x7;
          sprintf(instruction, "%s r%d, #0x%02x", table_thumb[n].instr, rd, immediate);
          return 2;
        case OP_ALU:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s r%d, r%d", table_thumb[n].instr, rd, rs);
          return 2;
        case OP_HI:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;
          h1 = (opcode >> 7) & 0x1;
          h2 = (opcode >> 6) & 0x1;
          sprintf(instruction, "%s r%d, r%d", table_thumb[n].instr, rd + (h1 * 8), rs + (h2 * 8));
          return 2;
        case OP_HI_BX:
          rs = (opcode >> 3) & 0x7;
          h2 = (opcode >> 6) & 0x1;
          sprintf(instruction, "%s r%d", table_thumb[n].instr, rs + (h2 * 8));
          return 2;
        case OP_PC_RELATIVE_LOAD:
          rd = (opcode >> 8) & 0x7;
          offset=(opcode & 0xff) << 2;
          sprintf(instruction, "%s r%d, [PC, #%d]  (0x%x)", table_thumb[n].instr, rd, offset, ((address + 4) & 0xfffffffc) + offset);
          return 2;
        case OP_LOAD_STORE:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;  // rb
          rn = (opcode >> 6) & 0x7;  // ro
          sprintf(instruction, "%s r%d, [r%d, r%d]", table_thumb[n].instr, rd, rs, rn);
          return 2;
        case OP_LOAD_STORE_SIGN_EXT_HALF_WORD:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;  // rb
          rn = (opcode >> 6) & 0x7;  // ro
          sprintf(instruction, "%s r%d, [r%d, r%d]", table_thumb[n].instr, rd, rs, rn);
          return 2;
        case OP_LOAD_STORE_IMM_OFFSET_WORD:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;  // rb
          offset = (opcode >> 6) & 0x1f;
          sprintf(instruction, "%s r%d, [r%d, #%d]", table_thumb[n].instr, rd, rs, offset << 2);
          return 2;
        case OP_LOAD_STORE_IMM_OFFSET:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;  // rb
          offset = (opcode >> 6) & 0x1f;
          sprintf(instruction, "%s r%d, [r%d, #%d]", table_thumb[n].instr, rd, rs, offset);
          return 2;
        case OP_LOAD_STORE_IMM_OFFSET_HALF_WORD:
          rd = opcode & 0x7;
          rs = (opcode >> 3) & 0x7;  // rb
          offset = (opcode >> 6) & 0x1f;
          sprintf(instruction, "%s r%d, [r%d, #%d]", table_thumb[n].instr, rd, rs, offset << 1);
          return 2;
        case OP_LOAD_STORE_SP_RELATIVE:
          rd = (opcode >> 8) & 0x7;
          offset = opcode & 0xff;
          sprintf(instruction, "%s r%d, [SP, #%d]", table_thumb[n].instr, rd, offset << 2);
          return 2;
        case OP_LOAD_ADDRESS:
          rd = (opcode >> 8) & 0x7;
          offset = opcode & 0xff;
          rs = (opcode >> 11) & 0x1;  // SP (0=PC,1=SP)
          sprintf(instruction, "%s r%d, %s, #%d", table_thumb[n].instr, rd, (rs == 0) ? "PC" : "SP", offset << 2);
          return 2;
        case OP_ADD_OFFSET_TO_SP:
          rs = (opcode >> 7) & 0x1;  // S (0=positive,1=negative)
          offset = opcode & 0x7f;
          sprintf(instruction, "%s SP, #%s%d", table_thumb[n].instr, (rs == 0) ? "" : "-", offset << 2);
          return 2;
        case OP_PUSH_POP_REGISTERS:
          rs = opcode & 0xff;      // Rlist
          rn = opcode >> 8 & 0x1;    // PC or LR
          get_rlist(temp, rs);
          if (rn == 1)
          {
            if (temp[0] != 0) { strcat(temp, ","); }
            if (((opcode >> 11) & 1) == 0) { strcat(temp, " LR"); }
            else { strcat(temp, " PC"); }
          }
          sprintf(instruction, "%s {%s }", table_thumb[n].instr, temp);
          return 2;
        case OP_MULTIPLE_LOAD_STORE:
          rd = opcode >> 8 & 0x7;    // Rb!
          rs = opcode & 0xff;        // Rlist
          get_rlist(temp, rs);
          sprintf(instruction, "%s r%d!, {%s }", table_thumb[n].instr, rd, temp);
          return 2;
        case OP_CONDITIONAL_BRANCH:
          offset = ((int8_t)(opcode & 0xff)) << 1;
          sprintf(instruction, "%s 0x%04x (%d)", table_thumb[n].instr, address + 4 + offset, offset);
          return 2;
        case OP_SOFTWARE_INTERRUPT:
          offset = opcode & 0xff;
          sprintf(instruction, "%s 0x%02x", table_thumb[n].instr, offset);
          return 2;
        case OP_UNCONDITIONAL_BRANCH:
          offset=opcode&0x3ff;
          if ((offset&0x200)!=0)
          {
            offset=-((offset^0x3ff)+1);
          }
          offset <<= 1;
          sprintf(instruction, "%s 0x%04x (%d)", table_thumb[n].instr, address+4+offset, offset);
          return 2;
        case OP_LONG_BRANCH_WITH_LINK:
          offset = READ_RAM16(address + 2);

          if ((offset & 0xf800) != 0xf800)
          {
            n++;
            continue;
          }

          rn = opcode & 0x7ff;
          offset = (rn << 11) | (offset  &0x7ff);
          if ((offset & 0x200000) != 0)
          {
            offset = -((offset ^ 0x3fffff) + 1);
          }
          offset <<= 1;
          sprintf(instruction, "%s 0x%04x (%d)", table_thumb[n].instr, address + 4 + offset, offset);
          return 4;
        case OP_SP_SP_IMM:
          sprintf(instruction, "%s SP, SP, #%d", table_thumb[n].instr, (opcode & 0x3f) * 4);
          return 2;
        case OP_REG_REG:
          sprintf(instruction, "%s r%d, r%d", table_thumb[n].instr, opcode & 7, (opcode >> 3) & 7);
          return 2;
        case OP_CPS:
          sprintf(instruction, "%s %s%s", table_thumb[n].instr,
            (opcode & 0x2) == 2 ? "i" : "",
            (opcode & 0x1) == 1 ? "f" : "");
          return 2;
        case OP_UINT8:
          immediate = opcode & 0xff;
          sprintf(instruction, "%s #0x%02x", table_thumb[n].instr, immediate);
          return 2;
        case OP_REGISTER_ADDRESS:
          immediate = opcode & 0xff;
          sprintf(instruction, "%s r%d, 0x%04x", table_thumb[n].instr, (opcode >> 8) & 0x7, address + 4 + immediate);
          return 2;
        case OP_MRS:
          immediate = opcode = READ_RAM16(address + 2);
          if ((immediate & 0xf000) != 0x8000)
          {
            n++;
            continue;
          }
          rd = (immediate >> 8) & 0xf;
          get_special_register(temp, immediate & 0xff);
          sprintf(instruction, "%s r%d, %s", table_thumb[n].instr, rd, temp);
          return 4;
        case OP_MSR:
          immediate = opcode = READ_RAM16(address + 2);
          if ((immediate & 0xff00) != 0x8800)
          {
            n++;
            continue;
          }
          rn = (opcode >> 8) & 0xf;
          get_special_register(temp, immediate & 0xff);
          sprintf(instruction, "%s %s, r%d", table_thumb[n].instr, temp, rn);
          return 4;
        case OP_REG_LOW:
          rd = opcode & 0x7;
          rn = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s r%d, r%d", table_thumb[n].instr, rd, rn);
          return 2;
        default:
          strcpy(instruction, "???");
          return 2;
      }
    }
    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_thumb(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  int count;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_thumb(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%04x: %04x  %-40s cycles: ",
      start,
      memory_read_m(&asm_context->memory, start) |
     (memory_read_m(&asm_context->memory, start + 1) << 8),
      instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    if (count == 4)
    {
      fprintf(asm_context->list, "        %04x\n",
        memory_read_m(&asm_context->memory, start + 2) |
       (memory_read_m(&asm_context->memory, start + 3) << 8));
    }

    start += count;
  }
}

void disasm_range_thumb(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count=disasm_thumb(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: %04x  %-40s ?\n", start, READ_RAM16(start), instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %04x  %-40s %d\n", start, READ_RAM16(start), instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %04x  %-40s %d-%d\n", start, READ_RAM16(start), instruction, cycles_min, cycles_max);
    }

    if (count == 4)
    {
      printf("        %04x\n", READ_RAM16(start+2));
    }

    start += count;
  }
}


