/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 * Z80 simulation completed by D.L. Karmann
 *
 * Current Z80 simulator support:
 *  - All standard Z80 instructions as described in the below Reference.
 *  - The IN/OUT I/O space is simulated as 256 bytes of read/write memory
 *    (viewable via the 'dumpram' command).
 *  - No IX/IY or any other non-standard half-register Z80 assembler instructions supported.
 *
 * Reference: Zilog Z80 CPU User Manual UM008011-0816 (2016)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "asm/z80.h"
#include "disasm/z80.h"
#include "simulate/z80.h"
#include "table/z80.h"

#define ILLEGAL_INSTRUCTION       -1
#define UNSUPPORTED_INSTRUCTION   -2
#define HALT_INSTRUCTION          -4

#define VFLAG_OVERFLOW    -1
#define VFLAG_PARITY      -2
#define VFLAG_UNCHANGED   -3
#define FLAG_UNCHANGED    -1

#define SIZE_8BITS        8
#define SIZE_16BITS       16

#define SHOW_STACK        work_sp, memory->read8((work_sp+1) & 0xffff), memory->read8(work_sp & 0xffff)

#define READ_RAM(a)       memory->read8(a & 0xffff)
#define READ_RAM16(a)     (memory->read8((a + 1) & 0xffff) << 8) | \
                          memory->read8(a & 0xffff)

#define READ_OPCODE16(a)  (memory->read8(a & 0xffff) << 8) | \
                          memory->read8((a + 1) & 0xffff)

#define WRITE_RAM(a, b) \
                          if ((a) == (uint32_t)break_io) \
                          { \
                            exit(b); \
                          } \
                          memory->write8(a & 0xffff, b)
#define WRITE_RAM16(a,b)  memory->write8((a + 1) & 0xffff, b >> 8); \
                          memory->write8(a & 0xffff, b & 0xff)

#define GET_S() ((reg[REG_F] >> 7) & 1)
#define GET_Z() ((reg[REG_F] >> 6) & 1)
#define GET_X() ((reg[REG_F] >> 5) & 1)
#define GET_H() ((reg[REG_F] >> 4) & 1)
#define GET_Y() ((reg[REG_F] >> 3) & 1)
#define GET_V() ((reg[REG_F] >> 2) & 1)
#define GET_N() ((reg[REG_F] >> 1) & 1)
#define GET_C() ((reg[REG_F] >> 0) & 1)

#define SET_S() reg[REG_F] |= (1 << 7);
#define SET_Z() reg[REG_F] |= (1 << 6);
#define SET_X() reg[REG_F] |= (1 << 5);
#define SET_H() reg[REG_F] |= (1 << 4);
#define SET_Y() reg[REG_F] |= (1 << 3);
#define SET_V() reg[REG_F] |= (1 << 2);
#define SET_N() reg[REG_F] |= (1 << 1);
#define SET_C() reg[REG_F] |= (1 << 0);

#define CLR_S() reg[REG_F] &= (0xff ^ (1 << 7));
#define CLR_Z() reg[REG_F] &= (0xff ^ (1 << 6));
#define CLR_X() reg[REG_F] &= (0xff ^ (1 << 5));
#define CLR_H() reg[REG_F] &= (0xff ^ (1 << 4));
#define CLR_Y() reg[REG_F] &= (0xff ^ (1 << 3));
#define CLR_V() reg[REG_F] &= (0xff ^ (1 << 2));
#define CLR_N() reg[REG_F] &= (0xff ^ (1 << 1));
#define CLR_C() reg[REG_F] &= (0xff ^ (1 << 0));

const char * const SimulateZ80::flags[] = { "S", "Z", "nc", "HC", "nc", "PV", "N", "CY" };

SimulateZ80::SimulateZ80(Memory * memory) : Simulate(memory)
{
  reset();
}

SimulateZ80::~SimulateZ80()
{
  reset();
}

Simulate * SimulateZ80::init(Memory * memory)
{
  return new SimulateZ80(memory);
}

void SimulateZ80::reset()
{
  cycle_count = 0;
  nested_call_count = 0;

  // clear all registers and I/O memory
  memset(reg, 0, sizeof(reg));
  ix = 0;
  iy = 0;
  sp = 0;
  pc = org;
  af_tick = 0;
  bc_tick = 0;
  de_tick = 0;
  hl_tick = 0;
  iff1 = 0;
  iff2 = 0;
  im = 0;
  iv = 0;
  rr = 0;
  memset(io_mem, 0, sizeof(io_mem));
}

void SimulateZ80::push(uint32_t value)
{
  sp -= 2;
  WRITE_RAM16(sp, (uint16_t)value);
}

uint32_t SimulateZ80::pop()
{
  uint32_t value;

  value = READ_RAM16(sp);
  sp += 2;
  return value;
}

// set register or set/clear flag
int SimulateZ80::set_reg(const char * reg_string, uint32_t value)
{
  int reg_item;
  int rslt = 0;

  // skip leading spaces
  while (*reg_string == ' ')
  {
    ++reg_string;
  }

  reg_item = get_reg_id(reg_string);
  if (reg_item == -1)    // might be flags
  {
    for (int n = 0; n < (sizeof(flags) / sizeof(char *)); ++n)
    {
      if (strcasecmp(reg_string, "nc") == 0)
      {
        // unused flag, ignore
      }
      else if (strcasecmp(reg_string, flags[n]) == 0)
      {
        if (value)
        {
          reg[REG_F] |= (1 << (7 - n));
        }
        else
        {
          reg[REG_F] &= (0xffff ^ (1 << (7 - n)));
        }

        reg_item = REG_PC + 1;     // indication command is handled
        break;
      }
    }
  }

  if (reg_item == -1)
  {
    printf("Unknown register/flag '%s'\n", reg_string);
    rslt = -1;    // invalid register
  }

  if (rslt == 0)
  {
    if (reg_item < REG_IV)
    {
      reg[reg_item] = (uint8_t)value;
    }
    else
    {
      switch (reg_item)
      {
        case REG_IV:
          iv = (uint8_t)value;
          break;
        case REG_RR:
          rr = (uint8_t)value;
          break;
        case REG_IFF:
          if (value != 0)
          {
            value = 1;
          }

          iff1 = (uint8_t)value;
          iff2 = (uint8_t)value;
          break;
        case REG_IM:
          if (value <= 2)
          {
            im = (uint8_t)value;
          }
          break;
        case REG_IX:
          ix = (uint16_t)value;
          break;
        case REG_IY:
          iy = (uint16_t)value;
          break;
        case REG_SP:
          sp = (uint16_t)value;
          break;
        case REG_PC:
          set_pc(value);
          break;
        case REG_PC + 1:
          break;        // already handled as a flag
        default:
          rslt = -1;    // invalid register - should not happen
          break;
      }
    }
  }

  return rslt;
}

uint32_t SimulateZ80::get_reg(const char * reg_string)
{
  int reg_id = 0;

  reg_id = get_reg_id(reg_string);

  switch (reg_id)
  {
    case REG_A:
    case REG_F:
    case REG_B:
    case REG_C:
    case REG_D:
    case REG_E:
    case REG_H:
    case REG_L:
      return reg[reg_id & 0x07];
    case REG_IV:
      return iv;
    case REG_RR:
      return rr;
    case REG_IFF:
      return iff1 | iff2;
    case REG_IM:
      return im;
    case REG_IX:
      return ix;
    case REG_IY:
      return iy;
    case REG_SP:
      return sp;
    case REG_PC:
      return pc;
    default:
      return -1;    // not a valid register
  }
}

// get register ID of register string
int SimulateZ80::get_reg_id(const char * reg_string)
{
  char rstr[10] = { 0 };
  int ndx = 0;

  // skip leading spaces
  while (*reg_string == ' ')
  {
    ++reg_string;
  }

  // save to trailing space
  while (*reg_string != ' ')
  {
    if (*reg_string == '\0')
    {
      break;
    }
    rstr[ndx++] = *reg_string;
    ++reg_string;
  }

  // check for Z80 registers
  if (strcasecmp(rstr, "A") == 0)
  {
    return REG_A;
  }
  else if (strcasecmp(rstr, "F") == 0)
  {
    return REG_F;
  }
  else if (strcasecmp(rstr, "B") == 0)
  {
    return REG_B;
  }
  else if (strcasecmp(rstr, "C") == 0)
  {
    return REG_C;
  }
  else if (strcasecmp(rstr, "D") == 0)
  {
    return REG_D;
  }
  else if (strcasecmp(rstr, "E") == 0)
  {
    return REG_E;
  }
  else if (strcasecmp(rstr, "H") == 0)
  {
    return REG_H;
  }
  else if (strcasecmp(rstr, "L") == 0)
  {
    return REG_L;
  }
  else if (strcasecmp(rstr, "I") == 0)
  {
    return REG_IV;
  }
  else if (strcasecmp(rstr, "R") == 0)
  {
    return REG_RR;
  }
  else if (strcasecmp(rstr, "IFF") == 0)
  {
    return REG_IFF;
  }
  else if (strcasecmp(rstr, "IM") == 0)
  {
    return REG_IM;
  }
  else if (strcasecmp(rstr, "IX") == 0)
  {
    return REG_IX;
  }
  else if (strcasecmp(rstr, "IY") == 0)
  {
    return REG_IY;
  }
  else if (strcasecmp(rstr, "SP") == 0)
  {
    return REG_SP;
  }
  else if (strcasecmp(rstr, "PC") == 0)
  {
    return REG_PC;
  }
  else
  {
    return -1;    // not a valid register
  }
}

void SimulateZ80::set_pc(uint32_t value)
{
  pc = (uint16_t)value;
}

// Simulation Register Dump                                    Stack
// -----------------------------------------------------------------------
// Status: 00   S Z nc HC nc PV N CY    IFF  IM   I     R   0xfffe: 0x0000
//              0 0 0  0  0  0  0 0      0   0    00    00 *0x0000: 0x0000
//   A: 00 F: 00   B: 00 C: 00   D: 00 E: 00   H: 00 L: 00  0x0002: 0x0000
//  IX: 0000      IY: 0000      SP: 0000      PC: 0000      0x0004: 0x0000
// AF': 0000     BC': 0000     DE': 0000     HL': 0000      0x0006: 0x0000
void SimulateZ80::dump_registers()
{
  uint16_t work_sp = sp - 2;

  printf("\nSimulation Register Dump                                    Stack\n");
  printf("-----------------------------------------------------------------------\n");

  printf("Status: %02x   ", reg[REG_F]);
  for (int i = 0; i < (sizeof(flags) / sizeof(char *)); ++i)
  {
    printf("%s ", flags[i]);
  }

  printf("   IFF  IM   I     R   0x%04x: 0x%02x%02x\n", SHOW_STACK);
  work_sp += 2;
  printf("             %d %d %d  %d  %d  %d  %d %d      %d   %d    %02x    %02x *0x%04x: 0x%02x%02x\n",
    GET_S(), GET_Z(), GET_X(), GET_H(), GET_Y(), GET_V(), GET_N(), GET_C(),
    (iff1 | iff2), im, iv, rr, SHOW_STACK);
  work_sp += 2;

  printf("  A: %02x F: %02x   B: %02x C: %02x  "
         " D: %02x E: %02x   H: %02x L: %02x  0x%04x: 0x%02x%02x\n",
         reg[REG_A],
         reg[REG_F],
         reg[REG_B],
         reg[REG_C],
         reg[REG_D],
         reg[REG_E],
         reg[REG_H],
         reg[REG_L], SHOW_STACK);
  work_sp += 2;
  printf(" IX: %04x      IY: %04x      SP: %04x      PC: %04x      0x%04x: 0x%02x%02x\n",
         ix, iy, sp, pc, SHOW_STACK);
  work_sp += 2;
  printf("AF': %04x     BC': %04x     DE': %04x     HL': %04x      0x%04x: 0x%02x%02x\n",
         af_tick, bc_tick, de_tick, hl_tick, SHOW_STACK);

  printf("\n");
  printf("%d clock cycles have passed since last reset.\n\n", cycle_count);
}

