/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/msp430.h"
#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/tokens.h"
#include "disasm/msp430.h"
#include "table/msp430.h"

enum
{
  OPTYPE_ERROR,
  OPTYPE_REGISTER,
  OPTYPE_INDEXED,
  OPTYPE_REGISTER_INDIRECT,
  OPTYPE_REGISTER_INDIRECT_INC,
  OPTYPE_SYMBOLIC,
  OPTYPE_IMMEDIATE,
  OPTYPE_ABSOLUTE,
};

struct _operand
{
  int32_t value; // number value
  uint8_t reg;   // register
  uint8_t type;  // OPTYPE
  uint8_t error; // if expression can't be evaluated on pass 1
  uint8_t mode;  // As or Ad
};

struct _data
{
  struct _params
  {
    int value;
    int reg;
    int mode;
    int add_value;
  } params[2];
  int count;
};

#define CMD_SP_INC 0xdead0001
#define CMD_PC 0xdead0002
#define CMD_R3 0xdead0003
#define CMD_DST_DST 0xdead0004
#define CMD_SRC_DST 0xdead0005

static struct _aliases
{
  const char *instr;
  int operand_count;
  int opcode;
  const char *alt;
  int cmd;
} aliases[] =
{
  { "adc", 1, 0, "addc", 0 },
  { "br", 1, 0, "mov", CMD_PC },
  { "clrc", 0, 0xc312, NULL, 0 },
  { "clrn", 0, 0xc222, NULL, 0 },
  { "clrz", 0, 0xc322, NULL, 0 },
  { "dadc", 1, 0, "dadd", 0 },
  { "dec", 1, 0, "sub", 1 },
  { "decx", 1, 0, "subx", 1 },
  { "decd", 1, 0, "sub", 2 },
  { "dint", 0, 0xc232, NULL, 0 },
  { "eint", 0, 0xd232, NULL, 0 },
  { "inc", 1, 0, "add", 1 },
  { "incx", 1, 0, "addx", 1 },
  { "incd", 1, 0, "add", 2 },
  { "inv", 1, 0, "xor", -1 },
  { "nop", 0, 0x4303, NULL, CMD_R3 },
  { "pop", 1, 0, "mov", CMD_SP_INC },
  { "ret", 0, 0x4130, NULL, 0 },
  { "rla", 1, 0, "add", CMD_DST_DST },
  { "rlc", 1, 0, "addc", CMD_DST_DST },
  { "sbc", 1, 0, "subc", 0 },
  { "sbb", 2, 0, "subc", CMD_SRC_DST },
  { "setc", 0, 0xd312, NULL, 0 },
  { "setn", 0, 0xd222, NULL, 0 },
  { "setz", 0, 0xd322, NULL, 0 },
  { "tst", 1, 0, "cmp", 0 },
  { "clr", 1, 0, "mov", 0 },
  // MSP430X
  { "adcx", 1, 0, "addcx", 0 },
  { "bra", 1, 0, "mova", CMD_PC },
  { "clrx", 1, 0, "movx", 0 },
  { "clra", 1, 0, "mov", 0 },
  { "dadcx", 1, 0, "daddx", 0 },
  { "decx", 1, 0, "subx", 1 },
  { "decda", 1, 0, "suba", 2 },
  { "decdx", 1, 0, "subx", 2 },
  { "incda", 1, 0, "adda", 2 },
  { "incdx", 1, 0, "addx", 2 },
  { "invx", 1, 0, "xorx", -1 },
  { "popx", 1, 0, "movx", CMD_SP_INC },
  { "rlax", 1, 0, "addx", CMD_DST_DST },
  { "rlcx", 1, 0, "addcx", CMD_DST_DST },
  { "sbcx", 1, 0, "subcx", 0 },
  { "sbbx", 2, 0, "subcx", CMD_SRC_DST },
  { "reta", 0, 0x0110, NULL, 0 },
  { "tsta", 1, 0, "cmpa", 0 },
  { "tstx", 1, 0, "cmpx", 0 },
  { NULL, 0, 0, NULL, 0 },
};

