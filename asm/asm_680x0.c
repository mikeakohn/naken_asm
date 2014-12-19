/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_680x0.h"
#include "asm_common.h"
#include "assembler.h"
#include "disasm_arm.h"
#include "tokens.h"
#include "eval_expression.h"
#include "table_680x0.h"

//extern struct _table_680x0_no_operands table_680x0_no_operands[];
extern struct _table_680x0 table_680x0[];
extern char *table_680x0_condition_codes[];

#define NO_EXTRA_IMM 0xffffffff

enum
{
  OPERAND_D_REG,
  OPERAND_A_REG,
  OPERAND_A_REG_INDEX,
  OPERAND_A_REG_INDEX_PLUS,
  OPERAND_A_REG_INDEX_MINUS,
  OPERAND_INDEX_DATA16_A_REG,
  OPERAND_INDEX_DATA8_A_REG_XN,   // implement me
  OPERAND_IMMEDIATE,
  OPERAND_INDEX_DATA16_PC,            // implement me
  OPERAND_INDEX_DATA8_PC_XN,          // implement me
  OPERAND_ADDRESS,
  OPERAND_ADDRESS_W,
  OPERAND_ADDRESS_L,
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
  char error;
  char dis_reg;
};

static int get_register_d_680x0(char *token)
{
  if (token[0]!='d' && token[0]!='D') return -1;
  if (token[1]>='0' && token[1]<='7' && token[2]==0)
  {
    return token[1]-'0';
  }

  return -1;
}

static int get_register_a_680x0(char *token)
{
  if (strcasecmp(token, "sp")==0) return 7;
  if (token[0]!='a' && token[0]!='A') return -1;
  if (token[1]>='0' && token[1]<='7' && token[2]==0)
  {
    return token[1]-'0';
  }

  return -1;
}

static int get_register_special_680x0(char *token)
{
  if (strcasecmp(token,"sr")==0) { return SPECIAL_SR; }
  if (strcasecmp(token,"ccr")==0) { return SPECIAL_CCR; }

  return -1;
}

static int calc_displacement_size(int address, int new_address)
{
int offset;

  offset=new_address-(address+2);
  if (offset>=-128 && offset<=127) { return SIZE_B; }
  offset=new_address-(address+4);
  if (offset>=-32768 && offset<=32767) { return SIZE_W; }

#if WE_SUPPORT_68020
  return SIZE_L;
#endif

  return SIZE_W;
}

static int ea_immediate(struct _asm_context *asm_context, int opcode, int size, struct _operand *operand)
{
  add_bin16(asm_context, opcode|(operand->type<<3)|0x4, IS_OPCODE);

  if (size==SIZE_L)
  {
    add_bin32(asm_context, operand->value, IS_OPCODE);
    return 6;
  }
    else
  {
    if (operand->value<-32768 || operand->value>65535)
    {
      print_error_range("Immediate", -32768, 65535, asm_context);
      return -1;
    }
    add_bin16(asm_context, operand->value, IS_OPCODE);
    return 4;
  }
}

static int ea_address(struct _asm_context *asm_context, int opcode, struct _operand *operand, uint32_t extra_imm)
{
  unsigned int value;
  int len;

  if (asm_context->pass==1)
  {
    if (operand->error==1)
    {
      len=6;
      add_bin16(asm_context, 0x0100, IS_OPCODE);
      if (extra_imm!=NO_EXTRA_IMM)
      {
        add_bin16(asm_context, extra_imm, IS_OPCODE);
        len+=2;
      }
      add_bin16(asm_context, 0x0000, IS_OPCODE);
      add_bin16(asm_context, 0x0000, IS_OPCODE);
      return len;
    }
  }

  value=operand->value;

  if (memory_read(asm_context, asm_context->address)==1)
  {
    len=6;
    add_bin16(asm_context, opcode|(0x7<<3)|0x1, IS_OPCODE);
    if (extra_imm!=NO_EXTRA_IMM)
    {
      add_bin16(asm_context, extra_imm, IS_OPCODE);
      len+=2;
    }
    add_bin32(asm_context, value, IS_OPCODE);
    return len;
  }
    else
  if ((value&0x00ff8000)==0x00000000 ||
      (value&0xffff8000)==0x00ff8000)
  {
    len=4;
    add_bin16(asm_context, opcode|(0x7<<3)|0x0, IS_OPCODE);
    if (extra_imm!=NO_EXTRA_IMM)
    {
      add_bin16(asm_context, extra_imm, IS_OPCODE);
      len+=2;
    }
    add_bin16(asm_context, value, IS_OPCODE);
    return len;
  }
    else
  {
    len=6;
    add_bin16(asm_context, opcode|(0x7<<3)|0x1, IS_OPCODE);
    if (extra_imm!=NO_EXTRA_IMM)
    {
      add_bin16(asm_context, extra_imm, IS_OPCODE);
      len+=2;
    }
    add_bin32(asm_context, operand->value, IS_OPCODE);
    return len;
  }
}

static int ea_displacement(struct _asm_context *asm_context, int opcode, struct _operand *operand)
{
  if (operand->value<-32768 || operand->value>32767)
  {
    print_error_range("Displacement", -32768, 32767, asm_context);
    return -1;
  }

  if (operand->type==OPERAND_INDEX_DATA16_A_REG)
  {
    add_bin16(asm_context, opcode|(0x5<<3)|operand->dis_reg, IS_OPCODE);
  }
    else
  if (operand->type==OPERAND_INDEX_DATA16_PC)
  {
    add_bin16(asm_context, opcode|(0x7<<3)|0x2, IS_OPCODE);
  }

  add_bin16(asm_context, operand->value, IS_OPCODE);

  return 4;
}

