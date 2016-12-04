/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
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
  int reg;     // register
  int value;   // number value
  int type;    // OPTYPE
  int error;   // if expression can't be evaluated on pass 1
  int a;       // As or Ad
};

struct _data
{
  int data[2];
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
  { NULL, 0, 0, NULL, 0 },
};

static char *one_oper[] = { "rrc", "swpb", "rra", "sxt", "push", "call", NULL };
static char *jumps[] = { "jne", "jeq", "jlo", "jhs", "jn", "jge", "jl", "jmp", NULL };
static char *jumps_a[] = { "jnz", "jz", "jnc", "jc", NULL, NULL, NULL, NULL, NULL };
static char *two_oper[] =
{
  "mov", "add", "addc", "subc", "sub", "cmp", "dadd", "bit",
  "bic", "bis", "xor", "and", NULL
};
static char *msp430x_ext[] =
{
  "rrcx", "swpbx", "rrax", "sxtx", "pushx", "movx", "addx", "addcx", "subcx",
  "subx", "cmpx", "daddx", "bitx", "bicx", "bisx", "xorx", "andx", NULL
};
static char *msp430x_shift[] = { "rrcm", "rram", "rlam", "rrum", NULL };
static char *msp430x_stack[] = { "pushm", "popm", NULL };
static char *msp430x_alu[] = { "mova", "cmpa", "adda", "suba", NULL };
static char *msp430x_rpt[] = { "rpt", "rptz", "rptc",  NULL };

#if 0
static void print_operand_error(const char *s, int count, struct _asm_context *asm_context)
{
  printf("Error: Instruction '%s' takes %d operand%s at %s:%d\n", s, count, count==1?"":"s", asm_context->filename, asm_context->line);
}
#endif

static int process_operand(struct _asm_context *asm_context, struct _operand *operand, struct _data *data, int size, int is_dest)
{
  if (size == 0) { size = 16; }

  if (operand->type == OPTYPE_IMMEDIATE)
  {
    if (size == 8)
    {
      if (operand->value > 0xff || operand->value < -128)
      {
        print_error("Constant larger than 8 bit.", asm_context);
        return -1;
      }

      operand->value = (((uint16_t)operand->value) & 0xff);
    }
      else
    if (size == 16)
    {
      if (operand->value > 0xffff || operand->value < -32768)
      {
        print_error("Constant larger than 16 bit.", asm_context);
        return -1;
      }

      operand->value = (((uint32_t)operand->value) & 0xffff);
    }
      else
    if (size == 20)
    {
      if (operand->value > 0xfffff || operand->value < -524288)
      {
        print_error("Constant larger than 20 bit.", asm_context);
        return -1;
      }

      operand->value = (((uint32_t)operand->value) & 0xfffff);
    }
  }
    else
  if (operand->type >= OPTYPE_ABSOLUTE ||
      operand->type >= OPTYPE_INDEXED ||
      operand->type >= OPTYPE_SYMBOLIC)
  {
    if (operand->value > 0xffff || operand->value < -32768)
    {
      print_error("Constant larger than 16 bit.", asm_context);
      return -1;
    }

    operand->value = (((uint32_t)operand->value) & 0xffff);
  }

  switch(operand->type)
  {
    case OPTYPE_REGISTER:
      operand->a = 0;
      break;
    case OPTYPE_INDEXED:
      operand->a = 1;
      data->data[data->count++] = operand->value;
      break;
    case OPTYPE_REGISTER_INDIRECT:
      if (is_dest == 1)
      {
        if (asm_context->pass == 2)
        {
          printf("Warning: Addressing mode of @r%d being changed to 0(r%d) at %s:%d.\n", operand->reg, operand->reg, asm_context->filename, asm_context->line);
        }

        data->data[data->count++] = 0;
        operand->a = 1;
        operand->value = 0;
      }
        else
      {
        operand->a = 2;
      }
      break;
    case OPTYPE_REGISTER_INDIRECT_INC:
      if (is_dest == 1)
      {
        printf("Error: Indirect autoincrement not allowed for dest operand at %s:%d.\n", asm_context->filename, asm_context->line);
        return -1;
      }
      operand->a = 3;
      break;
    case OPTYPE_SYMBOLIC:
      operand->a = 1;
      operand->reg = 0;
      data->data[data->count] = (operand->value - (asm_context->address + (data->count * 2 + 2))) & 0xffff;
      data->count++;
      break;
    case OPTYPE_IMMEDIATE:
      if (is_dest == 1)
      {
        printf("Error: Immediate not allowed for dest operand at %s:%d.\n", asm_context->filename, asm_context->line);
        return -1;
      }
      if (memory_read(asm_context, asm_context->address)!=0 ||
          operand->error != 0)
      {
        operand->a = 3;
        operand->reg = 0;
        data->data[data->count++] = operand->value;
      }
        else
      if (operand->value == 0) { operand->reg = 3; operand->a = 0; }
      else if (operand->value == 1) { operand->reg = 3; operand->a = 1; }
      else if (operand->value == 2) { operand->reg = 3; operand->a = 2; }
      else if (operand->value == 4) { operand->reg = 2; operand->a = 2; }
      else if (operand->value == 8) { operand->reg = 2; operand->a = 3; }
        else
      if (operand->value == 0xff && size == 8)
      {
        operand->a = 3;
        operand->reg = 3;
      }
        else
      if (operand->value == 0xffff && size == 16)
      {
        operand->a = 3;
        operand->reg = 3;
      }
        else
      if (operand->value == 0xfffff && size == 20)
      {
        operand->a = 3;
        operand->reg = 3;
      }
        else
      {
        operand->a = 3;
        operand->reg = 0;
        data->data[data->count++] = operand->value;
      }
      break;
    case OPTYPE_ABSOLUTE:
      operand->a = 1;
      operand->reg = 2;
      data->data[data->count++] = operand->value;
      break;
    default:
      print_error_internal(asm_context, __FILE__, __LINE__);
      break;
  }

  return 0;
}

