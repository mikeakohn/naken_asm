/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/68000.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "disasm/arm.h"
#include "table/68000.h"

#define NO_EXTRA_IMM 0xffffffff

enum
{
  OPERAND_D_REG,
  OPERAND_A_REG,
  OPERAND_A_REG_INDEX,
  OPERAND_A_REG_INDEX_PLUS,
  OPERAND_A_REG_INDEX_MINUS,
  OPERAND_INDEX_DATA16_A_REG,
  OPERAND_INDEX_DATA8_A_REG_XN,
  OPERAND_IMMEDIATE,
  OPERAND_INDEX_DATA16_PC,
  OPERAND_INDEX_DATA8_PC_XN,
  OPERAND_ADDRESS,                // for branches/jumps
  OPERAND_ADDRESS_W,              // for word memory writes
  OPERAND_ADDRESS_L,              // for long memory writes
  OPERAND_SPECIAL_REG,
  OPERAND_MULTIPLE_REG,
};

enum
{
  SIZE_NONE=-1,
  SIZE_B,
  SIZE_W,
  SIZE_L,
  SIZE_S,
};

enum
{
  SPECIAL_CCR,
  SPECIAL_SR,
};

enum
{
  EA_NOFLAG=0,
  EA_NO_PC=1,
  EA_NO_IMM=2,
  EA_NO_A=4,
  EA_NO_D=8,
  EA_NO_PLUS=16,
  EA_NO_MINUS=32,
  //EA_SKIP_BYTES=64,
};

struct _operand
{
  int value;
  int type;
  //char error;
  char dis_reg;
  char xn_reg;
  char xn_size;
};

static int get_register_d_68000(char *token)
{
  if (token[0] != 'd' && token[0] != 'D') return -1;
  if (token[1] >= '0' && token[1] <= '7' && token[2] == 0)
  {
    return token[1] - '0';
  }

  return -1;
}

static int get_register_a_68000(char *token)
{
  if (strcasecmp(token, "sp") == 0) return 7;
  if (token[0] != 'a' && token[0] != 'A') return -1;
  if (token[1] >= '0' && token[1] <= '7' && token[2] == 0)
  {
    return token[1] - '0';
  }

  return -1;
}

static int get_register_special_68000(char *token)
{
  if (strcasecmp(token,"sr") == 0) { return SPECIAL_SR; }
  if (strcasecmp(token,"ccr") == 0) { return SPECIAL_CCR; }

  return -1;
}

#if 0
static int calc_branch_size(int address, int new_address)
{
  int offset;

  offset = new_address - (address + 2);
  if (offset >= -128 && offset <= 127) { return SIZE_B; }
  offset = new_address - (address + 4);
  if (offset >= -32768 && offset <= 32767) { return SIZE_W; }

#if WE_SUPPORT_68020
  return SIZE_L;
#endif

  return SIZE_W;
}
#endif

static int check_size(int size, uint8_t omit_size)
{
  if (omit_size == NO_SIZE)
  {
    return (size == SIZE_NONE) ? 0 : -1;
  }

  if (size == SIZE_NONE) { return -1; }
  if (size == SIZE_B && (omit_size & OMIT_B) != 0) { return -1; }
  if (size == SIZE_W && (omit_size & OMIT_W) != 0) { return -1; }
  if (size == SIZE_L && (omit_size & OMIT_L) != 0) { return -1; }

  return 0;
}

static int check_reg(int type, uint8_t omit_mode)
{
  switch (type)
  {
    case OPERAND_D_REG:
      if ((omit_mode & MODE_DN) != 0) { return -1; }
      break;
    case OPERAND_A_REG:
      if ((omit_mode & MODE_AN) != 0) { return -1; }
      break;
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
      if ((omit_mode & MODE_AN_P) != 0) { return -1; }
      break;
    case OPERAND_A_REG_INDEX_MINUS:
      if ((omit_mode & MODE_AN_N) != 0) { return -1; }
      break;
    //case OPERAND_INDEX_DATA16_A_REG:
    //case OPERAND_INDEX_DATA8_A_REG_XN:
    case OPERAND_IMMEDIATE:
      if ((omit_mode & MODE_IMM) != 0) { return -1; }
      break;
    //case OPERAND_INDEX_DATA16_PC:
    //case OPERAND_INDEX_DATA8_PC_XN:
    //case OPERAND_ADDRESS:
    //case OPERAND_ADDRESS_W:
    //case OPERAND_ADDRESS_L:
    default:
      break;
  }

  return 0;
}

static int ea_immediate(
  struct _asm_context *asm_context,
  int opcode,
  int size,
  struct _operand *operand)
{
  add_bin16(asm_context, opcode | (operand->type << 3) | 0x4, IS_OPCODE);

  if (size == SIZE_L)
  {
    add_bin32(asm_context, operand->value, IS_OPCODE);
    return 6;
  }
    else
  {
    if (operand->value < -32768 || operand->value > 65535)
    {
      print_error_range("Immediate", -32768, 65535, asm_context);
      return -1;
    }
    add_bin16(asm_context, operand->value, IS_OPCODE);
    return 4;
  }
}

static int ea_address(
  struct _asm_context *asm_context,
  int opcode,
  struct _operand *operand,
  uint32_t extra_imm,
  int size)
{
  uint32_t value;
  int len;

  if (asm_context->pass == 1)
  {
    len = 4;
    if (size == 4)
    {
      add_bin16(asm_context, 0x0404, IS_OPCODE);
      add_bin16(asm_context, 0x0404, IS_OPCODE);
      add_bin16(asm_context, 0x0404, IS_OPCODE);
      len += 2;
    }
      else
    {
      add_bin16(asm_context, 0x0000, IS_OPCODE);
      add_bin16(asm_context, 0x0000, IS_OPCODE);
    }

    if (extra_imm != NO_EXTRA_IMM)
    {
      add_bin16(asm_context, extra_imm, IS_OPCODE);
      len += 2;
    }

    return len;
  }

  value = operand->value;
  len = 4;

  add_bin16(asm_context, opcode | (0x7 << 3) | ((size == 4) ? 1 : 0), IS_OPCODE);

  if (extra_imm != NO_EXTRA_IMM)
  {
    add_bin16(asm_context, extra_imm, IS_OPCODE);
    len += 2;
  }

  if (size == 4)
  {
    add_bin32(asm_context, value, IS_OPCODE);
    len += 2;
  }
    else
  {
    add_bin16(asm_context, value, IS_OPCODE);
  }

  return len;
}

static int ea_displacement(struct _asm_context *asm_context, int opcode, struct _operand *operand)
{
  if (operand->value < -32768 || operand->value > 32767)
  {
    print_error_range("Displacement", -32768, 32767, asm_context);
    return -1;
  }

  if (operand->type == OPERAND_INDEX_DATA16_A_REG)
  {
    add_bin16(asm_context, opcode | (0x5 << 3) | operand->dis_reg, IS_OPCODE);
  }
    else
  if (operand->type == OPERAND_INDEX_DATA16_PC)
  {
    add_bin16(asm_context, opcode | (0x7 << 3) | 0x2, IS_OPCODE);
  }

  add_bin16(asm_context, operand->value, IS_OPCODE);

  return 4;
}

