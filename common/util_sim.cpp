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

  if (simulate->is_break_point_set())
  {
    printf("<not set>\n");
  }
    else
  {
    printf("0x%04x (%d)\n",
      simulate->get_break_point(),
      simulate->get_break_point());
  }

  printf("  Instr Delay: ");

  if (simulate->in_step_mode())
  {
    printf("<step mode>\n");
  }
    else
  {
    printf("%d us\n", simulate->get_delay());
  }

  printf("      Display: %s\n", simulate->get_show() == true ? "On" : "Off");
}

int sim_set_register(UtilContext *util_context, String &arg)
{
  int offset = arg.find('=');

  if (offset == -1)
  {
    printf("Error: missing =.\n");
    return -1;
  }

  String value = arg.value() + offset + 1;
  value.trim();
  arg.replace_at(offset, 0);
  arg.rtrim();

  uint32_t num = value.as_int();

  if (util_context->simulate->set_reg(arg.value(), num) == 0)
  {
    printf("Register %s set to 0x%04x.\n", arg.value(), num);
  }
    else
  {
    printf("Syntax error.\n");
  }

  return 0;
}

int sim_clear_flag(UtilContext *util_context, String &arg)
{
  const char *flag = arg.value();

  if (util_context->simulate->set_reg(flag, 0) == 0)
  {
    printf("Flag %s cleared.\n", flag);
  }
    else
  {
    printf("Syntax error: Unknown flag %s\n", flag);
  }

  return 0;
}

int sim_set_speed(UtilContext *util_context, String &arg)
{
  int value = arg.len() == 0 ? 0 : arg.as_int();

#if 0
  if (arg.len() == 0)
  {
    util_context->simulate->set_delay(0);
    printf("Simulator now in single step mode.\n");

    return 0;
  }

  int a = arg.as_int();
#endif

  if (value == 0)
  {
    util_context->simulate->set_delay(0);

    printf("Simulator now in single step mode.\n");
  }
    else
  {
    util_context->simulate->set_delay(1000000 / value);

    printf("Instruction delay is now %dus\n",
      util_context->simulate->get_delay());
  }

  return 0;
}

int sim_stack_push(UtilContext *util_context, String &arg)
{
  uint32_t num;

  util_get_num(arg.value(), &num);
  util_context->simulate->push(num);
  printf("Pushed 0x%04x.\n", num);

  return 0;
}

int sim_set_breakpoint(UtilContext *util_context, String &arg)
{
  if (arg.len() == 0)
  {
    printf("Breakpoint removed.\n");
    util_context->simulate->remove_break_point();
    return 0;
  }

  uint32_t address;
  const char *value = arg.value();

  const char *end = util_get_address(util_context, value, &address);

  if (end == NULL)
  {
    printf("Error: Unknown address '%s'\n", value);
    return -1;
  }

  printf("Breakpoint added at 0x%04x.\n", address);
  util_context->simulate->set_break_point(address);

  return 0;
}

