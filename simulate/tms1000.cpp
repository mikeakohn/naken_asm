/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/tms1000.h"
#include "simulate/tms1000.h"
#include "table/tms1000.h"

SimulateTms1000::SimulateTms1000(Memory *memory) : Simulate(memory)
{
  reset();
  org = 0x3c0;
}

SimulateTms1000::~SimulateTms1000()
{
}

Simulate *SimulateTms1000::init(Memory *memory)
{
  return new SimulateTms1000(memory);
}

void SimulateTms1000::reset()
{
  pc = 0x00;
  pb = 0xf;
  pa = 0xf;
  cl = 0x0;
  sr = 0x0;
  s_flag = 0;
  reg_a = 0x0;
  reg_x = 0x0;
  reg_y = 0x0;

  r_pins = 0;
  o_pins = 0;
  k_pins = 0;

  memset(ram, 0, sizeof(ram));
}

void SimulateTms1000::push(uint32_t value)
{
}

int SimulateTms1000::set_reg(const char *reg_string, uint32_t value)
{
  if (strcasecmp(reg_string, "a") == 0)
  {
    reg_a = value;
  }
    else
  if (strcasecmp(reg_string, "x") == 0)
  {
    reg_x = value;
  }
    else
  if (strcasecmp(reg_string, "y") == 0)
  {
    reg_y = value;
  }
    else
  if (strcasecmp(reg_string, "r") == 0)
  {
    r_pins = value;
  }
    else
  if (strcasecmp(reg_string, "o") == 0)
  {
    o_pins = value;
  }
    else
  if (strcasecmp(reg_string, "k") == 0)
  {
    k_pins = value & 0xf;
  }

  return 0;
}

uint32_t SimulateTms1000::get_reg(const char *reg_string)
{
  if (strcasecmp(reg_string, "a") == 0)
  {
    return reg_a;
  }
    else
  if (strcasecmp(reg_string, "x") == 0)
  {
    return reg_x;
  }
    else
  if (strcasecmp(reg_string, "y") == 0)
  {
    return reg_y;
  }
    else
  if (strcasecmp(reg_string, "r") == 0)
  {
    return r_pins;
  }
    else
  if (strcasecmp(reg_string, "o") == 0)
  {
    return o_pins;
  }
    else
  if (strcasecmp(reg_string, "k") == 0)
  {
    return k_pins;
  }

  return 0xffffffff;
}

void SimulateTms1000::set_pc(uint32_t value)
{
  pc = value & 0x3f;
  pa = (value >> 6) & 0xf;
}

void SimulateTms1000::dump_registers()
{
  printf(" REG   I/O     BR     PC\n");

  printf(" a=%x   r=%04x  pb=%x   pa/pc=%x/%02x\n",
    reg_a,
    r_pins,
    pb,
    pa, pc);

  printf(" x=%x   o=%02x    sr=%d   s=%d\n",
    reg_x,
    o_pins,
    sr,
    s_flag);

  printf(" y=%x   k=%x     cl=%d   xy=%02x\n",
    reg_y,
    k_pins,
    cl,
    (reg_x << 4) | reg_y);

  printf("\n");
}