static int ea_displacement_xn(struct _asm_context *asm_context, int opcode, struct _operand *operand)
{
  if (operand->value < -128 || operand->value > 127)
  {
    print_error_range("Displacement", -128, 127, asm_context);
    return -1;
  }

  if (operand->type == OPERAND_INDEX_DATA8_A_REG_XN)
  {
    add_bin16(asm_context, opcode | (0x6 << 3) | operand->dis_reg, IS_OPCODE);
  }
    else
  if (operand->type == OPERAND_INDEX_DATA8_PC_XN)
  {
    add_bin16(asm_context, opcode | (0x7 << 3) | 0x3, IS_OPCODE);
  }

  // [D=0/A=1] [REG3] [W=0/L=1] [000] [DISP8]
  //add_bin16(asm_context, (operand->xn_reg << 12) | (operand->xn_size == SIZE_L ? (1 << 11) | (operand->value & 0xff) : 0), IS_OPCODE);
  add_bin16(asm_context, (operand->xn_reg << 12) | (operand->xn_size == SIZE_L ? (1 << 11) : 0) | (operand->value & 0xff), IS_OPCODE);

  return 4;
}

static int ea_generic_all(struct _asm_context *asm_context, struct _operand *operand, char *instr, int opcode, int size, int flags, uint32_t extra_imm)
{
#if 0
  if (flags&EA_SKIP_BYTES)
  {
    skip
    extra_imm=NO_EXTRA_IMM;
  }
#endif

  switch (operand->type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      if ((flags & EA_NO_D) && operand->type == OPERAND_D_REG) { break; }
      if ((flags & EA_NO_A) && operand->type == OPERAND_A_REG) { break; }
      if ((flags & EA_NO_PLUS) && operand->type == OPERAND_A_REG_INDEX_PLUS) { break;}
      if ((flags & EA_NO_MINUS) && operand->type == OPERAND_A_REG_INDEX_MINUS) { break; }
      add_bin16(asm_context, opcode | (operand->type << 3) | operand->value, IS_OPCODE);
      if (extra_imm != NO_EXTRA_IMM)
      {
        add_bin16(asm_context, extra_imm, IS_OPCODE);
        return 4;
      }
      return 2;
    case OPERAND_INDEX_DATA16_A_REG:
      return ea_displacement(asm_context, opcode, operand);
    case OPERAND_INDEX_DATA8_A_REG_XN:
      return ea_displacement_xn(asm_context, opcode, operand);
    case OPERAND_ADDRESS_W:
      return ea_address(asm_context, opcode, operand, extra_imm, 2);
    case OPERAND_ADDRESS_L:
      return ea_address(asm_context, opcode, operand, extra_imm, 4);
    case OPERAND_IMMEDIATE:
      if (flags & EA_NO_IMM) { break; }
      return ea_immediate(asm_context, opcode, size, operand);
    case OPERAND_INDEX_DATA16_PC:
      if (flags & EA_NO_PC) { break; }
      return ea_displacement(asm_context, opcode, operand);
    case OPERAND_INDEX_DATA8_PC_XN:
      return ea_displacement_xn(asm_context, opcode, operand);
    default:
      break;
  }

  print_error_illegal_operands(instr, asm_context);
  return -1;
}

// This should replace ea_generic_all since it gets its information from
// table_68000 on which modes are valid.  Wish I would have done it this
// way from the start.
static int ea_generic_new(
  struct _asm_context *asm_context,
  struct _operand *operand,
  char *instr,
  int size,
  struct _table_68000 *table,
  int is_dst,
  uint32_t extra_imm,
  int opcode_extra)
{
  int omit_mode = (is_dst == 1) ? table->omit_dst : table->omit_src;
  int opcode = table->opcode | opcode_extra;

  switch (operand->type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      if ((omit_mode & MODE_DN) && operand->type == OPERAND_D_REG) { break; }
      if ((omit_mode & MODE_AN) && operand->type == OPERAND_A_REG) { break; }
      if ((omit_mode & MODE_AN_P) && operand->type == OPERAND_A_REG_INDEX_PLUS) { break;}
      if ((omit_mode & MODE_AN_N) && operand->type == OPERAND_A_REG_INDEX_MINUS) { break; }
      add_bin16(asm_context, opcode | (operand->type << 3) | operand->value, IS_OPCODE);
      if (extra_imm != NO_EXTRA_IMM)
      {
        add_bin16(asm_context, extra_imm, IS_OPCODE);
        return 4;
      }
      return 2;
    case OPERAND_INDEX_DATA16_A_REG:
      return ea_displacement(asm_context, opcode, operand);
    case OPERAND_INDEX_DATA8_A_REG_XN:
      return ea_displacement_xn(asm_context, opcode, operand);
    case OPERAND_ADDRESS_W:
      return ea_address(asm_context, opcode, operand, extra_imm, 2);
    case OPERAND_ADDRESS_L:
      return ea_address(asm_context, opcode, operand, extra_imm, 4);
    case OPERAND_IMMEDIATE:
      if (omit_mode & MODE_IMM) { break; }
      return ea_immediate(asm_context, opcode, size, operand);
    case OPERAND_INDEX_DATA16_PC:
      if (omit_mode & MODE_D16_PC) { break; }
      return ea_displacement(asm_context, opcode, operand);
    case OPERAND_INDEX_DATA8_PC_XN:
      if (omit_mode & MODE_D8_PC_XN) { break; }
      return ea_displacement_xn(asm_context, opcode, operand);
    default:
      break;
  }

  print_error_illegal_operands(instr, asm_context);
  return -1;
}

static int write_single_ea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 1) { return 0; }

  uint16_t extra_opcode = size << 6;

  return ea_generic_new(asm_context, &operands[0], instr, size, table, 1, NO_EXTRA_IMM, extra_opcode);
}

static int write_single_ea_no_size(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, struct _table_68000 *table)
{
  if (operand_count != 1) { return 0; }

  return ea_generic_new(asm_context, &operands[0], instr, 0, table, 0, NO_EXTRA_IMM, 0);
}

static int write_reg_and_ea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (size == SIZE_NONE) { return 0; }

  int opmode;
  int reg;
  int opcode_extra;

  if (operands[1].type == OPERAND_D_REG)
  {
    reg = operands[1].value;
    opmode = 0;
    opcode_extra = (reg << 9) | (opmode << 8) | (size << 6);
    return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, opcode_extra);
  }
    else
  if (operands[0].type == OPERAND_D_REG)
  {
    reg = operands[0].value;
    opmode = 1;
    opcode_extra = (reg << 9) | (opmode << 8) | (size << 6);
    return ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, opcode_extra);
  }
    else
  {
    return 0;
  }
}