int SimulateZ80::dump_ram(int start, int end)
{
  int n, count;

  count = 0;
  if (end >= sizeof(io_mem))
  {
    end = sizeof(io_mem) - 1;   // limit IO space
  }

  for (n = start; n <= end; ++n)
  {
    if ((count % 16) == 0)
    {
      printf("\n0x%02x: ", n);
    }

    printf(" %02x", io_mem[n]);
    ++count;
  }

  printf("\n\n");

  return 0;
}

int SimulateZ80::run(int max_cycles, int step)
{
  char instruction[128];
  int cycles = 0;
  int ret;
  int pc_current;

  if (show == true)
  {
    printf("Running... Press Ctl-C to break.\n");
  }

  while (stop_running == false)
  {
    int cycles_min, cycles_max;
    pc_current = pc;

    if (show == true)
    { printf("\x1b[1J\x1b[1;1H"); }

    int count = disasm_z80(
      memory,
      pc_current,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    // Insert code execution
    ret = execute();

    pc += (uint16_t)count;

    if ((ret > 0) || (ret == HALT_INSTRUCTION))
    {
      cycle_count += abs(ret);
    }

    if (show == true)
    {
      dump_registers();
      int disasm_pc = pc_current;

      for (int n = 0; n < 6; ++n)
      {
        int count = disasm_z80(
          memory,
          disasm_pc,
          instruction,
          sizeof(instruction),
          0,
          &cycles_min,
          &cycles_max);

        if (cycles_min == -1)
        { break; }

        // '*' - breakpoint indicator
        // '!' - current instruction indicator
        // '>' - next instruction indicator

        if (disasm_pc == break_point)
        {
            printf("%s", "*");  // breakpoint
        }
        else
        {
            printf("%s", " ");
        }

        if (n == 0)
        {
          printf("! ");         // current instruction
        }
          else
        if (disasm_pc == pc)
        {
          printf("> ");         // next instruction
        }
          else
        {
          printf("  ");
        }

        char hex[32];

        if (count == 1)
        {
          snprintf(hex, sizeof(hex), "%02x         ", READ_RAM(disasm_pc));
        }
          else
        if (count == 2)
        {
          snprintf(hex, sizeof(hex), "%02x %02x      ",
            READ_RAM(disasm_pc),
            READ_RAM(disasm_pc + 1));
        }
          else
        if (count == 3)
        {
          snprintf(hex, sizeof(hex), "%02x %02x %02x   ",
            READ_RAM(disasm_pc),
            READ_RAM(disasm_pc + 1),
            READ_RAM(disasm_pc + 2));
        }
          else
        if (count == 4)
        {
          snprintf(hex, sizeof(hex), "%02x %02x %02x %02x",
            READ_RAM(disasm_pc),
            READ_RAM(disasm_pc + 1),
            READ_RAM(disasm_pc + 2),
            READ_RAM(disasm_pc + 3));
        }
          else
        {
          snprintf(hex, sizeof(hex), "         ???");
        }

        if (cycles_min < 1)
        {
          printf("0x%04x: %s %-40s ?\n", disasm_pc, hex, instruction);
        }
          else
        if (cycles_min == cycles_max)
        {
          printf("0x%04x: %s %-40s %d\n", disasm_pc, hex, instruction, cycles_min);
        }
          else
        {
          printf("0x%04x: %s %-40s %d-%d\n", disasm_pc, hex, instruction, cycles_min, cycles_max);
        }

        disasm_pc += count;
      }
    }

    if ((auto_run == true) && (nested_call_count < 0))
    { return 0; }

    switch (ret)
    {
      case ILLEGAL_INSTRUCTION:
        printf("Illegal instruction at address 0x%04x\n", pc_current);
        return -1;
      case UNSUPPORTED_INSTRUCTION:
        printf("Unsupported simulate instruction at address 0x%04x\n", pc_current);
        return -1;
      case HALT_INSTRUCTION:
        printf("Halted at address 0x%04x\n", pc_current);
        break;
    }

    if (ret == HALT_INSTRUCTION)
    {
      break;
    }

    if ((max_cycles != -1) && (cycles > max_cycles))
    { break; }

    if (break_point == pc)
    {
      printf("Breakpoint hit at 0x%04x\n", break_point);
      break;
    }

    if ((usec == 0) || (step == 1))
    {
      disable_signal_handler();
      return 0;
    }

    if (pc == 0xffff)     //  end of Z80 memory space
    {
      printf("Function ended.  Total cycles: %d\n", cycle_count);
      step_mode = 0;
      pc = READ_RAM16(0xfffe);
      disable_signal_handler();
      return 0;
    }

    usleep(usec);
  }

  disable_signal_handler();

  printf("Stopped.  PC=0x%04x.\n", pc);
  printf("%d clock cycles have passed since last reset.\n", cycle_count);

  return 0;
}

// get BC or DE or HL or SP
int SimulateZ80::get_q(int reg16)
{
  int value = 0;

  switch (reg16)
  {
    case OP_REG_BC:
      value = (reg[REG_B] << 8) | reg[REG_C];
      break;
    case OP_REG_DE:
      value = (reg[REG_D] << 8) | reg[REG_E];
      break;
    case OP_REG_HL:
      value = (reg[REG_H] << 8) | reg[REG_L];
      break;
    case OP_REG_SP:
      value = sp;
      break;
  }

  return value;
}

// get BC or DE or HL or AF
int SimulateZ80::get_p(int reg16)
{
  int value = 0;

  if (reg16 == OP_REG_AF)
  {
    value = (reg[REG_A] << 8) | reg[REG_F];
  }
  else
  {
    value = get_q(reg16);
  }

  return value;
}

// set BC or DE or HL or AF
void SimulateZ80::set_p(int reg16, uint16_t value)
{
  switch (reg16)
  {
    case OP_REG_BC:
      reg[REG_B] = value >> 8;
      reg[REG_C] = value & 0xff;
      break;
    case OP_REG_DE:
      reg[REG_D] = value >> 8;
      reg[REG_E] = value & 0xff;
      break;
    case OP_REG_HL:
      reg[REG_H] = value >> 8;
      reg[REG_L] = value & 0xff;
      break;
    case OP_REG_AF:
      reg[REG_A] = value >> 8;
      reg[REG_F] = value & 0xff;
      break;
  }
}

// set BC or DE or HL or SP
void SimulateZ80::set_q(int reg16, uint16_t value)
{
  if (reg16 == OP_REG_SP)
  {
    sp = value;
  }
  else
  {
    set_p(reg16, value);
  }
}

// set IX or IY
void SimulateZ80::set_xy(int xy, uint16_t value)
{
  if (xy == 0)
  {
    ix = value;
  }
    else
  {
    iy = value;
  }
}

// get IX or IY
uint16_t SimulateZ80::get_xy(int xy)
{
  return xy == 0 ? ix : iy;
}

// set I or R
void SimulateZ80::set_ir(int ir, uint8_t value)
{
  if (ir == 0)
  {
    iv = value;
  }
  else
  {
    rr = value;
  }
}

// get I or R
uint8_t SimulateZ80::get_ir(int ir)
{
  if (iff2 != 0)
  { SET_V(); } else { CLR_V(); }

  if (ir == 0)
  {
    return iv;
  }
  else
  {
    return rr;
  }
}

void SimulateZ80::set_parity(uint8_t val)
{
  int parity;

  // Hmm is 4 bit look-up table better?
  // this calculates odd parity (or parity value to make even parity with a parity bit)
  parity = ((val & 0x01) >> 0) ^
           ((val & 0x02) >> 1) ^
           ((val & 0x04) >> 2) ^
           ((val & 0x08) >> 3) ^
           ((val & 0x10) >> 4) ^
           ((val & 0x20) >> 5) ^
           ((val & 0x40) >> 6) ^
           ((val & 0x80) >> 7);
  parity = ~parity & 1;
  if (parity != 0)
  { SET_V(); } else { CLR_V(); }
}

void SimulateZ80::set_add_subtract(bool neg)
{
  if (neg == true)
  { SET_N(); } else { CLR_N(); }
}

void SimulateZ80::set_zero8(int val)
{
  if ((val & 0xff) == 0)
  { SET_Z(); } else { CLR_Z(); }
}

void SimulateZ80::set_negative8(int val)
{
  if ((val & 0x80) != 0)
  { SET_S(); } else { CLR_S(); }
}

void SimulateZ80::set_half_carry8(int _new, int old, int number, bool neg)
{
  if (neg == true)
  {
    if ((((old & 0x08) == 0) && ((number & 0x08) != 0)) ||
        (((number & 0x08) != 0) && ((_new & 0x08) != 0)) ||
        (((_new & 0x08) != 0) && ((old & 0x08) == 0)))
    { SET_H(); } else { CLR_H(); }
  }
  else
  {
    if ((((old & 0x08) != 0) && ((number & 0x08) != 0)) ||
        (((number & 0x08) != 0) && ((_new & 0x08) == 0)) ||
        (((_new & 0x08) == 0) && ((old & 0x08) != 0)))
    { SET_H(); } else { CLR_H(); }
  }
}

void SimulateZ80::set_carry8(int _new, int old, int number, bool neg)
{
  if (neg == true)
  {
    if ((((old & 0x80) == 0) && ((number & 0x80) != 0)) ||
        (((old & 0x80) == 0) && ((_new & 0x80) != 0)) ||
        (((old & 0x80) != 0) && ((number & 0x80) != 0) && ((_new & 0x80) != 0)))
    { SET_C(); } else { CLR_C(); }
  }
  else
  {
    if ((((old & 0x80) != 0) && ((number & 0x80) != 0)) ||
        (((old & 0x80) != 0) && ((_new & 0x80) == 0)) ||
        (((number & 0x80) != 0) && ((_new & 0x80) == 0)))
    { SET_C(); } else { CLR_C(); }
  }
}

// '_new' is result of op, 'old' is value prior to op, 'number' is value used in op
void SimulateZ80::set_overflow8(int _new, int old, int number, bool neg)
{
  if (neg == true)
  {
    // subtracting operands with different signs can cause overflow
    // subtracting operands with same signs never cause overflow
    if ((old & 0x80) == (number & 0x80))    // same signs
    {
      CLR_V();
    }
    else  // different signs
    {
      if ((_new & 0x80) == (number & 0x80))
      { SET_V(); } else { CLR_V(); }
    }
  }
  else
  {
    // adding operands with different signs never cause overflow
    // adding operands with same signs with the result with a different sign,
    // the Overflow Flag is set
    if ((old & 0x80) != (number & 0x80))    // different signs
    {
      CLR_V();
    }
    else  // same signs
    {
      if (((_new & 0x80) != (old & 0x80)) ||     // result is different sign
          ((_new & 0x80) != (number & 0x80)))    // result is different sign
      { SET_V(); } else { CLR_V(); }
    }
  }
}

void SimulateZ80::set_zero16(int val)
{
  if ((val & 0xffff) == 0)
  { SET_Z(); } else { CLR_Z(); }
}

void SimulateZ80::set_negative16(int val)
{
  if ((val & 0x8000) != 0)
  { SET_S(); } else { CLR_S(); }
}

void SimulateZ80::set_half_carry16(int _new, int old, int number, bool neg)
{
  if (neg == true)
  {
    if ((((old & 0x0800) == 0) && ((number & 0x0800) != 0)) ||
        (((number & 0x0800) != 0) && ((_new & 0x0800) != 0)) ||
        (((_new & 0x0800) != 0) && ((old & 0x0800) == 0)))
    { SET_H(); } else { CLR_H(); }
  }
  else
  {
    if ((((old & 0x0800) != 0) && ((number & 0x0800) != 0)) ||
        (((number & 0x0800) != 0) && ((_new & 0x0800) == 0)) ||
        (((_new & 0x0800) == 0) && ((old & 0x0800) != 0)))
    { SET_H(); } else { CLR_H(); }
  }
}

void SimulateZ80::set_carry16(int _new, int old, int number, bool neg)
{
  if (neg == true)
  {
    if ((((old & 0x8000) == 0) && ((number & 0x8000) != 0)) ||
        (((old & 0x8000) == 0) && ((_new & 0x8000) != 0)) ||
        (((old & 0x8000) != 0) && ((number & 0x8000) != 0) && ((_new & 0x8000) != 0)))
    { SET_C(); } else { CLR_C(); }
  }
  else
  {
    if ((((old & 0x8000) != 0) && ((number & 0x8000) != 0)) ||
        (((old & 0x8000) != 0) && ((_new & 0x8000) == 0)) ||
        (((number & 0x8000) != 0) && ((_new & 0x8000) == 0)))
    { SET_C(); } else { CLR_C(); }
  }
}

void SimulateZ80::set_overflow16(int _new, int old, int number, bool neg)
{
  if (neg == true)
  {
    // subtracting operands with different signs can cause overflow
    // subtracting operands with same signs never cause overflow
    if ((old & 0x8000) == (number & 0x8000))    // same signs
    {
      CLR_V();
    }
    else  // different signs
    {
      if ((_new & 0x8000) == (number & 0x8000))
      { SET_V(); } else { CLR_V(); }
    }
  }
  else
  {
    // adding operands with different signs never cause overflow
    // adding operands with same signs with the result with a different sign,
    // the Overflow Flag is set
    if ((old & 0x8000) != (number & 0x8000))    // different signs
    {
      CLR_V();
    }
    else  // same signs
    {
      if (((_new & 0x8000) != (old & 0x8000)) ||     // result is different sign
          ((_new & 0x8000) != (number & 0x8000)))    // result is different sign
      { SET_V(); } else { CLR_V(); }
    }
  }
}

// Do 'S', 'Z', 'H', 'PV', 'CY' flags
// 'a' is result of op, 'number' is value used in op, 'vflag' is V flag calculation to do
void SimulateZ80::set_flags_a(int a, int number, int8_t vflag, uint8_t instr_enum)
{
  int a0 = reg[REG_A];    // original value of A

  // overflow/parity - (P/V)
  switch (vflag)
  {
    case VFLAG_OVERFLOW:
      switch (instr_enum)
      {
        case Z80_ADC:
        case Z80_ADD:
        case Z80_AND:
        case Z80_OR:
        case Z80_XOR:
          set_overflow8(a, a0, number, false);
          break;
        case Z80_SUB:
        case Z80_SBC:
        case Z80_CP:
        case Z80_NEG:
          set_overflow8(a, a0, number, true);
          break;
      }
      break;
    case VFLAG_PARITY:
      set_parity((uint8_t)a);
      break;
    case VFLAG_UNCHANGED:
      break;
  }

  a &= 0xff;

  // result returned
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_ADD:
    case Z80_SBC:
    case Z80_AND:
    case Z80_OR:
    case Z80_SUB:
    case Z80_XOR:
    case Z80_NEG:
    case Z80_LD:
      reg[REG_A] = (uint8_t)a;
      break;
  }

  // zero - (Z)
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_ADD:
    case Z80_SBC:
    case Z80_AND:
    case Z80_OR:
    case Z80_CP:
    case Z80_SUB:
    case Z80_XOR:
    case Z80_NEG:
    case Z80_CPI:
    case Z80_CPD:
    case Z80_CPIR:
    case Z80_CPDR:
    case Z80_LD:
      set_zero8(a);
      break;
  }

  // negative - (S)
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_ADD:
    case Z80_SBC:
    case Z80_AND:
    case Z80_OR:
    case Z80_CP:
    case Z80_SUB:
    case Z80_XOR:
    case Z80_NEG:
    case Z80_CPI:
    case Z80_CPD:
    case Z80_CPIR:
    case Z80_CPDR:
    case Z80_LD:
      set_negative8(a);
      break;
  }

  // half-carry - (H)
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_ADD:
      set_half_carry8(a, a0, number, false);
      break;
    case Z80_SUB:
    case Z80_SBC:
    case Z80_CP:
    case Z80_NEG:

    case Z80_CPI:
    case Z80_CPD:
    case Z80_CPIR:
    case Z80_CPDR:
      set_half_carry8(a, a0, number, true);
      break;
  }

  // carry - (CY)
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_ADD:
      set_carry8(a, a0, number, false);
      break;
    case Z80_SUB:
    case Z80_SBC:
    case Z80_CP:
      set_carry8(a, a0, number, true);
      break;
  }
}

