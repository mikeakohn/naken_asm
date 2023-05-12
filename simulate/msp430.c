/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "asm/msp430.h"
#include "disasm/msp430.h"
#include "simulate/msp430.h"

/*

        8   7     6     5       4   3 2 1 0
Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C

*/

#define SHOW_STACK sp, memory_read_m(simulate->memory, sp+1), memory_read_m(simulate->memory, sp)
#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define WRITE_RAM(a,b) \
  if (a == simulate->break_io) \
  { \
    exit(b); \
  } \
  memory_write_m(simulate->memory, a, b);

#define GET_V() ((simulate_msp430->reg[2] >>  8) & 1)
#define GET_SCG1() ((simulate_msp430->reg[2] >> 7) & 1)
#define GET_SCG0() ((simulate_msp430->reg[2] >> 6) & 1)
#define GET_OSCOFF() ((simulate_msp430->reg[2] >> 5) & 1)
#define GET_CPUOFF() ((simulate_msp430->reg[2] >> 4) & 1)
#define GET_GIE() ((simulate_msp430->reg[2] >> 3) & 1)
#define GET_N() ((simulate_msp430->reg[2] >> 2) & 1)
#define GET_Z() ((simulate_msp430->reg[2] >> 1) & 1)
#define GET_C() ((simulate_msp430->reg[2]) & 1)

#define SET_V() simulate_msp430->reg[2] |= 256;
#define SET_SCG1() simulate_msp430->reg[2] |= 128;
#define SET_SCG0() simulate_msp430->reg[2] |= 64;
#define SET_OSCOFF() simulate_msp430->reg[2] |= 32;
#define SET_CPUOFF() simulate_msp430->reg[2] |= 16;
#define SET_GIE() simulate_msp430->reg[2] |= 8;
#define SET_N() simulate_msp430->reg[2] |= 4;
#define SET_Z() simulate_msp430->reg[2] |= 2;
#define SET_C() simulate_msp430->reg[2] |= 1;

#define CLEAR_V() simulate_msp430->reg[2] &= (0xffff^256);
#define CLEAR_SCG1() simulate_msp430->reg[2] &= (0xffff^128);
#define CLEAR_SCG0() simulate_msp430->reg[2] &= (0xffff^64);
#define CLEAR_OSCOFF() simulate_msp430->reg[2] &= (0xffff^32);
#define CLEAR_CPUOFF() simulate_msp430->reg[2] &= (0xffff^16);
#define CLEAR_GIE() simulate_msp430->reg[2] &= (0xffff^8);
#define CLEAR_N() simulate_msp430->reg[2] &= (0xffff^4);
#define CLEAR_Z() simulate_msp430->reg[2] &= (0xffff^2);
#define CLEAR_C() simulate_msp430->reg[2] &= (0xffff^1);

#define AFFECTS_NZ(a) \
  if (bw == 0) \
  { \
    if (a & 0x8000) { SET_N(); } else { CLEAR_N(); } \
  } \
    else \
  { \
    if (a & 0x80) { SET_N(); } else { CLEAR_N(); } \
  } \
  if (a == 0) { SET_Z(); } else { CLEAR_Z(); }

#define CHECK_CARRY(a) \
  if (bw == 0) \
  { \
    if ((a & 0xffff0000) == 0) { CLEAR_C(); } else { SET_C(); } \
  } \
    else \
  { \
    if ((a & 0xffffff00) == 0) { CLEAR_C(); } else { SET_C(); } \
  }

#define CHECK_OVERFLOW() \
  if (bw == 0) \
  { \
    if (((((uint16_t)dst) & 0x8000) == (((uint16_t)src) & 0x8000)) && (((((uint16_t)result) & 0x8000)) != (((uint16_t)dst) & 0x8000))) { SET_V(); } else { CLEAR_V(); } \
  } \
    else \
  { \
    if (((((uint8_t)dst) & 0x80) == (((uint8_t)src) & 0x80)) && (((((uint8_t)result) & 0x80))!=(((uint8_t)dst) & 0x80))) { SET_V(); } else { CLEAR_V(); } \
  }

#define CHECK_OVERFLOW_WITH_C() \
  if (bw==0) \
  { \
    if ((((int32_t)dst + (int32_t)src + GET_C()) & 0xffff0000) != 0) { SET_V(); } else { CLEAR_V(); } \
  } \
    else \
  { \
    if ((((int32_t)dst + (int32_t)src + GET_C()) & 0xffffff00) != 0) { SET_V(); } else { CLEAR_V(); } \
  }

