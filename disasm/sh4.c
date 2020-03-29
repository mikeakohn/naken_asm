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

#include "disasm/sh4.h"
#include "table/sh4.h"

int get_cycle_count_sh4(unsigned short int opcode)
{
  return -1;
}

int disasm_sh4(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint16_t opcode;
  const char *special;
  int rm, rn;
  int8_t imm_s8;
  uint8_t imm_u8;
  int offset, disp;
  int n;

  opcode = memory_read16_m(memory, address);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;

  while (table_sh4[n].instr != NULL)
  {
    if ((opcode & table_sh4[n].mask) == table_sh4[n].opcode)
    {
      switch (table_sh4[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_sh4[n].instr);
          return 2;
        }
        case OP_REG:
        {
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_FREG:
        {
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_DREG:
        {
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s dr%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_REG_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d, r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_FREG_FREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr%d, fr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_DREG_DREG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s dr%d, dr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_DREG_XDREG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s dr%d, xd%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_XDREG_DREG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s xdr%d, dr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_XDREG_XDREG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s xdr%d, xd%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_FVREG_FVREG:
        {
          rm = ((opcode >> 8) & 0x3) * 4;
          rn = ((opcode >> 10) & 0x3) * 4;
          sprintf(instruction, "%s fv%d, fv%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_IMM_REG:
        {
          imm_s8 = (int8_t)(opcode & 0xff);
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s #%d, r%d", table_sh4[n].instr, imm_s8, rn);
          return 2;
        }
        case OP_IMM_R0:
        {
          imm_u8 = (uint8_t)(opcode & 0xff);
          sprintf(instruction, "%s #0x%02x, r0", table_sh4[n].instr, imm_u8);
          return 2;
        }
        case OP_IMM_AT_R0_GBR:
        {
          imm_u8 = (uint16_t)(opcode & 0xff);
          sprintf(instruction, "%s #0x%02x, @(r0,GBR)",
            table_sh4[n].instr, imm_u8);
          return 2;
        }
        case OP_BRANCH_S9:
        {
          offset = (int8_t)(opcode & 0xff);
          offset *= 2;
          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_sh4[n].instr, address + 4 + offset, offset);
          return 2;
        }
        case OP_BRANCH_S13:
        {
          offset = opcode & 0xfff;
          if ((offset & 0x800) != 0) { offset |= 0xfffff000; }
          offset *= 2;
          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_sh4[n].instr, address + 4 + offset, offset);
          return 2;
        }
        case OP_FREG_FPUL:
        {
          rm = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr%d, FPUL", table_sh4[n].instr, rm);
          return 2;
        }
        case OP_DREG_FPUL:
        {
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s dr%d, FPUL", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_FPUL_FREG:
        {
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s FPUL, fr%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_FPUL_DREG:
        {
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s FPUL, dr%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_FR0_FREG_FREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr0, fr%d, fr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_FREG_AT_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr%d, @r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_DREG_AT_REG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s dr%d, @r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_FREG_AT_MINUS_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr%d, @-r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_DREG_AT_MINUS_REG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s dr%d, @-r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_FREG_AT_R0_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s fr%d, @(r0,r%d)", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_DREG_AT_R0_REG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s dr%d, @(r0,r%d)", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_XDREG_AT_REG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s dr%d, @r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_XDREG_AT_MINUS_REG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s dr%d, @-r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_XDREG_AT_R0_REG:
        {
          rm = ((opcode >> 5) & 0x7) * 2;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s dr%d, @(r0,r%d)", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_DREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s @r%d, dr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_PLUS_DREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s @r%d+, dr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_R0_REG_DREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s @(r0,r%d), dr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_FREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d, fr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_PLUS_FREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d+, fr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_R0_REG_FREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0x7;
          sprintf(instruction, "%s @(r0,r%d), fr%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_XDREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s @r%d, xd%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_PLUS_XDREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s @r%d+, xd%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_R0_REG_XDREG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = ((opcode >> 9) & 0x7) * 2;
          sprintf(instruction, "%s @(r0,r%d), xd%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_XMTRX_FVREG:
        {
          rn = ((opcode >> 10) & 0x3) * 4;
          sprintf(instruction, "%s XMTRX, fv%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_AT_REG:
        {
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_REG_SPECIAL:
        {
          special = sh4_specials[table_sh4[n].special];
          rm = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d, %s", table_sh4[n].instr, rm, special);
          return 2;
        }
        case OP_REG_REG_BANK:
        {
          rm = (opcode >> 8) & 0xf;
          rn = (opcode >> 4) & 0x7;
          sprintf(instruction, "%s r%d, r%d_bank", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_PLUS_SPECIAL:
        {
          special = sh4_specials[table_sh4[n].special];
          rm = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d+, %s", table_sh4[n].instr, rm, special);
          return 2;
        }
        case OP_AT_REG_PLUS_REG_BANK:
        {
          special = sh4_specials[table_sh4[n].special];
          rm = (opcode >> 8) & 0xf;
          rn = (opcode >> 4) & 0x7;
          sprintf(instruction, "%s @r%d+, r%d_bank", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_PLUS_AT_REG_PLUS:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d+, @r%d+", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_REG_AT_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d, @r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_REG_AT_MINUS_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d, @-r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_REG_AT_R0_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d, @(r0,r%d)", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_R0_AT_DISP_GBR:
        {
          disp = (opcode & 0xff) * table_sh4[n].special;
          sprintf(instruction, "%s r0, @(%d,GBR)", table_sh4[n].instr, disp);
          return 2;
        }
        case OP_R0_AT_DISP_REG:
        {
          disp = (opcode & 0xf) * table_sh4[n].special;
          rn = (opcode >> 4) & 0xf;
          sprintf(instruction, "%s r0, @(%d,r%d)", table_sh4[n].instr, disp, rn);
          return 2;
        }
        case OP_AT_REG_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d, r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_REG_PLUS_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @r%d+, r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_R0_REG_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @(r0,r%d), r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_AT_DISP_GBR_R0:
        {
          disp = (opcode & 0xff) * table_sh4[n].special;
          sprintf(instruction, "%s @(%d,GBR), r0", table_sh4[n].instr, disp);
          return 2;
        }
        case OP_AT_DISP_REG_R0:
        {
          disp = (opcode & 0xf) * table_sh4[n].special;
          rm = (opcode >> 4) & 0xf;
          sprintf(instruction, "%s @(%d,r%d), r0", table_sh4[n].instr, disp, rm);
          return 2;
        }
        case OP_AT_DISP_PC_REG:
        {
          disp = (opcode & 0xff) * table_sh4[n].special;
          rn = (opcode >> 8) & 0xf;
          int calc_address = table_sh4[n].special == 2 ?
            address + 4 : (address + 4) & 0xfffffffc;

          sprintf(instruction, "%s @(%d,PC), r%d (address=0x%04x)",
            table_sh4[n].instr, disp, rn,
            calc_address + disp);
          return 2;
        }
        case OP_AT_DISP_REG_REG:
        {
          disp = (opcode & 0xf) * table_sh4[n].special;
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s @(%d,r%d), r%d", table_sh4[n].instr, disp, rm, rn);
          return 2;
        }
        case OP_AT_DISP_PC_R0:
        {
          disp = (opcode & 0xff) * table_sh4[n].special;
          sprintf(instruction, "%s @(%d,PC), r0 (address=0x%04x)",
            table_sh4[n].instr, disp, ((address + 4) & 0xfffffffc) + disp);
          return 2;
        }
        case OP_R0_AT_REG:
        {
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r0, @r%d", table_sh4[n].instr, rn);
          return 2;
        }
        case OP_SPECIAL_REG:
        {
          special = sh4_specials[table_sh4[n].special];
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s %s, r%d", table_sh4[n].instr, special, rn);
          return 2;
        }
        case OP_REG_BANK_REG:
        {
          rm = (opcode >> 4) & 0x7;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d_bank, r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_SPECIAL_AT_MINUS_REG:
        {
          special = sh4_specials[table_sh4[n].special];
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s %s, @-r%d", table_sh4[n].instr, special, rn);
          return 2;
        }
        case OP_REG_BANK_AT_MINUS_REG:
        {
          rm = (opcode >> 4) & 0x7;
          rn = (opcode >> 8) & 0xf;
          sprintf(instruction, "%s r%d_bank, @-r%d", table_sh4[n].instr, rm, rn);
          return 2;
        }
        case OP_IMM:
        {
          imm_u8 = (uint8_t)(opcode & 0xff);
          sprintf(instruction, "%s #0x%02x", table_sh4[n].instr, imm_u8);
          return 2;
        }
        case OP_REG_AT_DISP_REG:
        {
          rm = (opcode >> 4) & 0xf;
          rn = (opcode >> 8) & 0xf;
          imm_u8 = (opcode & 0xf) * 4;
          sprintf(instruction, "%s r%d, @(%d,r%d)", table_sh4[n].instr, rm, imm_u8, rn);
          return 2;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }

    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_sh4(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_sh4(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(&asm_context->memory, start);

    fprintf(asm_context->list, "0x%04x: %04x   %-40s\n", start, opcode, instruction);

    start += count;
  }
}

void disasm_range_sh4(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_sh4(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: %04x   %-40s\n", start, opcode, instruction);

    start = start + count;
  }
}