static int write_dreg_ea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (size == SIZE_NONE) { return 0; }
  if (operands[0].type != OPERAND_D_REG) { return 0; }

  int opmode;
  int reg;
  int opcode_extra;

  reg = operands[0].value;
  opmode = 0x4 | size;
  opcode_extra = (reg << 9) | (opmode << 6);

  return ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, opcode_extra);
}

static int write_immediate(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table_68000,
  int size)
{
  uint16_t opcode = table_68000->opcode;

  if (operand_count != 2) { return 0; }
  if (size == SIZE_NONE) { return 0; }
  if (operands[0].type != OPERAND_IMMEDIATE) { return 0; }
  if (operands[1].type == OPERAND_SPECIAL_REG) { return 0; }

  int len = 2;

  opcode |= (size << 6);

  switch (operands[1].type)
  {
    case OPERAND_A_REG:
      if ((table_68000->omit_dst & MODE_AN) != 0)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }
      break;
    case OPERAND_INDEX_DATA16_PC:
      if ((table_68000->omit_dst & MODE_D16_PC) != 0)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }
      break;
    case OPERAND_INDEX_DATA8_PC_XN:
      if ((table_68000->omit_dst & MODE_D8_PC_XN) != 0)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }
      break;
  }

  switch (operands[1].type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      opcode |= (operands[1].type << 3) | operands[1].value;
      break;
    case OPERAND_INDEX_DATA16_A_REG:
      //ea_displacement(asm_context, opcode, &operands[1]);
      opcode |= (5 << 3) | operands[1].dis_reg;
      break;
    case OPERAND_INDEX_DATA8_A_REG_XN:
      opcode |= (6 << 3) | operands[1].dis_reg;
      break;
    case OPERAND_INDEX_DATA16_PC:
      opcode |= (7 << 3) | 2;
      break;
    case OPERAND_INDEX_DATA8_PC_XN:
      opcode |= (7 << 3) | 3;
      break;
    case OPERAND_ADDRESS_W:
      opcode |= (7 << 3) | 0;
      break;
    case OPERAND_ADDRESS_L:
      opcode |= (7 << 3) | 1;
      break;
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }

  add_bin16(asm_context, opcode, IS_OPCODE);

  if (size < SIZE_L)
  {
    add_bin16(asm_context, operands[0].value, IS_OPCODE);
    len += 2;
  }
    else
  {
    add_bin32(asm_context, operands[0].value, IS_OPCODE);
    len += 4;
  }

  switch (operands[1].type)
  {
    case OPERAND_INDEX_DATA16_A_REG:
      add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
      break;
    case OPERAND_INDEX_DATA8_A_REG_XN:
      add_bin16(asm_context, (operands[1].xn_reg << 12) | (operands[1].xn_size == SIZE_L ? (1 << 11) : 0) | (operands[1].value & 0xffff), IS_OPCODE);
      break;
    case OPERAND_INDEX_DATA16_PC:
      add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
      break;
    case OPERAND_INDEX_DATA8_PC_XN:
      // [D=0/A=1] [REG3] [W=0/L=1] [000] [DISP8]
      add_bin16(asm_context, (operands[1].xn_reg << 12) | (operands[1].xn_size == SIZE_L ? (1 << 11) : 0) | (operands[1].value & 0xff), IS_OPCODE);
      len += 2;
      break;
    case OPERAND_ADDRESS_W:
      add_bin16(asm_context, operands[1].value, IS_OPCODE);
      len += 2;
      break;
    case OPERAND_ADDRESS_L:
      add_bin32(asm_context, operands[1].value, IS_OPCODE);
      len += 4;
      break;
  }

  return len;
}

static int write_shift(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 2) { return 0; }

  if (operands[0].type == OPERAND_IMMEDIATE && operands[1].type == OPERAND_D_REG)
  {
    if (operands[0].value < 1 || operands[0].value > 8)
    {
      print_error_range("Shift", 1, 8, asm_context);
      return -1;
    }

    add_bin16(asm_context, opcode | ((operands[0].value & 7) << 9) | (size << 6) | operands[1].value, IS_OPCODE);
  }
    else
  if (operands[0].type == OPERAND_D_REG && operands[1].type == OPERAND_D_REG)
  {
    add_bin16(asm_context, opcode | (operands[0].value << 9) | (size << 6) | (1 << 5) | operands[1].value, IS_OPCODE);
  }
    else
  {
    return 0;
  }

  return 2;
}

static int write_vector(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size,
  int type)
{
  if (operand_count != 1) { return 0; }
  if (size != SIZE_NONE) { return 0; }
  if (operands[0].type != OPERAND_IMMEDIATE) { return 0; }

  if (type == OP_VECTOR)
  {
    if (operands[0].value < 0 || operands[0].value > 15)
    {
      print_error_range("Vector", 0, 15, asm_context);
      return -1;
    }
  }
    else
  {
    if (operands[0].value < 0 || operands[0].value > 7)
    {
      print_error_range("Vector", 0, 7, asm_context);
      return -1;
    }
  }


  add_bin16(asm_context, opcode | operands[0].value, IS_OPCODE);
  return 2;
}

static int write_areg(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 1) { return 0; }
  if (size != SIZE_NONE) { return 0; }
  if (operands[0].type != OPERAND_A_REG) { return 0; }

  add_bin16(asm_context, opcode | operands[0].value, IS_OPCODE);
  return 2;
}

static int write_reg(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 1) { return 0; }

  if (operands[0].type == OPERAND_D_REG)
  {
    add_bin16(asm_context, opcode | operands[0].value, IS_OPCODE);
  }
    else
  if (operands[0].type == OPERAND_A_REG)
  {
    add_bin16(asm_context, opcode | 8 | operands[0].value, IS_OPCODE);
  }
    else
  {
    return 0;
  }

  return 2;
}

static int write_ea_areg(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (size == SIZE_NONE || size == SIZE_B) { return 0; }
  if (operands[1].type != OPERAND_A_REG) { return 0; }

  int opmode;
  int reg = operands[1].value;
  int opcode_extra;

  opmode = (size == SIZE_W) ? 3 : 7;
  opcode_extra = (reg << 9) | (opmode << 6);

  return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, opcode_extra);
}

static int write_ea_dreg(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (check_size(size, table->omit_size) != 0) { return 0; }
  if (check_reg(operands[0].type, table->omit_src) != 0) { return 0; }
  if (operands[1].type != OPERAND_D_REG) { return 0; }

  int opmode;
  int reg = operands[1].value;
  int opcode_extra;

  opmode = size;
  opcode_extra = (reg << 9) | (opmode << 6);

  return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, opcode_extra);
}

static int write_load_ea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  //if (size != SIZE_NONE) { return 0; }
  if (operands[1].type != OPERAND_A_REG) { return 0; }

  int reg = operands[1].value;
  uint16_t opcode_extra = reg << 9;

  return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, opcode_extra);
}