#define CHECK_Z() \
  if (bw == 0) \
  { \
    if (result & 0x8000) { SET_N(); } else { CLEAR_N(); } \
  } \
    else \
  { \
    if (result & 0x80) { SET_N(); } else { CLEAR_N(); } \
  }

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static void sp_inc(int *sp)
{
  (*sp) += 2;
  if (*sp > 0xffff) *sp = 0;
}

static uint16_t get_data(struct _simulate *simulate, int reg, int As, int bw)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;

  if (reg == 3) // CG
  {
    if (As == 0)
    {
      return 0;
    }
      else
    if (As == 1)
    {
      return 1;
    }
      else
    if (As == 2)
    {
      return 2;
    }
      else
    if (As == 3)
    {
      return (bw == 0) ? 0xffff : 0xff;
    }
  }

  if (As == 0) // Rn
  {
    return (bw == 0) ?
      simulate_msp430->reg[reg] : simulate_msp430->reg[reg] & 0xff;
  }

  if (reg == 2)
  {
    if (As == 1) // &LABEL
    {
      int PC = simulate_msp430->reg[0];
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC+1) << 8);

      simulate_msp430->reg[0] += 2;

      if (bw == 0)
      {
        return READ_RAM(a) | (READ_RAM(a+1) << 8);
      }
        else
      {
        return READ_RAM(a);
      }
    }
      else
    if (As == 2)
    {
      return 4;
    }
      else
    if (As == 3)
    {
      return 8;
    }
  }

  if (reg == 0) // PC
  {
    // This is probably worthless.. some other condition should pick this up
    if (As == 3) // #immediate
    {
      uint16_t a = READ_RAM(simulate_msp430->reg[0]) | (READ_RAM(simulate_msp430->reg[0] + 1) << 8);

      simulate_msp430->reg[0] += 2;

      return (bw == 0) ? a : a & 0xff;
    }
  }

  if (As == 1) // x(Rn)
  {
    uint16_t a = READ_RAM(simulate_msp430->reg[0]) | (READ_RAM(simulate_msp430->reg[0] + 1) << 8);
    uint16_t index = simulate_msp430->reg[reg] + ((int16_t)a);

    simulate_msp430->reg[0] += 2;

    if (bw == 0)
    {
      return READ_RAM(index) | (READ_RAM(index+1) << 8);
    }
      else
    {
      return READ_RAM(index);
    }
  }
    else
  if (As == 2 || As == 3) // @Rn (mode 2) or @Rn+ (mode 3)
  {
    uint16_t index = simulate_msp430->reg[reg];

    if (bw == 0)
    {
      return READ_RAM(index) | (READ_RAM(index + 1) << 8);
    }
      else
    {
      return READ_RAM(simulate_msp430->reg[reg]);
    }
  }

  printf("Error: Unrecognized source addressing mode %d\n", As);

  return 0;
}

static void update_reg(struct _simulate *simulate, int reg, int mode, int bw)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;

  if (reg == 0) { return; }
  if (reg == 2) { return; }
  if (reg == 3) { return; }

  if (mode == 3) // @Rn+
  {
    if (bw == 0)
    {
      simulate_msp430->reg[reg] += 2;
    }
      else
    {
      simulate_msp430->reg[reg] += 1;
    }
  }
}

