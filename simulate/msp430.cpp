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

#include "asm/msp430.h"
#include "disasm/msp430.h"
#include "simulate/msp430.h"

#define SHOW_STACK sp, memory->read8(sp + 1), memory->read8(sp)
#define READ_RAM(a) ((a == serial_addr) ? fgetc(serial_in) : memory->read8(a))
#define WRITE_RAM(a,b) \
  if (a == break_io) \
  { \
    exit(b); \
  } \
  if (a == serial_addr) \
  { \
    fputc(b, serial_out); \
  } else \
    memory->write8(a, b);

const char *SimulateMsp430::flags[] =
{
  "C",
  "Z",
  "N",
  "GIE",
  "CPUOFF",
  "OSCOFF",
  "SCG0",
  "SCG1",
  "V"
};

SimulateMsp430::SimulateMsp430(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateMsp430::~SimulateMsp430()
{
}

Simulate *SimulateMsp430::init(Memory *memory)
{
  return new SimulateMsp430(memory);
}

void SimulateMsp430::reset()
{
  cycle_count = 0;
  nested_call_count = 0;
  memset(reg, 0, sizeof(reg));

  // Set PC to reset vector.
  reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);

  // FIXME - A real chip wouldn't set the SP to this, but this is
  // in case someone is simulating code that won't run on a chip.
  reg[1] = 0x800;
  break_point = -1;
}

void SimulateMsp430::push(uint32_t value)
{
  reg[1] -= 2;
  WRITE_RAM(reg[1], value & 0xff);
  WRITE_RAM(reg[1] + 1, value >> 8);
}

int SimulateMsp430::set_reg(const char *reg_string, uint32_t value)
{
  while (*reg_string == ' ') { reg_string++; }

  int index = get_register_msp430(reg_string);

  if (index == -1)
  {
    for (int n = 0; n < 9; n++)
    {
      if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value == 1)
        {
          reg[2] |= (1 << n);
        }
          else
        {
          reg[2] &= 0xffff ^ (1 << n);
        }

        return 0;
      }
    }
    return -1;
  }

  reg[index] = value;

  return 0;
}

uint32_t SimulateMsp430::get_reg(const char *reg_string)
{
  int index;

  index = get_register_msp430(reg_string);

  if (index == -1)
  {
    printf("Unknown register '%s'\n", reg_string);
    return -1;
  }

  return reg[index];
}

void SimulateMsp430::set_pc(uint32_t value)
{
  reg[0] = value;
}