static int write_quick(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (operands[0].type != OPERAND_IMMEDIATE) { return 0; }

  if (table->type == OP_MOVE_QUICK)
  {
    //if (size != SIZE_NONE) { return 0; }
    if (operands[1].type != OPERAND_D_REG) { return 0; }
    if (operands[0].value < -128 || operands[0].value > 127)
    {
      print_error_range("Quick", -128, 127, asm_context);
      return -1;
    }
    uint8_t value = (uint8_t)operands[0].value;
    add_bin16(asm_context, table->opcode | (operands[1].value << 9) | value, IS_OPCODE);
    return 2;
  }

  //if (size == SIZE_NONE) { return 0; }
  if (operands[0].value < 1 || operands[0].value > 8)
  {
    print_error_range("Quick", 1, 8, asm_context);
    return -1;
  }

  int data = (operands[0].value == 0) ? 8 : operands[0].value;
  int opcode_extra = (data << 9) | (size << 6);

  return ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, opcode_extra);
}

static int write_move_special(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }

  if (table->type == OP_MOVE_TO_CCR &&
      operands[1].type == OPERAND_SPECIAL_REG &&
      operands[1].value == SPECIAL_CCR)
  {
    return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, 0);
  }

  if (table->type == OP_MOVE_FROM_CCR &&
      operands[0].type == OPERAND_SPECIAL_REG &&
      operands[0].value == SPECIAL_CCR)
  {
    return ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, 0);
  }

  if (table->type == OP_MOVE_TO_SR &&
      operands[1].type == OPERAND_SPECIAL_REG &&
      operands[1].value == SPECIAL_SR)
  {
    return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, 0);
  }

  if (table->type == OP_MOVE_FROM_SR &&
      operands[0].type == OPERAND_SPECIAL_REG &&
      operands[0].value == SPECIAL_SR)
  {
    return ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, 0);
  }

  return 0;
}

static int write_movea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (operands[1].type != OPERAND_A_REG) { return 0; }

  int size_a = (size == SIZE_W) ? 3 : 2;
  int opcode_extra = (size_a << 12) | (operands[1].value << 9);

  //return ea_generic_all(asm_context, &operands[0], instr, opcode | (size_a << 12) | (operands[1].value << 9), size, 0, NO_EXTRA_IMM);
  return ea_generic_new(asm_context, &operands[0], instr, size, table, 0, NO_EXTRA_IMM, opcode_extra);
}

static int write_cmpm(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (size == SIZE_NONE) { return 0; }
  if (operands[0].type != OPERAND_A_REG_INDEX_PLUS) { return 0; }
  if (operands[1].type != OPERAND_A_REG_INDEX_PLUS) { return 0; }

  add_bin16(asm_context, opcode | (size << 6) | (operands[1].value << 9) | operands[0].value, IS_OPCODE);
  return 2;
}

static int write_bcd(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table)
{
  if (operand_count != 2) { return 0; }

  int rm = -1;

  if (operands[0].type == OPERAND_A_REG_INDEX_MINUS &&
      operands[1].type == OPERAND_A_REG_INDEX_MINUS)
  {
    rm = 8;
  }
    else
  if (operands[0].type == OPERAND_D_REG &&
      operands[1].type == OPERAND_D_REG)
  {
    rm = 0;
  }

  if (rm == -1) { return 0; }

  add_bin16(asm_context, table->opcode | (operands[1].value << 9) | rm | operands[0].value, IS_OPCODE);

  return 2;
}

static int write_extended(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (size == SIZE_NONE) { return 0; }

  int rm = -1;

  if (operands[0].type == OPERAND_A_REG_INDEX_MINUS &&
      operands[1].type == OPERAND_A_REG_INDEX_MINUS)
  {
    rm=8;
  }
    else
  if (operands[0].type == OPERAND_D_REG &&
      operands[1].type == OPERAND_D_REG)
  {
    rm = 0;
  }

  if (rm == -1) { return 0; }

  add_bin16(asm_context, opcode | (operands[1].value << 9) | (size << 6) | rm | operands[0].value, IS_OPCODE);

  return 2;
}

static int write_rox(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size,
  int type)
{
  if (type == OP_ROX_MEM)
  {
    if (size != SIZE_NONE) { return 0; }
    if (operand_count != 1) { return 0; }

    return ea_generic_all(asm_context, &operands[0], instr, opcode, size, EA_NO_D | EA_NO_A | EA_NO_PC | EA_NO_IMM, NO_EXTRA_IMM);
  }
    else
  if (type == OP_ROX)
  {
    if (size == SIZE_NONE) { return 0; }
    if (operand_count != 2) { return 0; }
    if (operands[1].type != OPERAND_D_REG) { return 0; }

    if (operands[0].type == OPERAND_D_REG)
    {
      add_bin16(asm_context, opcode | (operands[0].value << 9) | (size << 6) | (1 << 5) | operands[1].value, IS_OPCODE);
      return 2;
    }
      else
    if (operands[0].type == OPERAND_IMMEDIATE)
    {
      int count = (operands[0].value == 8) ? 0 : operands[0].value;
      add_bin16(asm_context, opcode | (count << 9) | (size << 6) | operands[1].value, IS_OPCODE);
      return 2;
    }
  }

  return 0;
}

static int write_exchange(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }

  if (operands[0].type == OPERAND_D_REG && operands[1].type == OPERAND_D_REG)
  {
    add_bin16(asm_context, table->opcode | (operands[0].value << 9) | (0x8 << 3) | operands[1].value, IS_OPCODE);
    return 2;
  }
    else
  if (operands[0].type == OPERAND_A_REG && operands[1].type == OPERAND_A_REG)
  {
    add_bin16(asm_context, table->opcode | (operands[0].value << 9) | (0x9 << 3) | operands[1].value, IS_OPCODE);
    return 2;
  }
    else
  if (operands[0].type == OPERAND_D_REG && operands[1].type == OPERAND_A_REG)
  {
    add_bin16(asm_context, table->opcode | (operands[0].value << 9) | (0x11 << 3) | operands[1].value, IS_OPCODE);
    return 2;
  }

  return 0;
}

static int write_bit_reg_ea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  //if (size != SIZE_NONE) { return 0; }

  if (operands[0].type == OPERAND_D_REG)
  {
    uint16_t opcode_extra = (operands[0].value << 9);
    return ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, opcode_extra);
  }

  return 0;
}

static int write_bit_imm_ea(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  //if (size != SIZE_NONE) { return 0; }

  if (operands[0].type == OPERAND_IMMEDIATE)
  {
    if (operands[0].value < 0 || operands[0].value > 127)
    {
      print_error_range("Immediate", 0, 127, asm_context);
      return -1;
    }

    //return ea_generic_all(asm_context, &operands[1], instr, opcode, 0, EA_NO_A | EA_NO_IMM | EA_NO_PC, operands[0].value);
    int len = ea_generic_new(asm_context, &operands[1], instr, size, table, 1, NO_EXTRA_IMM, 0);

    if (len <= 0)
    {
      print_error_illegal_expression(instr, asm_context);
      return -1;
    }

    if (len > 2)
    {
      // Need to shift the extra bytes of the EA forward 16 bytes so
      // the bit number can fit.
      uint8_t ea[32];
      int distance = len - 2;
      int n;

      if (distance > 32) { return -1; }

      asm_context->address -= distance;

      for (n = 0; n < distance; n++)
      {
        ea[n] = memory_read(asm_context, asm_context->address + n);
      }

      add_bin16(asm_context, operands[0].value, IS_OPCODE);

      for (n = 0; n < distance; n += 2)
      {
        int data = (ea[n] << 8) | ea[n + 1];
        add_bin16(asm_context, data, IS_OPCODE);
      }
    }
      else
    {
      add_bin16(asm_context, operands[0].value, IS_OPCODE);
    }

    return len + 2;
  }

  return 0;
}

