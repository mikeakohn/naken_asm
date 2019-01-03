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

#include "disasm/dspic.h"
#include "table/dspic.h"

#define READ_RAM(a) memory_read_m(memory, a)

static char accum[] = { 'A','B' };
static char *bflag[] = { "", ".b" };
static char *addr_modes[] = { "w%d", "[w%d]", "[w%d--]", "[w%d++]", "[--w%d]", "[++w%d]" };
static char *mmm_table[] = { "w4*w5", "w4*w6", "w4*w7", "???", "w5*w6", "w5*w7", "w6*w7", "???" };

int get_cycle_count_dspic(unsigned short int opcode)
{
  return -1;
}

static void get_wd(char *temp, int reg, int attr, int reg2)
{
  switch(attr)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      sprintf(temp, addr_modes[attr], reg);
      break;
    case 6:
      sprintf(temp, "[w%d+w%d]", reg, reg2);
      break;
    default:
      strcpy(temp, "???");
      break;
  }
}

static void get_prefetch(char *temp, int w, int xx, int iiii)
{
  if (iiii == 0xc) { sprintf(temp, ", [w%d+12], w%d", w, xx + 4); return; }
  if (iiii >= 8) { w++; iiii = iiii & 0x7; }
  if (iiii == 0) { sprintf(temp, ", [w%d], w%d", w, xx + 4); return; }
  if ((iiii & 0x4)!=0)
  {
    iiii = ((iiii ^ 0x7) + 1);
    sprintf(temp, ", [w%d]-=%d, w%d", w, iiii * 2, xx + 4);
  }
    else
  {
    sprintf(temp, ", [w%d]+=%d, w%d", w, iiii * 2, xx + 4);
  }
}

static void get_prefetch_half(char *temp, int w, int iiii)
{
  if (iiii == 0xc) { sprintf(temp, ", [w%d+12]", w); return; }
  if (iiii >= 8) { w++; iiii = iiii & 0x7; }
  if (iiii == 0) { sprintf(temp, ", [w%d]", w); return; }
  if ((iiii & 0x4) != 0)
  {
    iiii = ((iiii ^ 0x7) + 1);
    sprintf(temp, ", [w%d]-=%d", w, iiii * 2);
  }
    else
  {
    sprintf(temp, ", [w%d]+=%d", w, iiii * 2);
  }
}

static void parse_dsp(char *instruction, uint32_t opcode, int has_aa)
{
  char temp[32];
  int xx = (opcode >> 12) & 0x3;
  int yy = (opcode >> 10) & 0x3;
  int iiii = (opcode >> 6) & 0xf;
  int jjjj = (opcode >> 2) & 0xf;
  int aa = opcode & 0x3;

  if (iiii != 4)
  {
    get_prefetch(temp, 8, xx, iiii);
    strcat(instruction, temp);
  }

  if (jjjj != 4)
  {
    get_prefetch(temp, 10, yy, jjjj);
    strcat(instruction, temp);
  }

  if (has_aa == 0) { return; }

  if (aa == 0)
  {
    strcat(instruction, ", w13");
  }
    else
  if (aa == 1)
  {
    strcat(instruction, ", [w13]+=2");
  }
}

