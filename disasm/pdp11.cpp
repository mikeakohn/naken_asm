/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/pdp11.h"
#include "table/pdp11.h"

static int pdp11_addressing_mode(
  Memory *memory,
  uint32_t address,
  char *temp,
  int length,
  int reg,
  int reg_mode)
{
  uint16_t value = memory->read16(address);

  // SP modes.
  if (reg == 6)
  {
    switch (reg_mode)
    {
      case 1:
        snprintf(temp, length, "(sp)");
        return 0;
      case 2:
        snprintf(temp, length, "(sp)+");
        return 0;
      case 3:
        snprintf(temp, length, "@(sp)+");
        return 0;
      case 4:
        snprintf(temp, length, "-(sp)");
        return 0;
      case 6:
        snprintf(temp, length, "0x%x(sp)", value);
        return 2;
      case 7:
        snprintf(temp, length, "@0x%x(sp)", value);
        return 2;
      default:
        break;
    }
  }

  // PC modes.
  if (reg == 7)
  {
    switch (reg_mode)
    {
      case 2:
        snprintf(temp, length, "#0x%04x", value);
        return 2;
      case 3:
        snprintf(temp, length, "@#0x%04x", value);
        return 2;
      case 6:
        snprintf(temp, length, "0x%04x", value);
        return 2;
      case 7:
        snprintf(temp, length, "@0x%04x", value);
        return 2;
      default:
        break;
    }
  }

  switch (reg_mode)
  {
    case 0:
      snprintf(temp, length, "r%d", reg);
      return 0;
    case 1:
      snprintf(temp, length, "(r%d)", reg);
      return 0;
    case 2:
      snprintf(temp, length, "(r%d)+", reg);
      return 0;
    case 3:
      snprintf(temp, length, "@(r%d)+", reg);
      return 0;
    case 4:
      snprintf(temp, length, "-(r%d)", reg);
      return 0;
    case 5:
      snprintf(temp, length, "@-(r%d)", reg);
      return 0;
    case 6:
      snprintf(temp, length, "0x%04x(r%d)", value, reg);
      return 2;
    case 7:
      snprintf(temp, length, "@0x%04x(r%d)", value, reg);
      return 2;
  }

  snprintf(temp, length, "???");

  return 0;
}

int disasm_pdp11(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  char temp[64];
  //int n;
  //int i, z, a;

  opcode = memory->read16(address);

  //int rs = (opcode >> 6) & 0x07;
  int rd      = opcode & 0x07;
  int rd_mode = (opcode >> 3) & 0x07;

  address += 2;

  for (int n = 0; table_pdp11[n].instr != NULL; n++)
  {
    if ((opcode & table_pdp11[n].mask) == table_pdp11[n].opcode)
    {
      switch (table_pdp11[n].type)
      {
        case OP_NONE:
        {
          snprintf(instruction, length, "%s", table_pdp11[n].instr);
          return 2;
        }
        case OP_DOUBLE:
        {
        }
        case OP_D_EXTRA:
        {
        }
        case OP_SINGLE:
        {
          pdp11_addressing_mode(
            memory,
            address,
            temp,
            sizeof(temp),
            rd,
            rd_mode);

          snprintf(instruction, length, "%s %s",table_pdp11[n].instr, temp);
          return 2;
        }
        case OP_BRANCH:
        {
        }
        case OP_SUB_BR:
        {
        }
        case OP_JSR:
        {
        }
        case OP_REG:
        {
          snprintf(instruction, length, "%s r%d",
            table_pdp11[n].instr,
            rd);
          return 2;
        }
        case OP_NN:
        {
          snprintf(instruction, length, "%s r%d",
            table_pdp11[n].instr,
            opcode & 0x3f);
          return 2;
        }
        case OP_S_OPER:
        {
        }
        case OP_NZVC:
        {
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_pdp11(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  uint32_t opcode;
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_pdp11(
      memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    fprintf(asm_context->list, "0%04x: %04x %s\n", start / 2, opcode, instruction);

    start += count;
  }
}

void disasm_range_pdp11(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  uint16_t opcode;

  printf("\n");

  printf("%-5s  %-5s %s\n", "Addr", "Opcode", "Instruction");
  printf("-----  ------ ----------------------------------\n");

  while (start <= end)
  {
    disasm_pdp11(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    printf("0%04x: %04x   %s\n", start / 2, opcode, instruction);

    start = start + 2;
  }
}