static int write_ea_dreg_wl(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (check_size(size, table->omit_size) != 0) { return 0; }
  if (check_reg(operands[0].type, table->omit_src) != 0) { return 0; }
  if (operands[1].type != OPERAND_D_REG) { return 0; }

  int size_a = (size == SIZE_W) ? 3 : 2;
  uint16_t opcode = table->opcode;

  return ea_generic_all(asm_context, &operands[0], instr, opcode | (operands[1].value << 9) | (size_a << 7), 0, EA_NO_A, NO_EXTRA_IMM);
}

static int write_logic_ccr(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  int use_sr = 0;

  if (operand_count != 2) { return 0; }
  if (check_size(size, table->omit_size) != 0) { return 0; }
  if (check_reg(operands[0].type, table->omit_src) != 0) { return 0; }
  if (operands[1].type != OPERAND_SPECIAL_REG) { return 0; }

  if (operands[1].value == SPECIAL_SR)
  {
    use_sr = 0x0040;
  }
    else
  if (operands[1].value != SPECIAL_CCR)
  {
    return 0;
  }

  if (use_sr == 0)
  {
    if (operands[0].value < 0 || operands[0].value > 255)
    {
      print_error_range("Immediate", 0, 255, asm_context);
      return -1;
    }
  }
  else
  {
    if (operands[0].value < 0 || operands[0].value > 0xffff)
    {
      print_error_range("Immediate", 0, 0xffff, asm_context);
      return -1;
    }
  }

  add_bin16(asm_context, table->opcode | use_sr, IS_OPCODE);
  add_bin16(asm_context, operands[0].value, IS_OPCODE);

  return 4;
}

static int write_branch(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 1) { return 0; }
  if (operands[0].type != OPERAND_ADDRESS) { return 0; }

  if (asm_context->pass == 1)
  {
    if (size == SIZE_NONE)
    {
      print_error_internal(asm_context, __FILE__, __LINE__);
      return -1;
    }

    add_bin16(asm_context, (size + 1) << 8, IS_OPCODE);
    int n;
    for (n = 0; n < size; n++) { add_bin16(asm_context, 0, IS_OPCODE); }

    return (size + 1) * 2;
  }

  //int len = memory_read(asm_context, asm_context->address);
  int offset;

  if (size == SIZE_B)
  {
    // NOTE: offset of 0 or -1 is invalid because 8 bit displacement of
    // 0 means the displacement is 16 bit.  -1 means the displacement is
    // 32 bit.  Maybe should give a different error message.
    offset = operands[0].value - (asm_context->address + 2);
    if ((offset < -128 || offset > 127) || offset == 0 || offset == -1)
    {
      print_error_range("Offset", -128, 127, asm_context);
      return -1;
    }

    add_bin16(asm_context, opcode | (offset & 0xff), IS_OPCODE);
    return 2;
  }
    else
  if (size == SIZE_W)
  {
    offset = operands[0].value - (asm_context->address + 2);

    if (offset < -32768 || offset > 32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }

    add_bin16(asm_context, opcode | 0x00, IS_OPCODE);
    add_bin16(asm_context, offset, IS_OPCODE);
    return 4;
  }
    else
  {
    offset = operands[0].value - (asm_context->address + 2);
    add_bin16(asm_context, opcode | 0xff, IS_OPCODE);
    add_bin32(asm_context, offset, IS_OPCODE);
    return 6;
  }
}

static int write_ext(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 1) { return 0; }
  if (operands[0].type != OPERAND_D_REG) { return 0; }

  if (size == SIZE_W)
  {
    add_bin16(asm_context, opcode | (2 << 6) | operands[0].value, IS_OPCODE);
    return 2;
  }
    else
  if (size == SIZE_L)
  {
    add_bin16(asm_context, opcode | (3 << 6) | operands[0].value, IS_OPCODE);
    return 2;
  }

  return 0;
}

static int write_link(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (operands[0].type != OPERAND_A_REG) { return 0; }
  if (operands[1].type != OPERAND_IMMEDIATE) { return 0; }
  //if (size != SIZE_NONE) { return 0; }

  add_bin16(asm_context, table->opcode | operands[0].value, IS_OPCODE);

  if (size == SIZE_W)
  {
    if (operands[1].value < -32768 || operands[1].value > 32767)
    {
      print_error_range("Displacement", -32768, 32767, asm_context);
      return -1;
    }

    add_bin16(asm_context, operands[1].value, IS_OPCODE);
    return 4;
  }
    else
  if (size == SIZE_L)
  {
    if (operands[1].value < (int)0x80000000 || operands[1].value > 0x7fffffff)
    {
      print_error_range("Displacement", -32768, 32767, asm_context);
      return -1;
    }

    add_bin16(asm_context, operands[1].value >> 16, IS_OPCODE);
    add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
    return 4;
  }

  return 0;
}

static int write_div_mul(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  if (operand_count != 2) { return 0; }
  if (size != SIZE_W) { return 0; }
  if (operands[1].type != OPERAND_D_REG) { return 0; }

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(operands[1].value << 9), 0, EA_NO_A, NO_EXTRA_IMM);
}

static int write_movep(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  int opmode;

  if (operand_count != 2) { return 0; }
  if (size != SIZE_W && size != SIZE_L) { return 0; }

  if (operands[0].type == OPERAND_D_REG &&
      operands[1].type == OPERAND_INDEX_DATA16_A_REG)
  {
    if (operands[1].value < -32768 || operands[1].value > 32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }
    opmode = (size == SIZE_W) ? 6 : 7;
    add_bin16(asm_context, opcode | (operands[0].value << 9) | (opmode << 6) | operands[1].dis_reg, IS_OPCODE);
    add_bin16(asm_context, operands[1].value, IS_OPCODE);
    return 4;
  }
    else
  if (operands[0].type == OPERAND_INDEX_DATA16_A_REG &&
      operands[1].type == OPERAND_D_REG)
  {
    if (operands[0].value < -32768 || operands[0].value > 32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }
    opmode = (size == SIZE_W) ? 4 : 5;
    add_bin16(asm_context, opcode | (operands[1].value << 9) | (opmode << 6) | operands[0].dis_reg, IS_OPCODE);
    add_bin16(asm_context, operands[0].value, IS_OPCODE);
    return 4;
  }

  return 0;
}