int disasm_dspic(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  char temp[32];
  char temp2[32];
  uint32_t opcode;
  int count = 4;
  int16_t offset;
  int n,b,d,f,a,w,lit;

  opcode = memory_read32_m(memory, address);

  n = 0;
  while(table_dspic[n].name != NULL)
  {
    if ((opcode & table_dspic[n].mask) == table_dspic[n].opcode)
    {
      //strcpy(instruction, table_dspic[n].name);
      *cycles_min = table_dspic[n].cycles_min;
      *cycles_max = table_dspic[n].cycles_max;

      switch(table_dspic[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_dspic[n].name);
          return 4;
        case OP_F:
          f = opcode & 0xffff;
          sprintf(instruction, "%s 0x%04x", table_dspic[n].name, f);
          return 4;
        case OP_F_WREG:
          b = (opcode >> 14) & 1;
          d = (opcode >> 13) & 1;
          f = opcode & 0x1fff;
          sprintf(instruction, "%s%s 0x%04x%s", table_dspic[n].name, bflag[b], f, (d == 0 ? ", wreg" : ""));
          return 4;
        case OP_F_OR_WREG:
          b = (opcode >> 14) & 1;
          d = (opcode >> 13) & 1;
          f = opcode & 0x1fff;
          if (d == 0)
          {
            sprintf(instruction, "%s%s wreg", table_dspic[n].name, bflag[b]);
          }
            else
          {
            sprintf(instruction, "%s%s 0x%04x", table_dspic[n].name, bflag[b], f);
          }
          return 4;
        case OP_WREG_F:
          b = (opcode >> 14) & 1;
          f = opcode & 0x1fff;
          sprintf(instruction, "%s%s wreg, 0x%04x", table_dspic[n].name, bflag[b], f);
          return 4;
        case OP_ACC:
          a = (opcode >> 15) & 1;
          sprintf(instruction, "%s %c", table_dspic[n].name, (a == 0) ? 'A' : 'B');
          return 4;
        case OP_ACC_LIT4_WD:
          b = (opcode >> 16) & 1;  // .R ?
          a = (opcode >> 15) & 1;
          lit = (opcode >> 7) & 0xf;
          if ((lit & 0x8) != 0) { lit = -((lit ^ 0xf) + 1); }
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, (opcode >> 11) & 0xf);

          if (lit == 0)
          {
            sprintf(instruction, "%s%s %c, %s", table_dspic[n].name, bflag[b], accum[a], temp);
          }
            else
          {
            sprintf(instruction, "%s%s %c, #%d, %s", table_dspic[n].name, bflag[b], accum[a], lit, temp);
          }
          return 4;
        case OP_ACC_LIT6:
          a = (opcode >> 15) & 1;
          lit = opcode & 0x3f;
          if ((lit & 0x20) != 0) { lit = -((lit ^ 0x3f) + 1); }
          sprintf(instruction, "%s %c, #%d", table_dspic[n].name, accum[a], lit);
          return 4;
        case OP_ACC_WB:
          a = (opcode >> 15) & 1;
          w = opcode & 0xf;
          sprintf(instruction, "%s %c, w%d", table_dspic[n].name, accum[a], w);
          return 4;
        case OP_BRA:
          offset = opcode & 0xffff;
          sprintf(instruction, "%s 0x%04x (%d)", table_dspic[n].name, (address / 2) + 2 + ((int32_t)(offset) * 2), ((int32_t)(offset) * 2));
          return 4;
        case OP_CP0_WS:
          b = (opcode >> 11) & 1;
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          sprintf(instruction, "%s%s %s", table_dspic[n].name, bflag[b], temp);
          return 4;
        case OP_CP_F:
          b = (opcode >> 14) & 1;
          f = opcode & 0x1fff;
          sprintf(instruction, "%s%s 0x%04x", table_dspic[n].name, bflag[b], f);
          return 4;
        case OP_D_WNS_WND_1:
          if (((opcode >> 7) & 0xf) == 15 && ((opcode >> 11) & 0x7) == 3)
          {
            n++;
            continue;
          }
          w = ((opcode >> 1) & 0x7) * 2;
          get_wd(temp, (opcode >> 7) & 0xf, (opcode >> 11) & 0x7, 0);
          sprintf(instruction, "%s.d w%d, %s", table_dspic[n].name, w, temp);
          return 4;
        case OP_D_WNS_WND_2:
          if ((opcode & 0xf) == 15 && ((opcode >> 4) & 0x7) == 4)
          {
            n++;
            continue;
          }
          w = ((opcode >> 8) & 0x7) * 2;
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          sprintf(instruction, "%s.d %s, w%d", table_dspic[n].name, temp, w);
          return 4;
        case OP_F_BIT4:
          f = opcode & 0x1fff;
          lit = (opcode >> 13) & 0x7;
          sprintf(instruction, "%s.b 0x%x, #%d", table_dspic[n].name, f, lit);
          return 4;
        case OP_F_BIT4_2:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          lit = (opcode >> 12) & 0xf;
          b = (opcode >> 11) & 1;
          sprintf(instruction, "%s.%c %s, #%d", table_dspic[n].name, (b == 0) ? 'c' : 'z', temp, lit);
          return 4;
        case OP_F_WND:
          f = ((opcode >> 4) & 0x7ffff) << 1;
          w = opcode & 0xf;
          sprintf(instruction, "%s 0x%04x, w%d", table_dspic[n].name, f, w);
          return 4;
        case OP_GOTO:
          f = (opcode & 0xffff) | (memory_read32_m(memory, address + 4) << 16);
          sprintf(instruction, "%s 0x%04x", table_dspic[n].name, f);
          return 8;
        case OP_LIT1:
          lit = opcode & 0x1;
          sprintf(instruction, "%s #%d", table_dspic[n].name, lit);
          return 4;
        case OP_LIT10_WN:
          lit = (opcode >> 4) & 0x3ff;
          b = (opcode >> 14) & 0x1;
          w = opcode & 0xf;
          sprintf(instruction, "%s%s #0x%x, w%d", table_dspic[n].name, bflag[b], lit, w);
          return 4;
        case OP_LIT14:
          lit = opcode & 0x3fff;
          sprintf(instruction, "%s #%d", table_dspic[n].name, lit);
          return 4;
        case OP_LIT14_EXPR:
          lit = opcode & 0x3fff;
          offset = memory_read32_m(memory, address+4);
          sprintf(instruction, "%s #%d, 0x%04x (%d)", table_dspic[n].name, lit, ((address / 2) + 2) + ((int32_t)(offset) * 2), ((int32_t)(offset) * 2));
          return 8;
        case OP_LIT16_WND:
          lit = (opcode >> 4) & 0xffff;
          w = opcode & 0xf;
          sprintf(instruction, "%s #0x%02x, w%d", table_dspic[n].name, lit, w);
          return 4;
        case OP_LIT8_WND:
          lit = (opcode >> 4) & 0xff;
          w = opcode & 0xf;
          sprintf(instruction, "%s #0x%02x, w%d", table_dspic[n].name, lit, w);
          return 4;
        case OP_LNK_LIT14:
          lit = opcode & 0x3fff;
          sprintf(instruction, "%s #0x%02x", table_dspic[n].name, lit);
          return 4;
        case OP_POP_D_WND:
          w = ((opcode >> 8) & 0x7) * 2;
          sprintf(instruction, "%s.d w%d", table_dspic[n].name, w);
          return 4;
        case OP_POP_S:
          sprintf(instruction, "%s.s", table_dspic[n].name);
          return 4;
        case OP_POP_WD:
          get_wd(temp, (opcode >> 7) & 0xf, (opcode >> 11) & 0x7, (opcode >> 15) & 0xf);
          sprintf(instruction, "%s %s", table_dspic[n].name, temp);
          return 4;
        case OP_PUSH_D_WNS:
          w = ((opcode >> 1) & 0x7) * 2;
          sprintf(instruction, "%s.d w%d", table_dspic[n].name, w);
          return 4;
        case OP_SS_WB_WS_WND:
        case OP_SU_WB_WS_WND:
        case OP_US_WB_WS_WND:
        case OP_UU_WB_WS_WND:
          w = (opcode >> 11) & 0xf;
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          d = (opcode >> 7) & 0xf;
          if (table_dspic[n].type == OP_SS_WB_WS_WND) { sprintf(temp2, ".ss"); }
            else
          if (table_dspic[n].type == OP_SU_WB_WS_WND) { sprintf(temp2, ".su"); }
            else
          if (table_dspic[n].type == OP_US_WB_WS_WND) { sprintf(temp2, ".us"); }
            else
          if (table_dspic[n].type == OP_UU_WB_WS_WND) { sprintf(temp2, ".uu"); }
            else
          { strcpy(temp2, "???"); }
          sprintf(instruction, "%s%s w%d, %s, w%d", table_dspic[n].name, temp2, w, temp, d);
          return 4;
        case OP_SU_WB_LIT5_WND:
        case OP_UU_WB_LIT5_WND:
          w = (opcode >> 11) & 0xf;
          lit = opcode & 0x1f;
          d = (opcode >> 7) & 0xf;
          if (table_dspic[n].type == OP_SU_WB_LIT5_WND)
          {
            sprintf(temp2, ".su");
          }
            else
          if (table_dspic[n].type == OP_UU_WB_LIT5_WND)
          {
            sprintf(temp2, ".uu");
          }
            else
          {
            strcpy(temp2, "???");
          }
          sprintf(instruction, "%s%s w%d, #%d, w%d", table_dspic[n].name, temp2, w, lit, d);
          return 4;
        case OP_S_WM_WN:
        case OP_U_WM_WN:
          w = (opcode >> 7) & 0xf;
          d = opcode & 0xf;
          b = (opcode >> 6) & 1;
          if (table_dspic[n].type == OP_S_WM_WN) { sprintf(temp2, ".s"); }
            else
          if (table_dspic[n].type == OP_U_WM_WN) { sprintf(temp2, ".u"); }
            else
          { strcpy(temp2, "???"); }
          if (b == 1) { strcat(temp2, "d"); }
          sprintf(instruction, "%s%s w%d, w%d", table_dspic[n].name, temp2, w, d);
          return 4;
        case OP_WB_LIT4_WND:
          w = (opcode >> 11) & 0xf;
          lit = opcode & 0xf;
          d = (opcode >> 7) & 0xf;
          sprintf(instruction, "%s w%d, #%d, w%d", table_dspic[n].name, w, lit, d);
          return 4;
        case OP_WB_LIT5:
          w = (opcode >> 11) & 0xf;
          lit = opcode & 0x1f;
          b = (opcode >> 10) & 0x1;
          sprintf(instruction, "%s%s w%d, #%d", table_dspic[n].name, bflag[b], w, lit);
          return 4;
        case OP_WB_LIT5_WD:
          w = (opcode >> 15) & 0xf;
          lit = opcode & 0x1f;
          get_wd(temp, (opcode >> 7) & 0xf, (opcode >> 11) & 0x7, 0);
          b = (opcode >> 14) & 0x1;
          sprintf(instruction, "%s%s w%d, #%d, %s", table_dspic[n].name, bflag[b], w, lit, temp);
          return 4;
        case OP_WB_WN:
          w = (opcode >> 11) & 0xf;
          d = opcode & 0xf;
          b = (opcode >> 10) & 0x1;
          sprintf(instruction, "%s%s w%d, w%d", table_dspic[n].name, bflag[b], w, d);
          return 4;
        case OP_WB_WNS_WND:
          w = (opcode >> 11) & 0xf;
          b = opcode & 0xf;
          d = (opcode >> 7) & 0xf;
          sprintf(instruction, "%s w%d, w%d, w%d", table_dspic[n].name, w, b, d);
          return 4;
        case OP_WB_WS:
          w = (opcode >> 11) & 0xf;
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          b = (opcode >> 10) & 0x1;
          sprintf(instruction, "%s%s w%d, %s", table_dspic[n].name, bflag[b], w, temp);
          return 4;
        case OP_WB_WS_WD:
          w = (opcode >> 15) & 0xf;
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          get_wd(temp2, (opcode >> 7) & 0xf, (opcode >> 11) & 0x7, 0);
          b = (opcode >> 14) & 1;
          sprintf(instruction, "%s%s w%d, %s, %s", table_dspic[n].name, bflag[b], w, temp, temp2);
          return 4;
        case OP_WD:
          get_wd(temp, (opcode >> 7) & 0xf, (opcode >> 11) & 0x7, 0);
          b = (opcode >> 14) & 1;
          sprintf(instruction, "%s%s %s", table_dspic[n].name, bflag[b], temp);
          return 4;
        case OP_WN:
          w = opcode & 0xf;
          sprintf(instruction, "%s w%d", table_dspic[n].name, w);
          return 4;
        case OP_B_WN:
          w = opcode & 0xf;
          b = (opcode >> 14) & 1;
          sprintf(instruction, "%s%s w%d", table_dspic[n].name, bflag[b], w);
          return 4;
        case OP_DAW_B_WN:
          w = opcode & 0xf;
          sprintf(instruction, "%s.b w%d", table_dspic[n].name, w);
          return 4;
        case OP_WN_EXPR:
          offset = memory_read32_m(memory, address + 4);
          w = opcode & 0xf;
          sprintf(instruction, "%s w%d, 0x%04x (%d)", table_dspic[n].name, w, ((address / 2) + 2) + ((int32_t)(offset) * 2), ((int32_t)(offset) * 2));
          return 8;
        case OP_WNS_F:
          w = opcode & 0xf;
          f = (((opcode >> 4) & 0x7fff) << 1);
          sprintf(instruction, "%s w%d, 0x%04x", table_dspic[n].name, w, f);
          return 4;
        case OP_WNS_WD_LIT10:
          w = opcode & 0xf;
          d = (opcode >> 7) & 0xf;
          lit = (((opcode >> 15) & 0xf) << 6) | (((opcode >> 11) & 0x7) << 3) | ((opcode >> 4) & 0x7);
          b = (opcode >> 14) & 1;
          if ((lit & 0x200) != 0) { lit = -((lit ^ 0x3ff) + 1); }
          if (b == 0)
          {
            lit = lit * 2;
          }
          sprintf(instruction, "%s%s w%d, [w%d%s%d]", table_dspic[n].name, bflag[b], w, d, (lit < 0) ? "" : "+", lit);
          return 4;
        case OP_WNS_WND:
          w = opcode & 0xf;
          d = (opcode >> 7) & 0xf;
          sprintf(instruction, "%s w%d, w%d", table_dspic[n].name, w, d);
          return 4;
        case OP_WS_BIT4:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          lit = (opcode >> 12) & 0xf;
          b = (opcode >> 10) & 1;
          sprintf(instruction, "%s%s %s, #%d", table_dspic[n].name, bflag[b], temp, lit);
          return 4;
        case OP_WS_BIT4_2:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          lit = (opcode >> 12) & 0xf;
          sprintf(instruction, "%s %s, #%d", table_dspic[n].name, temp, lit);
          return 4;
        case OP_WS_LIT10_WND:
          w = opcode & 0xf;
          d = (opcode >> 7) & 0xf;
          lit = (((opcode >> 15) & 0xf) << 6) | (((opcode >> 11) & 0x7) << 3) | ((opcode >> 4) & 0x7);
          b = (opcode >> 14) & 1;
          if ((lit & 0x200) != 0) { lit = -((lit ^ 0x3ff) + 1); }
          if (b == 0)
          {
            lit = lit * 2;
          }
          sprintf(instruction, "%s%s [w%d%s%d], w%d", table_dspic[n].name, bflag[b], w, (lit < 0) ? "" : "+", lit, d);
          return 4;
        case OP_WS_LIT4_ACC:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, (opcode >> 11) & 0xf);
          lit = (opcode >> 7) & 0xf;
          if ((lit & 0x8) != 0) { lit = -((lit ^ 0xf) + 1); }
          a = (opcode >> 15) & 0x1;
          sprintf(instruction, "%s %s, #%d, %c", table_dspic[n].name, temp, lit, accum[a]);
          return 4;
        case OP_WS_PLUS_WB:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, (opcode >> 11) & 0xf);
          sprintf(instruction, "%s %s", table_dspic[n].name, temp);
          return 4;
        case OP_WS_WB:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, 0);
          w = (opcode >> 11) & 0xf;
          b = (opcode >> 14) & 1;
          sprintf(instruction, "%s%s %s, w%d", table_dspic[n].name, (b == 0) ? ".c" : ".z", temp, w);
          return 4;
        case OP_WS_WB_WD_WB:
          if (((opcode >> 7) & 0xf) == 15 && ((opcode >> 11) & 0x7) == 3)
          {
            n++;
            continue;
          }

          if ((opcode & 0xf) == 15 && ((opcode >> 4) & 0x7) == 4)
          {
            n++;
            continue;
          }
        case OP_WS_WD:
        case OP_WS_WND:
          get_wd(temp, opcode & 0xf, (opcode >> 4) & 0x7, (opcode >> 15) & 0xf);
          get_wd(temp2, (opcode >> 7) & 0xf, (opcode >> 11) & 0x7, (opcode >> 15) & 0xf);
          b = (opcode >> 14) & 1;
          sprintf(instruction, "%s%s %s, %s", table_dspic[n].name, bflag[b], temp, temp2);
          return 4;
        case OP_WM_WN:
          w = (opcode >> 11) & 0xf;
          d = opcode & 0xf;
          sprintf(instruction, "%s w%d, w%d", table_dspic[n].name, w, d);
          return 4;
        case OP_A_WX_WY_AWB:
          a = (opcode >> 15) & 0x1;
          sprintf(instruction, "%s %c", table_dspic[n].name, accum[a]);
          parse_dsp(instruction, opcode, 1);
          return 4;
        case OP_N_WM_WN_ACC_WX_WY:
          a = (opcode >> 15) & 0x1;
          d = (opcode >> 16) & 0x7;
          sprintf(instruction, "%s.n %s, %c", table_dspic[n].name, mmm_table[d], accum[a]);
          parse_dsp(instruction, opcode, 0);
          return 4;
        case OP_WM_WM_ACC_WX_WY:
          a = (opcode >> 15) & 0x1;
          d = ((opcode >> 16) & 0x3) + 4;
          sprintf(instruction, "%s w%d*w%d, %c", table_dspic[n].name, d + 4, d + 4, accum[a]);
          parse_dsp(instruction, opcode, 0);
          return 4;
        case OP_WM_WN_ACC_WX_WY:
          //if ((opcode & 0x3)<2) { continue; }
          a = (opcode >> 15) & 0x1;
          d = (opcode >> 16) & 0x7;
          sprintf(instruction, "%s %s, %c", table_dspic[n].name, mmm_table[d], accum[a]);
          parse_dsp(instruction, opcode, 0);
          return 4;
        case OP_WM_WM_ACC_WX_WY_WXD:
          a = (opcode >> 15) & 0x1;
          d = (opcode >> 16) & 0x3;
          sprintf(instruction, "%s w%d*w%d, %c", table_dspic[n].name, d + 4, d + 4, accum[a]);
          get_prefetch_half(temp, 8, (opcode >> 6) & 0xf);
          strcat(instruction, temp);
          get_prefetch_half(temp, 10, (opcode >> 2) & 0xf);
          strcat(instruction, temp);
          sprintf(temp, ", w%d", ((opcode >> 12) & 0x3) + 4);
          strcat(instruction, temp);
          break;
        case OP_WM_WN_ACC_WX_WY_AWB:
          a = (opcode >> 15) & 0x1;
          d = (opcode >> 16) & 0x7;
          sprintf(instruction, "%s %s, %c", table_dspic[n].name, mmm_table[d], accum[a]);
          parse_dsp(instruction, opcode, 1);
          break;
        default:
          strcpy(instruction, "???");
          break;
      }

      return count;
    }

    n++;
  }

