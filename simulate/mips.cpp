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

#include "simulate/mips.h"
#include "disasm/mips.h"

static const char *reg_names[32] =
{
  "$0",  "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
  "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
  "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
  "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

SimulateMips::SimulateMips(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateMips::~SimulateMips()
{
}

Simulate *SimulateMips::init(Memory *memory)
{
  return new SimulateMips(memory);
}

void SimulateMips::reset()
{
  memset(reg, 0, sizeof(reg));
  hi = 0;
  lo = 0;
  ra_was_set = false;
  force_break = false;

  // FIXME: Should this be org?
  pc = memory->low_address;
  reg[29] = 0x80000000;

  // PIC32 kind of hack.  Need to figure out a better way to do this
  // later.  Problem is PIC32 has virtual memory (where code addresses)
  // and physical memory (where the hex file says the code is).
#if 0
  if (memory->low_address >= 0x1d000000 && memory->high_address <= 0x1d007fff)
  {
    uint32_t physical, virtual_address;
    uint32_t low_address, high_address;

    virtual_address = 0x9d000000 + (memory->low_address - 0x1d000000);

    printf("Copying physical 0x%x-0x%x to virtual 0x%x\n",
      memory->low_address,
      memory->high_address,
      virtual_address);

    pc = virtual_address;
    low_address = memory->low_address;
    high_address = memory->high_address;

    for (physical = low_address; physical <= high_address; physical++)
    {
      memory->write8(virtual_address++, memory->read8(physical));
    }
  }
#endif
}

void SimulateMips::push(uint32_t value)
{
}

int SimulateMips::set_reg(const char *reg_string, uint32_t value)
{
  if (reg_string[0] != '$') { return -1; }

  if (reg_string[1] >= '0' && reg_string[1] <= '9' &&
      reg_string[2] >= '0' && reg_string[2] <= '9' &&
      reg_string[3] == 0)
  {
    int index = atoi(reg_string + 1);
    if (index < 0 || index > 31) { return -1; }

    reg[index] = value;
    return 0;
  }

  for (int n = 0; n < 32; n++)
  {
    if (strcmp(reg_string, reg_names[n]) == 0)
    {
      reg[n] = value;
      return 0;
    }
  }

  return -1;
}

uint32_t SimulateMips::get_reg(const char *reg_string)
{
  if (reg_string[0] != '$') { return -1; }

  if (reg_string[1] >= '0' && reg_string[1] <= '9' &&
      reg_string[2] >= '0' && reg_string[2] <= '9' &&
      reg_string[3] == 0)
  {
    int index = atoi(reg_string + 1);
    if (index < 0 || index > 31) { return -1; }

    return reg[index];
  }

  for (int n = 0; n < 32; n++)
  {
    if (strcmp(reg_string, reg_names[n]) == 0)
    {
      return reg[n];
    }
  }

  return 0;
}

void SimulateMips::set_pc(uint32_t value)
{
  pc = value;
}

void SimulateMips::dump_registers()
{
  int n;

  printf("\nSimulation Register Dump\n");
  printf("-------------------------------------------------------------------\n");
  printf(" PC: 0x%08x  HI: 0x%08x  LO: 0x%08x\n", pc, hi, lo);

  for (n = 0; n < 32; n++)
  {
    printf("%c%3s: 0x%08x", (n & 0x3) == 0 ? '\n' : ' ', reg_names[n], reg[n]);
  }

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int SimulateMips::run(int max_cycles, int step)
{
  char instruction[128];
  uint32_t current_pc;

  while (stop_running == false)
  {
    current_pc = pc;

    int ret = execute();

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    cycle_count++;

    if (show == true)
    {
      dump_registers();

      int n = 0;
      while (n < 6)
      {
        int cycles_min, cycles_max;

        uint32_t opcode = memory->read32(current_pc);

        int count = disasm_mips(
          memory,
          current_pc,
          instruction,
          sizeof(instruction),
          MIPS_I | MIPS_II | MIPS_III | MIPS_32,
          &cycles_min,
          &cycles_max);

        if (count < 1) { break; }

        if (current_pc == (uint32_t)break_point)
        {
          printf("*");
        }
          else
        {
          printf(" ");
        }

        if (n == 0)
        {
          printf("! ");
        }
          else
        if (current_pc == pc)
        {
          printf("> ");
        }
          else
        {
          printf("  ");
        }

        printf("0x%04x: 0x%08x %-40s %d\n", current_pc, opcode, instruction, cycles_min);

        current_pc += 4;
        n++;
      }
    }

    if (pc == (uint32_t)break_point)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == true || force_break == true)
    {
      disable_signal_handler();
      return 0;
    }
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", pc);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

int32_t SimulateMips::get_offset16(uint32_t opcode)
{
  int32_t offset = (int32_t)((int16_t)(opcode & 0x3ffffff));
  offset = offset << 2;

  return offset;
}

int SimulateMips::delay_slot()
{
  uint32_t old_pc = pc;

  pc += 4;
  execute();
  pc = old_pc;

  return 0;
}

int SimulateMips::execute_shift(uint32_t opcode)
{
  int sa = (opcode >> 6) & 0x1f;
  int rd = (opcode >> 11) & 0x1f;
  int rt = (opcode >> 16) & 0x1f;

  switch (opcode & 0x3f)
  {
    case 0: // sll
      reg[rd] = reg[rt] << sa;
      break;
    case 2: // srl
      reg[rd] = ((uint32_t)reg[rt]) >> sa;
      break;
    case 3: // sra
      reg[rd] = reg[rt] >> sa;
      break;
    default:
      return -1;
  }

  return 0;
}

int SimulateMips::execute_mips_r(uint32_t opcode)
{
  int rd = (opcode >> 11) & 0x1f;
  int rt = (opcode >> 16) & 0x1f;
  int rs = (opcode >> 21) & 0x1f;

  opcode = opcode & 0x3f;

  switch (opcode)
  {
    case 0x20: // add
      // FIXME - need to trap on overflow
      reg[rd] = reg[rs] + reg[rt];
      break;
    case 0x21: // addu
      reg[rd] = reg[rs] + reg[rt];
      break;
    case 0x22: // sub
      // FIXME - need to trap on overflow
      reg[rd] = reg[rs] - reg[rt];
      break;
    case 0x23: // subu
      reg[rd] = reg[rs] - reg[rt];
      break;
    case 0x24: // and
      // FIXME - need to trap on overflow
      reg[rd] = reg[rs] & reg[rt];
      break;
    case 0x25: // or
      reg[rd] = reg[rs] | reg[rt];
      break;
    case 0x26: // xor
      reg[rd] = reg[rs] ^ reg[rt];
      break;
    case 0x27: // nor
      reg[rd] = ~(reg[rs] | reg[rt]);
      break;
    case 0x2a: // slt
      reg[rd] = (reg[rs] < reg[rt]) ? 1 : 0;
      break;
    case 0x2b: // sltu
      reg[rd] = (reg[rs] < reg[rt]) ? 1 : 0;
      break;
    default:
      return -1;
  }

  return 0;
}

int SimulateMips::execute_mips_i(uint32_t opcode)
{
  uint32_t address;

  int rs = (opcode >> 21) & 0x1f;
  int rt = (opcode >> 16) & 0x1f;

  switch (opcode >> 26)
  {
    case 0x08: // addi
      // FIXME - need to trap on overflow
      reg[rt] = reg[rs] + (int16_t)(opcode & 0xffff);
      break;
    case 0x09: // addiu
      reg[rt] = reg[rs] + (int16_t)(opcode & 0xffff);
      break;
    case 0x0a: // slti
      reg[rt] =
        (reg[rs] < (int16_t)(opcode & 0xffff)) ? 1 : 0;
      break;
    case 0x0b: // sltiu
      reg[rt] =
        (reg[rs] < (int16_t)(opcode & 0xffff)) ? 1 : 0;
      break;
    case 0x0c: // andi
      reg[rt] = reg[rs] & (opcode & 0xffff);
      break;
    case 0x0d: // ori
      reg[rt] = reg[rs] | (opcode & 0xffff);
      break;
    case 0x0e: // xori
      reg[rt] = reg[rs] ^ (opcode & 0xffff);
      break;
    case 0x0f: // lui
      if (rs != 0) { return -1; }
      reg[rt] = (opcode & 0xffff) << 16;
      break;
    case 0x20: // lb
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      reg[rt] = (int32_t)((int8_t)memory->read8(address));
      break;
    case 0x21: // lh
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      if ((address & 1) != 0)
      {
        printf("Alignment error.  Reading address 0x%04x\n", address);
        return -2;
      }
      reg[rt] = (int32_t)((int16_t)memory->read16(address));
      break;
    case 0x23: // lw
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      if ((address & 3) != 0)
      {
        printf("Alignment error.  Reading address 0x%04x\n", address);
        return -2;
      }
      reg[rt] = memory->read32(address);
      break;
    case 0x24: // lbu
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      reg[rt] = memory->read8(address);
      break;
    case 0x25: // lhu
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      if ((address & 1) != 0)
      {
        printf("Alignment error.  Reading address 0x%04x\n", address);
        return -2;
      }
      reg[rt] = (int32_t)((uint16_t)memory->read16(address));
      break;
    case 0x28: // sb
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      memory->write8(address, reg[rt] & 0xff);
      break;
    case 0x29: // sh
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      if ((address & 1) != 0)
      {
        printf("Alignment error.  Reading address 0x%04x\n", address);
        return -2;
      }
      memory->write16(address, reg[rt] & 0xffff);
      break;
    case 0x2b: // sw
      address = reg[rs] + ((int16_t)(opcode & 0xffff));
      if ((address & 3) != 0)
      {
        printf("Alignment error.  Reading address 0x%04x\n", address);
        return -2;
      }
      memory->write32(address, reg[rt]);
      break;
    default:
      return -1;
  }

  return 0;
}

int SimulateMips::execute()
{
  uint32_t opcode = memory->read32(pc);

  int rs = (opcode >> 21) & 0x1f;
  int rt = (opcode >> 16) & 0x1f;
  int rd = (opcode >> 11) & 0x1f;

  switch (opcode >> 26)
  {
    case 0x00:
      if (((opcode >> 6) & 0x3ff) == 0 && (opcode & 0x3f) == 0x1a)
      {
        // div
        hi = reg[rs] % reg[rt];
        lo = reg[rs] / reg[rt];
        break;
      }

      if (((opcode >> 6) & 0x3ff) == 0 && (opcode & 0x3f) == 0x1b)
      {
        // divu
        hi = reg[rs] % reg[rt];
        lo = reg[rs] / reg[rt];
        break;
      }

      if (((opcode >> 6) & 0x1f) == 0 && (opcode & 0x3f) == 0x18)
      {
        // mult
        uint64_t result = reg[rs] * reg[rt];
        hi = result >> 32;
        lo = result & 0xffffffff;
        break;
      }

      if (((opcode >> 6) & 0x1f) == 0 && (opcode & 0x3f) == 0x19)
      {
        // multu
        uint64_t result = reg[rs] * reg[rt];
        hi = result >> 32;
        lo = result & 0xffffffff;
        break;
      }

      if (((opcode >> 6) & 0x7fff) == 0 && (opcode & 0x3f) == 0x11)
      {
        // mthi
        hi = reg[rs];
        break;
      }

      if (((opcode >> 6) & 0x7fff) == 0 && (opcode & 0x3f) == 0x13)
      {
        // mtlo
        lo = reg[rs];
        break;
      }

      if (((opcode >> 16) & 0x3ff) == 0 &&
          ((opcode >> 6) & 0x1f) == 0 &&
           (opcode & 0x3f) == 0x10)
      {
        // mfhi
        reg[rd] = hi;
        break;
      }

      if (((opcode >> 16) & 0x3ff) == 0 &&
          ((opcode >> 6) & 0x1f) == 0 &&
           (opcode & 0x3f) == 0x12)
      {
        // mflo
        reg[rd] = lo;
        break;
      }

      if (((opcode >> 6) & 0x1f) == 0)
      {
        if (execute_mips_r(opcode) == 0) { break; }
      }

      if (((opcode >> 21) & 0x1f) == 0)
      {
        if (execute_shift(opcode) == 0) { break; }
      }

      if (((opcode >> 11) & 0x3ff) == 0 && (opcode & 0x3f) == 0x08)
      {
        // jr
        if (((opcode >> 21) & 0x1f) == 31)
        {
          //if (! ra_was_set) { force_break = true; }
        }

        delay_slot();
        pc = reg[(opcode >> 21) & 0x1f];
        return 0;
      }

      return -1;
    case 0x01:
      if (rt == 1) // bgez
      {
        if (reg[rs] >= 0)
        {
          delay_slot();
          pc += 4 + get_offset16(opcode);
          return 0;
        }
        break;
      }
      return -1;
    case 0x02: // j
      delay_slot();
      pc &= 0xfc000000;
      pc |= ((opcode & 0x3ffffff) << 2);
      return 0;
    case 0x03: // jal
      delay_slot();
      reg[31] = pc + 8;
      pc &= 0xfc000000;
      pc |= ((opcode & 0x3ffffff) << 2);
      return 0;
    case 0x04: // beq
      if (reg[rs] == reg[rt])
      {
        delay_slot();
        pc += 4 + get_offset16(opcode);
        return 0;
      }
      break;
    case 0x05: // bne
      if (reg[rs] != reg[rt])
      {
        delay_slot();
        pc += 4 + get_offset16(opcode);
        return 0;
      }
      break;
    case 0x06: // blez
      if (rt != 0) { return -1; }
      if (reg[rs] <= reg[rt])
      {
        delay_slot();
        pc += 4 + get_offset16(opcode);
        return 0;
      }
      break;
    case 0x07: // bgtz
      if (rt != 0) { return -1; }
      if (reg[rs] > reg[rt])
      {
        delay_slot();
        pc += 4 + get_offset16(opcode);
        return 0;
      }
      break;
    case 0x1c: // mul
      if (((opcode >> 6) & 0x1f) == 0)
      {
        reg[rd] = reg[rs] * reg[rt];
        break;
      }
      return -1;
    default:
      if (execute_mips_i(opcode) == 0) { break; }
      return -1;
      break;
  }

  reg[0] = 0;
  pc += 4;

  return 0;
}

