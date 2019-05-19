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

#include "disasm/xtensa.h"
#include "table/xtensa.h"

//#define READ_RAM(a) memory_read_m(memory, a)
//#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

int get_cycle_count_xtensa(unsigned short int opcode)
{
  return -1;
}

static int disasm_xtensa_le(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode, opcode16;
  int at, as, ar, ft, fs, fr, bt, bs, br, i, x, y;
  int n;

  opcode = memory_read_m(memory, address) |
          (memory_read_m(memory, address + 1) << 8) |
          (memory_read_m(memory, address + 2) << 16);

  opcode16 = memory_read_m(memory, address) |
            (memory_read_m(memory, address + 1) << 8);

  n = 0;

  while(table_xtensa[n].instr != NULL)
  {
    uint32_t mask = mask_xtensa[table_xtensa[n].type].mask_le;
    int bits = mask_xtensa[table_xtensa[n].type].bits;

    if (bits == 24)
    {
      if ((opcode & mask) == table_xtensa[n].opcode_le)
      {
        switch(table_xtensa[n].type)
        {
          case XTENSA_OP_NONE:
            strcpy(instruction, table_xtensa[n].instr);
            return 3;
          case XTENSA_OP_AR_AT:
            at = (opcode >> 4) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, ar, at);
            return 3;
          case XTENSA_OP_AT_AS:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, at, as);
            return 3;
          case XTENSA_OP_FR_FS:
            fs = (opcode >> 8) & 0xf;
            fr = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s f%d, f%d", table_xtensa[n].instr, fr, fs);
            return 3;
          case XTENSA_OP_FR_FS_FT:
            ft = (opcode >> 4) & 0xf;
            fs = (opcode >> 8) & 0xf;
            fr = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s f%d, f%d, f%d",
              table_xtensa[n].instr, fr, fs, ft);
            return 3;
          case XTENSA_OP_AR_AS_AT:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d, a%d",
              table_xtensa[n].instr, ar, as, at);
            return 3;
          case XTENSA_OP_AT_AS_N128_127:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i);
            return 3;
          case XTENSA_OP_AT_AS_N32768_32512:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = ((opcode >> 16) & 0xff) << 8;
            i = (int32_t)((int16_t)i);
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i);
            return 3;
          case XTENSA_OP_BT_BS4:
          case XTENSA_OP_BT_BS8:
            bt = (opcode >> 4) & 0xf;
            bs = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s b%d, b%d", table_xtensa[n].instr, bt, bs);
            return 3;
          case XTENSA_OP_BR_BS_BT:
            bt = (opcode >> 4) & 0xf;
            bs = (opcode >> 8) & 0xf;
            br = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s b%d, b%d, b%d", table_xtensa[n].instr, br, bs, bt);
            return 3;
          case XTENSA_OP_BRANCH_AS_AT_I8:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, at, as, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_B5_I8:
            x = (((opcode >> 12) & 1) << 4) | ((opcode >> 4) & 0xf);
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, %d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, x, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_B5_I8_L:
            x = (((opcode >> 12) & 1) << 4) | ((opcode >> 4) & 0xf);
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, %d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, x, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_C4_I8:
            ar = (opcode >> 12) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, %d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, xtensa_b4const[ar], address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_I12:
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 12) & 0xfff;
            if ((i & 0x800) == 0x800) { i |= 0xfffff000; }
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_BS_I8:
            bs = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s b%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, bs, address + 4 + i, i);
            return 3;
          case XTENSA_OP_NUM_NUM:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s %d, %d", table_xtensa[n].instr, at, as);
            return 3;
          case XTENSA_OP_CALL_I18:
            i = (opcode >> 6) & 0x03ffff;
            if ((i & 0x20000) == 0x20000) { i |= 0xfffc0000; }
            sprintf(instruction, "%s 0x%04x (offset=%d)",
              table_xtensa[n].instr,  address + 1 + i, i);
            return 3;
          case XTENSA_OP_JUMP_I18:
            i = (opcode >> 6) & 0x03ffff;
            if ((i & 0x20000) == 0x20000) { i |= 0xfffc0000; }
            sprintf(instruction, "%s 0x%04x (offset=%d)",
              table_xtensa[n].instr,  address + 4 + i, i);
            return 3;
          case XTENSA_OP_AS:
            as = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d", table_xtensa[n].instr, as);
            return 3;
          case XTENSA_OP_AR_FS_0_15:
            ar = (opcode >> 12) & 0xf;
            fs = (opcode >> 8) & 0xf;
            i = (opcode >> 4) & 0xf;
            sprintf(instruction, "%s a%d, f%d, %d",
              table_xtensa[n].instr, ar, fs, i);
            return 3;
          case XTENSA_OP_AR_AS_7_22:
            ar = (opcode >> 12) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = ((opcode >> 4) & 0xf) + 7;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, as, i);
            return 3;
          case XTENSA_OP_AS_0_1020:
            as = (opcode >> 8) & 0xf;
            i = ((opcode >> 16) & 0xff) << 2;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, as, i);
            return 3;
          case XTENSA_OP_AS_0_240:
            as = (opcode >> 8) & 0xf;
            i = ((opcode >> 20) & 0xf) << 4;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, as, i);
            return 3;
          case XTENSA_OP_AS_0_32760:
            as = (opcode >> 8) & 0xf;
            i = ((opcode >> 12) & 0xfff) << 3;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, as, i);
            return 3;
          case XTENSA_OP_AR_AT_SHIFT_MASK:
            ar = (opcode >> 12) & 0xf;
            at = (opcode >> 4) & 0xf;
            i =  (((opcode >> 16) & 1) << 4) | ((opcode >> 8) & 0xf);
            x = (opcode >> 20) & 0xf;
            sprintf(instruction, "%s a%d, a%d, %d, %d",
              table_xtensa[n].instr, ar, at, i, x + 1);
            return 3;
          case XTENSA_OP_FR_AS_0_15:
            fr = (opcode >> 12) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 4) & 0xf;
            sprintf(instruction, "%s f%d, a%d, %d",
              table_xtensa[n].instr, fr, as, i);
            return 3;
          case XTENSA_OP_AT_AS_0_255:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i);
            return 3;
          case XTENSA_OP_AT_AS_0_510:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i << 1);
            return 3;
          case XTENSA_OP_AT_AS_0_1020:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i << 2);
            return 3;
          case XTENSA_OP_AT_AS_N64_N4:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (int32_t)((int8_t)(0xf0 | ((opcode >> 12) & 0xf)));
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i * 4);
            return 3;
          case XTENSA_OP_AT_I16:
            at = (opcode >> 4) & 0xf;
            i = ((opcode >> 8) & 0xffff) | 0xffff0000;
            i = i * 4;
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, at, address + 3 + i, i);
            return 3;
          case XTENSA_OP_MW_AS:
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 12) & 0x3;
            sprintf(instruction, "%s m%d, a%d", table_xtensa[n].instr, i, as);
            return 3;
          case XTENSA_OP_LOOP_AS_LABEL:
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, address + 4 + i, i);
            return 3;
          case XTENSA_OP_FT_AS_0_1020:
            ft = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            sprintf(instruction, "%s f%d, a%d, %d",
              table_xtensa[n].instr, ft, as, i << 2);
            return 3;
          case XTENSA_OP_FR_AS_AT:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            fr = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s f%d, a%d, a%d",
              table_xtensa[n].instr, fr, as, at);
            return 3;
          case XTENSA_OP_AR_AS_AS:
            //at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, ar, as);
            return 3;
          case XTENSA_OP_FR_FS_AT:
            at = (opcode >> 4) & 0xf;
            fs = (opcode >> 8) & 0xf;
            fr = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s f%d, f%d, a%d",
              table_xtensa[n].instr, fr, fs, at);
            return 3;
          case XTENSA_OP_AR_AS_BT:
            bt = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d, b%d",
              table_xtensa[n].instr, ar, as, bt);
            return 3;
          case XTENSA_OP_FR_FS_BT:
            bt = (opcode >> 4) & 0xf;
            fs = (opcode >> 8) & 0xf;
            fr = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s f%d, f%d, b%d",
              table_xtensa[n].instr, fr, fs, bt);
            return 3;
          case XTENSA_OP_AT_N2048_2047:
            at = (opcode >> 4) & 0xf;
            i = (((opcode >> 8) & 0xf) << 8) | ((opcode >> 16) & 0xff);
            if ((i & 0x800) != 0) { i |= 0xfffff000; }
            sprintf(instruction, "%s a%d, %d",
              table_xtensa[n].instr, at, i);
            return 3;
          case XTENSA_OP_AS_AT:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, as, at);
            return 3;
          case XTENSA_OP_AS_MY:
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 6) & 0x1;
            sprintf(instruction, "%s a%d, m%d",
              table_xtensa[n].instr, as, i + 2);
            return 3;
          case XTENSA_OP_MX_AT:
            at = (opcode >> 4) & 0xf;
            i = (opcode >> 14) & 0x1;
            sprintf(instruction, "%s m%d, a%d",
              table_xtensa[n].instr, i, at);
            return 3;
          case XTENSA_OP_MX_MY:
            i = (opcode >> 14) & 0x1;
            x = (opcode >> 6) & 0x1;
            sprintf(instruction, "%s m%d, m%d",
              table_xtensa[n].instr, i, x + 2);
            return 3;
          case XTENSA_OP_MW_AS_MX_AT:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 12) & 0x3;
            x = (opcode >> 14) & 0x1;
            sprintf(instruction, "%s m%d, a%d, m%d, a%d",
              table_xtensa[n].instr, i, as, x, at);
            return 3;
          case XTENSA_OP_MW_AS_MX_MY:
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 12) & 0x3;
            x = (opcode >> 14) & 0x1;
            y = (opcode >> 6) & 0x1;
            sprintf(instruction, "%s m%d, a%d, m%d, m%d",
              table_xtensa[n].instr, i, as, x, y + 2);
            return 3;
          case XTENSA_OP_BR_FS_FT:
            ft = (opcode >> 4) & 0xf;
            fs = (opcode >> 8) & 0xf;
            br = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s b%d, f%d, f%d",
              table_xtensa[n].instr, br, fs, ft);
            return 3;
          case XTENSA_OP_0_15:
            i = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s %d", table_xtensa[n].instr, i);
            return 3;
          case XTENSA_OP_AR_FS:
            fs = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, f%d", table_xtensa[n].instr, ar, fs);
            return 3;
          case XTENSA_OP_N8_7:
            i = (opcode >> 4) & 0xf;
            if ((i & 0x8) != 0) { i |= 0xfffffff0; }
            sprintf(instruction, "%s %d", table_xtensa[n].instr, i);
            return 3;
          case XTENSA_OP_AT_0_15:
            at = (opcode >> 4) & 0xf;
            i = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, at, i);
            return 3;
          case XTENSA_OP_AT_SPR:
            at = (opcode >> 4) & 0xf;
            i = (opcode >> 8) & 0xff;
            if (i == 3)
            {
              sprintf(instruction, "%s a%d, sar", table_xtensa[n].instr, at);
            }
              else
            {
              sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, at, i);
            }
            return 3;
          case XTENSA_OP_AR_UR:
            ar = (opcode >> 12) & 0xf;
            i = (opcode >> 4) & 0xff;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, ar, i);
            return 3;
          case XTENSA_OP_AR_AS:
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, ar, as);
            return 3;
          case XTENSA_OP_AR_AS_1_31:
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            i = (((opcode >> 20) & 1) << 4) | ((opcode >> 4) & 0xf);
            i = 32 - i;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, as, i);
            return 3;
          case XTENSA_OP_AR_AT_1_31:
            at = (opcode >> 4) & 0xf;
            ar = (opcode >> 12) & 0xf;
            i = (((opcode >> 20) & 1) << 4) | ((opcode >> 8) & 0xf);
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, at, i);
            return 3;
          case XTENSA_OP_AR_AT_0_15:
            at = (opcode >> 4) & 0xf;
            ar = (opcode >> 12) & 0xf;
            i = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, at, i);
            return 3;
          case XTENSA_OP_0_31:
            i = (((opcode >> 4) & 1) << 4) | ((opcode >> 8) & 0xf);
            sprintf(instruction, "%s %d", table_xtensa[n].instr, i);
            return 3;
          case XTENSA_OP_FR_AS:
            as = (opcode >> 8) & 0xf;
            fr = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, fr, as);
            return 3;
          default:
            strcpy(instruction, "<error>");
            return -1;
        }
      }
    }
      else
    {
      if ((opcode16 & mask) == table_xtensa[n].opcode_le)
      {
        switch(table_xtensa[n].type)
        {
          case XTENSA_OP_N_NONE:
            strcpy(instruction, table_xtensa[n].instr);
            return 2;
          case XTENSA_OP_N_AR_AS_AT:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d, a%d",
              table_xtensa[n].instr, ar, as, at);
            return 2;
          case XTENSA_OP_N_AR_AS_N1_15:
            i = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            ar = (opcode >> 12) & 0xf;

            if (i == 0) { i = -1; }

            sprintf(instruction, "%s a%d, a%d, %d", table_xtensa[n].instr, ar, as, i);
            return 2;
          case XTENSA_OP_BRANCH_N_AS_I6:
            as = (opcode >> 8) & 0xf;
            i = (((opcode >> 4) & 0x3) << 4) | ((opcode >> 12) & 0xf);
            //if ((i & 0x20) == 0x20) { i |= 0xffffffc0; }
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, address + 4 + i, i);
            return 2;
          case XTENSA_OP_N_NUM:
            as = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s %d", table_xtensa[n].instr, as);
            return 2;
          case XTENSA_OP_N_AT_AS_0_60:
            as = (opcode >> 8) & 0xf;
            at = (opcode >> 4) & 0xf;
            i = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i << 2);
            return 2;
          case XTENSA_OP_N_AT_AS:
            at = (opcode >> 4) & 0xf;
            as = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, at, as);
            return 2;
          case XTENSA_OP_N_AS_N2048_2047:
            as = (opcode >> 8) & 0xf;
            i = (((opcode >> 4) & 0xf) << 4) | ((opcode >> 12) & 0xf);
            if ((i & 0x40) != 0) { i |= 0xffffff80; }
            sprintf(instruction, "%s a%d, %d",
              table_xtensa[n].instr, as, i);
            return 2;
          default:
            strcpy(instruction, "<error>");
            return -1;
        }
      }
    }

    n++;
  }

  sprintf(instruction, "???");

  return 1;
}