static int put_data(struct _simulate *simulate, int PC, int reg, int mode, int bw, uint32_t data)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;

  if (mode == 0) // Rn
  {
    if (bw == 0)
    { simulate_msp430->reg[reg] = data; }
      else
    {
      //simulate_msp430->reg[reg]&=0xff00;
      //simulate_msp430->reg[reg]|=data&0xff;
      simulate_msp430->reg[reg] = data & 0xff;
    }
    return 0;
  }

  if (reg == 2)
  {
    if (mode == 1) // &LABEL
    {
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC+1) << 8);

      if (bw == 0)
      {
        WRITE_RAM(a, data & 0xff);
        WRITE_RAM(a + 1, data >> 8);
      }
        else
      { WRITE_RAM(a, data & 0xff); }

      return 0;
    }
  }
  if (reg == 0) // PC
  {
    if (mode == 1) // LABEL
    {
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);

      if (bw==0)
      {
        WRITE_RAM(PC + a, data & 0xff);
        WRITE_RAM(PC + a + 1, data >> 8);
      }
        else
      {
        WRITE_RAM(PC + a, data & 0xff);
      }

      return 0;
    }
  }

  if (mode == 1) // x(Rn)
  {
    uint16_t a = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);
    int address = simulate_msp430->reg[reg] + ((int16_t)a);

    if (bw == 0)
    {
      WRITE_RAM(address, data & 0xff);
      WRITE_RAM(address + 1, data >> 8);
    }
      else
    {
      WRITE_RAM(address, data & 0xff);
    }

    return 0;
  }
    else
  if (mode == 2 || mode == 3) // @Rn (mode 2) or @Rn+ (mode 3)
  {
    uint16_t index = simulate_msp430->reg[reg];

    if (bw == 0)
    {
      WRITE_RAM(index, data & 0xff);
      WRITE_RAM(index + 1, data >> 8);
    }
      else
    {
      WRITE_RAM(index, data & 0xff);
    }

    return 0;
  }

  printf("Error: Unrecognized addressing mode for destination %d\n", mode);

  return -1;
}

static int one_operand_exe(struct _simulate *simulate, uint16_t opcode)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;
  int o;
  int reg;
  int As,bw;
  int count = 1;
  uint32_t result;
  int src;
  int pc;

  o = (opcode & 0x0380) >> 7;

  if (o == 7) { return 1; }
  if (o == 6) { return count; }

  As = (opcode & 0x0030) >> 4;
  bw = (opcode & 0x0040) >> 6;
  reg = opcode & 0x000f;

  switch(o)
  {
    case 0:  // RRC
      pc = simulate_msp430->reg[0];
      src = get_data(simulate, reg, As, bw);
      int c = GET_C();
      if ((src & 1) == 1) { SET_C(); } else { CLEAR_C(); }
      if (bw == 0)
      { result = (c << 15) | (((uint16_t)src) >> 1); }
        else
      { result = (c << 7) | (((uint8_t)src) >> 1); }
      put_data(simulate, pc, reg, As, bw, result);
      update_reg(simulate, reg, As, bw);
      AFFECTS_NZ(result);
      CLEAR_V();
      break;
    case 1:  // SWPB (no bw)
      pc = simulate_msp430->reg[0];
      src = get_data(simulate, reg, As, bw);
      result = ((src & 0xff00) >> 8) | ((src & 0xff) << 8);
      put_data(simulate, pc, reg, As, bw, result);
      update_reg(simulate, reg, As, bw);
      break;
    case 2:  // RRA
      pc = simulate_msp430->reg[0];
      src = get_data(simulate, reg, As, bw);
      if ((src & 1) == 1) { SET_C(); } else { CLEAR_C(); }
      if (bw == 0)
      { result = ((int16_t)src) >> 1; }
        else
      { result = ((int8_t)src) >> 1; }
      put_data(simulate, pc, reg, As, bw, result);
      update_reg(simulate, reg, As, bw);
      AFFECTS_NZ(result);
      CLEAR_V();
      break;
    case 3:  // SXT (no bw)
      pc = simulate_msp430->reg[0];
      src = get_data(simulate, reg, As, bw);
      result = (int16_t)((int8_t)((uint8_t)src));
      put_data(simulate, pc, reg, As, bw, result);
      update_reg(simulate, reg, As, bw);
      AFFECTS_NZ(result);
      CHECK_CARRY(result);
      CLEAR_V();
      break;
    case 4:  // PUSH
      simulate_msp430->reg[1] -= 2;
      src = get_data(simulate, reg, As, bw);
      update_reg(simulate, reg, As, bw);
      WRITE_RAM(simulate_msp430->reg[1], src & 0xff);
      WRITE_RAM(simulate_msp430->reg[1] + 1, src >> 8);
      break;
    case 5:  // CALL (no bw)
      src = get_data(simulate, reg, As, bw);
      update_reg(simulate, reg, As, bw);
      simulate_msp430->reg[1] -= 2;
      WRITE_RAM(simulate_msp430->reg[1], simulate_msp430->reg[0] & 0xff);
      WRITE_RAM(simulate_msp430->reg[1] + 1, simulate_msp430->reg[0] >> 8);
      simulate_msp430->reg[0] = src;
      simulate->nested_call_count++;
      break;
    case 6:  // RETI
      break;
    default:
      return -1;
  }

  return 0;
}

