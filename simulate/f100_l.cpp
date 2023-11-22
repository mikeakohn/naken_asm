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
#include <ctype.h>

#include "disasm/f100_l.h"
#include "simulate/f100_l.h"

SimulateF100L::SimulateF100L(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateF100L::~SimulateF100L()
{
}

Simulate *SimulateF100L::init(Memory *memory)
{
  return new SimulateF100L(memory);
}

void SimulateF100L::reset()
{
  memset(&cr, 0, sizeof(cr));

  pc = org;
}

void SimulateF100L::push(uint32_t value)
{
}

int SimulateF100L::set_reg(const char *reg_string, uint32_t value)
{
  if (strcasecmp(reg_string, "a") == 0)
  {
    accum = value;
    return 0;
  }

  if (strcasecmp(reg_string, "cr") == 0)
  {
    cr.value = value;
    return 0;
  }

  if (strcasecmp(reg_string, "lsp") == 0)
  {
    memory->write16(0, value);
    return 0;
  }

  return -1;
}

uint32_t SimulateF100L::get_reg(const char *reg_string)
{
  if (strcasecmp(reg_string, "a") == 0)
  {
    return accum;
  }

  if (strcasecmp(reg_string, "cr") == 0)
  {
    return cr.value;
  }

  if (strcasecmp(reg_string, "lsp") == 0)
  {
    return memory->read16(0);
  }

  return -1;
}

void SimulateF100L::set_pc(uint32_t value)
{
  pc = value * 2;
}

void SimulateF100L::dump_registers()
{
  printf("PC=0x%04x A=%d | F=%d M=%d C=%d S=%d V=%d Z=%d I=%d\n",
    get_pc(),
    memory->read16(0),
    cr.get_f(),
    cr.get_m(),
    cr.get_c(),
    cr.get_s(),
    cr.get_v(),
    cr.get_z(),
    cr.get_i());

  printf("\n");
}

int SimulateF100L::run(int max_cycles, int step)
{
  char instruction[128];
  uint16_t opcode;
  int cycles = 0;
  int ret;
  int pc_current;
  int n;

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == false)
  {
    pc_current = pc;

    opcode = memory->read16(pc_current);

#if 0
    c = get_cycle_count(opcode);

    if (c > 0)  { cycle_count += c; }
#endif

    pc += 2;

    if (show == true) printf("\x1b[1J\x1b[1;1H");

    ret = execute_instruction(opcode);

    if (show == true)
    {
      dump_registers();

      n = 0;

      while (n < 12)
      {
        int cycles_min,cycles_max;
        int num, count;

        num = memory->read8(pc_current);

        count = disasm_f100_l(
          memory,
          pc_current,
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        if (pc_current == break_point * 2) { printf("*"); }
        else { printf(" "); }

        if (n == 0) { printf("! "); }
        else if (pc_current == pc) { printf("> "); }
        else { printf("  "); }

        printf("0x%04x: 0x%04x %-40s\n", pc_current, num, instruction);

        n = n + count;
        pc_current += 2;
        count -= 2;

        while (count > 0)
        {
          if (pc_current == break_point * 2) { printf("*"); }
          else { printf(" "); }

          num = memory->read8(pc_current);
          printf("  0x%04x: 0x%02x\n", pc_current, num);
          pc_current += 1;
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
      printf("Illegal instruction 0x%04x at address 0x%04x\n", opcode, pc_current);
      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    printf("\n");

    if (break_point * 2 == pc)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == 1)
    {
      //step_mode = 0;
      signal(SIGINT, SIG_DFL);
      return 0;
    }

#if 0
    if (reg[0] == 0xffff)
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      pc = memory->read16(0xfffe);

      disable_signal_handler();
      return 0;
    }
#endif

    usleep(usec);
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", pc);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

int SimulateF100L::execute_instruction(uint16_t opcode)
{
  // Check for halt.
  if ((opcode & 0xfc00) == 0x0400)
  {
    stop_running = true;
    return 1;
  }

  int type = opcode >> 12;
  int lsp;

  switch (type)
  {
    case 0:
      bit_ops(opcode);
      break;
    case 1:
      // sjm
      pc += accum;
      break;
    case 3:
      lsp = memory->read16(0) * 2;
      pc = memory->read16(lsp + 2) * 2;
      if (cr.get_m()) { cr.value = memory->read16(lsp + 4); }
      memory->write16(0, lsp / 2);
      break;
    default:
      alu(opcode);
      break;
  };

  return 0;
}

void SimulateF100L::bit_ops(uint16_t opcode)
{
  //int t = (opcode >> 10) & 3;
  int r = (opcode >> 8) & 3;
  int s = (opcode >> 6) & 3;
  int j = (opcode >> 4) & 3;
  int bits = opcode & 0xf;
  int data;
  int ea = 0;

  switch (r)
  {
    case 1: data = accum; break;
    case 3:
      ea = memory->read16(pc);
      pc += 2;
      data = memory->read16(ea * 2);
      break;
    default: data = accum; break;
  }

  switch (s)
  {
    case 0:
    {
      if (!cr.get_m())
      {
      }
        else
      {
        shift_right(data, bits, j);
      }
      break;
    }
    case 1:
    {
      if (!cr.get_m())
      {
      }
        else
      {
        shift_left(data, bits, j);
      }
      break;
    }
#if 0
    case 2:
    {
      break;
    }
#endif
    case 3:
    {
      if (j == 3)
      {
        data &= 0xffff ^ (1 << bits);
      }
        else
      if (j == 2)
      {
        data |= 1 << bits;
      }
      break;
    }
  }

  switch (r)
  {
    case 1: cr.value = data; break;
    case 3: memory->write16(ea * 2, data); break;
    default: accum = data; break;
  }
}

void SimulateF100L::shift_right(int &data, int bits, int j)
{
  switch (j)
  {
    case 2:
      data = data >> bits;
      break;
    case 3:
      for (int n = 0; n < bits; n++)
      {
        int c = data & 1;
        data = (c << 15) | (data >> 1);
      }
    default:
      data = (int16_t)data >> bits;
      break;
  }
}

void SimulateF100L::shift_left(int &data, int bits, int j)
{
  switch (j)
  {
    case 2:
      data = data << bits;
      break;
    case 3:
      for (int n = 0; n < bits; n++)
      {
        int c = (data & 0x8000) != 0 ? 1 : 0;
        data = (data << 1) | c;
      }
    default:
      data = data << bits;
      break;
  }
}

void SimulateF100L::alu(uint16_t opcode)
{
  int type = opcode >> 12;

  int i = (opcode >> 11) & 1;
  int r = (opcode >> 8) & 3;
  int n = (opcode >> 5) & 0x7ff;
  int p = (opcode >> 8) & 0xff;
  int ea = 0;
  int lsp;

  if (i == 0)
  {
    if (n != 0)
    {
      ea = n;
    }
      else
    {
      if (type == OP_JMP) { return; }

      ea = pc;
      pc += 2;
    }
  }
    else
  {
    if (p != 0)
    {
      ea = memory->read16(n * 2);

      if (r == 1)
      {
        ea += 1;
        memory->write16(n * 2, ea);
      }
        else
      if (r == 3)
      {
        memory->write16(n * 2, ea - 1);
      }
    }
      else
    {
      ea = memory->read16(pc);
      pc += 2;
    }
  }

  int data = memory->read16(ea * 2);
  int c = cr.get_m() ? cr.get_c() : 0;

  bool both_neg = (int16_t)data < 0 && (int16_t)accum < 0;
  bool both_pos = (int16_t)data > 0 && (int16_t)accum > 0;
  bool diff_sign = !both_neg && !both_pos;
  int dest = DEST_NONE;

  switch (type)
  {
    case OP_ADD:
      data = data + accum + c;
      dest = DEST_A;
      if (both_neg) { if ((int16_t)data > 0) { cr.set_v(); } }
      if (both_pos) { if ((int16_t)data < 0) { cr.set_v(); } }
      break;
    case OP_ADS:
      data = data + accum + c;
      dest = DEST_EA;
      if (both_neg) { if ((int16_t)data > 0) { cr.set_v(); } }
      if (both_pos) { if ((int16_t)data < 0) { cr.set_v(); } }
      break;
    case OP_AND:
      data = data & accum;
      dest = DEST_A;
      break;
    case OP_CAL:
      dest = DEST_CALL;
      break;
    case OP_CMP:
      if (!cr.get_m())
      {
        data = data - accum;
      }
        else
      {
        data = data - accum + c - 1;
      }
      dest = DEST_NONE;
      break;
    case OP_ICZ:
      data = data + 1; dest = DEST_ICZ;
      break;
    case OP_JMP:
      dest = DEST_PC;
      break;
    case OP_LDA:
      dest = DEST_A;
      break;
    case OP_NEQ:
      data = data ^ accum;
      dest = DEST_A;
      break;
    case OP_SBS:
      if (!cr.get_m())
      {
        data = data - accum;
      }
        else
      {
        data = data - accum + c - 1;
      }
      if (diff_sign && (data & 0x8000) == (accum & 0x8000)) { cr.set_v(); }
      dest = DEST_EA;
      break;
    case OP_STO:
      data = accum;
      dest = DEST_EA;
      break;
    case OP_SUB:
      if (!cr.get_m())
      {
        data = data - accum;
      }
        else
      {
        data = data - accum + c - 1;
      }
      if (diff_sign && (data & 0x8000) == (accum & 0x8000)) { cr.set_v(); }
      dest = DEST_A;
      break;
  }

  cr.value = 0;
  if (data >= 0x10000) { cr.set_c(); }
  if (data == 0) { cr.set_z(); }
  if ((data & 0x8000) == 0x8000) { cr.set_s(); }

  data = data & 0xffff;

  switch (dest)
  {
    case DEST_NONE: break;
    case DEST_A: accum = data; break;
    case DEST_PC: pc = data * 2; break;
    case DEST_EA: memory->write16(ea * 2, data); break;
    case DEST_CALL:
      lsp = memory->read16(0) * 2;
      memory->write16(lsp + 2, pc / 2);
      memory->write16(lsp + 4, cr.value);
      lsp += 4;
      memory->write16(0, lsp / 2);
      pc = ea;
      break;
    case DEST_ICZ:
      ea = memory->read16(pc);
      pc += 2;

      if (cr.get_z())
      {
        pc = ea;
      }
      break;
  }
}