static int ea_generic_all(struct _asm_context *asm_context, struct _operand *operand, char *instr, int opcode, int size, int flags, unsigned int extra_imm)
{
#if 0
  if (flags&EA_SKIP_BYTES)
  {
    skip
    extra_imm=NO_EXTRA_IMM;
  }
#endif

  switch(operand->type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      if (flags&EA_NO_D && operand->type==OPERAND_D_REG) { break; }
      if (flags&EA_NO_A && operand->type==OPERAND_A_REG) { break; }
      if (flags&EA_NO_PLUS && operand->type==OPERAND_A_REG_INDEX_PLUS) { break;}
      if (flags&EA_NO_MINUS && operand->type==OPERAND_A_REG_INDEX_MINUS) { break; }
      add_bin16(asm_context, opcode|(operand->type<<3)|operand->value, IS_OPCODE);
      if (extra_imm!=NO_EXTRA_IMM)
      {
        add_bin16(asm_context, extra_imm, IS_OPCODE);
        return 4;
      }
      return 2;
    case OPERAND_INDEX_DATA16_PC:
      if (flags&EA_NO_PC) { break; }
    case OPERAND_INDEX_DATA16_A_REG:
      return ea_displacement(asm_context, opcode, operand);
    case OPERAND_IMMEDIATE:
      if (flags&EA_NO_IMM) { break; }
      return ea_immediate(asm_context, opcode, size, operand);
    case OPERAND_ADDRESS:
      return ea_address(asm_context, opcode, operand, extra_imm);
    default:
      break;
  }

  print_error_illegal_operands(instr, asm_context);
  return -1;
}

static int write_single_ea(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=1) { return 0; }

  switch(operands[0].type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      add_bin16(asm_context, opcode|(size<<6)|(operands[0].type<<3)|operands[0].value, IS_OPCODE);
      return 2;
    case OPERAND_IMMEDIATE:
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }
}

static int write_single_ea_no_size(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode)
{
  if (operand_count!=1) { return 0; }

  switch(operands[0].type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      add_bin16(asm_context, opcode|(operands[0].type<<3)|operands[0].value, IS_OPCODE);
      return 2;
    case OPERAND_INDEX_DATA16_A_REG:
      return ea_displacement(asm_context, opcode, &operands[0]);
    case OPERAND_ADDRESS:
      return ea_address(asm_context, opcode, &operands[0], NO_EXTRA_IMM);
    case OPERAND_IMMEDIATE:
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }
}

#if 0
static int write_single_ea_to_addr(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode)
{
  if (operand_count!=1) { return 0; }

  switch(operands[0].type)
  {
    case OPERAND_A_REG_INDEX:
      add_bin16(asm_context, opcode|(operands[0].type<<3)|operands[0].value, IS_OPCODE);
      return 2;
    case OPERAND_INDEX_DATA16_A_REG:
      return ea_displacement(asm_context, opcode, &operands[0]);
    case OPERAND_ADDRESS:
      return ea_address(asm_context, opcode, &operands[0], NO_EXTRA_IMM);
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }
}
#endif

static int write_reg_and_ea(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE) { return 0; }

  int opmode;
  int reg;

  if (operands[1].type==OPERAND_D_REG)
  {
    reg=operands[1].value;
    opmode=0;
    return ea_generic_all(asm_context, &operands[0], instr, opcode|(reg<<9)|(opmode<<8)|(size<<6), size, 0, NO_EXTRA_IMM);
  }
    else
  if (operands[0].type==OPERAND_D_REG)
  {
    reg=operands[0].value;
    opmode=1;
    return ea_generic_all(asm_context, &operands[1], instr, opcode|(reg<<9)|(opmode<<8)|(size<<6), size, EA_NO_PC|EA_NO_D|EA_NO_A|EA_NO_IMM, NO_EXTRA_IMM);
  }
    else
  {
    return 0;
  }
}

static int write_immediate(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE) { return 0; }
  if (operands[0].type!=OPERAND_IMMEDIATE) { return 0; }
  if (operands[1].type==OPERAND_SPECIAL_REG) { return 0; }

  switch(operands[1].type)
  {
    case OPERAND_D_REG:
    case OPERAND_A_REG:
    case OPERAND_A_REG_INDEX:
    case OPERAND_A_REG_INDEX_PLUS:
    case OPERAND_A_REG_INDEX_MINUS:
      opcode|=(size<<6)|(operands[1].type<<3)|operands[1].value;
      break;
    case OPERAND_IMMEDIATE:
      print_error_illegal_operands(instr, asm_context);
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }

  add_bin16(asm_context, opcode, IS_OPCODE);
  if (size<2) { add_bin16(asm_context, operands[0].value, IS_OPCODE); return 4; }
  else { add_bin32(asm_context, operands[0].value, IS_OPCODE); return 6; }
}