static int relative_jump_exe(struct _simulate *simulate, uint16_t opcode)
{
  struct _simulate_msp430 *simulate_msp430=(struct _simulate_msp430 *)simulate->context;
  int o;

  o = (opcode & 0x1c00) >> 10;

  int offset = opcode & 0x03ff;
  if ((offset & 0x0200) != 0)
  {
    offset = -((offset ^ 0x03ff) + 1);
  }

  offset *= 2;

  switch(o)
  {
    case 0:  // JNE/JNZ  Z==0
      if (GET_Z() == 0) simulate_msp430->reg[0] += offset;
      break;
    case 1:  // JEQ/JZ   Z==1
      if (GET_Z() == 1) simulate_msp430->reg[0] += offset;
      break;
    case 2:  // JNC/JLO  C==0
      if (GET_C() == 0) simulate_msp430->reg[0] += offset;
      break;
    case 3:  // JC/JHS   C==1
      if (GET_C() == 1) simulate_msp430->reg[0] += offset;
      break;
    case 4:  // JN       N==1
      if (GET_N() == 1) simulate_msp430->reg[0] += offset;
      break;
    case 5:  // JGE      (N^V)==0
      if ((GET_N() ^ GET_V()) == 0) simulate_msp430->reg[0] += offset;
      break;
    case 6:  // JL       (N^V)==1
      if ((GET_N() ^ GET_V()) == 1) simulate_msp430->reg[0] += offset;
      break;
    case 7:  // JMP
      simulate_msp430->reg[0] += offset;
      break;
    default:
      return -1;
  }

  return 0;
}

