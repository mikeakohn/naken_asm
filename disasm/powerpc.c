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

#include "disasm/powerpc.h"
#include "table/powerpc.h"

#define READ_RAM(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a+1)<<16)|(memory_read_m(memory, a+2)<<8)|memory_read_m(memory, a+3)

static char *cmp_bits[] = { "fl", "fg", "fe", "fu", "4", "5", "6", "7" };

int get_cycle_count_powerpc(unsigned short int opcode)
{
  return -1;
}

const char *get_spr_name(int value)
{
  int n = 0;

  while(powerpc_spr[n].name != NULL)
  {
    if (value == powerpc_spr[n].value) { return powerpc_spr[n].name; }
    n++;
  }

  return NULL;
}

int disasm_powerpc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  const char *name;
  uint32_t opcode;
  int32_t offset;
  int32_t temp;
  int cr, bf, u, bfa;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);

  n = 0;
  while(table_powerpc[n].instr != NULL)
  {
    if ((opcode & table_powerpc[n].mask) == table_powerpc[n].opcode)
    {
      const uint32_t rd = (opcode >> 21) & 0x1f;
      const uint32_t ra = (opcode >> 16) & 0x1f;
      const uint32_t rb = (opcode >> 11) & 0x1f;
      const uint32_t rc = opcode & 0x1;
      const uint32_t bo = rd;
      const uint32_t rs = rd;
      const uint32_t bi = ra;
      const int16_t simm = opcode & 0xffff;
      const uint16_t uimm = opcode & 0xffff;
      const char *instr = table_powerpc[n].instr;
      uint32_t vc;
      int8_t vsimm;
      const char *dot = "";

      switch(table_powerpc[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", instr);
          break;
        case OP_RD_RA_RB:
        case OP_RS_RA_RB:
          sprintf(instruction, "%s%s r%d, r%d, r%d",
            instr, (rc == 1) ? "." : "", rd, ra, rb);
          break;
        case OP_RA_RS_RB:
          sprintf(instruction, "%s%s r%d, r%d, r%d",
            instr, (rc == 1) ? "." : "", ra, rd, rb);
          break;
        case OP_RA_RS_SH:
          sprintf(instruction, "%s%s r%d, r%d, %d",
            instr, (rc == 1) ? "." : "", ra, rd, rb);
          break;
        case OP_RD_RA:
          sprintf(instruction, "%s%s r%d, r%d",
            instr, (rc == 1) ? "." : "", rd, ra);
          break;
        case OP_RA_RS:
          sprintf(instruction, "%s%s r%d, r%d",
            instr, (rc == 1) ? "." : "", ra, rs);
          break;
        case OP_RD:
          sprintf(instruction, "%s r%d", instr, rd);
          break;
        case OP_RD_RA_SIMM:
          sprintf(instruction, "%s%s r%d, r%d, %d (0x%04x)",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            rd, ra, simm, simm);
          break;
        case OP_RD_SIMM:
          sprintf(instruction, "%s%s r%d, %d (0x%04x)",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            rd, simm, simm);
          break;
        case OP_RA_RS_UIMM:
          sprintf(instruction, "%s%s r%d, r%d, %d (0x%04x)",
            instr,
            ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) != 0) ? "." : "",
            ra, rd, uimm, uimm);
          break;
        case OP_BRANCH:
          offset = opcode & 0x03fffffc;
          if ((offset & 0x02000000) != 0) { offset |= 0xfc000000; }
          sprintf(instruction, "%s 0x%x (offset=%d)", instr, address + offset, offset);
          break;
        case OP_JUMP:
          temp = opcode & 0x03fffffc;
          if ((temp & 0x02000000) != 0) { temp |= 0xfc000000; }
          sprintf(instruction, "%s 0x%x", instr, temp);
          break;
        case OP_BRANCH_COND_BD:
          offset = opcode & 0xfffc;
          if ((offset & 0x8000) != 0) { offset |= 0xffff0000; }
          sprintf(instruction, "%s %d, %d, 0x%x (offset=%d)", instr, bo, bi, address + offset, offset);
          break;
        case OP_JUMP_COND_BD:
          temp = opcode & 0xfffc;
          if ((temp & 0x8000) != 0) { temp |= 0xffff0000; }
          sprintf(instruction, "%s %d, %d, 0x%x", instr, bo, bi, temp);
          break;
        case OP_BRANCH_COND:
          sprintf(instruction, "%s %d, %d", instr, bo, bi);
          break;
        case OP_BRANCH_COND_ALIAS:
          offset = opcode & 0xfffc;
          if ((offset & 0x8000) != 0) { offset |= 0xffff0000; }
          sprintf(instruction, "%s %d, 0x%x  (bo=%d, bi=%d, offset=%d)", instr, bi, address + offset, bo, bi, offset);
          break;
        case OP_CMP:
          cr = (opcode >> 23) & 0x7;
          sprintf(instruction, "%s cr%d, r%d, r%d", instr, cr, ra, rb);
          break;
        case OP_CMPI:
          cr = (opcode >> 23) & 0x7;
          sprintf(instruction, "%s cr%d, r%d, %d", instr, cr, ra, simm);
          break;
        case OP_CRB_CRB_CRB:
          sprintf(instruction, "%s %d, %d, %d", instr, rd, ra, rb);
          break;
        case OP_RD_OFFSET_RA:
        case OP_RS_OFFSET_RA:
          sprintf(instruction, "%s r%d, %d(r%d)", instr, rd, simm, ra);
          break;
        case OP_RD_RA_NB:
        case OP_RS_RA_NB:
          sprintf(instruction, "%s r%d, r%d, %d", instr, rd, ra, rb);
          break;
        case OP_CRD_CRS:
          sprintf(instruction, "%s cr%d, cr%d", instr, rd >> 2, ra >> 2);
          break;
        case OP_CRD:
          sprintf(instruction, "%s cr%d", instr, rd >> 2);
          break;
        case OP_RD_SPR:
          temp = (rb << 5) | ra;
          name = get_spr_name(temp);

          if (name != NULL)
          {
            sprintf(instruction, "%s r%d, %s (spr=%d)", instr, rd, name, temp);
          }
            else
          {
            sprintf(instruction, "%s r%d, %d", instr, rd, temp);
          }
          break;
        case OP_SPR_RS:
          temp = (rb << 5) | ra;
          name = get_spr_name(temp);

          if (name != NULL)
          {
            sprintf(instruction, "%s  %s (spr=%d), r%d", instr, name, temp, rs);
          }
            else
          {
            sprintf(instruction, "%s %d, r%d", instr, temp, rs);
          }
          break;
        case OP_RD_TBR:
          temp = (rb << 5) | ra;

          if (temp == 268) { name = "tbl"; }
          else if (temp == 269) { name = "tbu"; }
          else { name = NULL; }

          if (name != NULL)
          {
            sprintf(instruction, "%s r%d, %s (tbr=%d)", instr, rd, name, temp);
          }
            else
          {
            sprintf(instruction, "%s r%d, %d", instr, rd, temp);
          }
          break;
        case OP_CRM_RS:
          sprintf(instruction, "%s 0x%02x, r%d", instr, (opcode >> 12) & 0xff, rs);
          break;
        case OP_RA_RS_SH_MB_ME:
          sprintf(instruction, "%s%s r%d, r%d, %d, %d, %d",
            instr, (rc == 1) ? "." : "", ra, rs, rb,
            (opcode >> 6) & 0x1f, (opcode >> 1) & 0x1f);
          break;
        case OP_STRM:
          sprintf(instruction, "%s %d", instr, (opcode >> 21) & 0x3);
          break;
        case OP_RA_RB_STRM:
          sprintf(instruction, "%s r%d, r%d, %d",
            instr, ra, rb, (opcode >> 21) & 0x3);
          break;
        case OP_VD_RA_RB:
        case OP_VS_RA_RB:
          sprintf(instruction, "%s v%d, r%d, r%d", instr, rd, ra, rb);
          break;
        case OP_VD:
          sprintf(instruction, "%s v%d", instr, rd);
          break;
        case OP_VB:
          sprintf(instruction, "%s v%d", instr, rb);
          break;
        case OP_VD_VA_VB:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             (((opcode >> 10) & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s v%d, v%d, v%d", instr, dot, rd, ra, rb);
          break;
        case OP_VD_VA_VB_VC:
          vc = (opcode >> 6) & 0x1f;
          sprintf(instruction, "%s v%d, v%d, v%d, v%d", instr, rd, ra, rb, vc);
          break;
        case OP_VD_VA_VC_VB:
          vc = (opcode >> 6) & 0x1f;
          sprintf(instruction, "%s v%d, v%d, v%d, v%d", instr, rd, ra, vc, rb);
          break;
        case OP_VD_VB_UIMM:
          sprintf(instruction, "%s v%d, %d, v%d", instr, rd, ra, rb);
          break;
        case OP_VD_VB_SIMM:
          vsimm = ra;
          if ((ra & 0x10) != 0) { vsimm |= 0xf0; }
          sprintf(instruction, "%s v%d, %d, v%d", instr, rd, vsimm, rb);
          break;
        case OP_VD_SIMM:
          vsimm = ra;
          if ((ra & 0x10) != 0) { vsimm |= 0xf0; }
          sprintf(instruction, "%s v%d, %d", instr, rd, vsimm);
          break;
        case OP_VD_VB:
          sprintf(instruction, "%s v%d, v%d", instr, rd, ra);
          break;
        case OP_VD_VA_VB_SH:
          vc = (opcode >> 6) & 0xf;
          sprintf(instruction, "%s v%d, v%d, v%d, %d", instr, rd, ra, rb, vc);
          break;
        case OP_FRT_OFFSET_RA:
          sprintf(instruction, "%s fp%d, %d(r%d)", instr, rd, simm, ra);
          break;
        case OP_FRT_RA_RB:
          sprintf(instruction, "%s fp%d, r%d, r%d", instr, rd, ra, rb);
          break;
        case OP_FRT_FRB:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s fp%d, fp%d", instr, dot, rd, rb);
          break;
        case OP_FRT_FRA_FRB:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s fp%d, fp%d, fp%d", dot, instr, rd, ra, rb);
          break;
        case OP_FRT_FRA_FRC:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s fp%d, fp%d, fp%d", dot, instr, rd, ra, rc);
          break;
        case OP_FRT_FRA_FRC_FRB:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s fp%d, fp%d, fp%d, fp%d", instr, dot, rd, ra, rc, rb);
          break;
        case OP_BF_FRA_FRB:
          bf = (opcode >> 23) & 0x7;
          sprintf(instruction, "%s %s, fp%d, fp%d", instr, cmp_bits[bf], ra, rb);
          break;
        case OP_FRT:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s fp%d", instr, dot, rd);
          break;
        case OP_BF_BFA:
          bf = (opcode >> 23) & 0x7;
          bfa = (opcode >> 18) & 0x7;
          sprintf(instruction, "%s %s, %d", instr, cmp_bits[bf], bfa);
          break;
        case OP_BF_U:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          bf = (opcode >> 23) & 0x7;
          u = (opcode >> 12) & 0xf;
          sprintf(instruction, "%s%s %s, %d", instr, dot, cmp_bits[bf], u);
          break;
        case OP_FLM_FRB:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          temp = (opcode >> 17) & 0xff;
          sprintf(instruction, "%s%s 0x%02x, fp%d", instr, dot, temp, rb);
          break;
        case OP_BT:
          if ((table_powerpc[n].flags & FLAG_DOT) &&
             ((opcode & 1) != 0))
          {
            dot = ".";
          }
          sprintf(instruction, "%s%s %d", instr, dot, rd);
          break;
        default:
          strcpy(instruction, "???");
          break;
      }

      return 4;
    }

    n++;
  }

  strcpy(instruction, "???");

  return 4;
}

void list_output_powerpc(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_powerpc(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == -1)
    { fprintf(asm_context->list, "\n"); }
      else
    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += 4;
  }
}

void disasm_range_powerpc(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  uint32_t opcode;
  int cycles_min = 0,cycles_max = 0;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    opcode = memory_read32_m(memory, start);

    count = disasm_powerpc(memory, start, instruction, &cycles_min, &cycles_max);

    printf("0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == -1)
    { printf("\n"); }
      else
    if (cycles_min == cycles_max)
    { printf("%d\n", cycles_min); }
      else
    { printf("%d-%d\n", cycles_min, cycles_max); }

    if (count == 0) { count = 4; }

    start = start + count;
  }
}