static int write_shift(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }

  if (operands[0].type==OPERAND_IMMEDIATE && operands[1].type==OPERAND_D_REG)
  {
    if (operands[0].value<1 || operands[0].value>8)
    {
      print_error_range("Shift", 1, 8, asm_context);
      return -1;
    }

    add_bin16(asm_context, opcode|((operands[0].value&7)<<9)|(size<<6)|operands[1].value, IS_OPCODE);
  }
    else
  if (operands[0].type==OPERAND_D_REG && operands[1].type==OPERAND_D_REG)
  {
    add_bin16(asm_context, opcode|(operands[0].value<<9)|(size<<6)|(1<<5)|operands[1].value, IS_OPCODE);
  }
    else
  {
    return 0;
  }

  return 2;
}

static int write_vector(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size, int type)
{
  if (operand_count!=1) { return 0; }
  if (size!=SIZE_NONE) { return 0; }
  if (operands[0].type!=OPERAND_IMMEDIATE) { return 0; }

  if (type==OP_VECTOR)
  {
    if (operands[0].value<0 || operands[0].value>15)
    {
      print_error_range("Vector", 0, 15, asm_context);
      return -1;
    }
  }
    else
  {
    if (operands[0].value<0 || operands[0].value>7)
    {
      print_error_range("Vector", 0, 7, asm_context);
      return -1;
    }
  }


  add_bin16(asm_context, opcode|operands[0].value, IS_OPCODE);
  return 2;
}

static int write_areg(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=1) { return 0; }
  if (size!=SIZE_NONE) { return 0; }
  if (operands[0].type!=OPERAND_A_REG) { return 0; }

  add_bin16(asm_context, opcode|operands[0].value, IS_OPCODE);
  return 2;
}

static int write_reg(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=1) { return 0; }
  if (size!=SIZE_NONE) { return 0; }
  if (operands[0].type==OPERAND_D_REG)
  {
    add_bin16(asm_context, opcode|operands[0].value, IS_OPCODE);
  }
    else
  if (operands[0].type==OPERAND_A_REG)
  {
    add_bin16(asm_context, opcode|8|operands[0].value, IS_OPCODE);
  }
    else
  {
    return 0;
  }

  return 2;
}

static int write_ea_areg(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE || size==SIZE_B) { return 0; }
  if (operands[1].type!=OPERAND_A_REG) { return 0; }

  int opmode;
  int reg=operands[1].value;

  opmode=(size==SIZE_W)?3:7;

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(reg<<9)|(opmode<<6), size, 0, NO_EXTRA_IMM);
}

static int write_ea_dreg(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE) { return 0; }
  if (operands[1].type!=OPERAND_D_REG) { return 0; }

  int opmode;
  int reg=operands[1].value;

  opmode=size;

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(reg<<9)|(opmode<<6), size, 0, NO_EXTRA_IMM);
}

static int write_load_ea(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }
  if (operands[1].type!=OPERAND_A_REG) { return 0; }

  int reg=operands[1].value;

  switch(operands[0].type)
  {
    case OPERAND_A_REG_INDEX:
      add_bin16(asm_context, opcode|(reg<<9)|(operands[0].type<<3)|operands[0].value, IS_OPCODE);
      return 2;
    case OPERAND_INDEX_DATA16_A_REG:
      return ea_displacement(asm_context, opcode|(reg<<9), &operands[0]);
    case OPERAND_ADDRESS:
      return ea_address(asm_context, opcode|(reg<<9), &operands[0], NO_EXTRA_IMM);
    default:
      print_error_illegal_operands(instr, asm_context);
      return -1;
  }
}

static int write_quick(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size, int type)
{
  if (operand_count!=2) { return 0; }
  if (operands[0].type!=OPERAND_IMMEDIATE) { return 0; }

  if (type==OP_MOVE_QUICK)
  {
    if (size!=SIZE_NONE) { return 0; }
    if (operands[1].type!=OPERAND_D_REG) { return 0; }
    if (operands[0].value<0 || operands[0].value>255)
    {
      print_error_range("Quick", 0, 255, asm_context);
      return -1;
    }
    add_bin16(asm_context, opcode|(operands[1].value<<9)|operands[0].value, IS_OPCODE);
    return 2;
  }

  if (size==SIZE_NONE) { return 0; }
  if (operands[0].value<1 || operands[0].value>8)
  {
    print_error_range("Quick", 1, 8, asm_context);
    return -1;
  }

  int data=(operands[0].value==0)?8:operands[0].value;

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(data<<9)|(size<<6), size, EA_NO_PC, NO_EXTRA_IMM);
}

static int write_move_special(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size, int type)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  if (type==OP_MOVE_TO_CCR &&
      operands[1].type==OPERAND_SPECIAL_REG &&
      operands[1].value==SPECIAL_CCR)
  {
    return ea_generic_all(asm_context, &operands[0], instr, opcode, size, 0, NO_EXTRA_IMM);
  }

  if (type==OP_MOVE_FROM_CCR &&
      operands[0].type==OPERAND_SPECIAL_REG &&
      operands[0].value==SPECIAL_CCR)
  {
    return ea_generic_all(asm_context, &operands[1], instr, opcode, size, 0, NO_EXTRA_IMM);
  }

  if (type==OP_MOVE_FROM_SR &&
      operands[0].type==OPERAND_SPECIAL_REG &&
      operands[0].value==SPECIAL_SR)
  {
    return ea_generic_all(asm_context, &operands[1], instr, opcode, size, 0, NO_EXTRA_IMM);
  }

  return 0;
}