static int write_movem(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  struct _operand *operand = &operands[1];
  int mask = 0;
  int dir = 0;

  if (operand_count != 2) { return 0; }
  if (size != SIZE_W && size != SIZE_L) { return 0; }

  if (operands[0].type == OPERAND_MULTIPLE_REG)
  {
    mask = operands[0].value;
  }
    else
  if (operands[1].type == OPERAND_MULTIPLE_REG)
  {
    mask = operands[1].value;
    operand = &operands[0];
    dir = 1;
  }
    else
  {
    return 0;
  }

  size = (size == SIZE_W) ? 0 : 1;

  if (dir == 1)
  {
    // Memory to register dir=1
    return ea_generic_all(asm_context, operand, instr, opcode | (1 << 10) | (size << 6), 0, EA_NO_A | EA_NO_D | EA_NO_IMM | EA_NO_MINUS | EA_NO_PC, mask);
  }
    else
  {
    if (operands[1].type == OPERAND_A_REG_INDEX_MINUS)
    {
      mask = reverse_bits16(mask);
    }

    // Register to memory dir=0
    return ea_generic_all(asm_context, operand, instr, opcode | (size << 6), 0, EA_NO_A | EA_NO_D | EA_NO_IMM | EA_NO_PLUS | EA_NO_PC, mask);
  }
}

static int write_move(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  int opcode,
  int size)
{
  const int move_size[] = { 1, 3, 2, 0 };
  uint32_t address;
  uint16_t ea_src_bytes[32];
  uint16_t ea_dst_bytes[32];
  int src_len;
  int dst_len;
  int len = 2;
  int n;

  if (size == SIZE_NONE) { return 0; }

  size = move_size[size];

  // Translate to MOVEA
  if (operands[1].type == OPERAND_A_REG)
  {
    //return write_movea(asm_context, instr, operands, operand_count, 0x0040, size);
    strcpy(instr, "movea");
    return 0;
  }

  opcode = opcode | (size << 12);
  address = asm_context->address;

  src_len = ea_generic_all(asm_context, &operands[0], instr, opcode, size, 0, NO_EXTRA_IMM);

  if (src_len == -1) { return -1; }
  if (src_len == 0) { return 0; }

  for (n = 0; n < src_len; n++)
  {
    ea_src_bytes[n] = memory_read(asm_context, address + n);
  }

  asm_context->address = address;

  dst_len = ea_generic_all(asm_context, &operands[1], instr, opcode, size, EA_NO_A | EA_NO_IMM | EA_NO_PC, NO_EXTRA_IMM);

  if (dst_len == -1) { return -1; }
  if (dst_len == 0) { return 0; }

  for (n = 0; n < dst_len; n++)
  {
    ea_dst_bytes[n] = memory_read(asm_context, address + n);
  }

  asm_context->address = address;

  // Good lord Motorla :(
  uint16_t ea_dst = ea_dst_bytes[1] & 0x3f;
  ea_dst = (ea_dst >> 3) | ((ea_dst & 0x7) << 3);

  //opcode |= (ea_dst_bytes[1] & 0x3f) << 6;
  opcode |= (ea_dst) << 6;
  opcode |= (ea_src_bytes[1] & 0x3f);

  //printf("%04x\n", opcode);
  add_bin16(asm_context, opcode, IS_OPCODE);
  for (n = 2; n < src_len; n += 2)
  {
    opcode = (ea_src_bytes[n] << 8) | ea_src_bytes[n + 1];
    add_bin16(asm_context, opcode, IS_OPCODE);
    len += 2;
  }

  for (n = 2; n < dst_len; n += 2)
  {
    opcode = (ea_dst_bytes[n] << 8) | ea_dst_bytes[n + 1];
    add_bin16(asm_context, opcode, IS_OPCODE);
    len += 2;
  }

  return len;
}

static int write_jump(
  struct _asm_context *asm_context,
  char *instr,
  struct _operand *operands,
  int operand_count,
  struct _table_68000 *table,
  int size)
{
  int32_t offset = 0;

  if (operand_count != 1) { return 0; }

  if (operands[0].type == OPERAND_ADDRESS)
  {
    offset = operands[0].value - (asm_context->address + 2);
    if (offset < -32768 || offset > 32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
    }
    operands[0].value = offset;
    operands[0].type = OPERAND_INDEX_DATA16_PC;
  }

  return ea_generic_new(asm_context, &operands[0], instr, size, table, 1, NO_EXTRA_IMM, 0);
}

