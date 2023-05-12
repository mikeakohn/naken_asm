/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, D.L. Karmann
 *
 * stm8 file by D.L. Karmann
 *
 * Current STM8 simulator limitations:
 *  - MPU hardware peripherals (GPIO, UART, Timers, etc.) not supported
 *  - MPU device limits (Flash, RAM, EEPROM sizes) not supported (i.e. no memory restrictions inside address space)
 *  - Address space limited to simulation memory size (0x0000 to mem_size - 1, see memory_init() in main())
 *  - Initial/reset SP set to top of STM8S001 / STM8S003 / STM8S103 device RAM
 *  - STM7 instructions are not supported
 *
 * Reference: STMicroelectronics PM0044: STM8 CPU Programming Manual (Rev. 3 - September 2011)
 *            STMicroelectronics ES0102: STM8S001J3/003xx/103xx/903xx Errata sheet (Rev. 5 - July 2017)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "simulate/stm8.h"
#include "disasm/stm8.h"
#include "table/stm8.h"

#define UNKNOWN_INST      -1
#define INVALID_MEM_ADDR  -2

#define VECTOR_SIZE   4
#define RESET_VECTOR  0x8000
#define TRAP_VECTOR   RESET_VECTOR + VECTOR_SIZE
#define RESET_SP      0x03ff      // STM8S001 / STM8S003 / STM8S103   { this is device dependent! }

#define REG_A   simulate_stm8->reg_a
#define REG_X   simulate_stm8->reg_x
#define REG_Y   simulate_stm8->reg_y
#define REG_SP  simulate_stm8->reg_sp
#define REG_PC  simulate_stm8->reg_pc
#define REG_CC  simulate_stm8->reg_cc

#define SHOW_STACK    sp, memory_read_m(simulate->memory, sp)
#define READ_RAM(a)   memory_read_m(simulate->memory, a)
#define READ_RAM16(a) memory_read16_m(simulate->memory, a)
#define READ_RAM24(a) ((memory_read_m(simulate->memory, a) << 16 ) | (memory_read16_m(simulate->memory, a + 1)))

#define WRITE_RAM(a, b) \
  if ((a) == simulate->break_io) \
  { \
    exit(b); \
  } \
  memory_write_m(simulate->memory, a, b)
#define WRITE_RAM16(a, w) \
  if ((a) == simulate->break_io) \
  { \
    exit(w); \
  } \
  memory_write16_m(simulate->memory, a, w)

#define PUSH_STACK(n)   memory_write_m(simulate->memory, REG_SP, (n) & 0xff); --REG_SP  // caution: "--" side-effects
#define PUSH_STACK16(n) simulate_push16_stm8(simulate, n)
#define PUSH_STACK24(n) simulate_push24_stm8(simulate, n)
#define POP_STACK()     memory_read_m(simulate->memory, ++REG_SP)   // caution: "++" side-effects
#define POP_STACK16()   simulate_pop16_stm8(simulate)
#define POP_STACK24()   simulate_pop24_stm8(simulate)

#define BV(bit)     (1 << bit)

#define SIZE_8BITS  8
#define SIZE_16BITS 16

#define OP_TYPE_BIT 6
#define OP_ADD_TYPE 0
#define OP_SUB_TYPE BV(OP_TYPE_BIT)

// Condition Code flag bits
#define CC_V_FLAG   7
#define CC_I1_FLAG  5
#define CC_H_FLAG   4
#define CC_I0_FLAG  3
#define CC_N_FLAG   2
#define CC_Z_FLAG   1
#define CC_C_FLAG   0

#define GET_V()   ((REG_CC >> CC_V_FLAG) & 1)
#define GET_I1()  ((REG_CC >> CC_I1_FLAG) & 1)
#define GET_H()   ((REG_CC >> CC_H_FLAG) & 1)
#define GET_I0()  ((REG_CC >> CC_I0_FLAG) & 1)
#define GET_N()   ((REG_CC >> CC_N_FLAG) & 1)
#define GET_Z()   ((REG_CC >> CC_Z_FLAG) & 1)
#define GET_C()   ((REG_CC >> CC_C_FLAG) & 1)

#define SET_V()   REG_CC |= BV(CC_V_FLAG);
#define SET_I1()  REG_CC |= BV(CC_I1_FLAG);
#define SET_H()   REG_CC |= BV(CC_H_FLAG);
#define SET_I0()  REG_CC |= BV(CC_I0_FLAG);
#define SET_N()   REG_CC |= BV(CC_N_FLAG);
#define SET_Z()   REG_CC |= BV(CC_Z_FLAG);
#define SET_C()   REG_CC |= BV(CC_C_FLAG);

#define CLR_V()   REG_CC &= ~(BV(CC_V_FLAG));
#define CLR_I1()  REG_CC &= ~(BV(CC_I1_FLAG));
#define CLR_H()   REG_CC &= ~(BV(CC_H_FLAG));
#define CLR_I0()  REG_CC &= ~(BV(CC_I0_FLAG));
#define CLR_N()   REG_CC &= ~(BV(CC_N_FLAG));
#define CLR_Z()   REG_CC &= ~(BV(CC_Z_FLAG));
#define CLR_C()   REG_CC &= ~(BV(CC_C_FLAG));

// Prefixed opcodes
#define PRECODE_PWSP  0x72
#define PRECODE_PDY   0x90
#define PRECODE_PIY   0x91
#define PRECODE_PIX   0x92

#define CC_V_NDX  0
#define CC_I1_NDX 1
#define CC_H_NDX  2
#define CC_I0_NDX 3
#define CC_N_NDX  4
#define CC_Z_NDX  5
#define CC_C_NDX  6
static const char * const CC_Flags[] = { "V", "I1", "H", "I0", "N", "Z", "C" };

static int stop_running = 0;
static int stm8_int_opcode = -1;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

struct _simulate * simulate_init_stm8(struct _memory * memory)
{
  struct _simulate * simulate = NULL;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_stm8) + sizeof(struct _simulate));
  if (simulate != NULL)
  {
    simulate->simulate_init = simulate_init_stm8;
    simulate->simulate_free = simulate_free_stm8;
    simulate->simulate_dumpram = simulate_dumpram_stm8;
    simulate->simulate_push = simulate_push_stm8;
    simulate->simulate_set_reg = simulate_set_reg_stm8;
    simulate->simulate_get_reg = simulate_get_reg_stm8;
    simulate->simulate_set_pc = simulate_set_pc_stm8;
    simulate->simulate_reset = simulate_reset_stm8;
    simulate->simulate_dump_registers = simulate_dump_registers_stm8;
    simulate->simulate_run = simulate_run_stm8;

    simulate->memory = memory;
    simulate_reset_stm8(simulate);
    simulate->usec = 1000000;   // 1Hz
    simulate->step_mode = 0;
    simulate->show = 1;         // Show simulation
    simulate->auto_run = 0;     // Will this program stop on a ret from main
  }
  return simulate;
}

void simulate_push_stm8(struct _simulate * simulate, uint32_t value)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  PUSH_STACK(value);
}

static void simulate_push16_stm8(struct _simulate * simulate, uint32_t value)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  PUSH_STACK(value);
  PUSH_STACK(value >> 8);
}

static void simulate_push24_stm8(struct _simulate * simulate, uint32_t value)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  PUSH_STACK(value);
  PUSH_STACK(value >> 8);
  PUSH_STACK(value >> 16);
}

// Returns:
//    16-bit value popped off stack
static uint16_t simulate_pop16_stm8(struct _simulate * simulate)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t rslt;

  rslt = (POP_STACK() << 8);
  rslt |= POP_STACK();
  return rslt;
}

// Returns:
//    24-bit value popped off stack
static uint32_t simulate_pop24_stm8(struct _simulate * simulate)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t rslt;

  rslt = (POP_STACK() << 16);
  rslt |= (POP_STACK() << 8);
  rslt |= POP_STACK();
  return rslt;
}