//printf("%06x %06x %06x\n", opcode, opcode & 0xf8407f, 0x78004f);

  strcpy(instruction, "???");
  return 4;
}

void list_output_dspic(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min = 0, cycles_max = 0, count;
  char instruction[128];
  uint32_t opcode;

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    fprintf(asm_context->list, "\n");
    count=disasm_dspic(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%04x: 0x%06x %-40s cycles: ", start / 2, opcode, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    if (count == 8)
    {
      fprintf(asm_context->list, "0x%04x: 0x%02x%02x%02x\n",
        (start / 2) + 2,
        memory_read(asm_context, start + 4),
        memory_read(asm_context, start + 5),
        memory_read(asm_context, start + 6));
    }

    start += count;
  }
}

void disasm_range_dspic(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int num;

  printf("\n");

  printf("%-7s %-10s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ---------- ----------------------------------       ------\n");

  while(start <= end)
  {
    num = READ_RAM(start) |
         (READ_RAM(start + 1) << 8) |
         (READ_RAM(start + 2) << 16) |
         (READ_RAM(start + 3) << 24);

    int count = disasm_dspic(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start / 2, num, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start / 2, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start / 2, num, instruction, cycles_min, cycles_max);
    }

    count -= 4;
    while (count > 0)
    {
      start += 4;

      num = READ_RAM(start)|
           (READ_RAM(start + 1) << 8) |
           (READ_RAM(start + 2) << 16) |
           (READ_RAM(start + 3) << 24);
      printf("0x%04x: 0x%08x\n", start / 2, num);
      count -= 4;
    }

    start += 4;
  }
}