static int disasm_xtensa_be(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode, opcode16;
  int at, as, ar, ft, fs, fr, bt, bs, br, i, x, y;
  int n;

  opcode = memory_read_m(memory, address + 2) |
          (memory_read_m(memory, address + 1) << 8) |
          (memory_read_m(memory, address) << 16);

  opcode16 = memory_read_m(memory, address + 1) |
            (memory_read_m(memory, address) << 8);

  n = 0;

  while(table_xtensa[n].instr != NULL)
  {
    uint32_t mask = mask_xtensa[table_xtensa[n].type].mask_be;
    int bits = mask_xtensa[table_xtensa[n].type].bits;

    if (bits == 24)
    {
      if ((opcode & mask) == table_xtensa[n].opcode_be)
      {
        switch(table_xtensa[n].type)
        {
          case XTENSA_OP_NONE:
            strcpy(instruction, table_xtensa[n].instr);
            return 3;
          case XTENSA_OP_AR_AT:
            at = (opcode >> 16) & 0xf;
            ar = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, ar, at);
            return 3;
          case XTENSA_OP_AT_AS:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, at, as);
            return 3;
          case XTENSA_OP_FR_FS:
            fs = (opcode >> 12) & 0xf;
            fr = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, f%d", table_xtensa[n].instr, fr, fs);
            return 3;
          case XTENSA_OP_FR_FS_FT:
            ft = (opcode >> 16) & 0xf;
            fs = (opcode >> 12) & 0xf;
            fr = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, f%d, f%d",
              table_xtensa[n].instr, fr, fs, ft);
            return 3;
          case XTENSA_OP_AR_AS_AT:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            ar = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d, a%d",
              table_xtensa[n].instr, ar, as, at);
            return 3;
          case XTENSA_OP_AT_AS_N128_127:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i);
            return 3;
          case XTENSA_OP_AT_AS_N32768_32512:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = (opcode & 0xff) << 8;
            i = (int32_t)((int16_t)i);
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i);
            return 3;
          case XTENSA_OP_BT_BS4:
          case XTENSA_OP_BT_BS8:
            bt = (opcode >> 16) & 0xf;
            bs = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s b%d, b%d", table_xtensa[n].instr, bt, bs);
            return 3;
          case XTENSA_OP_BR_BS_BT:
            bt = (opcode >> 16) & 0xf;
            bs = (opcode >> 12) & 0xf;
            br = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s b%d, b%d, b%d", table_xtensa[n].instr, br, bs, bt);
            return 3;
          case XTENSA_OP_BRANCH_AS_AT_I8:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, at, as, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_B5_I8:
            x = (((opcode >> 8) & 1) << 4) | ((opcode >> 16) & 0xf);
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, %d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, x, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_B5_I8_L:
            x = (((opcode >> 12) & 1) << 4) | ((opcode >> 4) & 0xf);
            as = (opcode >> 8) & 0xf;
            i = (opcode >> 16) & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, %d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, x, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_C4_I8:
            ar = (opcode >> 8) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s a%d, %d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, xtensa_b4const[ar], address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_AS_I12:
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xfff;
            if ((i & 0x800) == 0x800) { i |= 0xfffff000; }
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, address + 4 + i, i);
            return 3;
          case XTENSA_OP_BRANCH_BS_I8:
            bs = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            i = (int32_t)((int8_t)i);
            sprintf(instruction, "%s b%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, bs, address + 4 + i, i);
            return 3;
          case XTENSA_OP_NUM_NUM:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s %d, %d", table_xtensa[n].instr, at, as);
            return 3;
          case XTENSA_OP_CALL_I18:
            i = opcode & 0x03ffff;
            if ((i & 0x20000) == 0x20000) { i |= 0xfffc0000; }
            sprintf(instruction, "%s 0x%04x (offset=%d)",
              table_xtensa[n].instr,  address + 1 + i, i);
            return 3;
          case XTENSA_OP_JUMP_I18:
            i = opcode & 0x03ffff;
            if ((i & 0x20000) == 0x20000) { i |= 0xfffc0000; }
            sprintf(instruction, "%s 0x%04x (offset=%d)",
              table_xtensa[n].instr,  address + 4 + i, i);
            return 3;
          case XTENSA_OP_AS:
            as = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d", table_xtensa[n].instr, as);
            return 3;
          case XTENSA_OP_AR_FS_0_15:
            ar = (opcode >> 8) & 0xf;
            fs = (opcode >> 12) & 0xf;
            i = (opcode >> 16) & 0xf;
            sprintf(instruction, "%s a%d, f%d, %d",
              table_xtensa[n].instr, ar, fs, i);
            return 3;
          case XTENSA_OP_AR_AS_7_22:
            ar = (opcode >> 8) & 0xf;
            fs = (opcode >> 12) & 0xf;
            i = ((opcode >> 16) & 0xf) + 7;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, fs, i);
            return 3;
          case XTENSA_OP_AS_0_1020:
            as = (opcode >> 12) & 0xf;
            i = (opcode & 0xff) << 2;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, as, i);
            return 3;
          case XTENSA_OP_AS_0_240:
            as = (opcode >> 12) & 0xf;
            i = (opcode & 0xf) << 4;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, as, i);
            return 3;
          case XTENSA_OP_AS_0_32760:
            as = (opcode >> 12) & 0xf;
            i = (opcode & 0xfff) << 3;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, as, i);
            return 3;
          case XTENSA_OP_AR_AT_SHIFT_MASK:
            ar = (opcode >> 8) & 0xf;
            at = (opcode >> 16) & 0xf;
            i =  (((opcode >> 4) & 1) << 4) | ((opcode >> 12) & 0xf);
            x = opcode & 0xf;
            sprintf(instruction, "%s a%d, a%d, %d, %d",
              table_xtensa[n].instr, ar, at, i, x + 1);
            return 3;
          case XTENSA_OP_FR_AS_0_15:
            fr = (opcode >> 8) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = (opcode >> 16) & 0xf;
            sprintf(instruction, "%s f%d, a%d, %d",
              table_xtensa[n].instr, fr, as, i);
            return 3;
          case XTENSA_OP_AT_AS_0_255:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i);
            return 3;
          case XTENSA_OP_AT_AS_0_510:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i << 1);
            return 3;
          case XTENSA_OP_AT_AS_0_1020:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i << 2);
            return 3;
          case XTENSA_OP_AT_AS_N64_N4:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = (int32_t)((int8_t)(0xf0 | ((opcode >> 8) & 0xf)));
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i * 4);
            return 3;
          case XTENSA_OP_AT_I16:
            at = (opcode >> 16) & 0xf;
            i = (opcode & 0xffff) | 0xffff0000;
            i = i * 4;
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, at, address + 3 + i, i);
            return 3;
          case XTENSA_OP_MW_AS:
            as = (opcode >> 12) & 0xf;
            i = (opcode >> 8) & 0x3;
            sprintf(instruction, "%s m%d, a%d", table_xtensa[n].instr, i, as);
            return 3;
          case XTENSA_OP_LOOP_AS_LABEL:
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, address + 4 + i, i);
            return 3;
          case XTENSA_OP_FT_AS_0_1020:
            ft = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = opcode & 0xff;
            sprintf(instruction, "%s f%d, a%d, %d",
              table_xtensa[n].instr, ft, as, i << 2);
            return 3;
          case XTENSA_OP_FR_AS_AT:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            fr = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, a%d, %d",
              table_xtensa[n].instr, fr, as, at);
            return 3;
          case XTENSA_OP_AR_AS_AS:
            //at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            ar = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, ar, as);
            return 3;
          case XTENSA_OP_FR_FS_AT:
            at = (opcode >> 16) & 0xf;
            fs = (opcode >> 12) & 0xf;
            fr = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, f%d, a%d",
              table_xtensa[n].instr, fr, fs, at);
            return 3;
          case XTENSA_OP_AR_AS_BT:
            bt = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            ar = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, a%d, b%d",
              table_xtensa[n].instr, ar, as, bt);
            return 3;
          case XTENSA_OP_FR_FS_BT:
            bt = (opcode >> 16) & 0xf;
            fs = (opcode >> 12) & 0xf;
            fr = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, f%d, b%d",
              table_xtensa[n].instr, fr, fs, bt);
            return 3;
          case XTENSA_OP_AT_N2048_2047:
            at = (opcode >> 8) & 0xf;
            i = (((opcode >> 12) & 0xf) << 8) | (opcode & 0xff);
            if ((i & 0x800) != 0) { i |= 0xfffff000; }
            sprintf(instruction, "%s a%d, b%d",
              table_xtensa[n].instr, at, i);
            return 3;
          case XTENSA_OP_AS_AT:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, as, at);
            return 3;
          case XTENSA_OP_AS_MY:
            as = (opcode >> 12) & 0xf;
            i = (opcode >> 18) & 0x1;
            sprintf(instruction, "%s a%d, m%d",
              table_xtensa[n].instr, as, i + 2);
            return 3;
          case XTENSA_OP_MX_AT:
            at = (opcode >> 16) & 0xf;
            i = (opcode >> 10) & 0x1;
            sprintf(instruction, "%s m%d, a%d",
              table_xtensa[n].instr, i, at);
            return 3;
          case XTENSA_OP_MX_MY:
            i = (opcode >> 10) & 0x1;
            x = (opcode >> 18) & 0x1;
            sprintf(instruction, "%s m%d, m%d",
              table_xtensa[n].instr, i, x + 2);
            return 3;
          case XTENSA_OP_MW_AS_MX_AT:
            at = (opcode >> 16) & 0xf;
            as = (opcode >> 12) & 0xf;
            i = (opcode >> 8) & 0x3;
            x = (opcode >> 10) & 0x1;
            sprintf(instruction, "%s m%d, a%d, m%d, a%d",
              table_xtensa[n].instr, i, as, x, at);
            return 3;
          case XTENSA_OP_MW_AS_MX_MY:
            as = (opcode >> 12) & 0xf;
            i = (opcode >> 8) & 0x3;
            x = (opcode >> 10) & 0x1;
            y = (opcode >> 18) & 0x1;
            sprintf(instruction, "%s m%d, a%d, m%d, m%d",
              table_xtensa[n].instr, i, as, x, y + 2);
            return 3;
          case XTENSA_OP_BR_FS_FT:
            ft = (opcode >> 16) & 0xf;
            fs = (opcode >> 12) & 0xf;
            br = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s b%d, f%d, f%d",
              table_xtensa[n].instr, br, fs, ft);
            return 3;
          case XTENSA_OP_0_15:
            i = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s %d", table_xtensa[n].instr, i);
            return 3;
          case XTENSA_OP_AR_FS:
            fs = (opcode >> 12) & 0xf;
            ar = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s a%d, f%d", table_xtensa[n].instr, ar, fs);
            return 3;
          case XTENSA_OP_N8_7:
            i = (opcode >> 16) & 0xf;
            if ((i & 0x8) != 0) { i |= 0xfffffff0; }
            sprintf(instruction, "%s %d", table_xtensa[n].instr, i);
            return 3;
          case XTENSA_OP_AT_0_15:
            at = (opcode >> 16) & 0xf;
            i = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, at, i);
            return 3;
          case XTENSA_OP_AT_SPR:
            at = (opcode >> 16) & 0xf;
            i = (opcode >> 8) & 0xff;
            if (i == 3)
            {
              sprintf(instruction, "%s a%d, sar", table_xtensa[n].instr, at);
            }
              else
            {
              sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, at, i);
            }
            return 3;
          case XTENSA_OP_AR_UR:
            ar = (opcode >> 8) & 0xf;
            i = (opcode >> 12) & 0xff;
            sprintf(instruction, "%s a%d, %d", table_xtensa[n].instr, ar, i);
            return 3;
          case XTENSA_OP_AR_AS:
            as = (opcode >> 12) & 0xf;
            ar = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, f%d", table_xtensa[n].instr, ar, as);
            return 3;
          case XTENSA_OP_AR_AS_1_31:
            as = (opcode >> 12) & 0xf;
            ar = (opcode >> 8) & 0xf;
            i = ((opcode & 1) << 4) | ((opcode >> 16) & 0xf);
            i = 32 - i;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, as, i);
            return 3;
          case XTENSA_OP_AR_AT_1_31:
            at = (opcode >> 16) & 0xf;
            ar = (opcode >> 8) & 0xf;
            i = ((opcode & 1) << 4) | ((opcode >> 12) & 0xf);
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, at, i);
            return 3;
          case XTENSA_OP_AR_AT_0_15:
            at = (opcode >> 16) & 0xf;
            ar = (opcode >> 8) & 0xf;
            i = (opcode >> 12) & 0xf;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, ar, at, i);
            return 3;
          case XTENSA_OP_0_31:
            i = (((opcode >> 16) & 1) << 4) | ((opcode >> 12) & 0xf);
            sprintf(instruction, "%s %d", table_xtensa[n].instr, i);
            return 3;
          case XTENSA_OP_FR_AS:
            as = (opcode >> 12) & 0xf;
            fr = (opcode >> 8) & 0xf;
            sprintf(instruction, "%s f%d, f%d", table_xtensa[n].instr, fr, as);
            return 3;
          default:
            strcpy(instruction, "<error>");
            return -1;
        }
      }
    }
      else
    {
      if ((opcode16 & mask) == table_xtensa[n].opcode_be)
      {
        switch(table_xtensa[n].type)
        {
          case XTENSA_OP_N_NONE:
            strcpy(instruction, table_xtensa[n].instr);
            return 2;
          case XTENSA_OP_N_AR_AS_AT:
            at = (opcode >> 8) & 0xf;
            as = (opcode >> 4) & 0xf;
            ar = opcode & 0xf;
            sprintf(instruction, "%s a%d, a%d, a%d",
              table_xtensa[n].instr, ar, as, at);
            return 2;
          case XTENSA_OP_N_AR_AS_N1_15:
            i = (opcode >> 8) & 0xf;
            as = (opcode >> 4) & 0xf;
            ar = opcode & 0xf;

            if (i == 0) { i = -1; }

            sprintf(instruction, "%s a%d, a%d, %d", table_xtensa[n].instr, ar, as, i);
            return 2;
          case XTENSA_OP_BRANCH_N_AS_I6:
            as = (opcode >> 4) & 0xf;
            i = (((opcode >> 8) & 0x3) << 4) | (opcode & 0xf);
            //if ((i & 0x20) == 0x20) { i |= 0xffffffc0; }
            sprintf(instruction, "%s a%d, 0x%04x (offset=%d)",
              table_xtensa[n].instr, as, address + 4 + i, i);
            return 2;
          case XTENSA_OP_N_NUM:
            as = (opcode >> 4) & 0xf;
            sprintf(instruction, "%s %d", table_xtensa[n].instr, as);
            return 2;
          case XTENSA_OP_N_AT_AS_0_60:
            as = (opcode >> 4) & 0xf;
            at = (opcode >> 8) & 0xf;
            i = opcode & 0xf;
            sprintf(instruction, "%s a%d, a%d, %d",
              table_xtensa[n].instr, at, as, i << 2);
            return 2;
          case XTENSA_OP_N_AT_AS:
            at = (opcode >> 8) & 0xf;
            as = (opcode >> 4) & 0xf;
            sprintf(instruction, "%s a%d, a%d", table_xtensa[n].instr, at, as);
            return 2;
          case XTENSA_OP_N_AS_N2048_2047:
            as = (opcode >> 4) & 0xf;
            i = (((opcode >> 8) & 0xf) << 4) | (opcode & 0xf);
            if ((i & 0x40) != 0) { i |= 0xffffff80; }
            sprintf(instruction, "%s a%d, %d",
              table_xtensa[n].instr, as, i);
            return 2;
          default:
            strcpy(instruction, "<error>");
            return -1;
        }
      }
    }

    n++;
  }

  sprintf(instruction, "???");

  return 1;
}

