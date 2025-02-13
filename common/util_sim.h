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

#ifndef UTIL_SIM_H
#define UTIL_SIM_H

#include "common/UtilContext.h"
#include "common/String.h"

void sim_show_info(UtilContext *util_context);
int sim_set_register(UtilContext *util_context, String &arg);
int sim_clear_flag(UtilContext *util_context, String &arg);
int sim_set_speed(UtilContext *util_context, String &arg);
int sim_stack_push(UtilContext *util_context, String &arg);
int sim_set_breakpoint(UtilContext *util_context, String &arg);

#endif

