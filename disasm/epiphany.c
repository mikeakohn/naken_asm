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

#include "disasm/epiphany.h"
#include "table/epiphany.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) ((memory_read_m(memory, a)) | \
                       (memory_read_m(memory, a+1)<<8))
#define READ_RAM32(a) ((memory_read_m(memory, a)) | \
                       (memory_read_m(memory, a+1)<<8) | \
                       (memory_read_m(memory, a+2)<<16) | \
                       (memory_read_m(memory, a+3)<<24))

#define SINGLE_OPCODE(pre, op, cycles, size, instr) \
  if (opcode==op && prefix==pre) \
  { \
    strcpy(instruction, instr); \
    *cycles_min=cycles; \
    *cycles_max=cycles; \
    return size; \
  }

#if 0
static const char *regs[] = {
  "a1", "a2", "a3", "a4",
  "v1", "v2", "v3", "v4", "v5", "sb", "sl", "fp",
  "r12",
  "sp", "lr"
};
#endif

int get_cycle_count_epiphany(unsigned short int opcode)
{
  return -1;
}

int disasm_epiphany(struct _memory *memory, uint32_t address, char *instr, int *cycles_min, int *cycles_max)
{
  uint32_t opcode32;
  uint16_t opcode16;
  uint32_t special;
  int offset;
  int imm;
  //int count = 1;
  int rd, rn, rm;
  int n, m;

  instr[0] = 0;

  opcode16 = READ_RAM16(address);
  opcode32 = READ_RAM32(address);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;
  while(table_epiphany[n].instr != NULL)
  {
    if ((table_epiphany[n].size == 16 &&
         table_epiphany[n].opcode == (opcode16 & table_epiphany[n].mask)) ||
        (table_epiphany[n].size == 32 &&
         table_epiphany[n].opcode == (opcode32 & table_epiphany[n].mask)))
    {
      if (table_epiphany[n].size == 16)
      {
        rd = (opcode16 >> 13) & 0x7;
        rn = (opcode16 >> 10) & 0x7;
        rm = (opcode16 >> 7) & 0x7;
      }
        else
      {
        rd = ((opcode32 >> 13) & 0x7) | (((opcode32 >> 29) & 0x7) << 3);
        rn = ((opcode32 >> 10) & 0x7) | (((opcode32 >> 26) & 0x7) << 3);
        rm = ((opcode32 >> 7) & 0x7) | (((opcode32 >> 23) & 0x7) << 3);
      }

      switch(table_epiphany[n].type)
      {
        case OP_BRANCH_16:
          offset = (int8_t)(opcode16 >> 8);
          offset = offset << 1;
          sprintf(instr, "%s 0x%x  (offset=%d)", table_epiphany[n].instr, address + offset, offset);
          return 2;
        case OP_BRANCH_32:
          offset = ((int32_t)opcode32) >> 8;
          offset = offset << 1;
          sprintf(instr, "%s 0x%x  (offset=%d)", table_epiphany[n].instr, address + offset, offset);
          return 4;
        case OP_DISP_IMM3_16:
          imm = (opcode16 >> 7) & 0x7;
          if (imm != 0)
          {
            sprintf(instr, "%s r%d, [r%d, #%d]", table_epiphany[n].instr, rd, rn, imm);
          }
            else
          {
            sprintf(instr, "%s r%d, [r%d]", table_epiphany[n].instr, rd, rn);
          }
          return 2;
        case OP_DISP_IMM11_32:
          imm = ((opcode32 >> 7) & 0x7) | (((opcode32 >> 16) & 0x7f) << 3);
          imm = ((opcode32 & 0x01000000) == 0) ? imm : -imm;
          if (imm != 0)
          {
            sprintf(instr, "%s r%d, [r%d, #%d]", table_epiphany[n].instr, rd, rn, imm);
          }
            else
          {
            sprintf(instr, "%s r%d, [r%d]", table_epiphany[n].instr, rd, rn);
          }
          return 4;
        case OP_INDEX_16:
          sprintf(instr, "%s r%d, [r%d, r%d]", table_epiphany[n].instr, rd, rn, rm);
          return 2;
        case OP_INDEX_32:
          if ((opcode32 & 0x00100000) == 0)
          {
            sprintf(instr, "%s r%d, [r%d, r%d]", table_epiphany[n].instr, rd, rn, rm);
          }
            else
          {
            sprintf(instr, "%s r%d, [r%d, -r%d]", table_epiphany[n].instr, rd, rn, rm);
          }
          return 4;
        case OP_POST_MOD_16:
          sprintf(instr, "%s r%d, [r%d], r%d", table_epiphany[n].instr, rd, rn, rm);
          return 2;
        case OP_POST_MOD_32:
          if ((opcode32 & 0x00100000) == 0)
          {
            sprintf(instr, "%s r%d, [r%d], r%d", table_epiphany[n].instr, rd, rn, rm);
          }
            else
          {
            sprintf(instr, "%s r%d, [r%d], -r%d", table_epiphany[n].instr, rd, rn, rm);
          }
          return 4;
        case OP_POST_MOD_DISP_32:
          imm = ((opcode32 >> 7) & 0x7) | (((opcode32 >> 16) & 0xff) << 3);
          imm = ((opcode32 & 0x01000000) == 0) ? imm : -imm;
          sprintf(instr, "%s r%d, [r%d], #%d", table_epiphany[n].instr, rd, rn, imm);
          return 4;
        case OP_REG_IMM_16:
          imm = ((opcode32 >> 5) & 0xff);
          sprintf(instr, "%s r%d, #0x%04x", table_epiphany[n].instr, rd, imm);
          return 2;
        case OP_REG_IMM_32:
          imm = ((opcode32 >> 5) & 0xff);
          imm |= ((opcode32 >> 20) & 0xff) << 8;
          sprintf(instr, "%s r%d, #0x%04x", table_epiphany[n].instr, rd, imm);
          return 4;
        case OP_REG_2_IMM_16:
          imm = ((opcode32 >> 7) & 0x7);
          if ((imm & 0x4) != 0) { imm = imm | 0xfffffff8; }
          sprintf(instr, "%s r%d, r%d, #%d", table_epiphany[n].instr, rd, rn, imm);
          return 2;
        case OP_REG_2_IMM_32:
          imm = ((opcode32 >> 7) & 0x7);
          imm |= ((opcode32 >> 16) & 0xff) << 3;
          if ((imm & 0x400) != 0) { imm = imm | 0xfffff800; }
          sprintf(instr, "%s r%d, r%d, #%d", table_epiphany[n].instr, rd, rn, imm);
          return 4;
        case OP_REG_2_IMM5_16:
          imm = ((opcode32 >> 5) & 0x1f);
          sprintf(instr, "%s r%d, r%d, #%d", table_epiphany[n].instr, rd, rn, imm);
          return 2;
        case OP_REG_2_IMM5_32:
          imm = ((opcode32 >> 5) & 0x1f);
          sprintf(instr, "%s r%d, r%d, #%d", table_epiphany[n].instr, rd, rn, imm);
          return 4;
        case OP_REG_2_ZERO_16:
          sprintf(instr, "%s r%d, r%d", table_epiphany[n].instr, rd, rn);
          return 2;
        case OP_REG_2_ZERO_32:
          sprintf(instr, "%s r%d, r%d", table_epiphany[n].instr, rd, rn);
          return 4;
        case OP_REG_3_16:
          sprintf(instr, "%s r%d, r%d, r%d", table_epiphany[n].instr, rd, rn, rm);
          return 2;
        case OP_REG_3_32:
          sprintf(instr, "%s r%d, r%d, r%d", table_epiphany[n].instr, rd, rn, rm);
          return 4;
        case OP_REG_2_16:
          sprintf(instr, "%s r%d, r%d", table_epiphany[n].instr, rd, rn);
          return 2;
        case OP_REG_2_32:
          sprintf(instr, "%s r%d, r%d", table_epiphany[n].instr, rd, rn);
          return 4;
        case OP_REG_1_16:
          sprintf(instr, "%s r%d", table_epiphany[n].instr, rn);
          return 2;
        case OP_REG_1_32:
          sprintf(instr, "%s r%d", table_epiphany[n].instr, rn);
          return 4;
        case OP_NUM6_16:
          imm = opcode32 >> 10;
          sprintf(instr, "%s %d", table_epiphany[n].instr, imm);
          return 2;
        case OP_NONE_16:
          sprintf(instr, "%s", table_epiphany[n].instr);
          return 2;
        case OP_NONE_32:
          sprintf(instr, "%s", table_epiphany[n].instr);
          return 4;
        case OP_SPECIAL_RN_16:
          special = 0xf0400 + (rn * 4);
          sprintf(instr, "%s 0x%05x, r%d", table_epiphany[n].instr, special, rd);
          return 2;
        case OP_RD_SPECIAL_16:
          special = 0xf0400 + (rn * 4);
          sprintf(instr, "%s r%d, 0x%05x", table_epiphany[n].instr, rd, special);
          return 2;
        case OP_SPECIAL_RN_32:
          m = (opcode32 >> 20) & 0x3;
          m = m + 4;
          special = (0xf0000 + (rn * 4)) | (m << 8);
          sprintf(instr, "%s 0x%05x, r%d", table_epiphany[n].instr, special, rd);
          return 4;
        case OP_RD_SPECIAL_32:
          m = (opcode32 >> 20) & 0x3;
          m = m + 4;
          special = (0xf0000 + (rn * 4)) | (m << 8);
          sprintf(instr, "%s r%d, 0x%05x", table_epiphany[n].instr, rd, special);
          return 4;
        default:
          break;
      }
    }
    n++;
  }

  sprintf(instr, "???");

  return 4;
}