int parse_instruction_68000(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case_c[TOKENLEN];
  char *instr_case = instr_case_c;
  struct _operand operands[3];
  int operand_count = 0;
  int operand_size = SIZE_NONE;
  int matched = 0;
  int num;
  int ret;
  int n;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  if (strcmp("dbhi", instr_case) == 0) { instr_case = "dbcc"; }
  else if (strcmp("dblo", instr_case) == 0) { instr_case = "dbcs"; }
  else if (strcmp("dbra", instr_case) == 0) { instr_case = "dbf"; }

  memset(&operands, 0, sizeof(operands));

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
#if 0
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
#endif
      break;
    }

    if (operand_count >= 3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if (IS_TOKEN(token, '.') && operand_count == 0 && operand_size == SIZE_NONE)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (strcasecmp(token, "b") == 0) { operand_size = SIZE_B; }
      else if (strcasecmp(token, "w") == 0) { operand_size = SIZE_W; }
      else if (strcasecmp(token, "l") == 0) { operand_size = SIZE_L; }
      else if (strcasecmp(token, "s") == 0) { operand_size = SIZE_S; }
      else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      continue;
    }

    if ((num = get_register_d_68000(token)) != -1)
    {
      operands[operand_count].type = OPERAND_D_REG;
      operands[operand_count].value = num;
    }
      else
    if ((num = get_register_a_68000(token)) != -1)
    {
      operands[operand_count].type = OPERAND_A_REG;
      operands[operand_count].value = num;
    }
      else
    if ((num = get_register_special_68000(token)) != -1)
    {
      operands[operand_count].type = OPERAND_SPECIAL_REG;
      operands[operand_count].value = num;
    }
      else
    if (token_type == TOKEN_POUND)
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          //operands[operand_count].error = 1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_IMMEDIATE;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token,'-'))
    {
      if (expect_token_s(asm_context,"(") != 0) { return -1; }
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if ((num = get_register_a_68000(token)) == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      if (expect_token_s(asm_context,")") != 0) { return -1; }

      operands[operand_count].type = OPERAND_A_REG_INDEX_MINUS;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token,'('))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if ((num = get_register_a_68000(token)) != -1)
      {
        operands[operand_count].value = num;
        if (expect_token_s(asm_context,")") != 0) { return -1; }
        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'+'))
        {
          operands[operand_count].type = OPERAND_A_REG_INDEX_PLUS;
        }
          else
        {
          operands[operand_count].type = OPERAND_A_REG_INDEX;
          tokens_push(asm_context, token, token_type);
        }
      }
        else
      if (asm_context->pass == 1)
      {
        tokens_push(asm_context, token, token_type);

        // If there are any commas in here, then it's a 16 bit + 16 bit
        // instruction.  If there are no comments then it's an absolute
        // address and is either 16 + either 16 or 32 bits.

        int has_comma = 0;
        int eval_error = 0;

        // First try to evaluate the expression
        if (eval_expression(asm_context, &num) != 0)
        {
          // Overflow 16 bit
          eval_error = 1;
        }

        while (1)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_TOKEN(token, ',')) { has_comma = 1; }
          else if (IS_TOKEN(token, ')')) { break; }
        }

        if (has_comma == 0)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_TOKEN(token, '.'))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (strcasecmp(token, "w") == 0)
            {
              //memory_write(asm_context, asm_context->address, 2, asm_context->tokens.line);
              operands[operand_count].type = OPERAND_ADDRESS_W;
            }
              else
            if (strcasecmp(token, "l") == 0)
            {
              memory_write(asm_context, asm_context->address, 4, asm_context->tokens.line);
              operands[operand_count].type = OPERAND_ADDRESS_L;
            }
              else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
            else
          {
            if (eval_error == 1 || num > 0xffff)
            {
              // Can't figure out the size, so assume 32 bit :(
              memory_write(asm_context, asm_context->address, 4, asm_context->tokens.line);
              operands[operand_count].type = OPERAND_ADDRESS_L;
            }
              else
            {
              //memory_write(asm_context, asm_context->address, 2, asm_context->tokens.line);
              operands[operand_count].type = OPERAND_ADDRESS_W;
            }

            tokens_push(asm_context, token, token_type);
          }
        }
          else
        {
          // Any one of these would work here since they are all 16 bit
          // plus an extra 16 bit for the offset.
          operands[operand_count].type = OPERAND_INDEX_DATA16_A_REG;
        }
      }
        else
      {
        // Check for displacement
        tokens_push(asm_context, token, token_type);

        int eval_error = 0;

        if (eval_expression(asm_context, &num) != 0)
        {
          if (asm_context->pass == 1)
          {
            eval_error = 1;
            ignore_operand(asm_context);
          }
            else
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }
        }

        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_INDEX_DATA16_A_REG;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, ','))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if ((num = get_register_a_68000(token)) != -1)
          {
            operands[operand_count].dis_reg = num;
          }
            else
          {
            if (strcasecmp(token, "pc") == 0)
            {
              operands[operand_count].type = OPERAND_INDEX_DATA16_PC;
            }
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_TOKEN(token, ','))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);

            if ((num = get_register_d_68000(token)) != -1)
            {
            }
              else
            if ((num = get_register_a_68000(token)) != -1)
            {
              num |= 0x8;
            }
              else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            if (operands[operand_count].type == OPERAND_INDEX_DATA16_PC)
            {
              operands[operand_count].type = OPERAND_INDEX_DATA8_PC_XN;
            }
              else
            {
              operands[operand_count].type = OPERAND_INDEX_DATA8_A_REG_XN;
            }

            operands[operand_count].xn_reg = num;
            operands[operand_count].xn_size = SIZE_L;

            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (IS_TOKEN(token, '.'))
            {
              token_type = tokens_get(asm_context, token, TOKENLEN);
              if (strcasecmp(token,"w") == 0)
              {
                operands[operand_count].xn_size = SIZE_W;
              }
                else
              if (strcasecmp(token,"l") == 0)
              {
                operands[operand_count].xn_size = SIZE_L;
              }
                else
              {
                print_error_unexp(token, asm_context);
                return -1;
              }

              token_type = tokens_get(asm_context, token, TOKENLEN);
            }
          }

          if (IS_NOT_TOKEN(token,')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
          else
        {
          if (IS_NOT_TOKEN(token,')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          if (eval_error == 1)
          {
            memory_write(asm_context, asm_context->address, 4, asm_context->tokens.line);
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, '.'))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (strcasecmp(token, "w") == 0)
            {
              operands[operand_count].type = OPERAND_ADDRESS_W;
              if (num < 0 || num > 0xffff)
              {
                print_error_range(instr, 0, 0xffff, asm_context);
                return -1;
              }
            }
              else
            if (strcasecmp(token, "l") == 0)
            {
              operands[operand_count].type = OPERAND_ADDRESS_L;
            }
              else
            {
              print_error_unexp(token, asm_context);
            }
          }
            else
          {
            if (memory_read(asm_context, asm_context->address) != 0 ||
                eval_error == 1 ||
                num > 0xffff)
            {
              operands[operand_count].type = OPERAND_ADDRESS_L;
            }
              else
            {
              operands[operand_count].type = OPERAND_ADDRESS_W;
            }

            tokens_push(asm_context, token, token_type);
          }
        }
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);
      //int eval_error = 0;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          //memory_write(asm_context, asm_context->address, 4, asm_context->tokens.line);
          //eval_error = 1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

#if 0
      if (memory_read(asm_context, asm_context->address) != 0 ||
          eval_error == 1 ||
          num > 0xffff)
      {
        operands[operand_count].type = OPERAND_ADDRESS_L;
      }
        else
      {
        operands[operand_count].type = OPERAND_ADDRESS_W;
      }
#endif

      operands[operand_count].type = OPERAND_ADDRESS;
      operands[operand_count].value = num;
    }

    // See if multiple registers are listed like: a3-a5/d1/d2-d3
    if (operands[operand_count].type == OPERAND_D_REG ||
        operands[operand_count].type == OPERAND_A_REG)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'-') || IS_TOKEN(token,'/'))
      {
        int type = operands[operand_count].type;
        int curr = operands[operand_count].value;
        int mask = (1 << operands[operand_count].value);
        if (type == OPERAND_A_REG) { mask = mask << 8; }
        operands[operand_count].type = OPERAND_MULTIPLE_REG;
        operands[operand_count].value = 0;

        while (1)
        {
          if (IS_TOKEN(token,'-'))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (type == OPERAND_D_REG)
            {
              num = get_register_d_68000(token);
              if (num == -1 || num < curr)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              for (n = curr; n <= num; n++) { mask |= (1 << n); }
              type = 0;
            }
              else
            if (type == OPERAND_A_REG)
            {
              num = get_register_a_68000(token);
              if (num == -1 || num < curr)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              for (n = curr; n <= num; n++) { mask |= (1 << (n + 8)); }
              type = 0;
            }

            token_type = tokens_get(asm_context, token, TOKENLEN);
          }
            else
          if (IS_TOKEN(token,'/'))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            if ((num = get_register_d_68000(token)) != -1)
            {
              type = OPERAND_D_REG;
              curr = num;
              mask |= (1 << num);
              token_type = tokens_get(asm_context, token, TOKENLEN);
            }
              else
            if ((num = get_register_a_68000(token)) != -1)
            {
              type = OPERAND_A_REG;
              curr = num;
              mask |= (1 << (num + 8));
              token_type = tokens_get(asm_context, token, TOKENLEN);
            }
              else
            {
              tokens_push(asm_context, token, token_type);
              break;
            }
          }
            else
          {
            tokens_push(asm_context, token, token_type);
            break;
          }
        }

        //printf("mask=0x%04x\n", mask);
        operands[operand_count].value = mask;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token,',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