static int two_operand_exe(struct _simulate *simulate, uint16_t opcode)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;
  int o;
  int src_reg,dst_reg;
  int Ad,As,bw;
  int dst,src;
  int pc;
  uint32_t result;

  o = opcode >> 12;
  Ad = (opcode & 0x0080) >> 7;
  As = (opcode & 0x0030) >> 4;
  bw = (opcode & 0x0040) >> 6;

  src_reg = (opcode >> 8) & 0x000f;
  dst_reg = opcode & 0x000f;

  switch(o)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      return -1;
    case 4:  // MOV
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      put_data(simulate, pc, dst_reg, Ad, bw, src);
      break;
    case 5:  // ADD
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = (uint16_t)dst + (uint16_t)src;
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(simulate, pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result);
      break;
    case 6:  // ADDC
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = (uint16_t)dst + (uint16_t)src + GET_C();
      //CHECK_OVERFLOW_WITH_C();
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(simulate, pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 7:  // SUBC
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      //src =~ ((uint16_t)src)+1;
      src = ((~((uint16_t)src)) & 0xffff);
      //result = (uint16_t)dst + (uint16_t)src + GET_C();
      // FIXME - Added GET_C().  Test it.
      result = dst + src + GET_C();
      //CHECK_OVERFLOW_WITH_C();
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(simulate, pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 8:  // SUB
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      src = ((~((uint16_t)src)) & 0xffff) + 1;
      result = dst + src;
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      put_data(simulate, pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 9:  // CMP
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      src = ((~((uint16_t)src)) & 0xffff) + 1;
      //result = (uint16_t)dst + (uint16_t)src;
      result = dst + src;
      CHECK_OVERFLOW();
      dst = result & 0xffff;
      //put_data(simulate, pc, dst_reg, Ad, bw, dst);
      AFFECTS_NZ(dst);
      CHECK_CARRY(result)
      break;
    case 10: // DADD
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = src + dst + GET_C();
      if (bw == 0)
      {
        int a;
        a = (src & 0xf) + (dst & 0xf) + GET_C();
        a = ((((src >>  4) & 0xf) + ((dst >>  4) & 0xf) + ((a >> 0)/10)) <<  4) | (((a >> 0) % 10)<<0);
        a = ((((src >>  8) & 0xf) + ((dst >>  8) & 0xf) + ((a >> 4)/10)) <<  8) | (((a >> 4) % 10)<<4) | (a & 0xf);
        a = ((((src >> 12) & 0xf) + ((dst >> 12) & 0xf) + ((a >> 8)/10)) << 12) | (((a >> 8) % 10)<<8) | (a & 0xff);
        if( (a>>12) >= 10 ) { a = (((a >> 12) % 10)<<12) | (a&0xfff); SET_C(); } else { CLEAR_C(); }
        result = a;
      }
        else
      {
        int a;
        a = (src & 0xf) + (dst & 0xf) + GET_C();
        a = ((((src >> 4) & 0xf) + ((dst >> 4) & 0xf) + ((a >> 0)/10)) << 4) | (((a >> 0) % 10)<<0);
        if( (a>>4) >= 10 ) { a = (((a >> 4) % 10) << 4) | (a & 0x0f); SET_C(); } else {CLEAR_C(); }
        result = a;
      }
      put_data(simulate, pc, dst_reg, Ad, bw, result);
      AFFECTS_NZ(result);
      break;
    case 11: // BIT (dest & src)
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = src & dst;
      AFFECTS_NZ(result);
      if (result != 0) { SET_C(); } else { CLEAR_C(); }
      CLEAR_V();
      break;
    case 12: // BIC (dest &= ~src)
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = (~src) & dst;
      put_data(simulate, pc, dst_reg, Ad, bw, result);
      break;
    case 13: // BIS (dest |= src)
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = src | dst;
      put_data(simulate, pc, dst_reg, Ad, bw, result);
      break;
    case 14: // XOR
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = src ^ dst;
      put_data(simulate, pc, dst_reg, Ad, bw, result);
      AFFECTS_NZ(result);
      if (result != 0) { SET_C(); } else { CLEAR_C(); }
      if ((src & 0x8000) && (dst & 0x8000)) { SET_V(); } else { CLEAR_V(); }
      break;
    case 15: // AND
      src = get_data(simulate, src_reg, As, bw);
      update_reg(simulate, src_reg, As, bw);
      pc = simulate_msp430->reg[0];
      dst = get_data(simulate, dst_reg, Ad, bw);
      result = src & dst;
      put_data(simulate, pc, dst_reg, Ad, bw, result);
      AFFECTS_NZ(result);
      if (result != 0) { SET_C(); } else { CLEAR_C(); }
      CLEAR_V();
      break;
    default:
      return -1;
  }

  return 0;
}

struct _simulate *simulate_init_msp430(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_msp430) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_msp430;
  simulate->simulate_free = simulate_free_msp430;
  simulate->simulate_dumpram = simulate_dumpram_msp430;
  simulate->simulate_push = simulate_push_msp430;
  simulate->simulate_set_reg = simulate_set_reg_msp430;
  simulate->simulate_get_reg = simulate_get_reg_msp430;
  simulate->simulate_set_pc = simulate_set_pc_msp430;
  simulate->simulate_reset = simulate_reset_msp430;
  simulate->simulate_dump_registers = simulate_dump_registers_msp430;
  simulate->simulate_run = simulate_run_msp430;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_msp430(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_msp430(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;

  simulate_msp430->reg[1] -= 2;
  WRITE_RAM(simulate_msp430->reg[1], value & 0xff);
  WRITE_RAM(simulate_msp430->reg[1] + 1, value >> 8);
}

static char *flags[] = { "C", "Z", "N", "GIE", "CPUOFF", "OSCOFF", "SCG0",
                       "SCG1", "V" };

int simulate_set_reg_msp430(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;
  int reg,n;

  while(*reg_string == ' ') { reg_string++; }
  reg = get_register_msp430(reg_string);
  if (reg == -1)
  {
    for (n = 0; n < 9; n++)
    {
      if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value == 1)
        { simulate_msp430->reg[2] |= (1 << n); }
          else
        { simulate_msp430->reg[2] &= 0xffff ^ (1 << n); }
        return 0;
      }
    }
    return -1;
  }

  simulate_msp430->reg[reg] = value;

  return 0;
}

uint32_t simulate_get_reg_msp430(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;
  int reg;

  reg = get_register_msp430(reg_string);
  if (reg == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return simulate_msp430->reg[reg];
}

void simulate_set_pc_msp430(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;

  simulate_msp430->reg[0] = value;
}

void simulate_reset_msp430(struct _simulate *simulate)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  memset(simulate_msp430->reg, 0, sizeof(simulate_msp430->reg));
  //memory_clear(&simulate->memory);
  simulate_msp430->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  simulate_msp430->reg[1] = 0x800;
  simulate->break_point = -1;
}