static char *msp430x_rpt[] = { "rpt", "rptz", "rptc",  NULL };

static void operand_to_cg(struct _asm_context *asm_context, struct _operand *operand, int bw)
{
  if (operand->type != OPTYPE_IMMEDIATE) { return; }

  if (memory_read(asm_context, asm_context->address) == 1) { return; }

  if (bw == 1 && operand->value == 0xff) { operand->value = -1; }
  if (bw == 0 && operand->value == 0xffff) { operand->value = -1; }

  switch (operand->value)
  {
    case -1:
      operand->type = OPTYPE_REGISTER;
      operand->mode = 3;
      operand->reg = 3;
      break;
    case 0:
      operand->type = OPTYPE_REGISTER;
      operand->mode = 0;
      operand->reg = 3;
      break;
    case 1:
      operand->type = OPTYPE_REGISTER;
      operand->mode = 1;
      operand->reg = 3;
      break;
    case 2:
      operand->type = OPTYPE_REGISTER;
      operand->mode = 2;
      operand->reg = 3;
      break;
    case 4:
      operand->type = OPTYPE_REGISTER;
      operand->mode = 2;
      operand->reg = 2;
      break;
    case 8:
      operand->type = OPTYPE_REGISTER;
      operand->mode = 3;
      operand->reg = 2;
      break;
    default:
      break;
  }
}

static int process_operand(
  struct _asm_context *asm_context,
  struct _operand *operand,
  struct _data *data,
  const char *instr,
  int size,
  int is_src,
  int is_extended)
{
  int count = data->count++;

  if (operand->type == OPTYPE_REGISTER)
  {
    data->params[count].reg = operand->reg;
    data->params[count].mode = operand->mode;
    return 0;
  }

  if (operand->type == OPTYPE_REGISTER_INDIRECT)
  {
    if (is_src == 1)
    {
      data->params[count].reg = operand->reg;
      data->params[count].mode = 2;
      return 0;
    }

    // If this is a destination, then turn @reg to 0(reg)
    operand->type = OPTYPE_INDEXED;
  }

  if (operand->type == OPTYPE_REGISTER_INDIRECT_INC)
  {
    if (is_src == 0)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    data->params[count].reg = operand->reg;
    data->params[count].mode = 3;
    return 0;
  }

  if (operand->type == OPTYPE_IMMEDIATE ||
      operand->type == OPTYPE_ABSOLUTE ||
      operand->type == OPTYPE_SYMBOLIC ||
      operand->type == OPTYPE_INDEXED)
  {
    int low = 0, high = 0xffff;
    int mode = 1;
    int value = operand->value;
    int reg = 0;
    const char *num_type = "Address";

    if (is_extended == 1) { high = 0xfffff; }

    if (operand->type == OPTYPE_IMMEDIATE)
    {
      num_type = "Immediate";

      if (size == 8)
      {
        low = -128;
        high = 0xff;
      }
        else
      if (size == 20 && is_extended == 1)
      {
        low = -524288;
        high = 0xfffff;
      }
        else
      {
        low = -32768;
      }

      mode = 3;
    }

    if (operand->type == OPTYPE_INDEXED)
    {
      low = -32768;
      mode = 1;
      reg = operand->reg;

      if (is_extended == 1)
      {
        low = -524288;
        high = 0xfffff;
      }
    }

    if (operand->type == OPTYPE_ABSOLUTE) { reg = 2; }

    if (value < low || value > high)
    {
      print_error_range(num_type, low, high, asm_context);
      return -1;
    }

    if (operand->type == OPTYPE_SYMBOLIC)
    {
      int len = (is_extended == 0) ? 2 : 4;

      if (count == 1 && data->params[0].add_value == 1)
      {
        value = value - (asm_context->address + len + 2);
      }
        else
      {
        value = value - (asm_context->address + len);
      }
    }

    if (is_src == 0 && mode > 1)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    data->params[count].reg = reg;
    data->params[count].mode = mode;
    data->params[count].value = value;
    data->params[count].add_value = 1;

    return 0;
  }

  print_error_illegal_operands(instr, asm_context);
  return -1;
}