static int write_movea(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE) { return 0; }
  if (size<SIZE_W) { return 0; }
  if (operands[1].type!=OPERAND_A_REG) { return 0; }

  int size_a=(size==SIZE_W)?3:2;

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(size_a<<12)|(operands[1].value<<9), size, 0, NO_EXTRA_IMM);
}

static int write_cmpm(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE) { return 0; }
  if (operands[0].type!=OPERAND_A_REG_INDEX_PLUS) { return 0; }
  if (operands[1].type!=OPERAND_A_REG_INDEX_PLUS) { return 0; }

  add_bin16(asm_context, opcode|(size<<6)|(operands[1].value<<9)|operands[0].value, IS_OPCODE);
  return 2;
}

static int write_bcd(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  int rm=-1;

  if (operands[0].type==OPERAND_A_REG_INDEX_MINUS &&
      operands[1].type==OPERAND_A_REG_INDEX_MINUS)
  {
    rm=8;
  }
    else
  if (operands[0].type==OPERAND_D_REG &&
      operands[1].type==OPERAND_D_REG)
  {
    rm=0;
  }

  if (rm==-1) { return 0; }

  add_bin16(asm_context, opcode|(operands[1].value<<9)|rm|operands[0].value, IS_OPCODE);

  return 2;
}

static int write_extended(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size==SIZE_NONE) { return 0; }

  int rm=-1;

  if (operands[0].type==OPERAND_A_REG_INDEX_MINUS &&
      operands[1].type==OPERAND_A_REG_INDEX_MINUS)
  {
    rm=8;
  }
    else
  if (operands[0].type==OPERAND_D_REG &&
      operands[1].type==OPERAND_D_REG)
  {
    rm=0;
  }

  if (rm==-1) { return 0; }

  add_bin16(asm_context, opcode|(operands[1].value<<9)|(size<<6)|rm|operands[0].value, IS_OPCODE);

  return 2;
}

static int write_rox(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size, int type)
{
  if (type==OP_ROX_MEM)
  {
    if (size!=SIZE_NONE) { return 0; }
    if (operand_count!=1) { return 0; }

    return ea_generic_all(asm_context, &operands[0], instr, opcode, size, EA_NO_D|EA_NO_A|EA_NO_PC|EA_NO_IMM, NO_EXTRA_IMM);
  }
    else
  if (type==OP_ROX)
  {
    if (size==SIZE_NONE) { return 0; }
    if (operand_count!=2) { return 0; }
    if (operands[1].type!=OPERAND_D_REG) { return 0; }

    if (operands[0].type==OPERAND_D_REG)
    {
      add_bin16(asm_context, opcode|(operands[0].value<<9)|(size<<6)|(1<<5)|operands[1].value, IS_OPCODE);
      return 2;
    }
      else
    if (operands[0].type==OPERAND_IMMEDIATE)
    {
      int count=(operands[0].value==8)?0:operands[0].value;
      add_bin16(asm_context, opcode|(count<<9)|(size<<6)|operands[1].value, IS_OPCODE);
      return 2;
    }
  }

  return 0;
}

static int write_exchange(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  if (operands[0].type==OPERAND_D_REG && operands[1].type==OPERAND_D_REG)
  {
    add_bin16(asm_context, opcode|(operands[0].value<<9)|(0x8<<3)|operands[1].value, IS_OPCODE);
    return 2;
  }
    else
  if (operands[0].type==OPERAND_A_REG && operands[1].type==OPERAND_A_REG)
  {
    add_bin16(asm_context, opcode|(operands[0].value<<9)|(0x9<<3)|operands[1].value, IS_OPCODE);
    return 2;
  }
    else
  if (operands[0].type==OPERAND_D_REG && operands[1].type==OPERAND_A_REG)
  {
    add_bin16(asm_context, opcode|(operands[0].value<<9)|(0x11<<3)|operands[1].value, IS_OPCODE);
    return 2;
  }

  return 0;
}

static int write_reg_ea_no_size(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  if (operands[0].type==OPERAND_D_REG)
  {
    return ea_generic_all(asm_context, &operands[1], instr, opcode|(operands[0].value<<9), 0, EA_NO_A|EA_NO_IMM|EA_NO_PC, NO_EXTRA_IMM);
  }

  return 0;
}

static int write_ea_extra_imm(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  if (operands[0].type==OPERAND_IMMEDIATE)
  {
    if (operands[0].value<0 || operands[0].value>255)
    {
      print_error_range("Immediate", 0, 255, asm_context);
      return -1;
    }

    return ea_generic_all(asm_context, &operands[1], instr, opcode, 0, EA_NO_A|EA_NO_IMM|EA_NO_PC, operands[0].value);
  }

  return 0;
}

static int write_ea_dreg_wl(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_W && size!=SIZE_L) { return 0; }
  if (operands[1].type!=OPERAND_D_REG) { return 0; }

  int size_a=(size==SIZE_W)?3:2;

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(operands[1].value<<9)|(size_a<<7), 0, EA_NO_A, NO_EXTRA_IMM);
}

static int write_logic_ccr(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_NONE) { return 0; }
  if (operands[0].type!=OPERAND_IMMEDIATE) { return 0; }
  if (operands[1].type!=OPERAND_SPECIAL_REG) { return 0; }
  if (operands[1].value!=SPECIAL_CCR) { return 0; }

  if (operands[0].value<0 || operands[0].value>255)
  {
    print_error_range("Immediate", 0, 255, asm_context);
    return -1;
  }

  add_bin16(asm_context, opcode, IS_OPCODE);
  add_bin16(asm_context, operands[0].value, IS_OPCODE);

  return 4;
}

