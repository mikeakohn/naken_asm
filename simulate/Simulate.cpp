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
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "Simulate.h"

bool Simulate::stop_running = false;

void Simulate::handle_signal(int sig)
{
  stop_running = true;
  signal(SIGINT, SIG_DFL);
}

void Simulate::enable_signal_handler()
{
  signal(SIGINT, handle_signal);
}

void Simulate::disable_signal_handler()
{
  signal(SIGINT, SIG_DFL);
}