void list_output_epiphany(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max,count;
  char instruction[128];

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_epiphany(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);
    fprintf(asm_context->list, "0x%04x: ", start);

    if (count == 2)
    {
      fprintf(asm_context->list, "%02x%02x     ",
        memory_read_m(&asm_context->memory, start + 1),
        memory_read_m(&asm_context->memory, start + 0));
    }
      else
    if (count == 4)
    {
      fprintf(asm_context->list, "%02x%02x%02x%02x ",
        memory_read_m(&asm_context->memory, start + 3),
        memory_read_m(&asm_context->memory, start + 2),
        memory_read_m(&asm_context->memory, start + 1),
        memory_read_m(&asm_context->memory, start + 0));
    }
     else
    {
      printf("Internal Error: %s:%d  count=%d\n", __FILE__, __LINE__, count);
    }

    fprintf(asm_context->list, " %-40s cycles: ", instruction);

    if (cycles_min < 1)
    {
      fprintf(asm_context->list, "?\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      fprintf(asm_context->list, "%d\n", cycles_min);
    }
      else
    {
      fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
    }

    start += count;
  }

}

void disasm_range_epiphany(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    num = READ_RAM(start) | (READ_RAM(start + 1) << 8);

    count = disasm_epiphany(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

    start = start + count;
  }
}

