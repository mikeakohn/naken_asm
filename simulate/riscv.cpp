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

#include "simulate/riscv.h"
#include "disasm/riscv.h"

SimulateRiscv::SimulateRiscv(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateRiscv::~SimulateRiscv()
{
}

Simulate *SimulateRiscv::init(Memory *memory)
{
  return new SimulateRiscv(memory);
}

void SimulateRiscv::reset()
{
  memset(reg, 0, sizeof(reg));
  pc = 0;
}

void SimulateRiscv::push(uint32_t value)
{
}

int SimulateRiscv::set_reg(const char *reg_string, uint32_t value)
{
  int index = get_register(reg_string);

  if (index == -1)
  {
    printf("Error: Unknown register %s\n", reg_string);
    return -1;
  }

  reg[index] = value;

  return 0;
}

uint32_t SimulateRiscv::get_reg(const char *reg_string)
{
  int index = get_register(reg_string);

  if (index == -1)
  {
    printf("Error: Unknown register %s\n", reg_string);
    return 0;
  }

  return reg[index];
}

void SimulateRiscv::set_pc(uint32_t value)
{
  pc = value;
}

void SimulateRiscv::dump_registers()
{
  char name[16];

  for (int n = 0; n < 32; n = n + 4)
  {
    for (int i = 0; i < 4; i++)
    {
      int index = n + i;
      snprintf(name, sizeof(name), "x%d/%s", index, riscv_reg_names[index]);
      printf(" %8s: %08x", name, reg[index]);
    }

    printf("\n");
  }

  printf("   pc: %08x\n\n", pc);
}

int SimulateRiscv::run(int max_cycles, int step)
{
  char instruction[128];
  uint32_t current_pc;

  while (stop_running == false)
  {
    current_pc = pc;

    uint32_t opcode = memory->read32(current_pc);
    int ret = execute(opcode);
  
    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (ret == -2)
    {
      printf("Break at address 0x%04x\n", pc);
      return -2;
    }

    pc += ret;

    cycle_count++;

    if (show == true)
    {
      dump_registers();

      int n = 0;
      while (n < 6)
      {
        int cycles_min, cycles_max;

        uint32_t opcode = memory->read32(current_pc);

        int count = disasm_riscv(
          memory,
          current_pc,
          instruction,
          sizeof(instruction),
          0,
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

        printf("0x%04x: 0x%08x %-40s\n", current_pc, opcode, instruction);

        current_pc += 4;
        n++;
      }

      printf("\n");
    }

    if (pc == (uint32_t)break_point)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == true)
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

int SimulateRiscv::get_register(const char *name)
{
  if (strcasecmp(name, "s0") == 0) { return 8; }

  for (int n = 0; n < 32; n++)
  {
    if (strcasecmp(name, riscv_reg_names[n]) == 0)
    {
      return n;
    }
  }

  if (name[0] != 'x' && name[0] != 'X') { return -1; }

  char *s;

  int num = strtol(name + 1, &s, 10);

  if (*s != 0) { return -1; }
  if (num < 0 || num > 32) { return -1; }

  return num;
}

int SimulateRiscv::execute(uint32_t opcode)
{
  const int operation = opcode & 0x7f;
  const int rd = (opcode >> 7) & 0x1f;
  const int rs1 = (opcode >> 15) & 0x1f;

  int length = 0;

  switch (operation)
  {
    case OP_LUI:
      reg[rd] = opcode & 0xfffff000;
      length = 4;
      break;
    case OP_AUIPC:
      reg[rd] = pc + (opcode & 0xfffff000);
      length = 4;
      break;
    case OP_JAL:
      reg[rd] = pc + 4;
      pc = pc + jal_address(opcode);
      length = 4;
      break;
    case OP_JALR:
      reg[rd] = pc + 4;
      pc = pc + reg[rs1] + jalr_address(opcode);
      length = 4;
      break;
    case OP_BRA:
      reg[rd] = pc + branch(opcode);
      length = 4;
      break;
    case OP_LD:
      load(opcode);
      length = 4;
      break;
    case OP_ST:
      store(opcode);
      length = 4;
      break;
    case OP_ALU:
      alu(opcode);
      length = 4;
      break;
    case OP_ALU_R:
      alu_reg(opcode);
      length = 4;
      break;
    case OP_BREAK:
      length = -2;
      break;
    default:
      length = -1;
      break;
  }

  reg[0] = 0;

  return length;
}

int SimulateRiscv::jal_address(uint32_t opcode)
{
  int n = 0;

  n |= ((opcode >> 31) & 0x01) << 20;
  n |= ((opcode >> 12) & 0xff) << 12;
  n |= ((opcode >> 20) & 0x01) << 11;
  n |= ((opcode >> 21) & 0x3ff) << 12;

  if (((opcode >> 31) & 0x01) == 1)
  {
    n |= 0xfff00000;
  }

  return n;
}

int SimulateRiscv::jalr_address(uint32_t opcode)
{
  int n = opcode >> 20;

  if (((opcode >> 31) & 1) == 1)
  {
    n |= 0xfffff000;
  }

  return n;
}

int SimulateRiscv::branch(uint32_t opcode)
{
  int n = 0;

  n |= ((opcode >> 31) & 0x01) << 12;
  n |= ((opcode >>  7) & 0x01) << 11;
  n |= ((opcode >> 25) & 0x3f) << 5;
  n |= ((opcode >>  8) & 0x0f) << 1;

  if (((opcode >> 31) & 0x01) == 1)
  {
    n |= 0xfffff000;
  }

  return n;
}

int SimulateRiscv::load(uint32_t opcode)
{
  const int rd = (opcode >> 7) & 0x1f;
  const int rs1 = (opcode >> 15) & 0x1f;
  const int funct3 = (opcode >> 12) & 0x7;
  int imm = (opcode >> 20) & 0xfff;
  int n;

  if ((imm & 0x800) != 0) { imm |= 0xfffff000; }
  uint32_t ea = reg[rs1] + imm;

  switch (funct3)
  {
    case 0:
      n = memory->read8(ea);
      if ((n & 0x80) != 0) { n |= 0xfffffff0; }
      reg[rd] = n;
      break;
    case 1:
      n = memory->read16(ea & 0xfffffffe);
      if ((n & 0x8000) != 0) { n |= 0xffff0000; }
      reg[rd] = n;
      break;
    case 2:
      reg[rd] = memory->read32(ea & 0xfffffffc);
      break;
    case 4:
      reg[rd] = memory->read8(ea);
      break;
    case 5:
      reg[rd] = memory->read16(ea & 0xfffffffe);
      break;
    default:
      break;
  }

  return 0;
}

int SimulateRiscv::store(uint32_t opcode)
{
  const int rd = (opcode >> 7) & 0x1f;
  const int rs1 = (opcode >> 15) & 0x1f;
  const int funct3 = (opcode >> 12) & 0x7;
  int imm = (opcode >> 25) & 0x7f;

  imm = (imm << 5) | funct3;

  if ((imm & 0x800) != 0) { imm |= 0xfffff000; }
  uint32_t ea = reg[rs1] + imm;

  switch (funct3)
  {
    case 0:
      memory->write8(ea, reg[rd]);
      break;
    case 1:
      memory->write16(ea & 0xfffffffe, reg[rd]);
      break;
    case 2:
      memory->write32(ea & 0xfffffffc, reg[rd]);
      break;
    default:
      break;
  }

  return 0;
}

int SimulateRiscv::alu(uint32_t opcode)
{
  const int rd = (opcode >> 7) & 0x1f;
  const int rs1 = (opcode >> 15) & 0x1f;
  const int shamt = (opcode >> 20) & 0x1f;
  const int funct3 = (opcode >> 12) & 0x7;
  const int funct7 = (opcode >> 25) & 0x7f;
  int imm = (opcode >> 20) & 0xfff;

  if ((imm & 0x800) != 0) { imm |= 0xfffff000; }

  switch (funct3)
  {
    case 0: reg[rd] = reg[rs1] + imm; break;
    case 1: reg[rd] = reg[rs1] << shamt; break;
    case 2: reg[rd] = reg[rs1] < imm ? 1 : 0; break;
    case 3: reg[rd] = (uint32_t)reg[rs1] < (imm & 0xfff) ? 1 : 0; break;
    case 4: reg[rd] = reg[rs1] ^ imm; break;
    case 5:
      if (funct7 == 0)
      {
        reg[rd] = reg[rs1] >> shamt; break;
      }
        else
      {
        reg[rd] = (uint32_t)reg[rs1] >> shamt; break;
      }
      break;
    case 6: reg[rd] = reg[rs1] | imm; break;
    case 7: reg[rd] = reg[rs1] & imm; break;
  }

  return 0;
}

int SimulateRiscv::alu_reg(uint32_t opcode)
{
  const int rd = (opcode >> 7) & 0x1f;
  const int rs1 = (opcode >> 15) & 0x1f;
  const int rs2 = (opcode >> 20) & 0x1f;
  const int funct7 = (opcode >> 25) & 0x7f;
  const int funct3 = (opcode >> 12) & 0x7;

  switch (funct7)
  {
    case 0x00:
      switch (funct3)
      {
        case 0: reg[rd] = reg[rs1] + reg[rs2]; break;
        case 1: reg[rd] = reg[rs1] << reg[rs2]; break;
        case 2: reg[rd] = reg[rs1] < reg[rs2] ? 1 : 0; break;
        case 3: reg[rd] = reg[rs1] << reg[rs2]; break;
        case 4: reg[rd] = reg[rs1] ^ reg[rs2]; break;
        case 5: reg[rd] = (uint32_t)reg[rs1] << (uint32_t)reg[rs2]; break;
        case 6: reg[rd] = reg[rs1] | reg[rs2]; break;
        case 7: reg[rd] = reg[rs1] & reg[rs2]; break;
      }
    case 0x20:
      switch (funct3)
      {
        case 0: reg[rd] = reg[rs1] - reg[rs2]; break;
        case 2: reg[rd] = (uint32_t)reg[rs1] < (uint32_t)reg[rs2] ? 1 : 0; break;
        case 5: reg[rd] = reg[rs1] >> reg[rs2]; break;
      }
  }

  return 0;
}