void SimulateMsp430::dump_registers()
{
  int n, sp = reg[1];

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("        8    7    6             4   3 2 1 0              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("Status: V SCG1 SCG0 OSCOFF CPUOFF GIE N Z C              0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);
  printf("        %d    %d    %d      %d      %d   %d %d %d %d              0x%04x: 0x%02x%02x\n",
         (reg[2] >> 8) & 1,
         (reg[2] >> 7) & 1,
         (reg[2] >> 6) & 1,
         (reg[2] >> 5) & 1,
         (reg[2] >> 4) & 1,
         (reg[2] >> 3) & 1,
         (reg[2] >> 2) & 1,
         (reg[2] >> 1) & 1,
         (reg[2]) & 1,
         SHOW_STACK);
  sp_inc(&sp);
  printf("                                                         0x%04x: 0x%02x%02x\n", SHOW_STACK);
  sp_inc(&sp);

  printf(" PC: 0x%04x,  SP: 0x%04x,  SR: 0x%04x,  CG: 0x%04x,",
         reg[0],
         reg[1],
         reg[2],
         reg[3]);

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

    char reg_string[4];
    snprintf(reg_string, sizeof(reg_string), "r%d",n);
    printf("%3s: 0x%04x,", reg_string, reg[n]);
  }
  printf("      0x%04x: 0x%02x%02x", SHOW_STACK);
  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int SimulateMsp430::run(int max_cycles, int step)
{
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc;
  int c;
  int n;

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    pc = reg[0];
    opcode = READ_RAM(pc) | (READ_RAM(pc + 1) << 8);
    c = get_cycle_count(opcode);
    if (c > 0) { cycle_count += c; }
    reg[0] += 2;

    if (show == true) printf("\x1b[1J\x1b[1;1H");

    if ((opcode & 0xfc00) == 0x1000)
    {
      ret = one_operand_exe(opcode);
    }
      else
    if ((opcode & 0xe000) == 0x2000)
    {
      ret = relative_jump_exe(opcode);
    }
      else
    {
      if (opcode == 0x4130) { nested_call_count--; }
      ret = two_operand_exe(opcode);
    }

    if (c > 0) { cycles += c; }

    if (show == true)
    {
      dump_registers();

      n = 0;
      while (n < 12)
      {
        int cycles_min,cycles_max;
        int num;
        num = (READ_RAM(pc + 1) << 8) | READ_RAM(pc);

        int count = disasm_msp430(
          memory,
          pc,
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        if (cycles_min == -1) { break; }

        printf("%s", pc == break_point ? "*" : " ");
/*
        if (pc == break_point) { printf("*"); }
        else { printf(" "); }
*/

        if (n == 0)
        {
          printf("! ");
        }
          else
        if (pc == reg[0])
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
          printf("%s", pc == break_point ? "*" : " ");
/*
          if (pc == break_point)
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

    if (auto_run == true && nested_call_count < 0)
    {
      return 0;
    }

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    if (break_point == reg[0])
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == true)
    {
      //step_mode=0;
      disable_signal_handler();
      return 0;
    }

    if (reg[0] == 0xffff)
    {
      printf("Function ended. Total cycles: %d\n", cycle_count);
      step_mode = false;
      reg[0] = READ_RAM(0xfffe) | (READ_RAM(0xffff) << 8);
      disable_signal_handler();
      return 0;
    }

    usleep(usec);
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", reg[0]);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

void SimulateMsp430::sp_inc(int *sp)
{
  (*sp) += 2;
  if (*sp > 0xffff) { *sp = 0; }
}

uint16_t SimulateMsp430::get_data(int reg_index, int As, int bw, int &ea)
{
  const int PC = reg[0];

  ea = -1;

  if (reg_index == 3) // CG
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
      return (bw == BW_WORD) ? 0xffff : 0xff;
    }
  }

  if (As == 0) // Rn
  {
    return (bw == BW_WORD) ? reg[reg_index] : reg[reg_index] & 0xff;
  }

  if (reg_index == 2)
  {
    if (As == 1) // &LABEL
    {
      ea = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);

      reg[0] += 2;

      if (bw == BW_WORD)
      {
        return READ_RAM(ea) | (READ_RAM(ea + 1) << 8);
      }
        else
      {
        return READ_RAM(ea);
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

  if (reg_index == 0) // PC
  {
    // This is probably worthless.. some other condition should pick this up
    if (As == 3) // #immediate
    {
      uint16_t a = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);

      reg[0] += 2;

      return (bw == BW_WORD) ? a : a & 0xff;
    }
  }

  if (As == 1) // x(Rn)
  {
    uint16_t a = READ_RAM(PC) | (READ_RAM(PC + 1) << 8);
    ea = (reg[reg_index] + ((int16_t)a)) & 0xffff;

    reg[0] += 2;

    if (bw == BW_WORD)
    {
      return READ_RAM(ea) | (READ_RAM(ea + 1) << 8);
    }
      else
    {
      return READ_RAM(ea);
    }
  }
    else
  if (As == 2 || As == 3) // @Rn (mode 2) or @Rn+ (mode 3)
  {
    ea = reg[reg_index];

    if (bw == BW_WORD)
    {
      return READ_RAM(ea) | (READ_RAM(ea + 1) << 8);
    }
      else
    {
      return READ_RAM(reg[ea]);
    }
  }

  printf("Error: Unrecognized source addressing mode %d\n", As);

  return 0;
}

void SimulateMsp430::update_reg(int reg_index, int mode, int bw)
{
  if (reg_index == 0) { return; }
  if (reg_index == 2) { return; }
  if (reg_index == 3) { return; }

  if (mode == 3) // @Rn+
  {
    if (bw == BW_WORD)
    {
      reg[reg_index] += 2;
    }
      else
    {
      reg[reg_index] += 1;
    }
  }
}

int SimulateMsp430::put_data(
  int ea,
  int reg_index,
  int mode,
  int bw,
  uint32_t data)
{
  if (mode == 0) // Rn
  {
    if (bw == BW_WORD)
    {
      reg[reg_index] = data;
    }
      else
    {
      reg[reg_index] = data & 0xff;
    }

    return 0;
  }

  if (bw == BW_WORD)
  {
    WRITE_RAM(ea,     data & 0xff);
    WRITE_RAM(ea + 1, data >> 8);
  }
    else
  {
    WRITE_RAM(ea, data & 0xff);
  }

  return 0;
}

int SimulateMsp430::one_operand_exe(uint16_t opcode)
{
  int o;
  int reg_index;
  int As, bw;
  int count = 1;
  uint32_t result;
  int src;

  o = (opcode & 0x0380) >> 7;

  if (o == 7) { return 1; }
  if (o == 6) { return count; }

  As = (opcode & 0x0030) >> 4;
  bw = (opcode & 0x0040) >> 6;
  reg_index = opcode & 0x000f;

  int ea = -1;

  switch (o)
  {
    case 0:  // RRC
    {
      src = get_data(reg_index, As, bw, ea);
      int c = get_c();
      if ((src & 1) == 1) { set_c(); } else { clear_c(); }
      if (bw == BW_WORD)
      { result = (c << 15) | (((uint16_t)src) >> 1); }
        else
      { result = (c << 7) | (((uint8_t)src) >> 1); }
      put_data(ea, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      update_nz(result, bw);
      clear_v();
      break;
    }
    case 1:  // SWPB (no bw)
    {
      src = get_data(reg_index, As, bw, ea);
      result = ((src & 0xff00) >> 8) | ((src & 0xff) << 8);
      put_data(ea, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      break;
    }
    case 2:  // RRA
    {
      src = get_data(reg_index, As, bw, ea);
      if ((src & 1) == 1) { set_c(); } else { clear_c(); }
      if (bw == BW_WORD)
      { result = ((int16_t)src) >> 1; }
        else
      { result = ((int8_t)src) >> 1; }
      put_data(ea, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      update_nz(result, bw);
      clear_v();
      break;
    }
    case 3:  // SXT (no bw)
    {
      src = get_data(reg_index, As, bw, ea);
      result = (int16_t)((int8_t)((uint8_t)src));
      put_data(ea, reg_index, As, bw, result);
      update_reg(reg_index, As, bw);
      update_nz(result, bw);
      update_c(result, bw);
      clear_v();
      break;
    }
    case 4:  // PUSH
    {
      reg[1] -= 2;
      src = get_data(reg_index, As, bw, ea);
      update_reg(reg_index, As, bw);
      WRITE_RAM(reg[1], src & 0xff);
      WRITE_RAM(reg[1] + 1, src >> 8);
      break;
    }
    case 5:  // CALL (no bw)
    {
      src = get_data(reg_index, As, bw, ea);
      update_reg(reg_index, As, bw);
      reg[1] -= 2;
      WRITE_RAM(reg[1], reg[0] & 0xff);
      WRITE_RAM(reg[1] + 1, reg[0] >> 8);
      reg[0] = src;
      nested_call_count++;
      break;
    }
    case 6:  // RETI
    {
      break;
    }
    default:
    {
      return -1;
    }
  }

  return 0;
}

int SimulateMsp430::relative_jump_exe(uint16_t opcode)
{
  int o;

  o = (opcode & 0x1c00) >> 10;

  int offset = opcode & 0x03ff;
  if ((offset & 0x0200) != 0)
  {
    offset = -((offset ^ 0x03ff) + 1);
  }

  offset *= 2;

  switch (o)
  {
    case 0:  // JNE/JNZ  Z==0
      if (get_z() == 0) { reg[0] += offset; }
      break;
    case 1:  // JEQ/JZ   Z==1
      if (get_z() == 1) { reg[0] += offset; }
      break;
    case 2:  // JNC/JLO  C==0
      if (get_c() == 0) { reg[0] += offset; }
      break;
    case 3:  // JC/JHS   C==1
      if (get_c() == 1) { reg[0] += offset; }
      break;
    case 4:  // JN       N==1
      if (get_n() == 1) { reg[0] += offset; }
      break;
    case 5:  // JGE      (N^V)==0
      if ((get_n() ^ get_v()) == 0) { reg[0] += offset; }
      break;
    case 6:  // JL       (N^V)==1
      if ((get_n() ^ get_v()) == 1) { reg[0] += offset; }
      break;
    case 7:  // JMP
      reg[0] += offset;
      break;
    default:
      return -1;
  }

  return 0;
}

int SimulateMsp430::two_operand_exe(uint16_t opcode)
{
  int o;
  int src_reg, dst_reg;
  int Ad, As, bw;
  int dst, src;
  uint32_t result;

  o = opcode >> 12;
  Ad = (opcode & 0x0080) >> 7;
  As = (opcode & 0x0030) >> 4;
  bw = (opcode & 0x0040) >> 6;

  src_reg = (opcode >> 8) & 0x000f;
  dst_reg = opcode & 0x000f;

  int ea = -1;

  switch (o)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      return -1;
    case 4:  // MOV
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      put_data(ea, dst_reg, Ad, bw, src);
      break;
    case 5:  // ADD
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      if (bw == BW_BYTE)
      {
        dst = dst & 0xff;
        src = src & 0xff;
      }
      result = (uint16_t)dst + (uint16_t)src;
      update_v(dst, src, result, bw);
      dst = result & 0xffff;
      put_data(ea, dst_reg, Ad, bw, dst);
      update_nz(dst, bw);
      update_c(result, bw);
      break;
    case 6:  // ADDC
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      if (bw == BW_BYTE)
      {
        dst = dst & 0xff;
        src = src & 0xff;
      }
      result = (uint16_t)dst + (uint16_t)src + get_c();
      update_v(dst, src, result, bw);
      dst = result & 0xffff;
      put_data(ea, dst_reg, Ad, bw, dst);
      update_nz(dst, bw);
      update_c(result, bw);
      break;
    case 7:  // SUBC
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      src = ((~((uint16_t)src)) & 0xffff);
      // FIXME - Added get_c().  Test it.
      if (bw == BW_BYTE)
      {
        dst = dst & 0xff;
        src = src & 0xff;
      }
      result = dst + src + get_c();
      update_v(dst, src, result, bw);
      dst = result & 0xffff;
      put_data(ea, dst_reg, Ad, bw, dst);
      update_nz(dst, bw);
      update_c(result, bw);
      break;
    case 8:  // SUB
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      src = ((~((uint16_t)src)) & 0xffff) + 1;
      if (bw == BW_BYTE)
      {
        dst = dst & 0xff;
        src = src & 0xff;
      }
      result = dst + src;
      update_v(dst, src, result, bw);
      dst = result & 0xffff;
      put_data(ea, dst_reg, Ad, bw, dst);
      update_nz(dst, bw);
      update_c(result, bw);
      break;
    case 9:  // CMP
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      src = ((~((uint16_t)src)) & 0xffff) + 1;
      if (bw == BW_BYTE)
      {
        dst = dst & 0xff;
        src = src & 0xff;
      }
      result = dst + src;
      update_v(dst, src, result, bw);
      dst = result & 0xffff;
      update_nz(dst, bw);
      update_c(result, bw);
      break;
    case 10: // DADD
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      result = src + dst + get_c();
      if (bw == BW_WORD)
      {
        int a;
        a = (src & 0xf) + (dst & 0xf) + get_c();
        a = ((((src >>  4) & 0xf) + ((dst >>  4) & 0xf) + ((a >> 0)/10)) <<  4) | (((a >> 0) % 10)<<0);
        a = ((((src >>  8) & 0xf) + ((dst >>  8) & 0xf) + ((a >> 4)/10)) <<  8) | (((a >> 4) % 10)<<4) | (a & 0xf);
        a = ((((src >> 12) & 0xf) + ((dst >> 12) & 0xf) + ((a >> 8)/10)) << 12) | (((a >> 8) % 10)<<8) | (a & 0xff);
        if( (a>>12) >= 10 ) { a = (((a >> 12) % 10)<<12) | (a&0xfff); set_c(); } else { clear_c(); }
        result = a;
      }
        else
      {
        int a;
        a = (src & 0xf) + (dst & 0xf) + get_c();
        a = ((((src >> 4) & 0xf) + ((dst >> 4) & 0xf) + ((a >> 0)/10)) << 4) | (((a >> 0) % 10)<<0);
        if( (a>>4) >= 10 ) { a = (((a >> 4) % 10) << 4) | (a & 0x0f); set_c(); } else {clear_c(); }
        result = a;
      }
      put_data(ea, dst_reg, Ad, bw, result);
      update_nz(result, bw);
      break;
    case 11: // BIT (dest & src)
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      result = src & dst;
      update_nz(result, bw);
      if (result != 0) { set_c(); } else { clear_c(); }
      clear_v();
      break;
    case 12: // BIC (dest &= ~src)
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      result = (~src) & dst;
      put_data(ea, dst_reg, Ad, bw, result);
      break;
    case 13: // BIS (dest |= src)
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      result = src | dst;
      put_data(ea, dst_reg, Ad, bw, result);
      break;
    case 14: // XOR
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      result = src ^ dst;
      put_data(ea, dst_reg, Ad, bw, result);
      update_nz(result, bw);
      if (result != 0) { set_c(); } else { clear_c(); }
      if ((src & 0x8000) && (dst & 0x8000)) { set_v(); } else { clear_v(); }
      break;
    case 15: // AND
      src = get_data(src_reg, As, bw, ea);
      update_reg(src_reg, As, bw);
      dst = get_data(dst_reg, Ad, bw, ea);
      result = src & dst;
      put_data(ea, dst_reg, Ad, bw, result);
      update_nz(result, bw);
      if (result != 0) { set_c(); } else { clear_c(); }
      clear_v();
      break;
    default:
      return -1;
  }

  return 0;
}