// Do 'S', 'Z', 'H', 'PV', 'CY' flags
// '_new' is result of op, 'old' is original value of modified item,
// 'number' is value used in op, 'vflag' is V flag calculation to do
void SimulateZ80::set_flags8(int _new, int old, int number, int8_t vflag, uint8_t instr_enum)
{
  // overflow/parity - (P/V)
  switch (vflag)
  {
    case VFLAG_OVERFLOW:
      switch (instr_enum)
      {
        case Z80_INC:
          if (old == 0x7f)
          { SET_V(); } else { CLR_V(); }
          break;
        case Z80_DEC:
          if (old == 0x80)
          { SET_V(); } else { CLR_V(); }
          break;
      }
      break;
    case VFLAG_PARITY:
      set_parity((uint8_t)_new);
      break;
    case VFLAG_UNCHANGED:
      break;
  }

  // zero - (Z)
  switch (instr_enum)
  {
    case Z80_INC:
    case Z80_DEC:
    case Z80_RL:
    case Z80_RLC:
    case Z80_RR:
    case Z80_RRC:
    case Z80_SLA:
    case Z80_SRA:
    case Z80_SLL:
    case Z80_SRL:
    case Z80_RLD:
    case Z80_RRD:
    case Z80_IN:
      set_zero8(_new);
      break;
  }

  // negative - (S)
  switch (instr_enum)
  {
    case Z80_INC:
    case Z80_DEC:
    case Z80_RL:
    case Z80_RLC:
    case Z80_RR:
    case Z80_RRC:
    case Z80_SLA:
    case Z80_SRA:
    case Z80_SLL:
    case Z80_SRL:
    case Z80_RLD:
    case Z80_RRD:
    case Z80_IN:
      set_negative8(_new);
      break;
  }

  // half-carry - (H)
  switch (instr_enum)
  {
    case Z80_INC:
      set_half_carry8(_new, old, number, false);
      break;
    case Z80_DEC:
      set_half_carry8(_new, old, number, true);
      break;
  }

  // carry - (CY)  {no 8-bit non-A registers instructions actually affect CY flag
  switch (instr_enum)
  {
    case 998:
      set_carry8(_new, old, number, false);
      break;
    case 999:
      set_carry8(_new, old, number, true);
      break;
  }
}

// '_new' is result of op, 'old' is original value of modified item,
// 'number' is value used in op, 'vflag' is V flag calculation to do
void SimulateZ80::set_flags16(int _new, int old, int number, int8_t vflag, uint8_t instr_enum)
{
  // overflow/parity - (P/V)
  switch (vflag)
  {
    case VFLAG_OVERFLOW:
      switch (instr_enum)
      {
        case Z80_ADC:
        case Z80_SBC:
          set_overflow16(_new, old, number, false);
          break;
      }
      break;
    case VFLAG_PARITY:
      break;                // unsupported/unneeded
    case VFLAG_UNCHANGED:
      break;
  }

  // zero - (Z)
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_SBC:
      set_zero16(_new);
      break;
  }

  // negative - (S)
  switch (instr_enum)
  {
    case Z80_ADC:
    case Z80_SBC:
      set_negative16(_new);
      break;
  }

  // half-carry - (H)
  switch (instr_enum)
  {
    case Z80_ADD:
    case Z80_ADC:
      set_half_carry16(_new, old, number, false);
      break;
    case Z80_SBC:
      set_half_carry16(_new, old, number, true);
      break;
  }

  // carry - (CY)
  switch (instr_enum)
  {
    case Z80_ADD:
    case Z80_ADC:
      set_carry16(_new, old, number, false);
      break;
    case Z80_SBC:
      set_carry16(_new, old, number, true);
      break;
  }
}