static int write_displacement(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=1) { return 0; }
  if (operands[0].type!=OPERAND_ADDRESS) { return 0; }

  if (asm_context->pass==1)
  {
    if (size==SIZE_NONE)
    {
      if (operands[0].error==1) { size=SIZE_W; }
      else { size=calc_displacement_size(asm_context->address, operands[0].value); }
    }

    add_bin16(asm_context, (size+1)<<8, IS_OPCODE);
    int n;
    for (n=0; n<size; n++) { add_bin16(asm_context, 0, IS_OPCODE); }

    return (size+1)*2;
  }

  int len=memory_read(asm_context, asm_context->address);
  int offset;

  if (len==1)
  {
    offset=operands[0].value-(asm_context->address+2);
    if ((offset<-128 && offset>127) || offset==0 || offset==-1)
    {
      print_error_range("Offset", -128, 127, asm_context);
      return -1;
    }

    add_bin16(asm_context, opcode|(offset&0xff), IS_OPCODE);
    return 2;
  }
    else
  if (len==2)
  {
    offset=operands[0].value-(asm_context->address+4);
    if (offset<-32768 && offset>32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }

    add_bin16(asm_context, opcode|0x00, IS_OPCODE);
    add_bin16(asm_context, offset, IS_OPCODE);
    return 4;
  }
    else
  {
    offset=operands[0].value-(asm_context->address+6);
    add_bin16(asm_context, opcode|0xff, IS_OPCODE);
    add_bin32(asm_context, offset, IS_OPCODE);
    return 6;
  }
}

static int write_ext(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=1) { return 0; }
  if (operands[0].type!=OPERAND_D_REG) { return 0; }

  if (size==SIZE_W)
  {
    add_bin16(asm_context, opcode|(2<<6)|operands[0].value, IS_OPCODE);
    return 2;
  }
    else
  if (size==SIZE_L)
  {
    add_bin16(asm_context, opcode|(3<<6)|operands[0].value, IS_OPCODE);
    return 2;
  }

  return 0;
}

static int write_link(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (operands[0].type!=OPERAND_A_REG) { return 0; }
  if (operands[1].type!=OPERAND_IMMEDIATE) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  if (operands[1].value<0 || operands[1].value>65535)
  {
    print_error_range("Displacement", 0, 65535, asm_context);
    return -1;
  }

  add_bin16(asm_context, opcode|operands[0].value, IS_OPCODE);
  add_bin16(asm_context, operands[1].value, IS_OPCODE);

  return 4;
}

static int write_div_mul(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
  if (operand_count!=2) { return 0; }
  if (size!=SIZE_W) { return 0; }
  if (operands[1].type!=OPERAND_D_REG) { return 0; }

  return ea_generic_all(asm_context, &operands[0], instr, opcode|(operands[1].value<<9), 0, EA_NO_A, NO_EXTRA_IMM);
}

static int write_movep(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
int opmode;

  if (operand_count!=2) { return 0; }
  if (size!=SIZE_W && size!=SIZE_L) { return 0; }

  if (operands[0].type==OPERAND_D_REG &&
      operands[1].type==OPERAND_INDEX_DATA16_A_REG)
  {
    if (operands[1].value<-32768 || operands[1].value>32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }
    opmode=(size==SIZE_W)?6:7;
    add_bin16(asm_context, opcode|(operands[0].value<<9)|(opmode<<6)|operands[1].dis_reg, IS_OPCODE);
    add_bin16(asm_context, operands[1].value, IS_OPCODE);
    return 4;
  }
    else
  if (operands[0].type==OPERAND_INDEX_DATA16_A_REG &&
      operands[1].type==OPERAND_D_REG)
  {
    if (operands[0].value<-32768 || operands[0].value>32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }
    opmode=(size==SIZE_W)?4:5;
    add_bin16(asm_context, opcode|(operands[1].value<<9)|(opmode<<6)|operands[0].dis_reg, IS_OPCODE);
    add_bin16(asm_context, operands[0].value, IS_OPCODE);
    return 4;
  }

  return 0;
}

static int write_movem(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
struct _operand *operand=&operands[1];
int mask=0;
int dir=0;

  if (operand_count!=2) { return 0; }
  if (size!=SIZE_W && size!=SIZE_L) { return 0; }

  if (operands[0].type==OPERAND_MULTIPLE_REG)
  {
    mask=operands[0].value;
  }
    else
  if (operands[1].type==OPERAND_MULTIPLE_REG)
  {
    mask=operands[1].value;
    operand=&operands[0];
    dir=1;
  }
    else
  {
    return 0;
  }

  size=(size==SIZE_W)?0:1;

  if (dir==1)
  {
    // Memory to register dir=1
    return ea_generic_all(asm_context, operand, instr, opcode|(1<<10)|(size<<6), 0, EA_NO_A|EA_NO_D|EA_NO_IMM|EA_NO_MINUS|EA_NO_PC, mask);
  }
    else
  {
    if (operands[1].type==OPERAND_A_REG_INDEX_MINUS)
    {
      mask=reverse_bits16(mask);
    }

    // Register to memory dir=0
    return ea_generic_all(asm_context, operand, instr, opcode|(size<<6), 0, EA_NO_A|EA_NO_D|EA_NO_IMM|EA_NO_PLUS|EA_NO_PC, mask);
  }
}