// Returns:
//    -1 = invalid register/flag or unsupported memory location
//     0 = OK
int simulate_set_reg_stm8(struct _simulate * simulate, char * reg_string, uint32_t value)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  while (*reg_string == ' ')
  {
    ++reg_string;
  }

  // check for STM8 registers
  if (strcasecmp(reg_string, "A") == 0)
  {
    REG_A = value & 0xff;
  }
  else if (strcasecmp(reg_string, "X") == 0)
  {
    REG_X = value & 0xffff;
  }
  else if (strcasecmp(reg_string, "Y") == 0)
  {
    REG_Y = value & 0xffff;
  }
  else if (strcasecmp(reg_string, "SP") == 0)
  {
    REG_SP = value & 0xffff;
  }
  else if (strcasecmp(reg_string, "PC") == 0)
  {
    simulate_set_pc_stm8(simulate, value);
    if (value >= simulate->memory->size)
    {
      return -1;
    }
  }
  else if (strcasecmp(reg_string, "CC") == 0)
  {
    REG_CC = value & (BV(CC_V_FLAG) | BV(CC_I1_FLAG) | BV(CC_H_FLAG) | 
                      BV(CC_I0_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG));
  }
  else  // check for condition code flags
  {
    if (strcasecmp(reg_string, CC_Flags[CC_V_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_V();
      }
      else
      {
        CLR_V();
      }
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_I1_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_I1();
      }
      else
      {
        CLR_I1();
      }
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_H_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_H();
      }
      else
      {
        CLR_H();
      }
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_I0_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_I0();
      }
      else
      {
        CLR_I0();
      }
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_N_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_N();
      }
      else
      {
        CLR_N();
      }
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_Z_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_Z();
      }
      else
      {
        CLR_Z();
      }
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_C_NDX]) == 0)
    {
      if (value == 1)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
    }
    else
    {
      return -1;
    }
  }

  return 0;
}

// Returns:
//    register or condition code flag value
//    0 returned for unknown registers or condition code flag names
uint32_t simulate_get_reg_stm8(struct _simulate * simulate, char * reg_string)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  // skip leading spaces
  while (*reg_string == ' ')
  {
    ++reg_string;
  }

  // check for STM8 registers
  if (strcasecmp(reg_string, "A") == 0)
  {
    return REG_A;
  }
  else if (strcasecmp(reg_string, "X") == 0)
  {
    return REG_X;
  }
  else if (strcasecmp(reg_string, "Y") == 0)
  {
    return REG_Y;
  }
  else if (strcasecmp(reg_string, "SP") == 0)
  {
    return REG_SP;
  }
  else if (strcasecmp(reg_string, "PC") == 0)
  {
    return REG_PC;
  }
  else if (strcasecmp(reg_string, "CC") == 0)
  {
    return REG_CC;
  }
  else  // check for condition code flags
  {
    if (strcasecmp(reg_string, CC_Flags[CC_V_NDX]) == 0)
    {
      return GET_V();
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_I1_NDX]) == 0)
    {
      return GET_I1();
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_H_NDX]) == 0)
    {
      return GET_H();
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_I0_NDX]) == 0)
    {
      return GET_I0();
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_N_NDX]) == 0)
    {
      return GET_N();
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_Z_NDX]) == 0)
    {
      return GET_Z();
    }
    else if (strcasecmp(reg_string, CC_Flags[CC_C_NDX]) == 0)
    {
      return GET_C();
    }
  }

  return 0;
}

void simulate_set_pc_stm8(struct _simulate * simulate, uint32_t value)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  if (value >= simulate->memory->size)
  {
    printf("Unsupported PC memory address !!!\n\n");
  }
  else
  {
    REG_PC = value & 0xffffff;
  }
}

void simulate_reset_stm8(struct _simulate * simulate)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;
  REG_A = 0;
  REG_X = 0;
  REG_Y = 0;
  REG_SP = RESET_SP;

  if (stm8_int_opcode < 0)
  {
    int ndx = 0;

    // find and save the STM8 "int" opcode for later use
    while (table_stm8_opcodes[ndx].instr_enum != STM8_NONE)
    {
      if (table_stm8_opcodes[ndx].instr_enum == STM8_INT)
      {
        stm8_int_opcode = table_stm8_opcodes[ndx].opcode;
        break;
      }
      ++ndx;
    }
  }

  REG_PC = 0;
  if (stm8_int_opcode > 0)
  {
    if (READ_RAM(RESET_VECTOR) == stm8_int_opcode)
    {
      REG_PC = READ_RAM24(RESET_VECTOR + 1);
      if (REG_PC >= simulate->memory->size)
      {
        REG_PC = 0;
      }
    }
  }
  REG_CC = BV(CC_I1_FLAG) | BV(CC_I0_FLAG);
  simulate->break_point = -1;
}

void simulate_free_stm8(struct _simulate * simulate)
{
  free(simulate);
}

// Returns:
//     0 = OK
//    -1 = not supported for this MPU
int simulate_dumpram_stm8(struct _simulate * simulate, int start, int end)
{
  return -1;    // Use print or print16 to display RAM.
}

void simulate_dump_registers_stm8(struct _simulate * simulate)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t sp = REG_SP;

  printf("\nSimulation Register Dump                               Stack\n");
  printf("------------------------------------------------------------\n");
  printf("                7 6  5 4  3 2 1 0               0x%04x: 0x%02x\n", SHOW_STACK);
  ++sp;

  printf("Condition Code: %s - %s %s %s %s %s %s               0x%04x: 0x%02x\n",
    CC_Flags[CC_V_NDX], CC_Flags[CC_I1_NDX], CC_Flags[CC_H_NDX], CC_Flags[CC_I0_NDX],
    CC_Flags[CC_N_NDX], CC_Flags[CC_Z_NDX], CC_Flags[CC_C_NDX], SHOW_STACK);
  ++sp;

  printf("                %d 0  %d %d  %d %d %d %d               0x%04x: 0x%02x\n",
    GET_V(), GET_I1(), GET_H(), GET_I0(), GET_N(), GET_Z(), GET_C(), SHOW_STACK);
  ++sp;

  printf("                                                0x%04x: 0x%02x\n", SHOW_STACK);
  ++sp;

  printf("  A=0x%02x   X=0x%04x   Y=0x%04x                  0x%04x: 0x%02x\n", REG_A, REG_X, REG_Y, SHOW_STACK);
  ++sp;
  printf(" CC=0x%02x  SP=0x%04x  PC=0x%06x                0x%04x: 0x%02x\n", REG_CC, REG_SP, REG_PC, SHOW_STACK);

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