int SimulateZ80::daa(uint8_t instr_enum)
{
  int tmp;
  int rslt;

  switch (instr_enum)
  {
    case  Z80_DAA:
      tmp = reg[REG_A];
      rslt = tmp;
      if (GET_N() == 1)   // from prior sub
      {
        if (((GET_C() == 1) || (tmp > 0x99)) &&
            ((GET_H() == 1) || ((tmp & 0x0f) > 9)))
        {
          rslt -= 0x66;
          SET_C();
        }
        else if ((GET_C() == 1) || (tmp > 0x99))
        {
          rslt -= 0x60;
          SET_C();
        }
        else if ((GET_H() == 1) || ((tmp & 0x0f) > 9))
        {
          rslt -= 0x06;
          CLR_C();
        }
        else
        {
          CLR_C();
        }
      }
      else  // from prior add
      {
        if (((GET_C() == 1) || (tmp > 0x99)) &&
            ((GET_H() == 1) || ((tmp & 0x0f) > 9)))
        {
          rslt += 0x66;
          SET_C();
        }
        else if ((GET_C() == 1) || (tmp > 0x99))
        {
          rslt += 0x60;
          SET_C();
        }
        else if ((GET_H() == 1) || ((tmp & 0x0f) > 9))
        {
          rslt += 0x06;
          CLR_C();
        }
        else
        {
          CLR_C();
        }
      }

      if ((rslt & 0x80) != 0)
      {
        SET_S();
      }
      else
      {
        CLR_S();
      }

      if ((rslt & 0xff) == 0)
      {
        SET_Z();
      }
      else
      {
        CLR_Z();
      }

      set_parity((uint8_t)rslt);

      if ((GET_N() == 0) && ((tmp & 0x0f) >= 0x0a))
      {
        SET_H();
      }
      else
      {
        CLR_H();
      }

      reg[REG_A] = (uint8_t)rslt;
      return 1;
  }

  return ILLEGAL_INSTRUCTION;
}

int SimulateZ80::execute_op_none(struct _table_z80 * table_z80_item)
{
  int tmp;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case  Z80_CCF:
      if (GET_C() == 1)
      { SET_H(); } else { CLR_H(); }

      if (GET_C() == 0)
      { SET_C(); } else { CLR_C(); }

      CLR_N();
      return cycles;
    case  Z80_CPL:
      reg[REG_A] = ~reg[REG_A];
      SET_H();
      SET_N();
      return cycles;

    case  Z80_DAA:
      tmp =  daa(table_z80_item->instr_enum);
      if (tmp == 1)   // OK
      {
        return cycles;
      }
      else
      {
        return tmp;
      }

    case  Z80_DI:
      iff1 = 0;
      iff2 = 0;
      return cycles;
    case  Z80_EI:
      iff1 = 1;
      iff2 = 1;
      return cycles;

    case  Z80_EX:
      tmp = get_p(OP_REG_AF);
      set_p(OP_REG_AF, af_tick);
      af_tick = (uint16_t)tmp;
      return cycles;
    case  Z80_EXX:
      tmp = get_q(OP_REG_BC);
      set_q(OP_REG_BC, bc_tick);
      bc_tick = (uint16_t)tmp;
      tmp = get_q(OP_REG_DE);
      set_q(OP_REG_DE, de_tick);
      de_tick = (uint16_t)tmp;
      tmp = get_q(OP_REG_HL);
      set_q(OP_REG_HL, hl_tick);
      hl_tick = (uint16_t)tmp;
      return cycles;

    case  Z80_HALT:
      return HALT_INSTRUCTION;

    case  Z80_NOP:
      return cycles;
    case  Z80_RET:
      tmp = pop();
      set_pc(tmp - 1);
      return cycles;

    case  Z80_RLA:
      tmp = reg[REG_A] & 0x80;
      reg[REG_A] <<= 1;
      reg[REG_A] |= GET_C();
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      CLR_H();
      CLR_N();
      return cycles;
    case  Z80_RLCA:
      tmp = ((reg[REG_A] >> 7) & 1);
      if ((reg[REG_A] & 0x80) == 0)
      { CLR_C(); } else { SET_C(); }

      reg[REG_A] <<= 1;
      reg[REG_A] |= tmp;
      CLR_H();
      CLR_N();
      return cycles;
    case  Z80_RRA:
      tmp = reg[REG_A] & 1;
      reg[REG_A] >>= 1;
      reg[REG_A] |= (GET_C() << 7);
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      CLR_H();
      CLR_N();
      return cycles;
    case  Z80_RRCA:
      if ((reg[REG_A] & 1) == 0)
      { CLR_C(); } else { SET_C(); }

      reg[REG_A] >>= 1;
      reg[REG_A] |= (GET_C() << 7);
      CLR_H();
      CLR_N();
      return cycles;
    case  Z80_SCF:
      CLR_H();
      CLR_N();
      SET_C();
      return cycles;
  }

  return ILLEGAL_INSTRUCTION;
}

int SimulateZ80::execute_op_cond_none(struct _table_z80 * table_z80_item, uint16_t opcode)
{
  int type = (opcode >> 3) & 0x07;
  int tmp;
  bool ret = false;

  switch (table_z80_item->instr_enum)
  {
    case  Z80_RET:
      switch (type)
      {
        case CC_NZ:
          if (!GET_Z())
          {
            ret = true;
          }
          break;
        case CC_Z:
          if (GET_Z())
          {
            ret = true;
          }
         break;
        case CC_NC:
          if (!GET_C())
          {
            ret = true;
          }
          break;
        case CC_C:
          if (GET_C())
          {
            ret = true;
          }
          break;
        case CC_PO:
          if (!GET_V())
          {
            ret = true;
          }
          break;
        case CC_PE:
          if (GET_V())
          {
            ret = true;
          }
         break;
        case CC_P:
          if (!GET_S())
          {
            ret = true;
          }
          break;
        case CC_M:
          if (GET_S())
          {
            ret = true;
          }
          break;
      }

      if (ret)
      {
        tmp = pop();
        set_pc(tmp - 1);
        --nested_call_count;
        return table_z80_item->cycles_max;
      }

      return table_z80_item->cycles_min;
  }

  return ILLEGAL_INSTRUCTION;
}