static uint16_t get_prefix(struct _asm_context *asm_context, int zc)
{
  char token[TOKENLEN];
  //int token_type;
  uint16_t prefix = 0xffff;
  int num;

  tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token,'#'))
  {
    if (eval_expression(asm_context, &num) != 0)
    {
      print_error_unexp(token, asm_context);
      return 0xffff;
    }

    if (num < 1 || num > 16)
    {
      print_error("Constant can only be between 1 and 16", asm_context);
      return 0xffff;
    }

    prefix = 0x1800 | (zc << 8) | (num - 1);
  }
    else
  {
    num = get_register_msp430(token);

    if (num >= 0)
    {
      prefix = 0x1880 | (zc << 8) | num;
    }
  }

  if (prefix == 0xffff)
  {
    print_error("Expecting register or immediate", asm_context);
    return -1;
  }

  if (expect_token(asm_context, ',') != 0)
  {
    return 0xffff;
  }

  return prefix;
}

int parse_instruction_msp430(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[3];
  struct _data data;
  int operand_count = 0;
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  //char *instr_case;
  int token_type;
  int size = 0;
  int num,n;
  int bw = 0, al = 0;
  int opcode;
  //int msp430x = 0;
  int prefix = 0;
  int offset, value, wa, reg;
  int count, found = 0;

  lower_copy(instr_case, instr);
  //instr_case = instr_case_mem;

  memset(&data, 0, sizeof(data));

  // Not sure if this is a good area for this.  If there isn't an instruction
  // here then it pads for no reason.
  if ((asm_context->address & 0x01) != 0)
  {
    if (asm_context->pass == 2)
    {
      printf("Warning: Instruction doesn't start on 16 bit boundary at %s:%d.  Padding with a 0.\n", asm_context->tokens.filename, asm_context->tokens.line);
    }
    memory_write_inc(asm_context, 0, DL_NO_CG);
  }

  // check for RPT prefix
  if (asm_context->cpu_type == CPU_TYPE_MSP430X)
  {
    n = 0;

    while (msp430x_rpt[n] != NULL)
    {
      if (strcmp(instr_case, msp430x_rpt[n]) == 0)
      {
        prefix = get_prefix(asm_context, n & 1);

        if (prefix == 0xffff) { return -1; }

        while (1)
        {
          token_type = tokens_get(asm_context, instr, TOKENLEN);
          if (token_type != TOKEN_EOL) { break; }
          if (token_type == TOKEN_EOF)
          {
            print_error("Unexpected end of file", asm_context);
            return -1;
          }
          asm_context->tokens.line++;
        }

        lower_copy(instr_case, instr);

        break;
      }

      n++;
    }
  }

  memset(&operands, 0, sizeof(operands));
  //data.count = 0;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }

    if (operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    operands[operand_count].reg = -1;
    num = 0;

    if (operand_count == 0)
    {
      if (IS_TOKEN(token,'.'))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token,'b') || IS_TOKEN(token,'B')) { size = 8; }
        else if (IS_TOKEN(token,'w') || IS_TOKEN(token,'W')) { size = 16; }
        else if (IS_TOKEN(token,'a') || IS_TOKEN(token,'A')) { size = 20; }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

#if 0
        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (token_type < 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
#endif

        continue;
      }
    }

    if (IS_TOKEN(token,'#'))
    {
      operands[operand_count].type = OPTYPE_IMMEDIATE;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          operands[operand_count].error = 1;

          // Store a flag in this address to remind on pass 2 that this
          // instruction can't use CG.
          memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }

      operands[operand_count].value = num;
      operands[operand_count].error = memory_read(asm_context, asm_context->address);
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      operands[operand_count].type = OPTYPE_REGISTER_INDIRECT;
      token_type = tokens_get(asm_context, token, TOKENLEN);
      num = get_register_msp430(token);

      if (num < 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].reg = num;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type = OPTYPE_REGISTER_INDIRECT_INC;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token,'&'))
    {
      operands[operand_count].type = OPTYPE_ABSOLUTE;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          operands[operand_count].error = 1;
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }

      operands[operand_count].value=num;
    }
      else
    {
      num = get_register_msp430(token);
      if (num >= 0)
      {
        operands[operand_count].type = OPTYPE_REGISTER;
        operands[operand_count].reg = num;
      }
        else
      {
        operands[operand_count].type = OPTYPE_SYMBOLIC;

        // Issue #36: Instead of assuming this a 16 bit instruction
        // with 16 bits of data going with it, try and evaluate
        // the expression.  If it fails then mark it as 16 + 16 bit
        // no matter what.  If it's 0(reg) then it can be optimized
        // later to @reg.

        tokens_push(asm_context, token, token_type);

        int eval_error = eval_expression(asm_context, &num);

        if (asm_context->pass == 2 && eval_error != 0)
        {
          if (asm_context->pass == 2)
          {
            return -1;
          }
        }

        if (asm_context->pass == 1 && eval_error != 0)
        {
          ignore_operand(asm_context);

          // Set this operand up as something that can't be optimized.
          // It shouldn't matter what it is.
          operands[operand_count].value = 101;
        }
          else
        {
          operands[operand_count].value = num;

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token,'('))
          {
            operands[operand_count].type = OPTYPE_INDEXED;

            token_type = tokens_get(asm_context, token, TOKENLEN);
            {
              int reg = get_register_msp430(token);

              if (reg < 0)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }

              operands[operand_count].reg = reg;
            }

            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (IS_NOT_TOKEN(token,')'))
            {
              print_error_unexp(token, asm_context);
            }
          }
            else
          {
            tokens_push(asm_context, token, token_type);
          }
        }
      }
    }

    //operands[operand_count++].value = num;
    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
  printf("-------- line=%d instr=%s bw=%d\n", asm_context->tokens.line, instr, bw);
  for (n = 0; n < operand_count; n++)
  {
    printf("operand %d: value=%d type=%d error=%d\n", n, operands[n].value, operands[n].type, operands[n].error);
  }