int SimulateTms1000::run(int max_cycles, int step)
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
    opcode = memory->read8((pa << 6) | pc);

    pc_current = pc;
    pc = increment_pc(pc);

    if (show == true) { printf("\x1b[1J\x1b[1;1H"); }

    uint8_t curr_pa = pa;
    uint8_t update_s = 1;
    ret = execute(opcode, update_s);
    s_flag = update_s;
    cycle_count += 6;

    if (show == true)
    {
      dump_registers();
      dump_ram();

      n = 0;

      while (n < 10)
      {
        int cycles_min, cycles_max;
        int num, count;

        num = memory->read8((curr_pa << 6) | pc_current);

        count = disasm_tms1000(
          memory,
          (curr_pa << 6) | tms1000_lsfr_to_address[pc_current],
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        printf("%c", pc_current == break_point ? '*' : ' ');

        if (n == 0)
        {
          printf("! ");
        }
          else
        if (pc_current == pc)
        {
          printf("> ");
        }
          else
        {
          printf("  ");
        }

        printf("%x/%02x: 0x%04x %-40s\n", pa, pc_current, num, instruction);

        n = n + count;
        pc_current = increment_pc(pc_current);
#if 0
        count -= 1;

        while (count > 0)
        {
          if (pc_current == break_point) { printf("*"); }
          else { printf(" "); }

          num = (READ_RAM(pc_current + 1) << 8) | READ_RAM(pc_current);
          printf("  0x%04x: 0x%04x\n", pc_current, num);
          pc_current += 1;
          count -= 2;
        }
#endif
      }
    }

    if (auto_run == 1 && nested_call_count < 0)
    {
      return 0;
    }

    if (ret == -1)
    {
      printf("Illegal instruction 0x%04x at address 0x%04x\n",
        opcode,
        pc_current);

      return -1;
    }

    if (max_cycles != -1 && cycles > max_cycles) { break; }

    printf("\n");

    if (break_point == pc)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if (usec == 0 || step == 1)
    {
      disable_signal_handler();
      return 0;
    }

#if 0
    if (opcode == 0x0f)
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      pc = sr;

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

int SimulateTms1000::execute(uint8_t opcode, uint8_t &update_s)
{
  const int xy = reg_x << 4 | reg_y;

  if ((opcode & 0xfc) == 0x3c)
  {
    // 001111xx ldx [-]

    reg_x = tms1000_reverse_bit_address[opcode & 0x3];
    return 0;
  }

  if ((opcode & 0xf0) == 0x10)
  {
    // 0001xxxx ldp [-]

    pb = tms1000_reverse_constant[opcode & 0xf];
    return 0;
  }

  if ((opcode & 0xf0) == 0x40)
  {
    // 0100xxxx tcy [-]

    reg_y = tms1000_reverse_constant[opcode & 0xf];
    return 0;
  }

  if ((opcode & 0xf0) == 0x50)
  {
    // 0101xxxx ynec [-]
    update_s = ram[xy] != tms1000_reverse_constant[opcode & 0xf] ? 1 : 0;
    return 0;
  }

  if ((opcode & 0xf0) == 0x60)
  {
    // 0110xxxx tcmiy [-]
    ram[xy] = tms1000_reverse_constant[opcode & 0xf];
  }

  if ((opcode & 0xf0) == 0x70)
  {
    // 0111xxxx alec [-]
    update_s = reg_a <= tms1000_reverse_constant[opcode & 0xf] ? 1 : 0;
    return 0;
  }

  if ((opcode & 0xfc) == 0x30)
  {
    // 001100bb sbit [-]
    ram[xy] |= 1 << tms1000_reverse_bit_address[opcode & 0x3];
    return 0;
  }

  if ((opcode & 0xfc) == 0x34)
  {
    // 001101bb rbit [-]
    ram[xy] &= 0xf ^ (1 << tms1000_reverse_bit_address[opcode & 0x3]);
    return 0;
  }

  if ((opcode & 0xfc) == 0x38)
  {
    // 001110bb tbit1 [-]
    int bit = 1 << tms1000_reverse_bit_address[opcode & 0x3];
    update_s = (ram[xy] & bit) == 0 ? 0 : 1;
    return 0;
  }

  if ((opcode & 0xc0) == 0x80)
  {
    // 10xxxxxx br [-]

    if (s_flag == 1)
    {
      if (cl == 0) { pa = pb; }
      pc = opcode & 0x3f;
    }

    return 0;
  }

  if ((opcode & 0xc0) == 0xc0)
  {
    // 11xxxxxx call [-]

    if (s_flag == 1)
    {
      if (cl == 0)
      {
        uint8_t temp = pa;
        sr = pc;
        pa = pb;
        pb = temp;
        cl = 1;
      }
        else
      {
        pa = pb;
      }

      pc = opcode & 0x3f;
    }

    return 0;
  }

  uint8_t temp;

  switch (opcode)
  {
    case 0x00:
    {
      // comx.
      reg_x ^= 0x3;
      break;
    }
    case 0x01:
    {
      // a8aac.
      temp = reg_a + 8;
      reg_a &= 0xf;
      update_s = reg_a != temp ? 1 : 0;
      break;
    }
    case 0x02:
    {
      // ynea.
      update_s = reg_y != reg_a ? 1 : 0;
      break;
    }
    case 0x03:
    {
      // tam.
      ram[xy] = reg_a;
      break;
    }
    case 0x04:
    {
      // tamza.
      ram[xy] = reg_a;
      reg_a = 0;
      break;
    }
    case 0x05:
    {
      // a10aac.
      temp = reg_a + 10;
      reg_a &= 0xf;
      update_s = reg_a != temp ? 1 : 0;
      break;
    }
    case 0x06:
    {
      // a6aac.
      temp = reg_a + 6;
      reg_a &= 0xf;
      update_s = reg_a != temp ? 1 : 0;
      break;
    }
    case 0x07:
    {
      // dan.
      update_s = reg_a >= 1 ? 1 : 0;
      reg_a = (reg_a - 1) & 0xf;
      break;
    }
    case 0x08:
    {
      // tka.
      reg_a = k_pins & 0xf;
      break;
    }
    case 0x09:
    {
      // knez.
      update_s = k_pins != 0 ? 1 : 0;
      break;
    }
    case 0x0a:
    {
      // tdo.
      if (s_flag == 1)
      {
        switch (reg_a)
        {
          case  0: o_pins = 0x7e; break;
          case  1: o_pins = 0x30; break;
          case  2: o_pins = 0x6d; break;
          case  3: o_pins = 0x79; break;
          case  4: o_pins = 0x33; break;
          case  5: o_pins = 0x5b; break;
          case  6: o_pins = 0x5f; break;
          case  7: o_pins = 0x70; break;
          case  8: o_pins = 0x7f; break;
          case  9: o_pins = 0x7b; break;
          case 10: o_pins = 0x77; break;
          case 11: o_pins = 0x1f; break;
          case 12: o_pins = 0x4e; break;
          case 13: o_pins = 0x3d; break;
          case 14: o_pins = 0x4f; break;
          case 15: o_pins = 0x47; break;
        }
      }
        else
      {
        o_pins = reg_a;
      }
      break;
    }
    case 0x0b:
    {
      // clo.
      o_pins = 0;
      break;
    }
    case 0x0c:
    {
      // rstr.
      r_pins &= 0xffff ^ (1 << reg_y);
      break;
    }
    case 0x0d:
    {
      // setr.
      r_pins |= 1 << reg_y;
      break;
    }
    case 0x0e:
    {
      // ia.
      reg_a = (reg_a + 1) & 0xf;
      break;
    }
    case 0x0f:
    {
      // retn.
      if (cl == 1)
      {
        pc = sr;
        cl = 0;
      }

      pa = pb;

      break;
    }
    case 0x20:
    {
      // tamiy.
      ram[xy] = reg_a;
      reg_y = (reg_y + 1) & 0xf;
      break;
    }
    case 0x21:
    {
      // tma.
      reg_a = ram[xy];
      break;
    }
    case 0x22:
    {
      // tmy.
      reg_y = ram[xy];
      break;
    }
    case 0x23:
    {
      // tya.
      reg_a = reg_y;
      break;
    }
    case 0x24:
    {
      // tay.
      reg_y = reg_a;
      break;
    }
    case 0x25:
    {
      // amaac.
      temp = ram[xy] + reg_a;
      reg_a = reg_a & 0xf;
      update_s = temp != reg_a ? 1 : 0;
      break;
    }
    case 0x26:
    {
      // mnez.
      update_s = ram[xy] != 0 ? 1 : 0;
      break;
    }
    case 0x27:
    {
      // saman.
      update_s = reg_a <= ram[xy] ? 1 : 0;
      reg_a = (ram[xy] - reg_a) & 0xf;
      break;
    }
    case 0x28:
    {
      // imac.
      temp = ram[xy] + 1;
      reg_a = temp & 0xf;
      update_s = temp != reg_a ? 1 : 0;
      break;
    }
    case 0x29:
    {
      // alem.
      update_s = reg_a <= ram[xy] ? 1 : 0;
      break;
    }
    case 0x2a:
    {
      // dman.
      temp = ram[xy];
      reg_a = (temp - 1) & 0xf;
      update_s = temp >= 1 ? 1 : 0;
      break;
    }
    case 0x2b:
    {
      // iyc.
      temp = reg_y + 1;
      reg_y = temp & 0xf;
      update_s = temp != reg_y ? 1 : 0;
      break;
    }
    case 0x2c:
    {
      // dyn.
      update_s = reg_y >= 1 ? 1: 0;
      reg_y = (reg_y - 1) & 0xf;
      break;
    }
    case 0x2d:
    {
      // cpaiz.
      reg_a = -reg_a & 0xf;
      update_s = reg_a == 0 ? 1 : 0;
      break;
    }
    case 0x2e:
    {
      // xma.
      temp = ram[xy];
      ram[xy] = reg_a;
      reg_a = temp;
      break;
    }
    case 0x2f:
    {
      // cla.
      reg_a = 0;
      break;
    }
    default:
    {
      //printf("Unknown opcode 0x%02x\n", opcode);
      return -1;
    }
  }

  return 0;
}

int SimulateTms1000::increment_pc(int pc)
{
  int fb;

  if (pc == 0x1f)
  {
    fb = 1;
  }
    else
  if (pc == 0x3f)
  {
    fb = 0;
  }
    else
  {
    int bit5 = (pc >> 5) & 1;
    int bit4 = (pc >> 4) & 1;
    fb = (bit5 ^ bit4) ^ 1;
  }

  pc = (pc << 1 | fb) & 0x3f;

  return pc;
}

void SimulateTms1000::dump_ram()
{
  printf("RAM:");

  for (int i = 0; i < 64; i++)
  {
    if ((i % 16) == 0)
    {
      printf("\n %2x:", i);
    }

    printf(" %x", ram[i]);
  }

  printf("\n\n");
}