int disasm_xtensa(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  *cycles_min = -1;
  *cycles_max = -1;

  if (memory->endian == ENDIAN_LITTLE)
  {
    return disasm_xtensa_le(memory, address, instruction, cycles_min, cycles_max);
  }
    else
  {
    return disasm_xtensa_be(memory, address, instruction, cycles_min, cycles_max);
  }
}

static void get_bytes(struct _memory *memory, int address, int count, char *bytes)
{
  if (count == 2)
  {
    if (memory->endian == ENDIAN_LITTLE)
    {
      sprintf(bytes, "  %02x%02x",
        memory_read_m(memory, address + 1),
        memory_read_m(memory, address + 0));
    }
      else
    {
      sprintf(bytes, "  %02x%02x",
        memory_read_m(memory, address + 0),
        memory_read_m(memory, address + 1));
    }
  }
    else
  {
    if (memory->endian == ENDIAN_LITTLE)
    {
      sprintf(bytes, "%02x%02x%02x",
        memory_read_m(memory, address + 2),
        memory_read_m(memory, address + 1),
        memory_read_m(memory, address + 0));
    }
      else
    {
      sprintf(bytes, "%02x%02x%02x",
        memory_read_m(memory, address + 0),
        memory_read_m(memory, address + 1),
        memory_read_m(memory, address + 2));
    }
  }
}

void list_output_xtensa(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char bytes[10];
  int count;

  struct _memory *memory = &asm_context->memory;

  count = disasm_xtensa(memory, start, instruction, &cycles_min, &cycles_max);

  get_bytes(memory, start, count, bytes);

  fprintf(asm_context->list, "0x%04x: %s  %-40s", start, bytes, instruction);
  fprintf(asm_context->list, "\n");
}

void disasm_range_xtensa(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min = 0, cycles_max = 0;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_xtensa(memory, start, instruction, &cycles_min, &cycles_max);

    get_bytes(memory, start, count, bytes);

    printf("0x%04x: %s  %-40s\n", start, bytes, instruction);

    start += count;
  }
}

