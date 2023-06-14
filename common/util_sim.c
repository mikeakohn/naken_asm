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
#include <string.h>
#include <stdint.h>

#include "common/util_sim.h"

void sim_show_info(UtilContext *util_context)
{
  Simulate *simulate = util_context->simulate;
  uint32_t start = util_context->memory.low_address / util_context->bytes_per_address;
  uint32_t end = util_context->memory.high_address / util_context->bytes_per_address;

  printf("Start address: 0x%04x (%d)\n", start, start);
  printf("  End address: 0x%04x (%d)\n", end, end);
  printf("  Break Point: ");

  if (simulate->break_point == -1)
  {
    printf("<not set>\n");
  }
    else
  {
    printf("0x%04x (%d)\n", simulate->break_point, simulate->break_point);
  }

  printf("  Instr Delay: ");

  if (simulate->usec == 0)
  {
    printf("<step mode>\n");
  }
    else
  {
    printf("%d us\n", simulate->usec);
  }

  printf("      Display: %s\n", simulate->show == 1 ? "On":"Off");
}

int sim_set_register(UtilContext *util_context, char *command)
{
  if (command[3] != ' ')
  {
    printf("Syntax error: set requires register=value\n");
    return -1;
  }

  char *s = command + 4;

  while (*s != 0)
  {
    if (*s == '=')
    {
      *s = 0;
      s++;
      uint32_t num;
      util_get_num(s, &num);

      if (util_context->simulate->simulate_set_reg(util_context->simulate, command + 4, num) == 0)
      {
        printf("Register %s set to 0x%04x.\n", command + 4, num);
      }
      else
      {
        printf("Syntax error.\n");
      }
      break;
    }
    s++;
  }

  if (*s == 0)
  {
    if (util_context->simulate->simulate_set_reg(util_context->simulate, command + 4, 1) == 0)
    {
      printf("Flag %s set.\n", command + 4);
    }
      else
    {
      printf("Syntax error.\n");
    }
  }

  return 0;
}

int sim_clear_flag(UtilContext *util_context, char *command)
{
  if (command[5] != ' ')
  {
    printf("Syntax error: set requires flag\n");
    return -1;
  }

  if (util_context->simulate->simulate_set_reg(util_context->simulate, command + 6, 0) == 0)
  {
    printf("Flag %s cleared.\n", command + 6);
  }
    else
  {
    printf("Syntax error: Unknown flag %s\n", command + 6);
  }

  return 0;
}

int sim_set_speed(UtilContext *util_context, char *command)
{
  if (command[5] != ' ')
  {
    util_context->simulate->usec = 0;
    printf("Simulator now in single step mode.\n");

    return 0;
  }

  int a = atoi(command + 6);

  if (a == 0)
  {
    util_context->simulate->usec = 0;
    printf("Simulator now in single step mode.\n");
  }
    else
  {
    util_context->simulate->usec = (1000000 / a);
    printf("Instruction delay is now %dus\n", util_context->simulate->usec);
  }

  return 0;
}

int sim_stack_push(UtilContext *util_context, char *command)
{
  uint32_t num;

  if (command[4] != ' ')
  {
    printf("Syntax error: push requires a value\n");
    return -1;
  }

  util_get_num(command + 5, &num);
  util_context->simulate->simulate_push(util_context->simulate, num);
  printf("Pushed 0x%04x.\n", num);

  return 0;
}

int sim_set_breakpoint(UtilContext *util_context, char *command)
{
  if (command[5] == 0)
  {
    printf("Breakpoint removed.\n");
    util_context->simulate->break_point = -1;
    return 0;
  }

  uint32_t address;

  char *end = util_get_address(util_context, command + 6, &address);

  if (end == NULL)
  {
    printf("Error: Unknown address '%s'\n", command + 6);
    return -1;
  }

  printf("Breakpoint added at 0x%04x.\n", address);
  util_context->simulate->break_point = address;

  return 0;
}