static int add_instruction(struct _asm_context *asm_context, struct _data *data, int error, int opcode)
{
  int n;

  //asm_context->instruction_count++;

  if (asm_context->pass == 1)
  {
    if (error == 1) { add_bin16(asm_context, 1, IS_DATA); }
    else { add_bin16(asm_context, 0, IS_DATA); }
  }
    else
  {
    add_bin16(asm_context, opcode, IS_OPCODE);
  }

  for (n = 0; n < data->count; n++)
  {
    add_bin16(asm_context, data->data[n], IS_DATA);
  }

  return 0;
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
    print_error("expecting register or immediate", asm_context);
    return -1;
  }

  return prefix;
}

int parse_instruction_msp430(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[3];
  struct _data data;
  int operand_count = 0;
  char token[TOKENLEN];
  char instr_lower_mem[TOKENLEN];
  char *instr_lower;
  int token_type;
  int size = 0;
  int num,n;
  int bw = 0;
  int opcode;
  int msp430x = 0;
  int prefix = 0;

  lower_copy(instr_lower_mem, instr);
  instr_lower = instr_lower_mem;

  // Not sure if this is a good area for this.  If there isn't an instruction
  // here then it pads for no reason.
  if ((asm_context->address & 0x01) != 0)
  {
    if (asm_context->pass == 2)
    {
      printf("Warning: Instruction doesn't start on 16 bit boundary at %s:%d.  Padding with a 0.\n", asm_context->filename, asm_context->line);
    }
    memory_write_inc(asm_context, 0, DL_NO_CG);
  }

  // check for RPT prefix
  if (asm_context->cpu_type == CPU_TYPE_MSP430X)
  {
    n=0;
    while(msp430x_rpt[n] != NULL)
    {
      if (strcmp(instr_lower, msp430x_rpt[n]) == 0)
      {
        prefix = get_prefix(asm_context, n & 1);
        if (prefix == 0xffff) return -1;
        while(1)
        {
          token_type = tokens_get(asm_context, instr, TOKENLEN);
          if (token_type != TOKEN_EOL) { break; }
          if (token_type == TOKEN_EOF)
          {
            print_error("Unexpected end of file", asm_context);
            return -1;
          }
          asm_context->line++;
        }

        lower_copy(instr_lower_mem, instr);

        break;
      }

      n++;
    }
  }

  memset(&operands, 0, sizeof(operands));
  data.count = 0;

  while(1)
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

#if 0
    if (IS_TOKEN(token,'('))
    {
      char token_reg[TOKENLEN];
      int token_type_reg;

      token_type_reg = tokens_get(asm_context, token_reg, TOKENLEN);
      num = get_register_msp430(token_reg);

      if (num >= 0)
      {
        operands[operand_count].type = OPTYPE_INDEXED;
        operands[operand_count].value = 0;
        operands[operand_count].reg = num;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (expect_token_s(asm_context, ")") == -1) { return -1; }

        break;
      }

      tokens_push(asm_context, token_reg, token_type_reg);
      tokens_push(asm_context, token, token_type);
    }
#endif

    if (IS_TOKEN(token,'#'))
    {
      operands[operand_count].type = OPTYPE_IMMEDIATE;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          eat_operand(asm_context);
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
          eat_operand(asm_context);
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

        if (asm_context->pass == 1)
        {
          // In pass 1 it will always be 2 words long, so who cares
          eat_operand(asm_context);
        }
          else
        {
          // FIXME - Ugly fix. The real problem is in eval_expression.
          int neg = 1;
          if (IS_TOKEN(token,'-')) { neg = -1; }
          else { tokens_push(asm_context, token, token_type); }

          if (eval_expression(asm_context, &num) != 0)
          {
            //print_error_unexp(token, asm_context);
            return -1;
          }

          num = num * neg;
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

    operands[operand_count++].value = num;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
  printf("-------- line=%d instr=%s bw=%d\n", asm_context->line, instr, bw);
  for (n = 0; n < operand_count; n++)
  {
    printf("operand %d: value=%d type=%d error=%d\n", n, operands[n].value, operands[n].type, operands[n].error);
  }
#endif

  // Do aliases first
  n = 0;
  while(aliases[n].instr != NULL)
  {
    if (strcmp(instr_lower, aliases[n].instr) == 0)
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

      switch(aliases[n].cmd)
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
          memset(&operands[0], 0, sizeof(struct _operand)*2);
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
      //instr_lower = (char *)aliases[n].alt;
      strcpy(instr_lower, (char *)aliases[n].alt);
      break;
    }

    n++;
  }

  if (size == 8) { bw = 1; }

  // Check for MSP430X version of MSP430 instruction
  if (asm_context->cpu_type == CPU_TYPE_MSP430X)
  {
    n = 0;

    while(msp430x_ext[n] != NULL)
    {
      if (strcmp(instr_lower, msp430x_ext[n]) == 0)
      {
        uint32_t src19_16 = 0;
        uint32_t dst19_16 = 0;
        int al;

        // Strip the 'x' off of the end of the instruction so it can
        // be processed below as a regular msp430 instruction.
        instr_lower[strlen(instr_lower) - 1] = 0;

        if (size == 8) { al = 1; bw = 1; }
        else if (size == 16) { al = 1; bw = 0; }
        else if (size == 20) { al = 0; bw = 1; }
        else { al = 1; bw = 0; }

        if (prefix != 0)
        {
          add_bin16(asm_context, prefix | (al << 6), IS_OPCODE);
          msp430x = 2;
          break;
        }

        msp430x = 1;

        if (operand_count > 0)
        {
          src19_16 = (((uint32_t)operands[0].value) & 0xf0000) >> 16;
        }

        if (operand_count > 1)
        {
          dst19_16 = (((uint32_t)operands[1].value) & 0xf0000) >> 16;
        }

        add_bin16(asm_context, 0x1800 | (src19_16 << 7) | (al << 6) | (dst19_16), IS_OPCODE);
        break;
      }

      n++;
    }
  }

  if (msp430x == 0 && prefix != 0)
  {
    print_error("Instruction doesn't support RPT", asm_context);
    return -1;
  }

  // One operand instructions
  n = 0;
  while(one_oper[n] != NULL)
  {
    if (strcmp(instr_lower,one_oper[n]) == 0)
    {
      if (msp430x == 0 && size > 16)
      {
        print_error("Instruction doesn't support .a", asm_context);
        return -1;
      }

      if ((n & 1) == 1 && (size == 8 || (msp430x == 0 && size == 16)))
      {
        printf("Error: Instruction '%s' can't be used with .b/w at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      if (operand_count != 1)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      if (process_operand(asm_context, &operands[0], &data, size, 0) < 0)
      {
        return -1;
      }

      if (msp430x == 2 && operands[0].a != 0)
      {
        print_error("RPT only supports registers.", asm_context);
        return -1;
      }

      opcode = 0x1000 | (n << 7) | (bw << 6) | (operands[0].a << 4) | operands[0].reg;
      if (asm_context->msp430_cpu4 == 1)
      {
        // A bug in the MSP430 for push #4 and push #8.  Cannot use CG.
        if ((opcode & 0xffbf) == 0x1222)
        {
          opcode = 0x1230 | (opcode & 0x0040);
          data.count = 1;
          data.data[0] = 4;
        }
          else
        if ((opcode & 0xffbf) == 0x1232)
        {
          opcode = 0x1230 | (opcode & 0x0040);
          data.count = 1;
          data.data[0] = 8;
        }
      }
      add_instruction(asm_context, &data, operands[0].error, opcode);

      return 0;
    }

    n++;
  }

  // Two operand instructions
  n = 0;
  while(two_oper[n] != NULL)
  {
    if (strcmp(instr_lower,two_oper[n]) == 0)
    {
      if (msp430x == 0 && size > 16)
      {
        print_error("Instruction doesn't support .a", asm_context);
        return -1;
      }

      if (operand_count != 2)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      if (process_operand(asm_context, &operands[0], &data, size, 0) < 0)
      {
        return -1;
      }

      if (process_operand(asm_context, &operands[1], &data, size, 1) < 0)
      {
        return -1;
      }

      if (msp430x == 2 && (operands[1].a != 0 ||
          !(operands[0].a == 0 || operands[0].reg == 3 ||
          (operands[0].reg == 2 && operands[0].a != 1))))
      {
        print_error("RPT only supports registers.", asm_context);
        return -1;
      }

      opcode=((n + 4) << 12) | (operands[0].reg << 8) | (operands[1].a << 7) |
              (bw << 6)| (operands[0].a << 4) | (operands[1].reg);
      add_instruction(asm_context, &data, operands[0].error, opcode);

      return 0;
    }

    n++;
  }

  if (prefix != 0)
  {
    print_error("Instruction doesn't support RPT", asm_context);
    return -1;
  }

  // Jumps
  n = 0;
  while(jumps[n] != NULL)
  {
    int offset;

    if (strcmp(instr_lower,jumps[n]) == 0 || (jumps_a[n] != NULL && strcmp(instr_lower,jumps_a[n]) == 0))
    {
      if (operand_count != 1)
      {
        //print_operand_error(instr, 1, asm_context);
        print_error_opcount(instr, asm_context);
        return -1;
      }

      if (size != 0)
      {
        print_error("Instruction doesn't take a size.", asm_context);
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

      offset = (offset - (asm_context->address + 2)) / 2;

      if (offset > 511)
      {
        printf("Error: Jump offset off by %d at %s:%d.\n", offset - 511, asm_context->filename, asm_context->line);
        return -1;
      }

      if (offset < -512)
      {
        printf("Error: Jump offset off by %d at %s:%d.\n", (-offset) - 512, asm_context->filename, asm_context->line);
        return -1;
      }

      opcode = 0x2000 | (n << 10);
      opcode |= ((uint32_t)offset) & 0x03ff;
      add_instruction(asm_context, &data, 0, opcode);

      return 0;
    }

    n++;
  }

  // Do reti
  if (strcmp(instr_lower, "reti") == 0)
  {
    if (size != 0)
    {
      print_error("reti doesn't take a size", asm_context);
      return -1;
    }

    if (operand_count != 0)
    {
      //print_operand_error(instr, 0, asm_context);
      print_error_opcount(instr, asm_context);
      return -1;
    }

    add_bin16(asm_context, 0x1300, IS_OPCODE);
    return 0;
  }

  if (asm_context->cpu_type != CPU_TYPE_MSP430X)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  // MSP430X CALLA
  if (strcmp(instr_lower, "calla") == 0)
  {
    if (operand_count != 1)
    {
      print_error("calla takes exactly one operand", asm_context);
      return -1;
    }

    if (size != 0)
    {
      print_error("calla doesn't take a size flag", asm_context);
      return -1;
    }

    int value = operands[0].value;
    if (value > 0xfffff || value < -524288)
    {
      print_error("Constant larger than 20 bit.", asm_context);
      return -1;
    }

    opcode = 0x1300;

    switch(operands[0].type)
    {
      case OPTYPE_ABSOLUTE:
        add_bin16(asm_context, opcode | 0x80 | ((value >> 16) & 0xf), IS_OPCODE);
        add_bin16(asm_context, value & 0xffff, IS_OPCODE);
        return 4;
      case OPTYPE_INDEXED:
        if (operands[0].reg == 0)
        {
          if (value > 0xfffff || value < -524288)
          {
            print_error_range("Index", -524288, 0x7ffff, asm_context);
            return -1;
          }
          add_bin16(asm_context, opcode | 0x90 | ((value >> 16) & 0xf), IS_OPCODE);
        }
          else
        {
          if (value > 0xffff || value < -32768)
          {
            print_error_range("Index", -32768, 32767, asm_context);
            return -1;
          }
          add_bin16(asm_context, opcode | 0x50 | operands[0].reg, IS_OPCODE);
        }

        add_bin16(asm_context, value & 0xffff, IS_OPCODE);
        return 4;
      case OPTYPE_SYMBOLIC:
        if (asm_context->pass == 1)
        {
          add_bin16(asm_context, opcode | 0x90 | 0, IS_OPCODE);
          add_bin16(asm_context, value & 0xffff, IS_OPCODE);
          return 4;
        }

        int offset = operands[0].value - (asm_context->address + 4);

        if (offset > 0xfffff || offset < -524288)
        {
          print_error_range("Offset", -524288, 0x7ffff, asm_context);
          return -1;
        }
        add_bin16(asm_context, opcode | 0x90 | ((offset >> 16) & 0xf), IS_OPCODE);
        add_bin16(asm_context, offset & 0xffff, IS_OPCODE);
        return 4;
      case OPTYPE_IMMEDIATE:
        add_bin16(asm_context, opcode | 0xb0 | ((value >> 16) & 0xf), IS_OPCODE);
        add_bin16(asm_context, value & 0xffff, IS_OPCODE);
        return 4;
      case OPTYPE_REGISTER:
        add_bin16(asm_context, opcode | 0x40 | value, IS_OPCODE);
        return 2;
      case OPTYPE_REGISTER_INDIRECT:
        add_bin16(asm_context, opcode | 0x60 | value, IS_OPCODE);
        return 2;
      case OPTYPE_REGISTER_INDIRECT_INC:
        add_bin16(asm_context, opcode | 0x70 | value, IS_OPCODE);
        return 2;
      default:
        break;
    }

    print_error("Unknown addressing mode for calla", asm_context);
    return -1;
  }

  // MSP430X SHIFT
  n = 0;
  while(msp430x_shift[n] != NULL)
  {
    if (strcmp(instr_lower,msp430x_shift[n]) == 0)
    {
      if (operand_count != 2 || operands[0].type != OPTYPE_IMMEDIATE ||
          operands[1].type != OPTYPE_REGISTER)
      {
        print_error("rotate expects an immediate and register", asm_context);
        return -1;
      }

      if (operands[0].value < 1 || operands[0].value > 4)
      {
        print_error("rotate can only shift between 1 and 4", asm_context);
        return -1;
      }

      if (size == 8)
      {
        print_error("msp430x rotate can only be 16 or 20 bit", asm_context);
        return -1;
      }

      int al = (size == 20) ? 0:1;
      opcode = ((operands[0].value - 1) << 10) | (n << 8) | (1 << 6) | (al << 4) | operands[1].value;
      add_bin16(asm_context, opcode, IS_OPCODE);
      return 2;
    }

    n++;
  }

  // MSP430X ALU
  n = 0;
  while(msp430x_alu[n] != NULL)
  {
    if (strcmp(instr_lower, msp430x_alu[n]) == 0)
    {
      int count = 2;

      if (operand_count != 2)
      {
        print_error("Instruction takes exactly two operands", asm_context);
        return -1;
      }

      // FIXME - Hack. This should probably all be done using the table
      // instead.  Fix later.
      if (n == 0)
      {
        if (operands[1].type != OPTYPE_REGISTER) { n++; continue; }
        if (operands[0].type != OPTYPE_IMMEDIATE &&
            operands[0].type != OPTYPE_REGISTER) { n++; continue; }
      }

      if (size!=0)
      {
        print_error("Instruction doesn't take a size", asm_context);
        return -1;
      }

      int value = operands[0].value;
      if (value > 0xfffff || value < -524288)
      {
        print_error("Constant larger than 20 bit.", asm_context);
        return -1;
      }

      if (operands[0].type == OPTYPE_IMMEDIATE &&
          operands[1].type == OPTYPE_REGISTER)
      {
        opcode = ((((uint32_t)value) & 0xf0000) >> 8) | (2 << 6) | (n << 4) | operands[1].reg;
        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, ((uint32_t)value) & 0xffff, IS_DATA);
        count = 4;
      }
        else
      if (operands[0].type == OPTYPE_REGISTER &&
          operands[1].type == OPTYPE_REGISTER)
      {
        opcode = (operands[0].reg << 8) | (3 << 6) | (n << 4) | operands[1].reg;
        add_bin16(asm_context, opcode, IS_OPCODE);
      }

      if (count == 0)
      {
        print_error("Unknown addressing mode", asm_context);
        return -1;
      }

      return count;
    }

    n++;
  }

  // MSP430X PUSH AND POP
  n = 0;
  while(msp430x_stack[n] != NULL)
  {
    if (strcmp(instr_lower, msp430x_stack[n]) == 0)
    {
      if (operand_count != 2 || operands[0].type != OPTYPE_IMMEDIATE ||
          operands[1].type != OPTYPE_REGISTER)
      {
        print_error("Excpecting an immediate and register", asm_context);
        return -1;
      }

      if (operands[0].value < 1 || operands[0].value > 16)
      {
        print_error("Constant can only be between 1 and 16", asm_context);
        return -1;
      }

      if (size == 8)
      {
        printf("Error: Instruction '%s' can't be used with .b at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      int reg = operands[1].reg;
      int num = operands[0].value - 1;
      int al = (size == 20) ? 0:1;

      opcode = 0x1400 | (n << 9) | (al << 8) | (num << 4) | reg;
      add_bin16(asm_context, opcode, IS_OPCODE);
      return 2;
    }

    n++;
  }

  // MSP430X MOVA
  if (strcmp(instr_lower, "mova") == 0)
  {
    if (operand_count != 2)
    {
      print_error("Instruction takes exactly two operands", asm_context);
      return -1;
    }

    if (size != 0)
    {
      print_error("mova doesn't take a size", asm_context);
      return -1;
    }

    if (operands[1].type == OPTYPE_REGISTER)
    {
      opcode = operands[1].reg;

      if (operands[0].type == OPTYPE_REGISTER_INDIRECT)
      {
        opcode |= (operands[0].reg << 8);
        add_bin16(asm_context, opcode, IS_OPCODE);
        return 2;
      }

      if (operands[0].type == OPTYPE_REGISTER_INDIRECT_INC)
      {
        opcode |= (1 << 4) | (operands[0].reg << 8);
        add_bin16(asm_context, opcode, IS_OPCODE);
        return 2;
      }

      if (operands[0].type == OPTYPE_ABSOLUTE)
      {
        if (operands[0].value > 0xfffff || operands[0].value < -524288)
        {
          print_error("Constant larger than 20 bit.", asm_context);
          return -1;
        }

        opcode |= (2 << 4) | (((uint32_t)operands[0].value & 0xf0000) >> 8);
        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, operands[0].value & 0xffff, IS_DATA);
        return 4;
      }

      if (operands[0].type == OPTYPE_SYMBOLIC)
      {
        // Assembler thinks this is symbolic, but it's really indexed.
        if (asm_context->pass == 1)
        {
          opcode |= (3 << 4) | (operands[0].reg << 8);
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
          return 4;
        }
      }

      if (operands[0].type == OPTYPE_INDEXED)
      {
        if (operands[0].value > 0xffff || operands[0].value < -32768)
        {
          print_error("Constant larger than 16 bit.", asm_context);
          return -1;
        }

        opcode |= (3 << 4) | (operands[0].reg << 8);
        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
        return 4;
      }

      print_error("Unknown addressing mode for mova", asm_context);

      return -1;
    }
      else
    if (operands[0].type == OPTYPE_REGISTER)
    {
      opcode = 0x0040 | (operands[0].reg << 8);

      if (operands[1].type == OPTYPE_ABSOLUTE)
      {
        if (operands[1].value > 0xfffff || operands[1].value < -524288)
        {
          print_error("Constant larger than 20 bit.", asm_context);
          return -1;
        }

        opcode |= 0x0020 | ((operands[1].value & 0xf0000) >> 16);
        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
        return 4;
      }

      if (operands[1].type == OPTYPE_SYMBOLIC)
      {
        // This is probably really INDEXED.
        if (asm_context->pass == 1)
        {
          opcode |= 0x0030 | operands[1].reg;
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
          return 4;
        }
      }

      if (operands[1].type == OPTYPE_INDEXED)
      {
        if (operands[1].value > 0xffff || operands[1].value < -32768)
        {
          print_error("Constant larger than 16 bit.", asm_context);
          return -1;
        }

        opcode |= 0x0030 | operands[1].reg;
        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
        return 4;
      }

      print_error("Unknown addressing mode for mova", asm_context);
      return -1;
    }

    print_error("Unknown addressing mode for mova", asm_context);
    return -1;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}