static int write_move(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
unsigned int address;
unsigned short int ea_src_bytes[32];
unsigned short int ea_dst_bytes[32];
int src_len;
int dst_len;
int len=2;
int n;

  if (size==SIZE_NONE) { return 0; }

  // Translate to MOVEA
  if (operands[1].type==OPERAND_A_REG)
  {
    return write_movea(asm_context, instr, operands, operand_count, 0x0040, size);
  }

  opcode=opcode|(size<<12);
  address=asm_context->address;

  src_len=ea_generic_all(asm_context, &operands[0], instr, opcode, size, 0, NO_EXTRA_IMM);
  if (src_len==0) { return 0; }

//printf("src_len=%d\n", src_len);
  for (n=0; n<src_len; n++)
  {
    ea_src_bytes[n]=memory_read(asm_context, address+n);
//printf("%d %d %02x\n", asm_context->address, address+n, ea_src_bytes[n]);
  }

  asm_context->address=address;

  dst_len=ea_generic_all(asm_context, &operands[1], instr, opcode, size, EA_NO_A|EA_NO_IMM|EA_NO_PC, NO_EXTRA_IMM);
  if (dst_len==0) { return 0; }
//printf("dst_len=%d\n", src_len);

  for (n=0; n<dst_len; n++)
  {
    ea_dst_bytes[n]=memory_read(asm_context, address+n);
//printf("%d %d %02x\n", asm_context->address, address+n, ea_dst_bytes[n]);
  }

  asm_context->address=address;

  opcode|=(ea_dst_bytes[1]&0x3f)<<6;
  opcode|=(ea_src_bytes[1]&0x3f);

  //printf("%04x\n", opcode);
  add_bin16(asm_context, opcode, IS_OPCODE);
  for (n=2; n<src_len; n+=2)
  {
    opcode=(ea_src_bytes[n]<<8)|ea_src_bytes[n+1];
    add_bin16(asm_context, opcode, IS_OPCODE);
    len+=2;
  }

  for (n=2; n<dst_len; n+=2)
  {
    opcode=(ea_dst_bytes[n]<<8)|ea_dst_bytes[n+1];
    add_bin16(asm_context, opcode, IS_OPCODE);
    len+=2;
  }

  return len;
}

static int write_jump(struct _asm_context *asm_context, char *instr, struct _operand *operands, int operand_count, int opcode, int size)
{
int32_t offset=0;

  if (operand_count!=1) { return 0; }
  if (size!=SIZE_NONE) { return 0; }

  if (operands[0].type==OPERAND_ADDRESS)
  {
    //offset=operands[0].value-(asm_context->address+4);
    offset=operands[0].value-(asm_context->address+2);
    if (offset<-32768 || offset>32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
    }
    operands[0].value=offset;
    operands[0].type=OPERAND_INDEX_DATA16_PC;
  }

  return ea_generic_all(asm_context, &operands[0], instr, opcode, 0, EA_NO_A|EA_NO_D|EA_NO_IMM|EA_NO_PLUS|EA_NO_MINUS, NO_EXTRA_IMM);
}