// Adjust specified condition code flags using operation values and results.
// Note: N, Z flags determined using only 'rslt' (and 'bit_size')
//       V, H, C flags determined using 'op1', 'op2', 'rslt' (and 'bit_size')
static void calculate_flags(struct _simulate * simulate, uint8_t flag_bits, uint16_t op1, uint16_t op2, uint16_t rslt, uint8_t bit_size)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;

  if (bit_size == SIZE_8BITS)
  {
    // overflow - V
    // {A=op1, M=op2, R=rslt}
    // ((A7.M7) + (A7.R7) + (A7.M7.R7)) ^ ((A6.M6) + (A6.R6) + (A6.M6.R6))   [CP/SBC/SUB]
    // ((A7.M7) + (A7.R7) +    (M7.R7)) ^ ((A6.M6) + (A6.R6) +    (M6.R6))   [ADC/ADD/INC/DEC]
    if ((flag_bits & BV(CC_V_FLAG)) != 0)
    {
      if ((flag_bits & BV(OP_TYPE_BIT)) != OP_ADD_TYPE) // [CP/SBC/SUB]
      {
        if (((((op1 & 0x80) & (op2 & 0x80)) |
              ((op1 & 0x80) & (rslt & 0x80)) |
              ((op1 & 0x80) & (op2 & 0x80) & (rslt & 0x80))) ^
             (((op1 & 0x40) & (op2 & 0x40)) |
              ((op1 & 0x40) & (rslt & 0x40)) |
              ((op1 & 0x40) & (op2 & 0x40) & (rslt & 0x40)))) != 0) 
        {
          SET_V();
        }
        else
        {
          CLR_V();
        }
      }
      else    // [ADC/ADD/INC/DEC]
      {
        if (((((op1 & 0x80) & (op2 & 0x80)) |
              ((op1 & 0x80) & (rslt & 0x80)) |
              ((op2 & 0x80) & (rslt & 0x80))) ^
             (((op1 & 0x40) & (op2 & 0x40)) |
              ((op1 & 0x40) & (rslt & 0x40)) |
              ((op2 & 0x40) & (rslt & 0x40)))) != 0) 
        {
          SET_V();
        }
        else
        {
          CLR_V();
        }
      }
    }

    // half-carry - H
    if ((flag_bits & BV(CC_H_FLAG)) != 0)
    {
      if ((((op1 & 0x08) != 0) && ((op2 & 0x08) != 0)) ||
          (((op2 & 0x08) != 0) && ((rslt & 0x08) == 0)) ||
          (((rslt & 0x08) == 0) && ((op1 & 0x08) != 0)))
      {
        SET_H();
      }
      else
      {
        CLR_H();
      }
    }

    // negative - N
    if ((flag_bits & BV(CC_N_FLAG)) != 0)
    {
      if ((rslt & 0x80) != 0)
      {
        SET_N();
      }
      else
      {
        CLR_N();
      }
    }

    // zero - Z
    if ((flag_bits & BV(CC_Z_FLAG)) != 0)
    {
      if ((rslt & 0xff) == 0)
      {
        SET_Z();
      }
      else
      {
        CLR_Z();
      }
    }

    // carry - C
    // {A=op1, M=op2, R=rslt}
    // (!A7.M7) + (!A7.R7) + (A7.M7.R7)  [SUB/CP]
    // (A7.M7)  + (M7.!R7) + (!R7.A7)    [ADD]
    if ((flag_bits & BV(CC_C_FLAG)) != 0)
    {
      if ((flag_bits & BV(OP_TYPE_BIT)) != OP_ADD_TYPE) // [SUB/CP]
      {
        if ((((op1 & 0x80) == 0) && ((op2 & 0x80) != 0)) ||
            (((op1 & 0x80) == 0) && ((rslt & 0x80) != 0)) ||
            (((op1 & 0x80) != 0) && ((op2 & 0x80) != 0) && ((rslt & 0x80) != 0)))
        {
          SET_C();
        }
        else
        {
          CLR_C();
        }
      }
      else    // [ADD]
      {
        if ((((op1 & 0x80) != 0) && ((op2 & 0x80) != 0)) ||
            (((op2 & 0x80) != 0) && ((rslt & 0x80) == 0)) ||
            (((rslt & 0x80) == 0) && ((op1 & 0x80) != 0)))
        {
          SET_C();
        }
        else
        {
          CLR_C();
        }
      }
    }
  }
  else if (bit_size == SIZE_16BITS)
  {
    // overflow - V
    // {X=op1, M=op2, R=rslt}
    // ((X15.M15) + (X15.R15) + (X15.M15.R15)) ^ ((X14.M14) + (X14.R14) + (X14.M14.R14))   [CPW/SUBW]
    // ((X15.M15) + (X15.R15) +     (M15.R15)) ^ ((X14.M14) + (X14.R14) +     (M14.R14))   [ADDW/DECW/INCW]
    if ((flag_bits & BV(CC_V_FLAG)) != 0)
    {
      if ((flag_bits & BV(OP_TYPE_BIT)) != OP_ADD_TYPE) // [CPW/SUBW]
      {
        if (((((op1 & 0x8000) & (op2 &  0x8000)) |
              ((op1 & 0x8000) & (rslt & 0x8000)) |
              ((op1 & 0x8000) & (op2 & 0x8000) & (rslt & 0x8000))) ^
             (((op1 & 0x4000) & (op2 &  0x4000)) |
              ((op1 & 0x4000) & (rslt & 0x4000)) |
              ((op1 & 0x4000) & (op2 & 0x4000) & (rslt & 0x4000)))) != 0)
        {
          SET_V();
        }
        else
        {
          CLR_V();
        }
      }
      else    // [ADDW/DECW/INCW]
      {
        if (((((op1 & 0x8000) & (op2 &  0x8000)) |
              ((op1 & 0x8000) & (rslt & 0x8000)) |
              ((op2 & 0x8000) & (rslt & 0x8000))) ^
             (((op1 & 0x4000) & (op2 &  0x4000)) |
              ((op1 & 0x4000) & (rslt & 0x4000)) |
              ((op2 & 0x4000) & (rslt & 0x4000)))) != 0)
        {
          SET_V();
        }
        else
        {
          CLR_V();
        }
      }
    }

    // half-carry - H
    if ((flag_bits & BV(CC_H_FLAG)) != 0)
    {
      if ((((op1 & 0x80) != 0) && ((op2 & 0x80) != 0)) ||
          (((op2 & 0x80) != 0) && ((rslt & 0x80) == 0)) ||
          (((rslt & 0x80) == 0) && ((op1 & 0x80) != 0)))
      {
        SET_H();
      }
      else
      {
        CLR_H();
      }
    }

    // negative - N
    if ((flag_bits & BV(CC_N_FLAG)) != 0)
    {
      if ((rslt & 0x8000) != 0)
      {
        SET_N();
      }
      else
      {
        CLR_N();
      }
    }

    // zero - Z
    if ((flag_bits & BV(CC_Z_FLAG)) != 0)
    {
      if ((rslt & 0xffff) == 0)
      {
        SET_Z();
      }
      else
      {
        CLR_Z();
      }
    }

    // carry - C
    // {X=op1, M=op2, R=rslt}
    // (!X15.M15) + (!X15.R15) + (X15.M15.R15)    [SUB/CP]
    // (X15.M15)  + (M15.!R15) + (!R15.X15)       [ADD]
    if ((flag_bits & BV(CC_C_FLAG)) != 0)
    {
      if ((flag_bits & BV(OP_TYPE_BIT)) != OP_ADD_TYPE) // [SUB/CP]
      {
        if ((((op1 & 0x8000) == 0) && ((op2 & 0x8000) != 0)) ||
            (((op1 & 0x8000) == 0) && ((rslt & 0x8000) != 0)) ||
            (((op1 & 0x8000) != 0) && ((op2 & 0x8000) != 0) && ((rslt & 0x8000) != 0)))
        {
          SET_C();
        }
        else
        {
          CLR_C();
        }
      }
      else    // [ADD]
      {
        if ((((op1 & 0x8000) != 0) && ((op2 & 0x8000) != 0)) ||
            (((op2 & 0x8000) != 0) && ((rslt & 0x8000) == 0)) ||
            (((rslt & 0x8000) == 0) && ((op1 & 0x8000) != 0)))
        {
          SET_C();
        }
        else
        {
          CLR_C();
        }
      }
    }
  }
}

// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_common(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8, uint32_t eff_addr)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t data;
  uint16_t data16;
  uint8_t flag_bits;
  uint16_t rslt;

  if (table_stm8->type == OP_NUMBER8)
  {
    data = eff_addr;
  }
  else
  {
    data = READ_RAM(eff_addr);
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_ADC:
      rslt = REG_A + data + GET_C();
      flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_ADD:
      rslt = REG_A + data;
      flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_AND:
      rslt = REG_A & data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_BCP:
      rslt = REG_A & data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_CALL:
      PUSH_STACK16(REG_PC);
      REG_PC = (REG_PC & 0xff0000) | eff_addr;
      return table_stm8->cycles_min;
    case STM8_CLR:
      WRITE_RAM(eff_addr, 0);
      CLR_N();
      SET_Z();
      return table_stm8->cycles_min;
    case STM8_CP:
      rslt = REG_A - data;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_CPW:
      data16 = READ_RAM16(eff_addr);
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      if (table_stm8->dest == OP_REG_X)
      {
        rslt = REG_X - data16;
        calculate_flags(simulate, flag_bits, REG_X, data16, rslt, SIZE_16BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->dest == OP_REG_Y)
      {
        rslt = REG_Y - data16;
        calculate_flags(simulate, flag_bits, REG_Y, data16, rslt, SIZE_16BITS);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }
    case STM8_CPL:
      rslt = data ^= 0xff;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      SET_C();
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_DEC:
      rslt = data - 1;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, data, 1, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_INC:
      rslt = data + 1;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, data, 1, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_JP:
      REG_PC = (REG_PC & 0xff0000) | eff_addr;
      return table_stm8->cycles_min;
    case STM8_LD:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = data;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }
    case STM8_LDW:
      if (table_stm8->dest == OP_REG_X)
      {
        rslt = READ_RAM16(eff_addr);
        REG_X = rslt;
      }
      else if (table_stm8->src == OP_REG_Y)
      {
        rslt = REG_Y;
        WRITE_RAM16(eff_addr, rslt);
      }
      else if (table_stm8->dest == OP_REG_Y)
      {
        rslt = READ_RAM16(eff_addr);
        REG_Y = rslt;
      }
      else if (table_stm8->src == OP_REG_X)
      {
        rslt = REG_X;
        WRITE_RAM16(eff_addr, rslt);
      }
      else
      {
        return UNKNOWN_INST;
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
      return table_stm8->cycles_min;
    case STM8_NEG:
      rslt = 0 - data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, data, 0, rslt, SIZE_8BITS);
      if (data == 0x80)
      {
        SET_V();
      }
      else
      {
        CLR_V()
      }

      if (data != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C()
      }
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_OR:
      rslt = REG_A | data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_RLC:
      flag_bits = data & 0x80;
      rslt = (data << 1) + GET_C();
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_RRC:
      flag_bits = data & 0x01;
      rslt = (data >> 1) | (GET_C() << 7);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SBC:
      rslt = REG_A - data - GET_C();
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SLL:
      flag_bits = data & 0x80;
      rslt = (data << 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SRA:
      flag_bits = data & 0x01;
      rslt = (data & 0x80) | (data >> 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SRL:
      flag_bits = data & 0x01;
      rslt = (data >> 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SUB:
      rslt = REG_A - data;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SWAP:
      rslt = (data >> 4) | (data << 4);
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_TNZ:
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, data, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_XOR:
      rslt = REG_A ^ data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_none(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t eff_addr;

  switch (table_stm8->instr_enum)
  {
    case STM8_BREAK:
      SET_I1();
      SET_I0();
      return table_stm8->cycles_min;
    case STM8_CCF:
      if (GET_C() != 0)
      {
        CLR_C();
      }
      else
      {
        SET_C();
      }
      return table_stm8->cycles_min;
    case STM8_HALT:
      SET_I1();
      CLR_I0();
      return table_stm8->cycles_min;
    case STM8_IRET:
      REG_CC = POP_STACK();
      REG_A = POP_STACK();
      REG_X = POP_STACK16();
      REG_Y = POP_STACK16();
      eff_addr = POP_STACK24();
      if (eff_addr >= simulate->memory->size)
      {
        return INVALID_MEM_ADDR;
      }
      else
      {
        REG_PC = eff_addr;
        return table_stm8->cycles_min;
      }
    case STM8_NOP:
      return table_stm8->cycles_min;
    case STM8_RCF:
      CLR_C();
      return table_stm8->cycles_min;
    case STM8_RET:
      REG_PC = REG_PC & 0xff0000;
      REG_PC |= POP_STACK16();
      return table_stm8->cycles_min;
    case STM8_RETF:
      eff_addr = POP_STACK24();
      if (eff_addr >= simulate->memory->size)
      {
        return INVALID_MEM_ADDR;
      }
      else
      {
        REG_PC = eff_addr;
        return table_stm8->cycles_min;
      }
    case STM8_RIM:
      SET_I1();
      CLR_I0();
      return table_stm8->cycles_min;
    case STM8_RVF:
      CLR_V();
      return table_stm8->cycles_min;
    case STM8_SCF:
      SET_C();
      return table_stm8->cycles_min;
    case STM8_SIM:
      SET_I1();
      SET_I0();
      return table_stm8->cycles_min;
    case STM8_TRAP:
      eff_addr = READ_RAM24(TRAP_VECTOR + 1);
      if (eff_addr >= simulate->memory->size)
      {
        return INVALID_MEM_ADDR;
      }
      else
      {
        PUSH_STACK24(REG_PC);
        PUSH_STACK16(REG_Y);
        PUSH_STACK16(REG_X);
        PUSH_STACK(REG_A);
        PUSH_STACK(REG_CC);
        SET_I1();
        SET_I0();
        REG_PC = eff_addr;
        return table_stm8->cycles_min;
      }
    case STM8_WFE:
      return table_stm8->cycles_min;
    case STM8_WFI:
      SET_I1();
      CLR_I0();
      return table_stm8->cycles_min;
    default:
      return UNKNOWN_INST;
  }
}

// #$10
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_number8(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint8_t data;

  next_byte = READ_RAM(REG_PC++);
  data = next_byte;

  switch (table_stm8->instr_enum)
  {
    case STM8_ADDW:
      REG_SP += data;
      return table_stm8->cycles_min;
    case STM8_PUSH:
      PUSH_STACK(data);
      return table_stm8->cycles_min;
    case STM8_SUB:
      switch (table_stm8->dest)
      {
        case OP_REG_A:
          return simulate_execute_stm8_op_common(simulate, table_stm8, data);
        case OP_SP:
          REG_SP -= data;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_CALL:
    case STM8_CLR:
    case STM8_CPW:
    case STM8_CPL:
    case STM8_DEC:
    case STM8_INC:
    case STM8_JP:
    case STM8_LDW:
    case STM8_NEG:
    case STM8_RLC:
    case STM8_RRC:
    case STM8_SLL:
    case STM8_SRA:
    case STM8_SRL:
    case STM8_SWAP:
    case STM8_TNZ:
      return UNKNOWN_INST;

    default:
      return simulate_execute_stm8_op_common(simulate, table_stm8, data);
  }
}

// #$1000
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_number16(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint16_t data;
  uint8_t flag_bits;
  uint16_t rslt;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  data = next_word;

  switch (table_stm8->instr_enum)
  {
    case STM8_ADDW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = REG_X + data;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = REG_Y + data;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_CPW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = REG_X - data;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data, rslt, SIZE_16BITS);
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = REG_Y - data;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data, rslt, SIZE_16BITS);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_LDW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = data;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = data;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SUBW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = REG_X - data;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = REG_Y - data;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// $10
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address8(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint32_t eff_addr;

  next_byte = READ_RAM(REG_PC++);
  eff_addr = next_byte;

  switch (table_stm8->instr_enum)
  {
    case STM8_CALL:
    case STM8_JP:
      return UNKNOWN_INST;

    default:
      return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
  }
}

// $1000
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address16(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint32_t eff_addr;
  uint8_t data;
  uint16_t data16;
  uint8_t flag_bits;
  uint16_t rslt;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  eff_addr = next_word;
  data = READ_RAM(eff_addr);

  switch (table_stm8->instr_enum)
  {
    case STM8_ADC:
      rslt = REG_A + data + GET_C();
      flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_ADD:
      rslt = REG_A + data;
      flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_ADDW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_X + data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data16, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_Y + data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data16, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_AND:
      rslt = REG_A & data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_BCP:
      rslt = REG_A & data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_CALL:
      PUSH_STACK16(REG_PC);
      REG_PC = (REG_PC & 0xff0000) | eff_addr;
      return table_stm8->cycles_min;
    case STM8_CLR:
      WRITE_RAM(eff_addr, 0);
      CLR_N();
      SET_Z();
      return table_stm8->cycles_min;
    case STM8_CP:
      rslt = REG_A - data;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_CPW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_X - data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data16, rslt, SIZE_16BITS);
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_Y - data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data16, rslt, SIZE_16BITS);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_CPL:
      rslt = data ^= 0xff;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      SET_C();
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_DEC:
      rslt = data - 1;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, data, 1, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_EXG:
      WRITE_RAM(eff_addr, REG_A);
      REG_A = data;
      return table_stm8->cycles_min;
    case STM8_INC:
      rslt = data + 1;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, data, 1, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_JP:
      REG_PC = (REG_PC & 0xff0000) | eff_addr;
      return table_stm8->cycles_min;
    case STM8_LD:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = data;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }
    case STM8_LDW:
      if (table_stm8->dest == OP_REG_X)
      {
        REG_X = READ_RAM16(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_X)
      {
        rslt = REG_X;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
        WRITE_RAM16(eff_addr, rslt);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->dest == OP_REG_Y)
      {
        REG_Y = READ_RAM16(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_Y, SIZE_16BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_Y)
      {
        rslt = REG_Y;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
        WRITE_RAM16(eff_addr, rslt);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }
    case STM8_NEG:
      rslt = 0 - data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, data, 0, rslt, SIZE_8BITS);
      if (data == 0x80)
      {
        SET_V();
      }
      else
      {
        CLR_V()
      }

      if (data != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C()
      }
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_OR:
      rslt = REG_A | data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_POP:
      rslt = POP_STACK();
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_PUSH:
      PUSH_STACK(data);
      return table_stm8->cycles_min;
    case STM8_RLC:
      flag_bits = data & 0x80;
      rslt = (data << 1) + GET_C();
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_RRC:
      flag_bits = data & 0x01;
      rslt = (data >> 1) | (GET_C() << 7);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SBC:
      rslt = REG_A - data - GET_C();
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SLL:
      flag_bits = data & 0x80;
      rslt = (data << 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SRA:
      flag_bits = data & 0x01;
      rslt = (data & 0x80) | (data >> 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SRL:
      flag_bits = data & 0x01;
      rslt = (data >> 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_SUB:
      rslt = REG_A - data;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, data, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SUBW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_X - data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data16, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_Y - data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data16, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SWAP:
      rslt = (data >> 4) | (data << 4);
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      WRITE_RAM(eff_addr, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_TNZ:
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, data, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_XOR:
      rslt = REG_A ^ data;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;

    default:
      return UNKNOWN_INST;
  }
}

// $35aa00
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address24(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t next_ext;
  uint32_t eff_addr;
  uint8_t flag_bits;
  uint16_t rslt;

  next_ext = READ_RAM(REG_PC++) << 16;
  next_ext += READ_RAM(REG_PC++) << 8;
  next_ext += READ_RAM(REG_PC++);
  eff_addr = next_ext;
  if (eff_addr >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_CALLF:
      PUSH_STACK24(REG_PC);
      REG_PC = eff_addr;
      return table_stm8->cycles_min;
    case STM8_INT:
      return table_stm8->cycles_min;
    case STM8_JPF:
      REG_PC = eff_addr;
      return table_stm8->cycles_min;
    case STM8_LDF:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = READ_RAM(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// (X)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_index_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t eff_addr;

  eff_addr = REG_X;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ($10,X)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset8_index_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint32_t eff_addr;

  next_byte = READ_RAM(REG_PC++);
  eff_addr = REG_X + next_byte;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ($1000,X)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset16_index_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint32_t eff_addr;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  eff_addr = REG_X + next_word;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);

}

// ($500000,X)
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset24_index_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t next_ext;
  uint32_t eff_addr;
  uint8_t flag_bits;
  uint16_t rslt;

  next_ext = READ_RAM(REG_PC++) << 16;
  next_ext += READ_RAM(REG_PC++) << 8;
  next_ext += READ_RAM(REG_PC++);
  eff_addr = REG_X + next_ext;
  if (eff_addr >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_LDF:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = READ_RAM(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// (Y)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_index_y(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t eff_addr;

  eff_addr = REG_Y;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ($10,Y)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset8_index_y(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint32_t eff_addr;

  next_byte = READ_RAM(REG_PC++);
  eff_addr = REG_Y + next_byte;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ($1000,Y)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset16_index_y(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint32_t eff_addr;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  eff_addr = REG_Y + next_word;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ($500000,Y)
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset24_index_y(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t next_ext;
  uint32_t eff_addr;
  uint8_t flag_bits;
  uint16_t rslt;

  next_ext = READ_RAM(REG_PC++) << 16;
  next_ext += READ_RAM(REG_PC++) << 8;
  next_ext += READ_RAM(REG_PC++);
  eff_addr = REG_Y + next_ext;
  if (eff_addr >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_LDF:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = READ_RAM(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// ($10,SP)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_offset8_index_sp(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint32_t eff_addr;
  uint16_t data16;
  uint8_t flag_bits;
  uint16_t rslt;

  next_byte = READ_RAM(REG_PC++);
  eff_addr = REG_SP + next_byte;

  switch (table_stm8->instr_enum)
  {
    case STM8_ADDW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_X + data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data16, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_Y + data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data16, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SUBW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_X - data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, data16, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data16 = READ_RAM16(eff_addr);
          rslt = REG_Y - data16;
          flag_bits = BV(CC_V_FLAG) | BV(CC_H_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | BV(CC_C_FLAG) | OP_SUB_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, data16, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_CALL:
    case STM8_JP:
      return UNKNOWN_INST;

    default:
      return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
  }
}

// [$10.w]
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect8(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint16_t ptr_addr;
  uint32_t eff_addr;

  next_byte = READ_RAM(REG_PC++);
  ptr_addr = READ_RAM16(next_byte);
  eff_addr = ptr_addr;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// [$1000.w]
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect16(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint16_t ptr_addr;
  uint32_t eff_addr;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  ptr_addr = READ_RAM16(next_word);
  eff_addr = ptr_addr;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// [$1000.e]
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect16_e(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint32_t eff_addr;
  uint8_t flag_bits;
  uint16_t rslt;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  eff_addr = READ_RAM24(next_word);
  if (eff_addr >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_CALLF:
      PUSH_STACK24(REG_PC);
      REG_PC = eff_addr;
      return table_stm8->cycles_min;
    case STM8_JPF:
      REG_PC = eff_addr;
      return table_stm8->cycles_min;
    case STM8_LDF:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = READ_RAM(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// ([$10.w],X)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect8_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint16_t ptr_addr;
  uint32_t eff_addr;

  next_byte = READ_RAM(REG_PC++);
  ptr_addr = READ_RAM16(next_byte);
  eff_addr = REG_X + ptr_addr;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ([$1000.w],X)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect16_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint16_t ptr_addr;
  uint32_t eff_addr;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  ptr_addr = READ_RAM16(next_word);
  eff_addr = REG_X + ptr_addr;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ([$1000.e],X)
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect16_e_x(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint32_t eff_addr;
  uint8_t flag_bits;
  uint16_t rslt;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  eff_addr = READ_RAM24(next_word);
  eff_addr += REG_X;
  if (eff_addr >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_LDF:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = READ_RAM(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// ([$10.w],Y)
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect8_y(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint16_t ptr_addr;
  uint32_t eff_addr;

  next_byte = READ_RAM(REG_PC++);
  ptr_addr = READ_RAM16(next_byte);
  eff_addr = REG_Y + ptr_addr;
  return simulate_execute_stm8_op_common(simulate, table_stm8, eff_addr);
}

// ([$1000.e],Y)
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_indirect16_e_y(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint32_t eff_addr;
  uint8_t flag_bits;
  uint16_t rslt;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  eff_addr = READ_RAM24(next_word);
  eff_addr += REG_Y;
  if (eff_addr >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_LDF:
      if (table_stm8->dest == OP_REG_A)
      {
        REG_A = READ_RAM(eff_addr);
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
        return table_stm8->cycles_min;
      }
      else if (table_stm8->src == OP_REG_A)
      {
        rslt = REG_A;
        flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
        WRITE_RAM(eff_addr, rslt & 0xff);
        return table_stm8->cycles_min;
      }
      else
      {
        return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// $1000, #2
// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address_bit(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint32_t op_pc = REG_PC - 1;            // kluge! - actual opcode not available in table_stm8
  uint8_t pos = 0;
  uint16_t next_word;
  uint8_t data;
  uint16_t rslt;

  pos = (READ_RAM(op_pc) >> 1) & 0x07;    // bit position is encoded in opcode
  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  data = READ_RAM(next_word);

  switch (table_stm8->instr_enum)
  {
    case STM8_BCCM:
      rslt = (data & ~(1 << pos)) | (GET_C() << pos);
      WRITE_RAM(next_word, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_BCPL:
      rslt = data ^ (1 << pos);
      WRITE_RAM(next_word, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_BRES:
      rslt = data & ~(1 << pos);
      WRITE_RAM(next_word, rslt & 0xff);
      return table_stm8->cycles_min;
    case STM8_BSET:
      rslt = data | (1 << pos);
      WRITE_RAM(next_word, rslt & 0xff);
      return table_stm8->cycles_min;

    default:
      return UNKNOWN_INST;
  }
}

// $1000, #2, loop
// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address_bit_loop(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  int32_t pc;
  uint32_t op_pc = REG_PC - 1;            // kluge! - actual opcode not available in table_stm8
  uint8_t pos;
  uint16_t next_word;
  int8_t offset;
  uint8_t data;
  uint16_t rslt;

  pos = (READ_RAM(op_pc) >> 1) & 0x07;    // bit position is encoded in opcode
  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  offset = READ_RAM(REG_PC++);
  pc = (int32_t)REG_PC;
  pc += offset;
  if ((uint32_t)pc >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }
  data = READ_RAM(next_word);

  switch (table_stm8->instr_enum)
  {
    case STM8_BTJF:
      rslt = data & (1 << pos);
      if (rslt == 0)
      {
        CLR_C();
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        SET_C();
        return table_stm8->cycles_min;
      }
    case STM8_BTJT:
      rslt = data & (1 << pos);
      if (rslt != 0)
      {
        SET_C();
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        CLR_C();
        return table_stm8->cycles_min;
      }

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_relative(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  int32_t pc;
  int8_t offset;

  offset = READ_RAM(REG_PC++);
  pc = (int32_t)REG_PC;
  pc += offset;
  if ((uint32_t)pc >= simulate->memory->size)
  {
    return INVALID_MEM_ADDR;
  }

  switch (table_stm8->instr_enum)
  {
    case STM8_CALLR:
      PUSH_STACK16(REG_PC);
      REG_PC = (uint32_t)pc;
      return table_stm8->cycles_min;
    case STM8_JRA:
      REG_PC = (uint32_t)pc;
      return table_stm8->cycles_min;
    case STM8_JRC:
      if (GET_C() != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JREQ:
      if (GET_Z() != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRF:
      if (1 == 0)     // never jump
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRH:
      if (GET_H() != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRIH:
      // see Errata
      return table_stm8->cycles_min;
    case STM8_JRIL:
      // see Errata
      REG_PC = (uint32_t)pc;
      return table_stm8->cycles_min;
    case STM8_JRM:
      if ((GET_I1() != 0) && (GET_I0() != 0))   // interrupts masked
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRMI:
      if (GET_N() != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRNC:
      if (GET_C() == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRNE:
      if (GET_Z() == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRNH:
      if (GET_H() == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRNM:
      if ((GET_I1() != 0) && (GET_I0() != 0))   // interrupts masked
      {
        return table_stm8->cycles_min;
      }
      else
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
    case STM8_JRNV:
      if (GET_V() == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRPL:
      if (GET_N() == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRSGE:
      if ((GET_N() ^ GET_V()) == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRSGT:
      if ((GET_Z() | (GET_N() ^ GET_V())) == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRSLE:
      if ((GET_Z() | (GET_N() ^ GET_V())) != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRSLT:
      if ((GET_N() ^ GET_V()) != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRT:
      if (1 == 1)     // always jump
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRUGE:
      if (GET_C() == 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRUGT:
      if ((GET_C() == 0) && (GET_Z() == 0))
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRULE:
      if ((GET_C() != 0) || (GET_Z() != 0))
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRULT:
      if (GET_C() != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }
    case STM8_JRV:
      if (GET_V() != 0)
      {
        REG_PC = (uint32_t)pc;
        return table_stm8->cycles_max;
      }
      else
      {
        return table_stm8->cycles_min;
      }

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_single_register(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t flag_bits;
  uint16_t rslt;

  switch (table_stm8->instr_enum)
  {
    case STM8_CLR:
      REG_A = 0;
      CLR_N();
      SET_Z();
      return table_stm8->cycles_min;
    case STM8_CLRW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          REG_X = 0;
          CLR_N();
          SET_Z();
          return table_stm8->cycles_min;
        case OP_REG_Y:
          REG_Y = 0;
          CLR_N();
          SET_Z();
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_CPL:
      REG_A ^= 0xff;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
      SET_C();
      return table_stm8->cycles_min;
    case STM8_CPLW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          REG_X ^= 0xffff;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
          SET_C();
          return table_stm8->cycles_min;
        case OP_REG_Y:
          REG_Y ^= 0xffff;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_Y, SIZE_16BITS);
          SET_C();
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_DEC:
      rslt = REG_A - 1;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, 1, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_DECW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = REG_X - 1;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, 1, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = REG_Y - 1;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, 1, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_INC:
      rslt = REG_A + 1;
      flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
      calculate_flags(simulate, flag_bits, REG_A, 1, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_INCW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = REG_X + 1;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_X, 1, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = REG_Y + 1;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG) | OP_ADD_TYPE;
          calculate_flags(simulate, flag_bits, REG_Y, 1, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_NEG:
      rslt = 0 - REG_A;
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, REG_A, 0, rslt, SIZE_8BITS);
      if (REG_A == 0x80)
      {
        SET_V();
      }
      else
      {
        CLR_V()
      }

      if (REG_A != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C()
      }
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_NEGW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = 0 - REG_X;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, REG_X, 0, rslt, SIZE_16BITS);
          if (REG_X == 0x8000)
          {
            SET_V();
          }
          else
          {
            CLR_V()
          }

          if (REG_X != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C()
          }
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = 0 - REG_Y;
          flag_bits = BV(CC_V_FLAG) | BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, REG_Y, 0, rslt, SIZE_16BITS);
          if (REG_Y == 0x8000)
          {
            SET_V();
          }
          else
          {
            CLR_V()
          }

          if (REG_Y != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C()
          }
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_POP:
      switch (table_stm8->dest)
      {
        case OP_REG_A:
          REG_A = POP_STACK();
          return table_stm8->cycles_min;
        case OP_REG_CC:
          REG_CC = POP_STACK();
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_POPW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          REG_X = POP_STACK16();
          return table_stm8->cycles_min;
        case OP_REG_Y:
          REG_Y = POP_STACK16();
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_PUSH:
      switch (table_stm8->dest)
      {
        case OP_REG_A:
          PUSH_STACK(REG_A);
          return table_stm8->cycles_min;
        case OP_REG_CC:
          PUSH_STACK(REG_CC);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_PUSHW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          PUSH_STACK16(REG_X);
          return table_stm8->cycles_min;
        case OP_REG_Y:
          PUSH_STACK16(REG_Y);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_RLC:
      flag_bits = REG_A & 0x80;
      rslt = (REG_A << 1) + GET_C();
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_RLCW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          flag_bits = REG_X >> 15;
          rslt = (REG_X << 1) + GET_C();
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          flag_bits = REG_Y >> 15;
          rslt = (REG_Y << 1) + GET_C();
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_RRC:
      flag_bits = REG_A & 0x01;
      rslt = (REG_A >> 1) | (GET_C() << 7);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_RRCW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          flag_bits = REG_X & 0x0001;
          rslt = (REG_X >> 1) | (GET_C() << 15);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          flag_bits = REG_Y & 0x0001;
          rslt = (REG_Y >> 1) | (GET_C() << 15);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SLL:
      flag_bits = REG_A & 0x80;
      rslt = (REG_A << 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SLLW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          flag_bits = REG_X >> 15;
          rslt = (REG_X << 1);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          flag_bits = REG_Y >> 15;
          rslt = (REG_Y << 1);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SRA:
      flag_bits = REG_A & 0x01;
      rslt = (REG_A & 0x80) | (REG_A >> 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SRAW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          flag_bits = REG_X & 0x0001;
          rslt = (REG_X & 0x8000) | (REG_X >> 1);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          flag_bits = REG_Y & 0x0001;
          rslt = (REG_Y & 0x8000) | (REG_Y >> 1);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SRL:
      flag_bits = REG_A & 0x01;
      rslt = (REG_A >> 1);
      if (flag_bits != 0)
      {
        SET_C();
      }
      else
      {
        CLR_C();
      }
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SRLW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          flag_bits = REG_X & 0x0001;
          rslt = (REG_X >> 1);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          flag_bits = REG_Y & 0x0001;
          rslt = (REG_Y >> 1);
          if (flag_bits != 0)
          {
            SET_C();
          }
          else
          {
            CLR_C();
          }
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_SWAP:
      rslt = (REG_A >> 4) | (REG_A << 4);
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_8BITS);
      REG_A = rslt & 0xff;
      return table_stm8->cycles_min;
    case STM8_SWAPW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          rslt = (REG_X >> 8) | (REG_X << 8);
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_X = rslt;
          return table_stm8->cycles_min;
        case OP_REG_Y:
          rslt = (REG_Y >> 8) | (REG_Y << 8);
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, rslt, SIZE_16BITS);
          REG_Y = rslt;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_TNZ:
      flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
      calculate_flags(simulate, flag_bits, 0, 0, REG_A, SIZE_8BITS);
      return table_stm8->cycles_min;
    case STM8_TNZW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
          return table_stm8->cycles_min;
        case OP_REG_Y:
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_Y, SIZE_16BITS);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_two_registers(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t flag_bits;
  uint8_t data;
  uint16_t data16;

  switch (table_stm8->instr_enum)
  {
    case STM8_DIV:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          if (REG_A != 0)
          {
            data16 = REG_X / REG_A;
            REG_A = REG_X % REG_A;
            REG_X = data16;
            CLR_V();
            CLR_H();
            CLR_N();
            flag_bits = BV(CC_Z_FLAG);
            calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
            CLR_C()
          }
          else  // divide-by-0
          {
            SET_C();
          }
          return table_stm8->cycles_min;
        case OP_REG_Y:
          if (REG_A != 0)
          {
            data16 = REG_Y / REG_A;
            REG_A = REG_Y % REG_A;
            REG_Y = data16;
            CLR_V();
            CLR_H();
            CLR_N();
            flag_bits = BV(CC_Z_FLAG);
            calculate_flags(simulate, flag_bits, 0, 0, REG_Y, SIZE_16BITS);
            CLR_C()
          }
          else  // divide-by-0
          {
            SET_C();
          }
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_DIVW:
      if (REG_Y != 0)
      {
        data16 = REG_X / REG_Y;
        REG_Y = REG_X % REG_Y;
        REG_X = data16;
        CLR_V();
        CLR_H();
        CLR_N();
        flag_bits = BV(CC_Z_FLAG);
        calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
        CLR_C()
      }
      else  // divide-by-0
      {
        SET_C();
      }
      return table_stm8->cycles_min;
    case STM8_EXG:
      switch (table_stm8->src)
      {
        case OP_REG_XL:
          data = REG_X & 0xff;
          REG_X = (REG_X & 0xff00) | REG_A;
          REG_A = data;
          return table_stm8->cycles_min;
        case OP_REG_YL:
          data = REG_Y & 0xff;
          REG_Y = (REG_Y & 0xff00) | REG_A;
          REG_A = data;
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_EXGW:
      data16 = REG_Y;
      REG_Y = REG_X;
      REG_X = data16;
      return table_stm8->cycles_min;
    case STM8_LD:
      if (table_stm8->dest == OP_REG_A)
      {
        switch (table_stm8->src)
        {
          case OP_REG_XL:
            REG_A = REG_X & 0xff;
            return table_stm8->cycles_min;
          case OP_REG_YL:
            REG_A = REG_Y & 0xff;
            return table_stm8->cycles_min;
          case OP_REG_XH:
            REG_A = REG_X >> 8;
            return table_stm8->cycles_min;
          case OP_REG_YH:
            REG_A = REG_Y >> 8;
            return table_stm8->cycles_min;
          default:
            return UNKNOWN_INST;
        }
      }
      else if (table_stm8->src == OP_REG_A)
      {
        switch (table_stm8->dest)
        {
          case OP_REG_XL:
            REG_X = (REG_X & 0xff00) | REG_A;
            return table_stm8->cycles_min;
          case OP_REG_YL:
            REG_Y = (REG_Y & 0xff00) | REG_A;
            return table_stm8->cycles_min;
          case OP_REG_XH:
            REG_X = (REG_X & 0x00ff) | (REG_A << 8);
            return table_stm8->cycles_min;
          case OP_REG_YH:
            REG_Y = (REG_Y & 0x00ff) | (REG_A << 8);
            return table_stm8->cycles_min;
          default:
            return UNKNOWN_INST;
        }
      }
      else
      {
        return UNKNOWN_INST;
      }
    case STM8_LDW:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          switch (table_stm8->src)
          {
            case OP_REG_Y:
              REG_X = REG_Y;
              return table_stm8->cycles_min;
            case OP_SP:
              REG_X = REG_SP;
              return table_stm8->cycles_min;
            default:
              return UNKNOWN_INST;
          }
        case OP_REG_Y:
          switch (table_stm8->src)
          {
            case OP_REG_X:
              REG_Y = REG_X;
              return table_stm8->cycles_min;
            case OP_SP:
              REG_Y = REG_SP;
              return table_stm8->cycles_min;
            default:
              return UNKNOWN_INST;
          }
        case OP_SP:
          switch (table_stm8->src)
          {
            case OP_REG_X:
              REG_SP = REG_X;
              return table_stm8->cycles_min;
            case OP_REG_Y:
              REG_SP = REG_Y;
              return table_stm8->cycles_min;
            default:
              return UNKNOWN_INST;
          }
        default:
          return UNKNOWN_INST;
      }
    case STM8_MUL:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          REG_X = (REG_X & 0xff) * REG_A;
          CLR_H();
          CLR_C()
          return table_stm8->cycles_min;
        case OP_REG_Y:
          REG_Y = (REG_Y & 0xff) * REG_A;
          CLR_H();
          CLR_C()
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_RLWA:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data = REG_X >> 8;
          REG_X = (REG_X << 8) | REG_A;
          REG_A = data;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data = REG_Y >> 8;
          REG_Y = (REG_Y << 8) | REG_A;
          REG_A = data;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_Y, SIZE_16BITS);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }
    case STM8_RRWA:
      switch (table_stm8->dest)
      {
        case OP_REG_X:
          data = REG_X & 0xff;
          REG_X = (REG_A << 8) | (REG_X >> 8);
          REG_A = data;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_X, SIZE_16BITS);
          return table_stm8->cycles_min;
        case OP_REG_Y:
          data = REG_Y & 0xff;
          REG_Y = (REG_A << 8) | (REG_Y >> 8);
          REG_A = data;
          flag_bits = BV(CC_N_FLAG) | BV(CC_Z_FLAG);
          calculate_flags(simulate, flag_bits, 0, 0, REG_Y, SIZE_16BITS);
          return table_stm8->cycles_min;
        default:
          return UNKNOWN_INST;
      }

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address16_number8(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint8_t next_byte;

  next_byte = READ_RAM(REG_PC++);
  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;

  switch (table_stm8->instr_enum)
  {
    case STM8_MOV:
      WRITE_RAM(next_word, next_byte);
      return table_stm8->cycles_min;

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address8_address8(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t next_byte;
  uint8_t next_byte2;
  uint8_t data;

  next_byte = READ_RAM(REG_PC++);
  next_byte2 = READ_RAM(REG_PC++);

  switch (table_stm8->instr_enum)
  {
    case STM8_MOV:
      data = READ_RAM(next_byte);
      WRITE_RAM(next_byte2, data);
      return table_stm8->cycles_min;

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8_op_address16_address16(struct _simulate * simulate, struct _table_stm8_opcodes * table_stm8)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint16_t next_word;
  uint16_t next_word2;
  uint8_t data;

  next_word = READ_RAM16(REG_PC);
  REG_PC += 2;
  next_word2 = READ_RAM16(REG_PC);
  REG_PC += 2;

  switch (table_stm8->instr_enum)
  {
    case STM8_MOV:
      data = READ_RAM(next_word);
      WRITE_RAM(next_word2, data);
      return table_stm8->cycles_min;

    default:
      return UNKNOWN_INST;
  }
}

// Returns:
//    -1 = hit unknown instruction
//    -2 = hit unsupported memory address
//    else = number of cycles for simulated instruction
static int simulate_execute_stm8(struct _simulate * simulate)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  uint8_t opcode;
  uint8_t prefix = 0;
  int n;

  opcode = READ_RAM(REG_PC++);

  if ((opcode == PRECODE_PWSP) || (opcode == PRECODE_PDY) ||
      (opcode == PRECODE_PIY) || (opcode == PRECODE_PIX))
  {
    prefix = opcode;
    opcode = READ_RAM(REG_PC++);
  }

  n = 0;
  while (table_stm8_opcodes[n].instr_enum != STM8_NONE)
  {
    if (table_stm8_opcodes[n].prefix == prefix)
    {
      if (table_stm8_opcodes[n].opcode == opcode)
      {
        break;
      }

      // prefixed instructions with opcode 0001_xxxxb
      if ((prefix != 0) && ((table_stm8_opcodes[n].opcode & 0xf0) == 0x10) &&
          (table_stm8_opcodes[n].opcode == (opcode & 0xf1)))
      {
        break;
      }

      // PWSP prefixed instructions with opcode 0000_xxxxb
      if ((prefix == PRECODE_PWSP) && ((table_stm8_opcodes[n].opcode & 0xf0) == 0x00) &&
          (table_stm8_opcodes[n].opcode == (opcode & 0xf1)))
      {
        break;
      }
    }
    n++;
  }

  // If instruction can't be found return -1.
  if (table_stm8_opcodes[n].instr_enum == STM8_NONE)
  {
    return UNKNOWN_INST;  // Unknown instruction
  }

  switch (table_stm8_opcodes[n].type)
  {
    case OP_NONE:
      return simulate_execute_stm8_op_none(simulate, &table_stm8_opcodes[n]);
    case OP_NUMBER8:                // #$10
      return simulate_execute_stm8_op_number8(simulate, &table_stm8_opcodes[n]);
    case OP_NUMBER16:               // #$1000
      return simulate_execute_stm8_op_number16(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS8:               // $10
      return simulate_execute_stm8_op_address8(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS16:              // $1000
      return simulate_execute_stm8_op_address16(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS24:              // $35aa00
      return simulate_execute_stm8_op_address24(simulate, &table_stm8_opcodes[n]);
    case OP_INDEX_X:                // (X)
      return simulate_execute_stm8_op_index_x(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET8_INDEX_X:        // ($10,X)
      return simulate_execute_stm8_op_offset8_index_x(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET16_INDEX_X:       // ($1000,X)
      return simulate_execute_stm8_op_offset16_index_x(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET24_INDEX_X:       // ($500000,X)
      return simulate_execute_stm8_op_offset24_index_x(simulate, &table_stm8_opcodes[n]);
    case OP_INDEX_Y:                // (Y)
      return simulate_execute_stm8_op_index_y(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET8_INDEX_Y:        // ($10,Y)
      return simulate_execute_stm8_op_offset8_index_y(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET16_INDEX_Y:       // ($1000,Y)
      return simulate_execute_stm8_op_offset16_index_y(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET24_INDEX_Y:       // ($500000,Y)
      return simulate_execute_stm8_op_offset24_index_y(simulate, &table_stm8_opcodes[n]);
    case OP_OFFSET8_INDEX_SP:       // ($10,SP)
      return simulate_execute_stm8_op_offset8_index_sp(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT8:              // [$10.w]
      return simulate_execute_stm8_op_indirect8(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT16:             // [$1000.w]
      return simulate_execute_stm8_op_indirect16(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT16_E:           // [$1000.e]
      return simulate_execute_stm8_op_indirect16_e(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT8_X:            // ([$10.w],X)
      return simulate_execute_stm8_op_indirect8_x(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT16_X:           // ([$1000.w],X)
      return simulate_execute_stm8_op_indirect16_x(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT16_E_X:         // ([$1000.e],X)
      return simulate_execute_stm8_op_indirect16_e_x(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT8_Y:            // ([$10.w],Y)
      return simulate_execute_stm8_op_indirect8_y(simulate, &table_stm8_opcodes[n]);
    case OP_INDIRECT16_E_Y:         // ([$1000.e],Y)
      return simulate_execute_stm8_op_indirect16_e_y(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS_BIT:            // $1000, #2
      return simulate_execute_stm8_op_address_bit(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS_BIT_LOOP:       // $1000, #2, loop
      return simulate_execute_stm8_op_address_bit_loop(simulate, &table_stm8_opcodes[n]);
    case OP_RELATIVE:
      return simulate_execute_stm8_op_relative(simulate, &table_stm8_opcodes[n]);
    case OP_SINGLE_REGISTER:
      return simulate_execute_stm8_op_single_register(simulate, &table_stm8_opcodes[n]);
    case OP_TWO_REGISTERS:
      return simulate_execute_stm8_op_two_registers(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS16_NUMBER8:
      return simulate_execute_stm8_op_address16_number8(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS8_ADDRESS8:
      return simulate_execute_stm8_op_address8_address8(simulate, &table_stm8_opcodes[n]);
    case OP_ADDRESS16_ADDRESS16:
      return simulate_execute_stm8_op_address16_address16(simulate, &table_stm8_opcodes[n]);

    default:
      break;
  }

  return UNKNOWN_INST;  // Unknown instruction
}

// Returns:
//    -1 = hit unknown instruction or unsupported memory address
//     0 = OK
int simulate_run_stm8(struct _simulate * simulate, int max_cycles, int step)
{
  struct _simulate_stm8 * simulate_stm8 = (struct _simulate_stm8 *)simulate->context;
  char instruction[128];
  char bytes[20];

  stop_running = 0;
  signal(SIGINT, handle_signal);

  if (max_cycles != 0)
  {
    printf("Running... Press Ctl-C to break.\n");
  }

  while (stop_running == 0)
  {
    int ret;
    int n;
    uint32_t pc = REG_PC;

    ret = simulate_execute_stm8(simulate);

    if (ret > 0)
    {
      simulate->cycle_count += ret;
    }

    if (simulate->show == 1)
    {
      uint32_t disasm_pc = pc;

      simulate_dump_registers_stm8(simulate);

      n = 0;
      while (n < 6)
      {
        int cycles_min;
        int cycles_max;
        int count = disasm_stm8(simulate->memory, disasm_pc, instruction, &cycles_min, &cycles_max);
        int i;

        // check for and remove additional line separator
        i = (int)strlen(instruction);
        if (instruction[i - 1] == '\n')
        {
          instruction[i - 1] = '\0';
        }

        bytes[0] = 0;
        for (i = 0; i < count; i++)
        {
          char temp[4];

          sprintf(temp, "%02x ", READ_RAM(disasm_pc + i));
          strcat(bytes, temp);
        }

        if (cycles_min == -1)
        {
          break;
        }

        // '*' - breakpoint indicator
        // '!' - current instruction indicator
        // '>' - next instruction indicator

        printf("%s", disasm_pc == simulate->break_point ? "*" : " ");   // breakpoint

        if (n == 0)
        {
          printf("! ");     // current instruction
        }
        else if (disasm_pc == REG_PC)
        {
          printf("> ");     // next instruction
        }
        else
        {
          printf("  ");
        }

        if (cycles_min < 1)
        {
          printf("0x%04x: %-15s %-35s ?\n", disasm_pc, bytes, instruction);
        }
        else if (cycles_min == cycles_max)
        {
          printf("0x%04x: %-15s %-35s %d\n", disasm_pc, bytes, instruction, cycles_min);
        }
        else
        {
          printf("0x%04x: %-15s %-35s %d-%d\n", disasm_pc, bytes, instruction, cycles_min, cycles_max);
        }

        if (count == 0)
        {
          break;
        }

        ++n;
        disasm_pc += count;
      }
    }

    if ((simulate->auto_run == 1) && (simulate->nested_call_count < 0))
    {
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    if (ret == UNKNOWN_INST)
    {
      signal(SIGINT, SIG_DFL);
      printf("Unknown instruction at address 0x%06x\n", pc);
      return -1;
    }
    else if (ret == INVALID_MEM_ADDR)
    {
      signal(SIGINT, SIG_DFL);
      printf("Unsupported memory space access at address 0x%06x\n", pc);
      return -1;
    }

    if (simulate->break_point == REG_PC)
    {
      printf("Breakpoint hit at 0x%04x\n", simulate->break_point);
      break;
    }

    if (REG_PC >= simulate->memory->size)
    {
      printf("End of memory - setting PC to reset vector.\n");
      simulate->step_mode = 0;

      REG_PC = 0;
      if (stm8_int_opcode > 0)
      {
        if (READ_RAM(RESET_VECTOR) == stm8_int_opcode)
        {
          REG_PC = READ_RAM24(RESET_VECTOR + 1);
          if (REG_PC >= simulate->memory->size)   // check supported memory space
          {
            REG_PC = 0;
          }
        }
      }

      break;
    }

    if ((simulate->usec == 0) || (step == 1))
    {
      signal(SIGINT, SIG_DFL);
      return 0;
    }

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%06x.\n", REG_PC);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