#endif

  // Issue #36: If this instruction is 0(reg) and -optimize is turned
  // on then turn into @reg.
  if (asm_context->optimize == 1)
  {
    if (operands[0].type == OPTYPE_INDEXED)
    {
      if (asm_context->pass == 1)
      {
        if (operands[0].value == 0)
        {
          memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
          operands[0].type = OPTYPE_REGISTER_INDIRECT;
        }
      }
        else
      {
        if (operands[0].value == 0 &&
            memory_read(asm_context, asm_context->address) == 1)
        {
          operands[0].type = OPTYPE_REGISTER_INDIRECT;
        }
      }
    }
  }

  // Do aliases first
  n = 0;

  while (aliases[n].instr != NULL)
  {
    if (strcmp(instr_case, aliases[n].instr) == 0)
    {
      if (aliases[n].operand_count != operand_count)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      if (aliases[n].operand_count == 0)
      {
        add_bin16(asm_context, aliases[n].opcode, IS_OPCODE);
        return 0;
      }

      switch (aliases[n].cmd)
      {
        case CMD_SP_INC:
          memcpy(&operands[1], &operands[0], sizeof(struct _operand));
          memset(&operands[0], 0, sizeof(struct _operand));
          operands[0].reg = 1;
          operands[0].type = OPTYPE_REGISTER_INDIRECT_INC;
          break;
        case CMD_PC:
          operands[1].reg = 0;
          operands[1].type = OPTYPE_REGISTER;
          break;
        case CMD_R3:
          memset(&operands[0], 0, sizeof(struct _operand) * 2);
          operands[0].value = 0;
          operands[0].type = OPTYPE_IMMEDIATE;
          operands[1].reg = 3;
          operands[1].type = OPTYPE_REGISTER;
          break;
        case CMD_DST_DST:
          memcpy(&operands[1], &operands[0], sizeof(struct _operand));
          break;
        case CMD_SRC_DST:
          break;
        default:
          memcpy(&operands[1], &operands[0], sizeof(struct _operand));
          memset(&operands[0], 0, sizeof(struct _operand));
          operands[0].value = aliases[n].cmd;
          operands[0].type = OPTYPE_IMMEDIATE;
          break;
      }

      operand_count = 2;
      //instr_case = (char *)aliases[n].alt;
      strcpy(instr_case, (char *)aliases[n].alt);
      break;
    }

    n++;
  }

  if (size == 8) { bw = 1; }

  n = 0;

  while (table_msp430[n].instr != NULL)
  {
    if (asm_context->cpu_type != CPU_TYPE_MSP430X &&
        table_msp430[n].version != VERSION_MSP430)
    {
      n++;
      continue;
    }

    if (strcmp(table_msp430[n].instr, instr_case) == 0)
    {
      opcode = table_msp430[n].opcode;
      found = 1;

      if (prefix != 0)
      {
        if (table_msp430[n].version == VERSION_MSP430X_EXT)
        {
          print_error("Instruction doesn't support RPT", asm_context);
          return -1;
        }

        add_bin16(asm_context, prefix, IS_OPCODE);
        prefix = 0;
      }

      if (table_msp430[n].version == VERSION_MSP430 && size == 20)
      {
        print_error("Instruction doesn't support .a", asm_context);
        return -1;
      }

      if (table_msp430[n].version == VERSION_MSP430X && size == 8)
      {
        print_error("Instruction doesn't support .b", asm_context);
        return -1;
      }

      switch (table_msp430[n].type)
      {
        case OP_NONE:
          add_bin16(asm_context, table_msp430[n].opcode, IS_OPCODE);
          return 2;
        case OP_ONE_OPERAND:
        case OP_ONE_OPERAND_W:
        case OP_ONE_OPERAND_X:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (table_msp430[n].type == OP_ONE_OPERAND_W)
          {
            if (size != 0 && size != 16)
            {
              printf("Error: Instruction '%s' can't be used with .b at %s:%d\n",
                instr, asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
          }
            else
          if (table_msp430[n].type == OP_ONE_OPERAND_X && size != 0)
          {
            if (size != 0)
            {
              printf("Error: Instruction '%s' can't be used with .b/w at %s:%d\n",
                 instr, asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
          }
            else
          {
            operand_to_cg(asm_context, &operands[0], bw);
          }

          opcode |= bw << 6;

          if (process_operand(asm_context, &operands[0], &data, instr, size, 1, 0) != 0)
          {
            return -1;
          }

          opcode |= (data.params[0].mode << 4) | data.params[0].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);

          if (data.params[0].add_value == 1)
          {
            add_bin16(asm_context, data.params[0].value, IS_OPCODE);
            return 4;
          }

          return 2;
        case OP_JUMP:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("Jump doesn't take a size.", asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            offset = asm_context->address;
          }
            else
          {
            if (operands[0].type != OPTYPE_SYMBOLIC)
            {
              print_error("Expecting a branch address", asm_context);
              return -1;
            }

            offset = operands[0].value;
          }

          if ((offset & 1) == 1)
          {
            print_error("Jump offset is odd", asm_context);
            return -1;
          }

          offset = offset - (asm_context->address + 2);

          if (offset < -1024 || offset > 1023)
          {
            print_error_range("Offset", -1024, 1022, asm_context);
            return -1;
          }

          offset = offset >> 1;

          opcode |= ((uint32_t)offset) & 0x03ff;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_TWO_OPERAND:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          operand_to_cg(asm_context, &operands[0], bw);

          opcode |= bw << 6;

          if (process_operand(asm_context, &operands[0], &data, instr, size, 1, 0) != 0)
          {
            return -1;
          }

          if (process_operand(asm_context, &operands[1], &data, instr, size, 0, 0) != 0)
          {
            return -1;
          }

          opcode |= (data.params[0].mode << 4) |
                    (data.params[1].mode << 7) |
                    (data.params[0].reg << 8) |
                     data.params[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);

          count = 2;

          if (data.params[0].add_value)
          {
            add_bin16(asm_context, data.params[0].value, IS_OPCODE);
            count += 2;
          }

          if (data.params[1].add_value)
          {
            add_bin16(asm_context, data.params[1].value, IS_OPCODE);
            count += 2;
          }

          return count;
        case OP_MOVA_AT_REG_REG:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("mova doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_REGISTER_INDIRECT ||
              operands[1].type != OPTYPE_REGISTER)
          {
            break;
          }

          opcode |= (operands[0].reg << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        case OP_MOVA_AT_REG_PLUS_REG:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("mova doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_REGISTER_INDIRECT_INC ||
              operands[1].type != OPTYPE_REGISTER)
          {
            break;
          }

          opcode |= (operands[0].reg << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        case OP_MOVA_ABS20_REG:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("mova doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_ABSOLUTE ||
              operands[1].type != OPTYPE_REGISTER)
          {
            break;
          }

          value = operands[0].value;

          if (value < 0 || value > 0xfffff)
          {
            print_error_range("Address", 0, 0xfffff, asm_context);
            return -1;
          }

          opcode |= (((operands[0].value >> 16) & 0xf) << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
          return 4;
        case OP_MOVA_INDEXED_REG:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("mova doesn't take a size flag", asm_context);
            return -1;
          }

          value = operands[0].value;

          if (operands[0].type == OPTYPE_SYMBOLIC)
          {
            operands[0].type = OPTYPE_INDEXED;
            operands[0].reg = 0;

            if (asm_context->pass == 1)
            {
              value = 0;
            }
              else
            {
              value = value - (asm_context->address + 2);
            }
          }

          if (operands[0].type != OPTYPE_INDEXED ||
              operands[1].type != OPTYPE_REGISTER)
          {
            break;
          }

          if (value < -32768 || value > 32767)
          {
            print_error_range("Index", -32768, 32768, asm_context);
            return -1;
          }

          opcode |= (operands[0].reg << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, value & 0xffff, IS_OPCODE);
          return 4;
        case OP_SHIFT20:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_IMMEDIATE ||
              operands[1].type != OPTYPE_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          value = operands[0].value;

          if (value < 1 || value > 4)
          {
            print_error_range("Constant", 0, 4, asm_context);
            return -1;
          }

          wa = (size == 16 || size == 0) ? 1 : 0;

          opcode |= ((operands[0].value - 1) << 10) |
                     (wa << 4) |
                      operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        case OP_MOVA_REG_ABS:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("mova doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_REGISTER ||
              operands[1].type != OPTYPE_ABSOLUTE)
          {
            break;
          }

          value = operands[1].value;

          if (value < 0 || value > 0xfffff)
          {
            print_error_range("Address", 0, 0xfffff, asm_context);
            return -1;
          }

          opcode |= (operands[0].reg << 8) | ((operands[1].value >> 16) & 0xf);
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
          return 4;
        case OP_MOVA_REG_INDEXED:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("mova doesn't take a size flag", asm_context);
            return -1;
          }

          value = operands[1].value;

          if (operands[1].type == OPTYPE_SYMBOLIC)
          {
            operands[1].type = OPTYPE_INDEXED;
            operands[1].reg = 0;

            if (asm_context->pass == 1)
            {
              value = 0;
            }
              else
            {
              value = value - (asm_context->address + 2);
            }
          }

          if (operands[0].type != OPTYPE_REGISTER ||
              operands[1].type != OPTYPE_INDEXED)
          {
            break;
          }

          if (value < -32768 || value > 32767)
          {
            print_error_range("Index", -32768, 32767, asm_context);
            return -1;
          }

          opcode |= (operands[0].reg << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
          return 4;
        case OP_IMMEDIATE_REG:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("Instruction doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_IMMEDIATE ||
              operands[1].type != OPTYPE_REGISTER)
          {
            break;
          }

          value = operands[1].value;

          if (value < -524288 || value > 0xfffff)
          {
            print_error_range("Index", -524288, 0xfffff, asm_context);
            return -1;
          }

          opcode |= (((operands[0].value >> 16) & 0xf) << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
          return 4;
        case OP_REG_REG:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("Instruction doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_REGISTER ||
              operands[1].type != OPTYPE_REGISTER)
          {
            break;
          }

          opcode |= (operands[0].reg << 8) | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        case OP_CALLA_SOURCE:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("calla doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type == OPTYPE_REGISTER)
          {
            opcode |= operands[0].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          if (operands[0].type == OPTYPE_REGISTER_INDIRECT)
          {
            opcode |= (2 << 4) | operands[0].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          if (operands[0].type == OPTYPE_REGISTER_INDIRECT_INC)
          {
            opcode |= (3 << 4) | operands[0].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          if (operands[0].type == OPTYPE_INDEXED)
          {
            // Optimization.  Use x(PC) instead of reg == PC.
            if (operands[0].reg == 0)
            {
              break;
            }

            if (operands[0].value < -32768 || operands[0].value > 32767)
            {
              print_error_range("Index", -32768, 32767, asm_context);
              return -1;
            }

            opcode |= (1 << 4) | operands[0].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
            return 2;
          }

          break;
        case OP_CALLA_ABS20:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("calla doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type == OPTYPE_ABSOLUTE)
          {
            value = operands[0].value;

            if (value < 0 || value > 0xfffff)
            {
              print_error_range("Address", 0, 0xfffff, asm_context);
              return -1;
            }

            opcode |= ((value >> 16) & 0xf) | operands[1].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, value & 0xffff, IS_OPCODE);
            return 4;
          }

          break;
        case OP_CALLA_INDIRECT_PC:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("calla doesn't take a size flag", asm_context);
            return -1;
          }

          value = operands[0].value;

          if (operands[0].type == OPTYPE_INDEXED &&
              operands[0].reg == 0)
          {
            if (value < -524288 || value > 0xfffff)
            {
              print_error_range("Index", -524288, 0xfffff, asm_context);
              return -1;
            }

            opcode |= ((value >> 16) & 0xf) | operands[1].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, value & 0xffff, IS_OPCODE);
            return 4;
          }

          if (operands[0].type == OPTYPE_SYMBOLIC)
          {
            if (value < 0 || value > 0xfffff)
            {
              print_error_range("Address", 0, 0xfffff, asm_context);
              return -1;
            }

            value = value - (asm_context->address + 2);

            opcode |= ((value >> 16) & 0xf) | operands[1].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, value & 0xffff, IS_OPCODE);
            return 4;
          }

          break;
        case OP_CALLA_IMMEDIATE:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (size != 0)
          {
            print_error("calla doesn't take a size flag", asm_context);
            return -1;
          }

          if (operands[0].type == OPTYPE_IMMEDIATE)
          {
            value = operands[0].value;

            if (value < 0 || value > 0xfffff)
            {
              print_error_range("Immediate", 0, 0xfffff, asm_context);
              return -1;
            }

            opcode |= ((value >> 16) & 0xf) | operands[1].reg;
            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, value & 0xffff, IS_OPCODE);
            return 4;
          }

          break;
        case OP_PUSH:
        case OP_POP:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPTYPE_IMMEDIATE ||
              operands[1].type != OPTYPE_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          value = operands[0].value;

          if (value < 1 || value > 16)
          {
            print_error_range("Constant", 1, 16, asm_context);
            return -1;
          }

          wa = (size == 16 || size == 0) ? 1 : 0;

          if (table_msp430[n].type == OP_POP)
          {
            reg = operands[1].reg - (operands[0].value - 1);
          }
            else
          {
            reg = operands[1].reg;
          }

          if (reg < 0 || reg > 15)
          {
            print_error_illegal_register(instr, asm_context);
            return -1;
          }

          opcode |= (wa << 8) | ((operands[0].value - 1) << 4) | reg;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_X_ONE_OPERAND:
        case OP_X_ONE_OPERAND_W:
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (table_msp430[n].type == OP_X_ONE_OPERAND_W)
          {
            if (size != 0 && size != 16)
            {
              printf("Error: Instruction '%s' can't be used with .b at %s:%d\n",
                instr, asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
          }
            else
          {
            operand_to_cg(asm_context, &operands[0], bw);
          }

          if (size == 8) { al = 1; bw = 1; }
          else if (size == 16) { al = 1; bw = 0; }
          else if (size == 20) { al = 0; bw = 1; }
          else { al = 1; bw = 0; }

          if (size == 20 &&
              operands[0].type == OPTYPE_IMMEDIATE &&
              operands[0].value == 0xfffff &&
              operands[0].error == 0)
          {
            operands[0].value = -1;
            operands[0].type = OPTYPE_REGISTER;
            operands[0].mode = 3;
            operands[0].reg = 3;
          }

          opcode |= bw << 6;

          if (process_operand(asm_context, &operands[0], &data, instr, size, 1, 1) != 0)
          {
            return -1;
          }

          prefix = 0x1800;
          prefix |= (al << 6);

          if (data.params[0].add_value)
          {
            prefix |= ((data.params[0].value >> 16) & 0xf) << 7;
          }

          add_bin16(asm_context, prefix, IS_OPCODE);

          opcode |= (data.params[0].mode << 4) | data.params[0].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);

          if (data.params[0].add_value == 1)
          {
            add_bin16(asm_context, data.params[0].value, IS_OPCODE);
            return 6;
          }

          return 4;
        case OP_X_TWO_OPERAND:
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          operand_to_cg(asm_context, &operands[0], bw);

          if (size == 8) { al = 1; bw = 1; }
          else if (size == 16) { al = 1; bw = 0; }
          else if (size == 20) { al = 0; bw = 1; }
          else { al = 1; bw = 0; }

          if (size == 20 &&
              operands[0].type == OPTYPE_IMMEDIATE &&
              operands[0].value == 0xfffff &&
              operands[0].error == 0)
          {
            operands[0].value = -1;
            operands[0].type = OPTYPE_REGISTER;
            operands[0].mode = 3;
            operands[0].reg = 3;
          }

          opcode |= bw << 6;

          if (process_operand(asm_context, &operands[0], &data, instr, size, 1, 1) != 0)
          {
            return -1;
          }

          if (process_operand(asm_context, &operands[1], &data, instr, size, 0, 1) != 0)
          {
            return -1;
          }

          prefix = 0x1800;
          prefix |= (al << 6);

          if (data.params[0].add_value)
          {
            prefix |= ((data.params[0].value >> 16) & 0xf) << 7;
          }

          if (data.params[1].add_value)
          {
            prefix |= (data.params[1].value >> 16) & 0xf;
          }

          add_bin16(asm_context, prefix, IS_OPCODE);

          opcode |= (data.params[0].mode << 4) |
                    (data.params[1].mode << 7) |
                    (data.params[0].reg << 8) |
                     data.params[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);

          count = 2;

          if (data.params[0].add_value)
          {
            add_bin16(asm_context, data.params[0].value, IS_OPCODE);
            count += 2;
          }

          if (data.params[1].add_value)
          {
            add_bin16(asm_context, data.params[1].value, IS_OPCODE);
            count += 2;
          }

          return count;
        default:
          print_error_illegal_operands(instr, asm_context);
          return -1;
      }
    }

    n++;
  }

  if (found == 1)
  {
    print_error_illegal_operands(instr, asm_context);
    return -1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

int link_function_msp430(
  struct _asm_context *asm_context,
  struct _imports *imports,
  const uint8_t *code,
  uint32_t function_offset,
  int size,
  uint8_t *obj_file,
  uint32_t obj_size)
{
  return -1;
}