int parse_instruction_680x0(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case_c[TOKENLEN];
char *instr_case=instr_case_c;
struct _operand operands[3];
int operand_count=0;
int operand_size=SIZE_NONE;
int matched=0;
int num;
int ret;
//int n,r;
//int count=1;
int n;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  if (strcmp("dbhi", instr_case)==0) { instr_case="dbcc"; }
  else if (strcmp("dblo", instr_case)==0) { instr_case="dbcs"; }
  else if (strcmp("dbra", instr_case)==0) { instr_case="dbt"; }

  memset(&operands, 0, sizeof(operands));
  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type==TOKEN_EOF)
    {
#if 0
      if (operand_count!=0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
#endif
      break;
    }

    if (operand_count>=3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if (IS_TOKEN(token, '.') && operand_count==0 && operand_size==SIZE_NONE)
    {
      token_type=tokens_get(asm_context, token, TOKENLEN);
      if (strcasecmp(token, "b")==0) { operand_size=SIZE_B; }
      else if (strcasecmp(token, "w")==0) { operand_size=SIZE_W; }
      else if (strcasecmp(token, "l")==0) { operand_size=SIZE_L; }
      else if (strcasecmp(token, "s")==0) { operand_size=SIZE_S; }
      else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      continue;
    }

    if ((num=get_register_d_680x0(token))!=-1)
    {
      operands[operand_count].type=OPERAND_D_REG;
      operands[operand_count].value=num;
    }
      else
    if ((num=get_register_a_680x0(token))!=-1)
    {
      operands[operand_count].type=OPERAND_A_REG;
      operands[operand_count].value=num;
    }
      else
    if ((num=get_register_special_680x0(token))!=-1)
    {
      operands[operand_count].type=OPERAND_SPECIAL_REG;
      operands[operand_count].value=num;
    }
      else
    if (token_type==TOKEN_POUND)
    {
      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          operands[operand_count].error=1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type=OPERAND_IMMEDIATE;
      operands[operand_count].value=num;
    }
      else
    if (IS_TOKEN(token,'-'))
    {
      if (expect_token_s(asm_context,"(")!=0) { return -1; }
      token_type=tokens_get(asm_context, token, TOKENLEN);
      if ((num=get_register_a_680x0(token))==-1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      if (expect_token_s(asm_context,")")!=0) { return -1; }

      operands[operand_count].type=OPERAND_A_REG_INDEX_MINUS;
      operands[operand_count].value=num;
    }
      else
    if (IS_TOKEN(token,'('))
    {
      token_type=tokens_get(asm_context, token, TOKENLEN);
      if ((num=get_register_a_680x0(token))!=-1)
      {
        operands[operand_count].value=num;
        if (expect_token_s(asm_context,")")!=0) { return -1; }
        token_type=tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'+'))
        {
          operands[operand_count].type=OPERAND_A_REG_INDEX_PLUS;
        }
          else
        {
          operands[operand_count].type=OPERAND_A_REG_INDEX;
          tokens_push(asm_context, token, token_type);
        }
      }
        else
      {
        // Check for displacement
        tokens_push(asm_context, token, token_type);

        int eval_error = 0;

        if (eval_expression(asm_context, &num)!=0)
        {
          if (asm_context->pass == 1)
          {
            eval_error = 1;
            eat_operand(asm_context);
          }
            else
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }
        }

        operands[operand_count].value=num;
        operands[operand_count].type=OPERAND_INDEX_DATA16_A_REG;

        token_type=tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, ','))
        {
          token_type=tokens_get(asm_context, token, TOKENLEN);
          if ((num=get_register_a_680x0(token))!=-1)
          {
            operands[operand_count].dis_reg=num;
          }
            else
          {
            if (strcasecmp(token, "pc")==0)
            {
              operands[operand_count].type=OPERAND_INDEX_DATA16_PC;
            }
          }

          if (expect_token_s(asm_context,")")!=0) { return -1; }
        }
          else
        {
          if (expect_token_s(asm_context,")")!=0) { return -1; }

          if (eval_error == 1)
          {
            memory_write(asm_context, asm_context->address, 4, asm_context->line);
          }

          token_type=tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, '.'))
          {
            token_type=tokens_get(asm_context, token, TOKENLEN);
            if (strcasecmp(token, "w") == 0)
            {
              //operands[operand_count].type = OPERAND_ADDRESS_W;
              operands[operand_count].type = OPERAND_ADDRESS;
              if (num < 0 || num > 0xffff)
              {
                print_error_range(instr, 0, 0xffff, asm_context);
                return -1;
              }
            }
              else
            if (strcasecmp(token, "l") == 0)
            {
              //operands[operand_count].type = OPERAND_ADDRESS_L;
              operands[operand_count].type = OPERAND_ADDRESS;
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
              //operands[operand_count].type = OPERAND_ADDRESS_L;
              operands[operand_count].type = OPERAND_ADDRESS;
            }

            tokens_push(asm_context, token, token_type);
          }
        }
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type=OPERAND_ADDRESS;
      operands[operand_count].value=num;
    }

    // See if multiple registers are listed like: a3-a5/d1/d2-d3
    if (operands[operand_count].type==OPERAND_D_REG ||
        operands[operand_count].type==OPERAND_A_REG)
    {
      token_type=tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'-') || IS_TOKEN(token,'/'))
      {
        int type=operands[operand_count].type;
        int curr=operands[operand_count].value;
        int mask=(1<<operands[operand_count].value);
        if (type==OPERAND_A_REG) { mask=mask<<8; }
        operands[operand_count].type=OPERAND_MULTIPLE_REG;
        operands[operand_count].value=0;

        while(1)
        {
          if (IS_TOKEN(token,'-'))
          {
            token_type=tokens_get(asm_context, token, TOKENLEN);
            if (type==OPERAND_D_REG)
            {
              num=get_register_d_680x0(token);
              if (num==-1 || num<curr)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              for (n=curr; n<=num; n++) { mask|=(1<<n); }
              type=0;
            }
              else
            if (type==OPERAND_A_REG)
            {
              num=get_register_a_680x0(token);
              if (num==-1 || num<curr)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              for (n=curr; n<=num; n++) { mask|=(1<<(n+8)); }
              type=0;
            }

            token_type=tokens_get(asm_context, token, TOKENLEN);
          }
            else
          if (IS_TOKEN(token,'/'))
          {
            token_type=tokens_get(asm_context, token, TOKENLEN);
            if ((num=get_register_d_680x0(token))!=-1)
            {
              type=OPERAND_D_REG;
              curr=num;
              mask|=(1<<num);
              token_type=tokens_get(asm_context, token, TOKENLEN);
            }
              else
            if ((num=get_register_a_680x0(token))!=-1)
            {
              type=OPERAND_A_REG;
              curr=num;
              mask|=(1<<(num+8));
              token_type=tokens_get(asm_context, token, TOKENLEN);
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
        operands[operand_count].value=mask;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }

    operand_count++;
    token_type=tokens_get(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token,',') || operand_count==3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
printf("-----\n");
for (n=0; n<operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  if (instr_case[0]=='d' && instr_case[1]=='b')
  {
    int opcode=0x50c8;

    for (n=0; n<16; n++)
    {
      if (strcmp(instr_case+2, table_680x0_condition_codes[n])!=0) { continue; }

      if (operand_size!=SIZE_NONE)
      {
        printf("Error: %s doesn't take a size attribute at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      if (operand_count!=2)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      if (operands[0].type!=OPERAND_D_REG || operands[1].type!=OPERAND_ADDRESS)
      {
        matched=1;
        continue;
      }

      int offset=operands[1].value-(asm_context->address+4);
      add_bin16(asm_context, opcode|(n<<8)|operands[0].value, IS_OPCODE);

      if (asm_context->pass==1) { add_bin16(asm_context, 0, IS_OPCODE); }
      else
      {
        if (offset<-32768 || offset>32767)
        {
          print_error_range("Offset", -32768, 32767, asm_context);
          return -1;
        }
        add_bin16(asm_context, offset, IS_OPCODE);
      }

      return 4;
    }
  }

  if (instr_case[0]=='b' && operand_count==1)
  {
    for (n=0; n<16; n++)
    {
      if (strcmp(instr_case+1, table_680x0_condition_codes[n])!=0) { continue; }
      if (operands[0].type!=OPERAND_ADDRESS) { continue; }
      matched=1;
      int opcode=0x6000|(n<<8);
      if (operand_size==SIZE_S) { operand_size=SIZE_B; }
      return write_displacement(asm_context, instr, operands, operand_count, opcode, operand_size);
    }
  }

  if (operand_size==SIZE_S && strcmp(instr_case, "bra")!=0)
  {
    print_error_unexp("s", asm_context);
    return -1;
  }

  if (instr_case[0]=='s' && operand_count==1)
  {
    for (n=0; n<16; n++)
    {
      if (strcmp(instr_case+1, table_680x0_condition_codes[n])!=0) { continue; }
      matched=1;
      if (operand_size!=SIZE_NONE) { continue; }
      int opcode=0x50c0|(n<<8);
      return ea_generic_all(asm_context, &operands[0], instr, opcode, 0, EA_NO_A|EA_NO_IMM|EA_NO_PC, NO_EXTRA_IMM);
    }
  }

  n=0;
  while(table_680x0[n].instr!=NULL)
  {
    if (strcmp(table_680x0[n].instr, instr_case)==0)
    {
      ret=0;
      matched=1;

      switch(table_680x0[n].type)
      {
        case OP_NONE:
          if (operand_count==0)
          {
            add_bin16(asm_context, table_680x0[n].opcode, IS_OPCODE);
            ret=2;
          }
          break;
        case OP_SINGLE_EA:
          ret=write_single_ea(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_SINGLE_EA_NO_SIZE:
          if (operand_size==SIZE_NONE)
          {
            ret=write_single_ea_no_size(asm_context, instr, operands, operand_count, table_680x0[n].opcode);
          }
          break;
#if 0
        case OP_SINGLE_EA_TO_ADDR:
          if (operand_size==SIZE_NONE)
          {
            ret=write_single_ea_to_addr(asm_context, instr, operands, operand_count, table_680x0[n].opcode);
          }
          break;
#endif
        case OP_IMMEDIATE:
          ret=write_immediate(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_SHIFT_EA:
          if (operand_size==SIZE_NONE || operand_size==SIZE_W)
          {
            ret=write_single_ea(asm_context, instr, operands, operand_count, table_680x0[n].opcode, 3);
          }
          break;
        case OP_SHIFT:
          ret=write_shift(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_REG_AND_EA:
          ret=write_reg_and_ea(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_VECTOR:
        case OP_VECTOR3:
          ret=write_vector(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size, table_680x0[n].type);
          break;
        case OP_AREG:
          ret=write_areg(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_REG:
          ret=write_reg(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_EA_AREG:
          ret=write_ea_areg(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_EA_DREG:
          ret=write_ea_dreg(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_LOAD_EA:
          ret=write_load_ea(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_QUICK:
        case OP_MOVE_QUICK:
          ret=write_quick(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size, table_680x0[n].type);
          break;
        case OP_MOVE_FROM_CCR:
        case OP_MOVE_TO_CCR:
        case OP_MOVE_FROM_SR:
          ret=write_move_special(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size, table_680x0[n].type);
          break;
        case OP_MOVEA:
          ret=write_movea(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_CMPM:
          ret=write_cmpm(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_BCD:
          ret=write_bcd(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_EXTENDED:
          ret=write_extended(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_ROX_MEM:
        case OP_ROX:
          ret=write_rox(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size, table_680x0[n].type);
          break;
        case OP_EXCHANGE:
          ret=write_exchange(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_REG_EA_NO_SIZE:
          ret=write_reg_ea_no_size(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_EXTRA_IMM_EA:
          ret=write_ea_extra_imm(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_EA_DREG_WL:
          ret=write_ea_dreg_wl(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_LOGIC_CCR:
          ret=write_logic_ccr(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_DISPLACEMENT:
          if (operand_size==SIZE_S) { operand_size=SIZE_B; }
          ret=write_displacement(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_EXT:
          ret=write_ext(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_LINK:
          ret=write_link(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_DIV_MUL:
          ret=write_div_mul(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_MOVEP:
          ret=write_movep(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_MOVEM:
          ret=write_movem(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_MOVE:
          ret=write_move(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        case OP_JUMP:
          ret=write_jump(asm_context, instr, operands, operand_count, table_680x0[n].opcode, operand_size);
          break;
        default:
          n++;
          continue;
      }
      if (ret!=0) { return ret; }
    }

    n++;
  }

  if (matched==1)
  {
    printf("Error: Unknown flag/operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1; 
}