void simulate_free_msp430(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

int simulate_dumpram_msp430(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_msp430(struct _simulate *simulate)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;
  int n,sp = simulate_msp430->reg[1];

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("        8    7    6             4   3 2 1 0              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("        %d    %d    %d      %d      %d   %d %d %d %d              0x%04x: 0x%02x%02x\n",
         (simulate_msp430->reg[2] >> 8) & 1,
         (simulate_msp430->reg[2] >> 7) & 1,
         (simulate_msp430->reg[2] >> 6) & 1,
         (simulate_msp430->reg[2] >> 5) & 1,
         (simulate_msp430->reg[2] >> 4) & 1,
         (simulate_msp430->reg[2] >> 3) & 1,
         (simulate_msp430->reg[2] >> 2) & 1,
         (simulate_msp430->reg[2] >> 1) & 1,
         (simulate_msp430->reg[2]) & 1,
         SHOW_STACK);
  sp_inc(&sp);
  printf("                                                         0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);

  printf(" PC: 0x%04x,  SP: 0x%04x,  SR: 0x%04x,  CG: 0x%04x,",
         simulate_msp430->reg[0],
         simulate_msp430->reg[1],
         simulate_msp430->reg[2],
         simulate_msp430->reg[3]);

  for (n = 4; n < 16; n++)
  {
    if ((n % 4) == 0)
    {
      printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
      printf("\n");
      sp_inc(&sp);
    }
      else
    { printf(" "); }

    char reg[4];
    sprintf(reg, "r%d",n);
    printf("%3s: 0x%04x,", reg, simulate_msp430->reg[n]);
  }
  printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_msp430(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_msp430 *simulate_msp430 = (struct _simulate_msp430 *)simulate->context;
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int c;
  int n;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running == 0)
  {
    pc = simulate_msp430->reg[0];
    opcode = READ_RAM(pc) | (READ_RAM(pc + 1) << 8);
    c = get_cycle_count(opcode);
    if (c > 0) simulate->cycle_count += c;
    simulate_msp430->reg[0] += 2;

    if (simulate->show == 1) printf("\x1b[1J\x1b[1;1H");

    if ((opcode & 0xfc00) == 0x1000)
    {
      ret = one_operand_exe(simulate, opcode);
    }
      else
    if ((opcode & 0xe000) == 0x2000)
    {
      ret = relative_jump_exe(simulate, opcode);
    }
      else
    {
      if (opcode == 0x4130) { simulate->nested_call_count--; }
      ret = two_operand_exe(simulate, opcode);
    }

    if (c > 0) { cycles += c; }

    if (simulate->show == 1)
    {
      simulate_dump_registers_msp430(simulate);

      n = 0;
      while(n < 12)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
        int count = disasm_msp430(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        if (cycles_min == -1) { break; }

        printf("%s", pc == simulate->break_point ? "*" : " ");
/*
        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }
*/

        if (n == 0)
        {
          printf("! ");
        }
          else
        if (pc == simulate_msp430->reg[0])
        {
          printf("> ");
        }
          else
        {
          printf("  ");
        }

        if (cycles_min < 1)
        {
          printf("0x%04x: 0x%04x %-40s ?\n", pc, num, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: 0x%04x %-40s %d\n", pc, num, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: 0x%04x %-40s %d-%d\n", pc, num, instruction, cycles_min, cycles_max);
        }

        n = n + count;
        pc += 2;
        count -= 2;
        while (count > 0)
        {
          printf("%s", pc == simulate->break_point ? "*" : " ");
/*
          if (pc == simulate->break_point)
          {
            printf("*");
          }
          else
          {
            printf(" ");
          }
*/

          num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);
          printf("  0x%04x: 0x%04x\n", pc, num);
          pc += 2;
          count -= 2;
        }
      }
    }

    if (simulate->auto_run == 1 && simulate->nested_call_count < 0)
    {
      return 0;
    }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    if (simulate->break_point == simulate_msp430->reg[0])
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

    if (simulate_msp430->reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", simulate->cycle_count);
      simulate->step_mode = 0;
      simulate_msp430->reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", simulate_msp430->reg[0]);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}