int SimulateZ80::execute_op_a_reg8(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int a = reg[REG_A];
  int rrr = opcode & 0x7;
  int number = reg[rrr];

  switch (table_z80_item->instr_enum)
  {
    case Z80_ADC:
      a += number + GET_C();
      CLR_N();
      break;
    case Z80_ADD:
      a += number;
      CLR_N();
      break;
    case Z80_SBC:
      a -= number + GET_C();
      SET_N();
      break;
  }

  set_flags_a(a, number, VFLAG_OVERFLOW, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_reg8(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int a = reg[REG_A];
  int rrr = opcode & 0x7;
  int number = reg[rrr];
  int8_t vflags = VFLAG_OVERFLOW;

  switch (table_z80_item->instr_enum)
  {
    case Z80_AND:
      a &= number;
      SET_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_CP:
      a -= number;
      SET_N();
      break;
    case Z80_OR:
      a |= number;
      CLR_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_SUB:
      a -= number;
      SET_N();
      break;
    case Z80_XOR:
      a ^= number;
      vflags = VFLAG_PARITY;
      CLR_H();
      CLR_N();
      CLR_C();
      break;
  }

  set_flags_a(a, number, vflags, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_reg8_cb(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int rrr = opcode16 & 0x7;
  uint8_t number = reg[rrr];
  uint8_t data = 0;
  uint8_t tmp;

  switch (table_z80_item->instr_enum)
  {
    case Z80_RL:
      tmp = number & 0x80;
      data = number <<= 1;
      data |= GET_C();
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }
      break;
    case Z80_RLC:
      if ((number & 0x80) == 0)
      { CLR_C(); } else { SET_C(); }

      data = number << 1;
      data |= ((number >> 7) & 1);
      break;
    case Z80_RR:
      tmp = number & 1;
      data = number >> 1;
      data |= (GET_C() << 7);
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }
      break;
    case Z80_RRC:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      data = number >> 1;
      data |= (tmp << 7);
      break;
    case Z80_SLA:
      tmp = number & 0x80;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      data = number << 1;
      break;
    case Z80_SRA:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      tmp = number & 0x80;
      data = number >> 1;
      data |= tmp;
      break;
    case Z80_SLL:   // undocumented - similar to SLA but always sets LSb
      tmp = number & 0x80;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      data = number << 1;
      data |= 1;
      break;
    case Z80_SRL:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      data = number >> 1;
      break;
  }

  reg[rrr] = data;
  CLR_H();
  CLR_N();

  set_flags8(data, number, 0, VFLAG_PARITY, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_a_number8(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int a = reg[REG_A];
  int number = opcode16 & 0xff;

  switch (table_z80_item->instr_enum)
  {
    case Z80_ADC:
      a += number + GET_C();
      CLR_N();
      break;
    case Z80_ADD:
      a += number;
      CLR_N();
      break;
    case Z80_SBC:
      a -= (number + GET_C());
      SET_N();
      break;
  }

  set_flags_a(a, number, VFLAG_OVERFLOW, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_number8(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int a = reg[REG_A];
  int number = opcode16 & 0xff;
  int8_t vflags = VFLAG_OVERFLOW;

  switch (table_z80_item->instr_enum)
  {
    case Z80_AND:
      a &= number;
      SET_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_CP:
      a -= number;
      SET_N();
      break;
    case Z80_OR:
      a |= number;
      CLR_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_SUB:
      a -= number;
      SET_N();
      break;
    case Z80_XOR:
      a ^= number;
      CLR_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
  }

  set_flags_a(a, number, vflags, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_offset8(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int number = opcode16 & 0xff;
  uint8_t cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_DJNZ:
      --reg[REG_B];
      if (reg[REG_B] != 0)
      {
        pc += (int8_t)number;
        cycles = table_z80_item->cycles_max;
      }
      break;
    case Z80_JR:
      pc += (int8_t)number;
      break;
  }

  return cycles;
}

int SimulateZ80::execute_op_cond_offset8(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int type = (opcode16 >> 11) & 0x03;
  int number = opcode16 & 0xff;
  uint8_t cycles = table_z80_item->cycles_min;
  bool jmp = false;

  switch (table_z80_item->instr_enum)
  {
    case Z80_JR:
      switch (type)
      {
        case CC_NZ:
          if (!GET_Z())
          {
            jmp = true;
          }
          break;
        case CC_Z:
          if (GET_Z())
          {
            jmp = true;
          }
         break;
        case CC_NC:
          if (!GET_C())
          {
            jmp = true;
          }
          break;
        case CC_C:
          if (GET_C())
          {
            jmp = true;
          }
          break;
      }
      break;
  }

  if (jmp)
  {
    pc += (int8_t)number;
    cycles = table_z80_item->cycles_max;
  }

  return cycles;
}

int SimulateZ80::execute_op_reg8_v2(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int rrr = (opcode >> 3) & 0x7;
  int old = reg[rrr];
  int number = 1;

  switch (table_z80_item->instr_enum)
  {
    case Z80_DEC:
      --reg[rrr];
      SET_N();
      break;
    case Z80_INC:
      ++reg[rrr];
      CLR_N();
      break;
  }

  set_flags8(reg[rrr], old, number, VFLAG_OVERFLOW, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_reg8_index_hl(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int rrr = (opcode >> 3) & 0x7;
  uint8_t tmp;

  // r = *HL
  tmp = READ_RAM(get_q(OP_REG_HL));
  reg[rrr] = tmp;
  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_hl_reg8(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int rrr = opcode & 0x7;
  int tmp;

  // *HL = r
  tmp = get_q(OP_REG_HL);
  WRITE_RAM(tmp, reg[rrr]);
  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_hl(struct _table_z80 * table_z80_item)
{
  int a = reg[REG_A];
  int tmp = 0;
  int addr;
  int8_t vflags = VFLAG_OVERFLOW;

  switch (table_z80_item->instr_enum)
  {
    case Z80_JP:        // PC = HL
      tmp = get_q(OP_REG_HL);
      set_pc(tmp - 1);
      return table_z80_item->cycles_min;
    case Z80_INC:       // *HL++
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a = tmp + 1;
      WRITE_RAM(addr, a);
      CLR_N();
      set_flags8(a, tmp, 1, vflags, table_z80_item->instr_enum);
      return table_z80_item->cycles_min;
    case Z80_DEC:       // *HL--
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a = tmp - 1;
      WRITE_RAM(addr, a);
      SET_N();
      set_flags8(a, tmp, 1, vflags, table_z80_item->instr_enum);
      return table_z80_item->cycles_min;

    case Z80_SUB:       // A -= *HL
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a -= tmp;
      SET_N();
      break;
    case Z80_AND:       // A &= *HL
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a &= tmp;
      SET_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_XOR:       // A ^= *HL
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a ^= tmp;
      CLR_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_OR:        // A |= *HL
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a |= tmp;
      CLR_H();
      CLR_N();
      CLR_C();
      vflags = VFLAG_PARITY;
      break;
    case Z80_CP:        // flags = A - *HL
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a -= tmp;
      SET_N();
      break;
  }

  set_flags_a(a, tmp, vflags, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_a_index_hl(struct _table_z80 * table_z80_item)
{
  int a = reg[REG_A];
  int tmp = 0;
  int addr;
  int8_t vflags = VFLAG_OVERFLOW;

  switch (table_z80_item->instr_enum)
  {
    case Z80_ADC:       // A += (*HL + CY)
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a += (tmp + GET_C());
      CLR_N();
      break;
    case Z80_ADD:       // A += *HL
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a += tmp;
      CLR_N();
      break;
    case Z80_SBC:       // A -= (*HL + CY)
      addr = get_q(OP_REG_HL);
      tmp = READ_RAM(addr);
      a -= (tmp + GET_C());
      SET_N();
      break;
  }

  set_flags_a(a, tmp, vflags, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_hl_cb(struct _table_z80 * table_z80_item)
{
  int addr;
  int number;
  int tmp;
  int val = 0;

  addr = get_q(OP_REG_HL);
  number = READ_RAM(addr);

  switch (table_z80_item->instr_enum)
  {
    case Z80_RL:
      tmp = number & 0x80;
      val = number <<= 1;
      val |= GET_C();
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }
      break;
    case Z80_RLC:
      if ((number & 0x80) == 0)
      { CLR_C(); } else { SET_C(); }

      val = number << 1;
      val |= ((number >> 7) & 1);
      break;
    case Z80_RR:
      tmp = number & 1;
      val = number >> 1;
      val |= (GET_C() << 7);
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }
      break;
    case Z80_RRC:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number >> 1;
      val |= (tmp << 7);
      break;
    case Z80_SLA:
      tmp = number & 0x80;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number << 1;
      break;
    case Z80_SRA:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      tmp = number & 0x80;
      val = number >> 1;
      val |= tmp;
      break;
    case Z80_SLL:   // undocumented - similar to SLA but always sets LSb
      tmp = number & 0x80;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number << 1;
      val |= 1;
      break;
    case Z80_SRL:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number >> 1;
      break;
  }

  WRITE_RAM(addr, val);
  CLR_H();
  CLR_N();

  set_flags8(val, number, 0, VFLAG_PARITY, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_long(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int xy = (opcode >> 5) & 0x1;
  int addr;
  int number = 0;
  int tmp;
  int val = 0;
  int8_t offset;

  addr = get_xy(xy);
  offset = READ_RAM(pc + 2);
  number = READ_RAM(addr + offset);

  switch (table_z80_item->instr_enum)
  {
    case Z80_RL:
      tmp = number & 0x80;
      val = number <<= 1;
      val |= GET_C();
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }
      break;
    case Z80_RLC:
      if ((number & 0x80) == 0)
      { CLR_C(); } else { SET_C(); }

      val = number << 1;
      val |= ((number >> 7) & 1);
      break;
    case Z80_RR:
      tmp = number & 1;
      val = number >> 1;
      val |= (GET_C() << 7);
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }
      break;
    case Z80_RRC:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number >> 1;
      val |= (tmp << 7);
      break;
    case Z80_SLA:
      tmp = number & 0x80;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number << 1;
      break;
    case Z80_SRA:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      tmp = number & 0x80;
      val = number >> 1;
      val |= tmp;
      break;
    case Z80_SLL:   // undocumented - similar to SLA but always sets LSb
      tmp = number & 0x80;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number << 1;
      val |= 1;
      break;
    case Z80_SRL:
      tmp = number & 1;
      if (tmp == 0)
      { CLR_C(); } else { SET_C(); }

      val = number >> 1;
      break;
  }

  WRITE_RAM(addr + offset, val);
  CLR_H();
  CLR_N();

  set_flags8(val, number, 0, VFLAG_PARITY, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_bit_reg8(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int bit = (opcode16 >> 3) & 0x7;
  int rrr = opcode16 & 0x7;
  uint8_t data = reg[rrr];
  uint8_t cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_BIT:
      SET_H();
      CLR_N();
      if ((data & (1 << bit)) == 0)
      { SET_Z(); } else { CLR_Z(); }
      break;
    case Z80_SET:
      data |= (1 << bit);    // set bit
      reg[rrr] = data;
      break;
    case Z80_RES:
      data &= ~(1 << bit);   // clear bit
      reg[rrr] = data;
      break;
  }

  return cycles;
}

int SimulateZ80::execute_op_bit_reg8_index_hl(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  uint16_t addr16;
  int bit = (opcode16 >> 3) & 0x7;
  int data;
  uint8_t cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_BIT:
      addr16 = get_q(OP_REG_HL);
      data = READ_RAM(addr16);
      cycles = table_z80_item->cycles_max;
      SET_H();
      CLR_N();
      if ((data & (1 << bit)) == 0)
      { SET_Z(); } else { CLR_Z(); }
      break;
    case Z80_SET:
      addr16 = get_q(OP_REG_HL);
      data = READ_RAM(addr16);
      data |= (1 << bit);    // set bit
      WRITE_RAM(addr16, data);
      cycles = table_z80_item->cycles_max;
      break;
    case Z80_RES:
      addr16 = get_q(OP_REG_HL);
      data = READ_RAM(addr16);
      data &= ~(1 << bit);   // clear bit
      WRITE_RAM(addr16, data);
      cycles = table_z80_item->cycles_max;
      break;
  }

  return cycles;
}

int SimulateZ80::execute_op_bit_index(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int xy = (opcode >> 5) & 0x1;
  int last_byte = READ_RAM(pc + 3);
  int bit = (last_byte >> 3) & 0x7;
  int addr;
  int number = 0;
  int8_t offset;
  uint8_t cycles = table_z80_item->cycles_min;
  uint8_t instr_enum;

  addr = get_xy(xy);
  offset = READ_RAM(pc + 2);
  number = READ_RAM(addr + offset);

  switch (last_byte & 0xc7)
  {
    case 0x46:
      instr_enum = Z80_BIT;
      break;
    case 0xc6:
      instr_enum = Z80_SET;
      break;
    case 0x86:
      instr_enum = Z80_RES;
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  switch (instr_enum)
  {
    case Z80_BIT:
      SET_H();
      CLR_N();
      if ((number & (1 << bit)) == 0)
      { SET_Z(); } else { CLR_Z(); }
      break;
    case Z80_SET:
      number |= (1 << bit);    // set bit
      cycles = table_z80_item->cycles_max;
      break;
    case Z80_RES:
      number &= ~(1 << bit);   // clear bit
      cycles = table_z80_item->cycles_max;
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  WRITE_RAM(addr + offset, number);
  return cycles;
}

int SimulateZ80::ldi_ldd(struct _table_z80 * table_z80_item)
{
  uint16_t de, hl, bc;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_LDIR:      // *DE++ = *HL++ while --BC != 0
      cycles = table_z80_item->cycles_max;
      de = get_q(OP_REG_DE);
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      if (bc == 0)
      {
        WRITE_RAM(de, READ_RAM(hl));
        ++de;
        ++hl;
        --bc;
        cycles = table_z80_item->cycles_min;
      }

      while (bc != 0)
      {
        WRITE_RAM(de, READ_RAM(hl));
        ++de;
        ++hl;
        --bc;
      }

      set_q(OP_REG_DE, de);
      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      CLR_V();
      CLR_H();
      CLR_N();
      break;

    case Z80_LDI:       // *DE++ = *HL++, --BC
      de = get_q(OP_REG_DE);
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      WRITE_RAM(de, READ_RAM(hl));
      ++de;
      ++hl;
      --bc;
      set_q(OP_REG_DE, de);
      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      CLR_H();
      CLR_N();
      if (bc == 0)
      { CLR_V(); } else { SET_V(); }
      break;

    case Z80_LDD:       // *DE-- = *HL--, --BC
      de = get_q(OP_REG_DE);
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      WRITE_RAM(de, READ_RAM(hl));
      --de;
      --hl;
      --bc;
      set_q(OP_REG_DE, de);
      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      CLR_H();
      CLR_N();
      if (bc == 0)
      { CLR_V(); } else { SET_V(); }
      break;

    case Z80_LDDR:      // *DE-- = *HL-- while --BC != 0
      cycles = table_z80_item->cycles_max;
      de = get_q(OP_REG_DE);
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      if (bc == 0)
      {
        WRITE_RAM(de, READ_RAM(hl));
        --de;
        --hl;
        --bc;
        cycles = table_z80_item->cycles_min;
      }

      while (bc != 0)
      {
        WRITE_RAM(de, READ_RAM(hl));
        --de;
        --hl;
        --bc;
      }

      set_q(OP_REG_DE, de);
      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      CLR_V();
      CLR_H();
      CLR_N();
      break;
  }

  return cycles;
}

int SimulateZ80::cpi_cpd(struct _table_z80 * table_z80_item)
{
  uint16_t hl, bc;
  int cycles = table_z80_item->cycles_min;
  int a = 0;
  int tmp = 0;
  bool mflag;

  switch (table_z80_item->instr_enum)
  {
    case Z80_CPIR:      // flags = A - *HL++, while --BC != 0
      cycles = table_z80_item->cycles_max;
      mflag = false;
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      if (bc == 0)
      {
        tmp = READ_RAM(hl);
        a = reg[REG_A] - tmp;
        ++hl;
        --bc;
        cycles = table_z80_item->cycles_min;
        if (a == 0)     // match
        {
          mflag = true;
        }
      }

      if (!mflag)
      {
        while (bc != 0)
        {
          tmp = READ_RAM(hl);
          a = reg[REG_A] - tmp;
          ++hl;
          --bc;
          if (a == 0)     // match
          {
            break;
          }
        }
      }

      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      set_flags_a(a, tmp, VFLAG_UNCHANGED, table_z80_item->instr_enum);
      SET_N();
      if (bc == 0)
      { CLR_V(); } else { SET_V(); }
      break;

    case Z80_CPI:       // flags = A - *HL++, --BC
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      tmp = READ_RAM(hl);
      a = reg[REG_A] - tmp;
      ++hl;
      --bc;
      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      set_flags_a(a, tmp, VFLAG_UNCHANGED, table_z80_item->instr_enum);
      SET_N();
      if (bc == 0)
      { CLR_V(); } else { SET_V(); }
      break;

    case Z80_CPD:       // flags = A - *HL--, --BC
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      tmp = READ_RAM(hl);
      a = reg[REG_A] - tmp;
      --hl;
      --bc;
      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      set_flags_a(a, tmp, VFLAG_UNCHANGED, table_z80_item->instr_enum);
      SET_N();
      if (bc == 0)
      { CLR_V(); } else { SET_V(); }
      break;

    case Z80_CPDR:      // flags = A - *HL--, while --BC != 0
      cycles = table_z80_item->cycles_max;
      mflag = false;
      hl = get_q(OP_REG_HL);
      bc = get_q(OP_REG_BC);
      if (bc == 0)
      {
        tmp = READ_RAM(hl);
        a = reg[REG_A] - tmp;
        --hl;
        --bc;
        cycles = table_z80_item->cycles_min;
        if (a == 0)     // match
        {
          mflag = true;
        }
      }

      if (!mflag)
      {
        while (bc != 0)
        {
          tmp = READ_RAM(hl);
          a = reg[REG_A] - tmp;
          --hl;
          --bc;
          if (a == 0)     // match
          {
            break;
          }
        }
      }

      set_q(OP_REG_HL, hl);
      set_q(OP_REG_BC, bc);
      set_flags_a(a, tmp, VFLAG_UNCHANGED, table_z80_item->instr_enum);
      SET_N();
      if (bc == 0)
      { CLR_V(); } else { SET_V(); }
      break;
  }

  return cycles;
}

int SimulateZ80::outi_outd(struct _table_z80 * table_z80_item)
{
  uint16_t hl;
  uint8_t hl_val;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case  Z80_OUTI:     // OUT(C) = *HL++, --B
      hl = get_q(OP_REG_HL);
      hl_val = READ_RAM(hl);
      io_mem[reg[REG_C]] = hl_val;
      ++hl;
      --reg[REG_B];
      set_q(OP_REG_HL, hl);
      SET_N();
      if (reg[REG_B] == 0)
      { SET_Z(); } else { CLR_Z(); }
      break;

    case  Z80_OUTD:     // OUT(C) = *HL--, --B
      hl = get_q(OP_REG_HL);
      hl_val = READ_RAM(hl);
      io_mem[reg[REG_C]] = hl_val;
      --hl;
      --reg[REG_B];
      set_q(OP_REG_HL, hl);
      SET_N();
      if (reg[REG_B] == 0)
      { SET_Z(); } else { CLR_Z(); }
      break;

    case  Z80_OTIR:     // OUT(C) = *HL++ while --B != 0
      hl = get_q(OP_REG_HL);
      if (reg[REG_B] == 0)
      {
        hl_val = READ_RAM(hl);
        io_mem[reg[REG_C]] = hl_val;
        ++hl;
      }

      while (1)
      {
        hl_val = READ_RAM(hl);
        io_mem[reg[REG_C]] = hl_val;
        ++hl;
        if (--reg[REG_B] == 0)
        { break; }
      }

      set_q(OP_REG_HL, hl);
      SET_N();
      SET_Z();
      break;

    case  Z80_OTDR:     // OUT(C) = *HL-- while --B != 0
      hl = get_q(OP_REG_HL);
      if (reg[REG_B] == 0)
      {
        hl_val = READ_RAM(hl);
        io_mem[reg[REG_C]] = hl_val;
        --hl;
      }

      while (1)
      {
        hl_val = READ_RAM(hl);
        io_mem[reg[REG_C]] = hl_val;
        --hl;
        if (--reg[REG_B] == 0)
        { break; }
      }

      set_q(OP_REG_HL, hl);
      SET_N();
      SET_Z();
      break;
  }

  return cycles;
}

int SimulateZ80::ini_ind(struct _table_z80 * table_z80_item)
{
  uint16_t hl;
  int hl_val;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case  Z80_INI:      // *HL++ = IN(C), --B
      hl = get_q(OP_REG_HL);
      --reg[REG_B];
      if (reg[REG_B] == 0)
      { SET_Z(); } else { CLR_Z(); }

      hl_val = io_mem[reg[REG_C]];
      WRITE_RAM(hl, hl_val);
      ++hl;
      set_q(OP_REG_HL, hl);
      SET_N();
      break;

    case  Z80_IND:      // *HL-- = IN(C), --B
      hl = get_q(OP_REG_HL);
      --reg[REG_B];
      if (reg[REG_B] == 0)
      { SET_Z(); } else { CLR_Z(); }

      hl_val = io_mem[reg[REG_C]];
      WRITE_RAM(hl, hl_val);
      --hl;
      set_q(OP_REG_HL, hl);
      SET_N();
      break;

    case  Z80_INIR:     // *HL++ = IN(C) while --B != 0
      hl = get_q(OP_REG_HL);
      while (1)
      {
        hl_val = io_mem[reg[REG_C]];
        WRITE_RAM(hl, hl_val);
        ++hl;
        if (--reg[REG_B] == 0)
        { break; }
      }

      set_q(OP_REG_HL, hl);
      SET_N();
      SET_Z();
      break;

    case  Z80_INDR:     // *HL-- = IN(C) while --B != 0
      hl = get_q(OP_REG_HL);
      while (1)
      {
        hl_val = io_mem[reg[REG_C]];
        WRITE_RAM(hl, hl_val);
        --hl;
        if (--reg[REG_B] == 0)
        { break; }
      }

      set_q(OP_REG_HL, hl);
      SET_N();
      SET_Z();
      break;
  }

  return cycles;
}

int SimulateZ80::rld_rrd(struct _table_z80 * table_z80_item)
{
  uint16_t hl;
  int hl_val;
  int new_hl_val;
  int cycles = table_z80_item->cycles_min;
  int a;

  switch (table_z80_item->instr_enum)
  {
    case  Z80_RLD:
      hl = get_q(OP_REG_HL);
      hl_val = READ_RAM(hl);
      a = reg[REG_A];
      new_hl_val = (hl_val & 0x0f) << 4;
      new_hl_val |= (a & 0x0f);
      a = (a & 0xf0) | ((hl_val >> 4) & 0x0f) ;
      reg[REG_A] = (uint8_t)a;
      WRITE_RAM(hl, new_hl_val);
      CLR_H();
      CLR_N();
      set_flags8(a, 0, 0, VFLAG_PARITY, table_z80_item->instr_enum);
      break;

    case  Z80_RRD:
      hl = get_q(OP_REG_HL);
      hl_val = READ_RAM(hl);
      a = reg[REG_A];
      new_hl_val = (a & 0x0f) << 4;
      new_hl_val |= ((hl_val & 0xf0) >> 4);
      a = (a & 0xf0) | (hl_val & 0x0f) ;
      reg[REG_A] = (uint8_t)a;
      WRITE_RAM(hl, new_hl_val);
      CLR_H();
      CLR_N();
      set_flags8(a, 0, 0, VFLAG_PARITY, table_z80_item->instr_enum);
      break;
  }

  return cycles;
}

int SimulateZ80::execute_op_none16(struct _table_z80 * table_z80_item)
{
  int cycles = table_z80_item->cycles_min;
  int a;
  int tmp;

  switch (table_z80_item->instr_enum)
  {
    case Z80_NEG:
      a = reg[REG_A];
      if (a == 0)
      { CLR_C(); } else { SET_C(); }

      if (a == 0x80)
      { SET_V(); } else { CLR_V(); }

      a = 0 - a;
      SET_N();
      set_flags_a(a, 0, VFLAG_UNCHANGED, table_z80_item->instr_enum);
      return cycles;

    case  Z80_RETI:
    case  Z80_RETN:
      tmp = pop();
      set_pc(tmp - 2);
      --nested_call_count;
      return cycles;

    case Z80_LDIR:      // *DE++ = *HL++ while --BC != 0
    case Z80_LDI:       // *DE++ = *HL++, --BC
    case Z80_LDD:       // *DE-- = *HL--, --BC
    case Z80_LDDR:      // *DE-- = *HL-- while --BC != 0
      return ldi_ldd(table_z80_item);

    case Z80_CPIR:      // flags = A - *HL++, while --BC != 0
    case Z80_CPI:       // flags = A - *HL++, --BC
    case Z80_CPD:       // flags = A - *HL--, --BC
    case Z80_CPDR:      // flags = A - *HL--, while --BC != 0
      return cpi_cpd(table_z80_item);

    case  Z80_OUTI:     // OUT(C) = *HL++, --B
    case  Z80_OUTD:     // OUT(C) = *HL--, --B
    case  Z80_OTIR:     // OUT(C) = *HL++ while --B != 0
    case  Z80_OTDR:     // OUT(C) = *HL-- while --B != 0
      return outi_outd(table_z80_item);

    case  Z80_INI:      // *HL++ = IN(C), --B
    case  Z80_IND:      // *HL-- = IN(C), --B
    case  Z80_INIR:     // *HL++ = IN(C) while --B != 0
    case  Z80_INDR:     // *HL-- = IN(C) while --B != 0
      return ini_ind(table_z80_item);

    case  Z80_RLD:
    case  Z80_RRD:
      return rld_rrd(table_z80_item);

    default:
      return ILLEGAL_INSTRUCTION;
  }
}

int SimulateZ80::execute_op_reg16(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int cycles = table_z80_item->cycles_min;

  old = get_q(reg16);

  switch (table_z80_item->instr_enum)
  {
    case Z80_DEC:
      --old;
      break;
    case Z80_INC:
      ++old;
      break;
  }

  set_q(reg16, old);

  return cycles;
}

int SimulateZ80::execute_op_reg16p(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int reg16 = (opcode >> 4) & 0x3;
  int value;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_PUSH:
      value = get_p(reg16);
      push(value);
      break;
    case Z80_POP:
      value = pop();
      set_p(reg16, (uint16_t)value);
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  return cycles;
}

int SimulateZ80::execute_op_hl_reg16_1(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int number;
  int value;
  int cycles = table_z80_item->cycles_min;

  number = get_q(reg16);
  old = get_q(OP_REG_HL);

  switch (table_z80_item->instr_enum)
  {
    case Z80_ADD:
      value = old + number;
      CLR_N();
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  set_q(OP_REG_HL, (uint16_t)value);

  set_flags16(value, old, number, VFLAG_UNCHANGED, table_z80_item->instr_enum);

  return cycles;
}

int SimulateZ80::execute_op_hl_reg16_2(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  uint16_t old;
  int reg16 = (opcode >> 4) & 0x3;
  int number;
  int value;
  int cycles = table_z80_item->cycles_min;

  number = get_q(reg16);
  old = get_q(OP_REG_HL);

  switch (table_z80_item->instr_enum)
  {
    case Z80_ADC:
      value = old + (number + GET_C());
      CLR_N();
      break;
    case Z80_SBC:
      value = old - (number + GET_C());
      SET_N();
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  set_q(OP_REG_HL, (uint16_t)value);

  set_flags16(value, old, number, VFLAG_OVERFLOW, table_z80_item->instr_enum);

  return cycles;
}

int SimulateZ80::execute_op_index(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int a = reg[REG_A];
  int xy = (opcode >> 5) & 0x1;
  int tmp = 0;
  int addr;
  int8_t offset;
  int8_t vflags = VFLAG_OVERFLOW;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_AND:       // A &= I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a &= tmp;
      vflags = VFLAG_PARITY;
      SET_H();
      CLR_N();
      CLR_C();
      break;
    case Z80_CP:        // flags = A - I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a -= tmp;
      SET_N();
      break;
    case Z80_OR:        // A |= I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a |= tmp;
      vflags = VFLAG_PARITY;
      CLR_H();
      CLR_N();
      CLR_C();
      break;
    case Z80_SUB:       // A -= I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a -= tmp;
      SET_N();
      break;
    case Z80_XOR:       // A ^= I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a ^= tmp;
      vflags = VFLAG_PARITY;
      CLR_H();
      CLR_N();
      CLR_C();
      break;

    case Z80_DEC:       // --I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a = tmp - 1;
      WRITE_RAM(addr + offset, a);
      SET_N();
      set_flags8(a, tmp, 1, vflags, table_z80_item->instr_enum);
      return cycles;
    case Z80_INC:       // ++I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a = tmp + 1;
      WRITE_RAM(addr + offset, a);
      CLR_N();
      set_flags8(a, tmp, 1, vflags, table_z80_item->instr_enum);
      return cycles;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  set_flags_a(a, tmp, vflags, table_z80_item->instr_enum);

  return cycles;
}

int SimulateZ80::execute_op_a_index(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int a = reg[REG_A];
  int xy = (opcode >> 5) & 0x1;
  int tmp = 0;
  int addr;
  int8_t offset;
  int8_t vflags = VFLAG_OVERFLOW;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_ADD:         // A += I?[n]
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a += tmp;
      CLR_N();
      break;
    case Z80_ADC:         // A += (I?[n] + CY)
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a += (tmp + GET_C());
      CLR_N();
      break;
    case Z80_SBC:         // A -= (I?[n] + CY)
      addr = get_xy(xy);
      offset = READ_RAM(pc + 2);
      tmp = READ_RAM(addr + offset);
      a -= (tmp + GET_C());
      SET_N();
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  set_flags_a(a, tmp, vflags, table_z80_item->instr_enum);

  return cycles;
}

int SimulateZ80::execute_op_xy(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int ix_iy = (opcode >> 5) & 0x1;
  uint16_t value;
  int cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_PUSH:
      value = get_xy(ix_iy);
      push(value);
      return cycles;
    case Z80_POP:
      value = (uint16_t)pop();
      set_xy(ix_iy, value);
      return cycles;

    case Z80_DEC:
      if (ix_iy != 0)    // IY
      {
        --iy;
      }
      else      // IX
      {
        --ix;
      }
      return cycles;
    case Z80_INC:
      if (ix_iy != 0)    // IY
      {
        ++iy;
      }
      else      // IX
      {
        ++ix;
      }
      return cycles;
    default:
      return ILLEGAL_INSTRUCTION;
  }
}

int SimulateZ80::execute_op_xy_reg16(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int ix_iy = (opcode16 >> 13) & 0x1;
  int reg16 = (opcode16 >> 4) & 0x3;
  int number = 0;
  int _new, old;

  // Ixy = Ixy + dd   (Z80_ADD, OP_XY_REG16)

  switch (reg16)
  {
    case OP_REG_BC:
    case OP_REG_DE:
    case OP_REG_SP:
      number = get_q(reg16);
      break;
    default:
      if (ix_iy != 0)    // IY
      {
        if (reg16 == OP_REG_IY)
        {
          number = iy;
        }
      }
      else      // IX
      {
        if (reg16 == OP_REG_IX)
        {
          number = ix;
        }
      }
      break;
  }

  old = get_xy(ix_iy);
  _new = old + number;
  set_xy(ix_iy, _new & 0xffff);

  CLR_N();
  set_flags16(_new, old, number, VFLAG_UNCHANGED, table_z80_item->instr_enum);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_xy(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int ix_iy = (opcode >> 5) & 0x1;
  uint16_t addr;

  switch (table_z80_item->instr_enum)
  {
    case Z80_JP:
      addr = get_xy(ix_iy);
      set_pc(addr - 2);
      return table_z80_item->cycles_min;
    default:
      return ILLEGAL_INSTRUCTION;
  }
}

int SimulateZ80::execute_op_index_sp_xy(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int ix_iy = (opcode >> 5) & 0x1;
  uint16_t tmp;

  if (ix_iy)    // IY
  {
    // IY <-> *SP
    // tmp = *SP
    tmp = READ_RAM16(sp);
    // *SP = IY
    WRITE_RAM16(sp, iy);
    // IY = tmp
    iy = tmp;
  }
  else      // IX
  {
    // IX <-> *SP
    // tmp = *SP
    tmp = READ_RAM16(sp);
    // *SP = IX
    WRITE_RAM16(sp, ix);
    // IX = tmp
    ix = tmp;
  }

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_address_xy(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int ix_iy = (opcode >> 5) & 0x1;
  uint16_t addr16;

  addr16 = READ_RAM16(pc + 2);

  if (ix_iy != 0)    // IY
  {
    // *nnnn = IY
    WRITE_RAM16(addr16, iy);
  }
  else      // IX
  {
    // *nnnn = IX
    WRITE_RAM16(addr16, ix);
  }

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_index_address_reg16(struct _table_z80 * table_z80_item, uint16_t opcode16)
{
  int reg16 = (opcode16 >> 4) & 0x3;
  uint16_t addr16;
  uint16_t ddreg;

  addr16 = READ_RAM16(pc + 2);
  ddreg = get_q(reg16);

  // *nnnn = dd
  WRITE_RAM16(addr16, ddreg);

  return table_z80_item->cycles_min;
}

int SimulateZ80::execute_op_address(struct _table_z80 * table_z80_item)
{
  uint16_t addr16 = READ_RAM16(pc + 1);
  uint8_t cycles = table_z80_item->cycles_min;

  switch (table_z80_item->instr_enum)
  {
    case Z80_CALL:
      push(pc + 3);
      ++nested_call_count;
      break;
    case Z80_JP:
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  set_pc(addr16 - 3);

  return cycles;
}

int SimulateZ80::execute_op_cond_address(struct _table_z80 * table_z80_item, uint8_t opcode)
{
  int type = (opcode >> 3) & 0x07;
  uint16_t addr16 = READ_RAM16(pc + 1);
  uint8_t cycles = table_z80_item->cycles_min;
  bool jmp = false;

  switch (table_z80_item->instr_enum)
  {
    case Z80_JP:
    case Z80_CALL:
      switch (type)
      {
        case CC_NZ:
          if (!GET_Z())
          {
            jmp = true;
          }
          break;
        case CC_Z:
          if (GET_Z())
          {
            jmp = true;
          }
         break;
        case CC_NC:
          if (!GET_C())
          {
            jmp = true;
          }
          break;
        case CC_C:
          if (GET_C())
          {
            jmp = true;
          }
          break;
        case CC_PO:
          if (!GET_V())
          {
            jmp = true;
          }
          break;
        case CC_PE:
          if (GET_V())
          {
            jmp = true;
          }
         break;
        case CC_P:
          if (!GET_S())
          {
            jmp = true;
          }
          break;
        case CC_M:
          if (GET_S())
          {
            jmp = true;
          }
          break;
        default:
          return ILLEGAL_INSTRUCTION;
      }
      break;
    default:
      return ILLEGAL_INSTRUCTION;
  }

  if (jmp)
  {
    switch (table_z80_item->instr_enum)
    {
      case Z80_CALL:
        push(pc + 3);
        ++nested_call_count;
        break;
      case Z80_JP:
        break;
    }

    set_pc(addr16 - 3);
    cycles = table_z80_item->cycles_max;
  }

  return cycles;
}

int SimulateZ80::execute()
{
  int index;
  int reg16, xy, ir;
  int8_t offset;
  int address;
  int tmp;
  uint8_t extra_opcode;

  uint16_t opcode = READ_RAM(pc);
  uint16_t opcode16 = READ_OPCODE16(pc);
  uint16_t addr16;

  for (int n = 0; table_z80[n].instr_enum != Z80_NONE; ++n)
  {
    if (table_z80[n].opcode == (opcode & table_z80[n].mask))
    {
      if (table_z80[n].mask > 0xff)
      { continue; }

      switch (table_z80[n].type)
      {
        case OP_NONE:
        case OP_AF_AF_TICK:
          return execute_op_none(&table_z80[n]);
        case OP_A_REG8:
          return execute_op_a_reg8(&table_z80[n], (uint8_t)opcode);
        case OP_REG8:
          return execute_op_reg8(&table_z80[n], (uint8_t)opcode);
        case OP_A_NUMBER8:
          return execute_op_a_number8(&table_z80[n], opcode16);
        case OP_HL_REG16_1:
          return execute_op_hl_reg16_1(&table_z80[n], (uint8_t)opcode);
        case OP_A_INDEX_HL:
          return execute_op_a_index_hl(&table_z80[n]);
        case OP_INDEX_HL:
          return execute_op_index_hl(&table_z80[n]);
        case OP_NUMBER8:
          return execute_op_number8(&table_z80[n], opcode16);
        case OP_ADDRESS:
          return execute_op_address(&table_z80[n]);
        case OP_COND_ADDRESS:
          return execute_op_cond_address(&table_z80[n], (uint8_t)opcode);
        case OP_REG8_V2:
          return execute_op_reg8_v2(&table_z80[n], (uint8_t)opcode);
        case OP_REG16:
          return execute_op_reg16(&table_z80[n], (uint8_t)opcode);
        case OP_INDEX_SP_HL:
          // *SP <-> HL
          // tmp = *SP
          tmp = READ_RAM16(sp);
          // *SP = HL
          WRITE_RAM16(sp, get_q(OP_REG_HL));
          // HL = tmp
          set_q(OP_REG_HL, (uint16_t)tmp);
          return table_z80[n].cycles_min;
        case OP_DE_HL:
          // DE <-> HL
          tmp = get_q(OP_REG_HL);
          set_q(OP_REG_HL, get_q(OP_REG_DE));
          set_q(OP_REG_DE, (uint16_t)tmp);
          return table_z80[n].cycles_min;
        case OP_SP_HL:
          // SP = HL
          set_q(OP_REG_SP, get_q(OP_REG_HL));
          return table_z80[n].cycles_min;
        case OP_A_INDEX_N:
          // A = IO(nn)
          tmp = READ_RAM(pc + 1);
          reg[REG_A] = io_mem[tmp];
          return table_z80[n].cycles_min;
        case OP_OFFSET8:
          return execute_op_offset8(&table_z80[n], opcode16);
        case OP_JR_COND_ADDRESS:
          return execute_op_cond_offset8(&table_z80[n], opcode16);
        case OP_REG8_REG8:
          // r1 = r2
          index = (opcode >> 3) & 0x7;
          reg[index] = reg[opcode & 0x7];
          return table_z80[n].cycles_min;
        case OP_REG8_NUMBER8:
          // r = nn
          index = (opcode16 >> 11) & 0x7;
          reg[index] = opcode16 & 0xff;
          return table_z80[n].cycles_min;
        case OP_REG8_INDEX_HL:
          // r = *HL
          return execute_op_reg8_index_hl(&table_z80[n], (uint8_t)opcode);
        case OP_INDEX_HL_REG8:
          // *HL = r
          return execute_op_index_hl_reg8(&table_z80[n], (uint8_t)opcode);
        case OP_INDEX_HL_NUMBER8:
          // *HL = nn
          tmp = get_q(OP_REG_HL);
          WRITE_RAM(tmp, READ_RAM(pc + 1));
          return table_z80[n].cycles_min;
        case OP_A_INDEX_ADDRESS:
          // A = *nnnn
          addr16 = READ_RAM16(pc + 1);
          reg[REG_A] = READ_RAM(addr16);
          return table_z80[n].cycles_min;
        case OP_INDEX_ADDRESS_A:
          // *nnnn = A
          addr16 = READ_RAM16(pc + 1);
          WRITE_RAM(addr16, reg[REG_A]);
          return table_z80[n].cycles_min;
        case OP_A_INDEX_BC:
          // A = *BC
          tmp = READ_RAM(get_q(OP_REG_BC));
          reg[REG_A] = (uint8_t)tmp;
          return table_z80[n].cycles_min;
        case OP_A_INDEX_DE:
          // A = *DE
          tmp = READ_RAM(get_q(OP_REG_DE));
          reg[REG_A] = (uint8_t)tmp;
          return table_z80[n].cycles_min;
        case OP_INDEX_DE_A:
          // *DE = A
          tmp = get_q(OP_REG_DE);
          WRITE_RAM(tmp, reg[REG_A]);
          return table_z80[n].cycles_min;
        case OP_INDEX_BC_A:
          // *BC = A
          tmp = get_q(OP_REG_BC);
          WRITE_RAM(tmp, reg[REG_A]);
          return table_z80[n].cycles_min;
        case OP_REG16_ADDRESS:
          // dd = nnnn
          reg16 = (opcode >> 4) & 0x3;
          set_q(reg16, READ_RAM16(pc + 1));
          return table_z80[n].cycles_min;
        case OP_INDEX_ADDRESS_HL:
          // *(nnnn) = HL
          addr16 = READ_RAM16(pc + 1);
          WRITE_RAM16(addr16, get_q(OP_REG_HL));
          return table_z80[n].cycles_min;
        case OP_HL_INDEX_ADDRESS:
          // HL = *(nnnn)
          addr16 = READ_RAM16(pc + 1);
          reg[REG_L] = READ_RAM(addr16);
          reg[REG_H] = READ_RAM(addr16 + 1);
          return table_z80[n].cycles_min;
        case OP_INDEX_ADDRESS8_A:
          // IO(nn) = A
          tmp = READ_RAM(pc + 1);
          io_mem[tmp] = reg[REG_A];
          return table_z80[n].cycles_min;
        case OP_REG16P:
          return execute_op_reg16p(&table_z80[n], (uint8_t)opcode);
        case OP_COND:
          return execute_op_cond_none(&table_z80[n], opcode);
        case OP_RESTART_ADDRESS:
          // RST nn
          address = opcode & 0x38;
          push(pc + 1);
          set_pc(address - 1);
          ++nested_call_count;
          return table_z80[n].cycles_min;
        default:
          return ILLEGAL_INSTRUCTION;
      }
    }
  }

  for (int n = 0; table_z80[n].instr_enum != Z80_NONE; ++n)
  {
    if (table_z80[n].mask <= 0xff)
    { continue; }

    if (table_z80[n].opcode == (opcode16 & table_z80[n].mask))
    {
      switch (table_z80[n].type)
      {
        case OP_NONE16:
          return execute_op_none16(&table_z80[n]);
        case OP_NONE24:
          return ILLEGAL_INSTRUCTION;
        case OP_A_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_B_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_C_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_D_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_E_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_A_INDEX:
          return execute_op_a_index(&table_z80[n], (uint8_t)opcode);
        case OP_HL_REG16_2:
          return execute_op_hl_reg16_2(&table_z80[n], (uint8_t)opcode16);
        case OP_XY_REG16:
          return execute_op_xy_reg16(&table_z80[n], opcode16);
        case OP_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_INDEX:
          return execute_op_index(&table_z80[n], (uint8_t)opcode);
        case OP_BIT_REG8:
          return execute_op_bit_reg8(&table_z80[n], opcode16);
        case OP_BIT_INDEX_HL:
          return execute_op_bit_reg8_index_hl(&table_z80[n], opcode16);
        case OP_BIT_INDEX:
          // (Ixy + n).b = 1, (Ixy + n).b = 0, Z = (Ixy + n).b
          return execute_op_bit_index(&table_z80[n], (uint8_t)opcode);
        case OP_REG_IHALF_V2:
          return ILLEGAL_INSTRUCTION;
        case OP_XY:
          return execute_op_xy(&table_z80[n], (uint8_t)opcode);
        case OP_INDEX_SP_XY:
          return execute_op_index_sp_xy(&table_z80[n], (uint8_t)opcode);
        case OP_IM_NUM:
          // IM = n
          tmp = (opcode16 >> 3) & 0x3;
          if (tmp > 0)
          { --tmp; }

          im = (uint8_t)tmp;
          return table_z80[n].cycles_min;
        case OP_REG8_INDEX_C:
          // r = IO(C)
          index = (opcode16 >> 3) & 0x7;
          tmp = io_mem[reg[REG_C]];
          reg[index] = (uint8_t)tmp;
          CLR_H();
          CLR_N();
          set_flags8(tmp, 0, 0, VFLAG_PARITY, table_z80[n].instr_enum);
          return table_z80[n].cycles_min;
        case OP_F_INDEX_C:
          return ILLEGAL_INSTRUCTION;
        case OP_INDEX_XY:
          return execute_op_index_xy(&table_z80[n], (uint8_t)opcode);
        case OP_REG8_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_REG_IHALF_REG8:
          return ILLEGAL_INSTRUCTION;
        case OP_REG_IHALF_REG_IHALF:
          return ILLEGAL_INSTRUCTION;
        case OP_REG8_INDEX:
          // r = *(Ixy + n)
          xy = (opcode >> 5) & 0x1;
          offset = READ_RAM(pc + 2);
          address =  get_xy(xy) + offset;
          index = (opcode16 >> 3) & 0x7;
          reg[index] = READ_RAM(address);
          return table_z80[n].cycles_min;
        case OP_INDEX_REG8:
          // *(Ixy + n) = r
          xy = (opcode >> 5) & 0x1;
          offset = READ_RAM(pc + 2);
          address =  get_xy(xy) + offset;
          index = opcode16 & 0x7;
          WRITE_RAM(address, reg[index]);
          return table_z80[n].cycles_min;
        case OP_INDEX_NUMBER8:
          // *(Ixy + n) = nn
          xy = (opcode >> 5) & 0x1;
          offset = READ_RAM(pc + 2);
          address =  get_xy(xy) + offset;
          WRITE_RAM(address, READ_RAM(pc + 3));
          return table_z80[n].cycles_min;
        case OP_IR_A:
          // I = A
          ir = (opcode16 >> 3) & 0x1;
          set_ir(ir, reg[REG_A]);
          return table_z80[n].cycles_min;
        case OP_A_IR:
          // A = I
          ir = (opcode16 >> 3) & 0x1;
          set_flags_a(get_ir(ir), 0, VFLAG_UNCHANGED, table_z80[n].instr_enum);
          CLR_H();
          CLR_N();
          return table_z80[n].cycles_min;
        case OP_XY_ADDRESS:
          // Ixy = nnnn
          xy = (opcode >> 5) & 0x1;
          set_xy(xy, READ_RAM16(pc + 2));
          return table_z80[n].cycles_min;
        case OP_REG16_INDEX_ADDRESS:
          // dd = *nnnn
          reg16 = (opcode16 >> 4) & 0x3;
          tmp = READ_RAM16(pc + 2);
          set_q(reg16, READ_RAM16(tmp));
          return table_z80[n].cycles_min;
        case OP_XY_INDEX_ADDRESS:
          // Ixy = *nnnn
          xy = (opcode >> 5) & 0x1;
          tmp = READ_RAM16(pc + 2);
          set_xy(xy, READ_RAM16(tmp));
          return table_z80[n].cycles_min;
        case OP_INDEX_ADDRESS_REG16:
          return execute_op_index_address_reg16(&table_z80[n], opcode16);
        case OP_INDEX_ADDRESS_XY:
          return execute_op_index_address_xy(&table_z80[n], (uint8_t)opcode);
        case OP_SP_XY:
          // SP = Ixy
          xy = (opcode >> 5) & 0x1;
          sp = get_xy(xy);
          return table_z80[n].cycles_min;
        case OP_INDEX_LONG:
          // RLC(Ixy + n), RL(Ixy + n), RRC(Ixy + n), RR(Ixy + n), SLA(Ixy + n),
          // SRA(Ixy + n), SRL(Ixy + n)
          extra_opcode = READ_RAM(pc + 3);
          if (extra_opcode == table_z80[n].extra_opcode)
          {
            return execute_op_index_long(&table_z80[n], (uint8_t)opcode);
          }
          break;
        case OP_INDEX_C_REG8:
          // IO(C) = r
          index = (opcode16 >> 3) & 0x7;
          tmp = reg[index];
          io_mem[reg[REG_C]] = (uint8_t)tmp;
          return table_z80[n].cycles_min;
        case OP_INDEX_C_ZERO:
          return ILLEGAL_INSTRUCTION;
        case OP_REG8_CB:
          return execute_op_reg8_cb(&table_z80[n], opcode16);
        case OP_INDEX_HL_CB:
          return execute_op_index_hl_cb(&table_z80[n]);
        case OP_BIT_INDEX_V2:   // in table_z80_4_byte[]
          return ILLEGAL_INSTRUCTION;
        case OP_BIT_INDEX_REG8: // in table_z80_4_byte[]
          return ILLEGAL_INSTRUCTION;
        default:
          return ILLEGAL_INSTRUCTION;
      }
    }
  }

  return ILLEGAL_INSTRUCTION;
}