printf("-----\n");
for (n = 0; n < operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  // M68000 FAMILY PROGRAMMER'S REFERENCE MANUAL, page 4-6
  // ADDA is used when the destintation is an address register.
  // ADDI and ADDQ are used when the source is immediate data.
  // Most assemblers automatically make this distinction.
  if (strcmp(instr_case, "add") == 0 && operand_count == 2)
  {
    if (operands[1].type == OPERAND_A_REG)
    {
      instr_case = "adda";
    }
      else
    if (operands[0].type == OPERAND_IMMEDIATE)
    {
      if (operands[0].value >= 1 && operands[0].value <= 8)
      {
        instr_case = "addq";
      }
        else
      {
        instr_case = "addi";
      }
    }
  }

  // DBcc - Decrement and branch on condition
  if (instr_case[0] == 'd' && instr_case[1] == 'b')
  {
    int opcode = 0x50c8;

    for (n = 0; n < 16; n++)
    {
      if (strcmp(instr_case + 2, table_68000_condition_codes[n]) != 0) { continue; }

      if (operand_size != SIZE_NONE)
      {
        printf("Error: %s doesn't take a size attribute at %s:%d\n", instr, asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

      if (operand_count != 2)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      if (operands[0].type != OPERAND_D_REG ||
          operands[1].type != OPERAND_ADDRESS)
      {
        matched = 1;
        continue;
      }

      int offset = operands[1].value - (asm_context->address + 2);
      add_bin16(asm_context, opcode | (n << 8) | operands[0].value, IS_OPCODE);

      if (asm_context->pass == 1) { add_bin16(asm_context, 0, IS_OPCODE); }
      else
      {
        if (offset < -32768 || offset > 32767)
        {
          print_error_range("Offset", -32768, 32767, asm_context);
          return -1;
        }
        add_bin16(asm_context, offset, IS_OPCODE);
      }

      return 4;
    }
  }

  // Bcc - Branch condition
  if (instr_case[0] == 'b' && operand_count == 1)
  {
    for (n = 0; n < 16; n++)
    {
      if (strcmp(instr_case + 1, table_68000_condition_codes[n]) != 0) { continue; }
      matched = 1;
      if (operands[0].type != OPERAND_ADDRESS) { continue; }
      int opcode = 0x6000 | (n << 8);
      if (operand_size == SIZE_S) { operand_size = SIZE_B; }

      if (operand_size == -1)
      {
        break;
      }

      return write_branch(asm_context, instr, operands, operand_count, opcode, operand_size);
    }
  }

  if (operand_size == SIZE_S && strcmp(instr_case, "bra") != 0)
  {
    print_error_unexp("s", asm_context);
    return -1;
  }

  // Scc - Set according to condition
  if (instr_case[0] == 's' && operand_count == 1)
  {
    for (n = 0; n < 16; n++)
    {
      if (strcmp(instr_case + 1, table_68000_condition_codes[n]) != 0) { continue; }
      matched = 1;
      if (operand_size != SIZE_NONE) { continue; }
      int opcode = 0x50c0 | (n << 8);
      return ea_generic_all(asm_context, &operands[0], instr, opcode, 0, EA_NO_A | EA_NO_IMM | EA_NO_PC, NO_EXTRA_IMM);
    }
  }

  n = 0;
  while (table_68000[n].instr != NULL)
  {
    if (strcmp(table_68000[n].instr, instr_case) == 0)
    {
      ret = 0;
      matched = 1;

      // WARNING: All instructions of the same name have to have the same
      // default size.
      if (operand_size == SIZE_NONE && table_68000[n].default_size != 0)
      {
        switch (table_68000[n].default_size)
        {
          case DEFAULT_B: operand_size = SIZE_B; break;
          case DEFAULT_W: operand_size = SIZE_W; break;
          case DEFAULT_L: operand_size = SIZE_L; break;
          default: break;
        }
      }

      if (check_size(operand_size, table_68000[n].omit_size) != 0)
      {
        n++;
        continue;
      }

      switch (table_68000[n].type)
      {
        case OP_NONE:
          if (operand_count == 0)
          {
            add_bin16(asm_context, table_68000[n].opcode, IS_OPCODE);
            ret = 2;
          }
          break;
        case OP_SINGLE_EA:
          ret = write_single_ea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_SINGLE_EA_NO_SIZE:
          ret = write_single_ea_no_size(asm_context, instr, operands, operand_count, &table_68000[n]);
          break;
        case OP_IMMEDIATE:
          ret = write_immediate(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_SHIFT_EA:
          ret = write_single_ea(asm_context, instr, operands, operand_count, &table_68000[n], 3);
          break;
        case OP_SHIFT:
          ret = write_shift(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_REG_AND_EA:
          ret = write_reg_and_ea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_VECTOR:
        case OP_VECTOR3:
          ret = write_vector(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size, table_68000[n].type);
          break;
        case OP_AREG:
          ret = write_areg(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_REG:
          ret = write_reg(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_EA_AREG:
          ret = write_ea_areg(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_EA_DREG:
          ret = write_ea_dreg(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_LOAD_EA:
          ret = write_load_ea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_QUICK:
        case OP_MOVE_QUICK:
          ret = write_quick(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_MOVE_FROM_CCR:
        case OP_MOVE_TO_CCR:
        case OP_MOVE_FROM_SR:
        case OP_MOVE_TO_SR:
          ret = write_move_special(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_MOVEA:
          ret = write_movea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_CMPM:
          ret = write_cmpm(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_BCD:
          ret = write_bcd(asm_context, instr, operands, operand_count, &table_68000[n]);
          break;
        case OP_EXTENDED:
          ret = write_extended(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_ROX_MEM:
        case OP_ROX:
          ret = write_rox(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size, table_68000[n].type);
          break;
        case OP_EXCHANGE:
          ret = write_exchange(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_BIT_REG_EA:
          ret = write_bit_reg_ea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_BIT_IMM_EA:
          ret = write_bit_imm_ea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_EA_DREG_WL:
          ret = write_ea_dreg_wl(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_LOGIC_CCR:
          ret = write_logic_ccr(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_LOGIC_SR:
          // ret = write_logic_sr(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          ret = -1;
          break;
        case OP_BRANCH:
          if (operand_size == SIZE_S) { operand_size = SIZE_B; }
          ret = write_branch(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_EXT:
          ret = write_ext(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_LINK_W:
          ret = write_link(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_LINK_L:
          ret = write_link(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_DIV_MUL:
          ret = write_div_mul(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_MOVEP:
          ret = write_movep(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_MOVEM:
          ret = write_movem(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_MOVE:
          ret = write_move(asm_context, instr, operands, operand_count, table_68000[n].opcode, operand_size);
          break;
        case OP_JUMP:
          ret = write_jump(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        case OP_DREG_EA:
          ret = write_dreg_ea(asm_context, instr, operands, operand_count, &table_68000[n], operand_size);
          break;
        default:
          n++;
          continue;
      }

      if (ret != 0) { return ret; }
    }

    n++;
  }

  if (matched == 1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

