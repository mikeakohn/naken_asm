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

#ifndef UTIL_SIM_H
#define UTIL_SIM_H

#include "common/util_context.h"

void sim_show_info(struct _util_context *util_context);
int sim_set_register(struct _util_context *util_context, char *command);
int sim_clear_flag(struct _util_context *util_context, char *command);
int sim_set_speed(struct _util_context *util_context, char *command);
int sim_stack_push(struct _util_context *util_context, char *command);
int sim_set_breakpoint(struct _util_context *util_context, char *command);
 
#endif

