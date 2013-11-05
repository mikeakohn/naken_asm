/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _SERIAL_H
#define _SERIAL_H

#include <termios.h>

struct _serial
{
  int fd;
  struct termios oldtio;
  struct termios newtio;
};

int serial_open(struct _serial *serial, char *device);
int serial_send(struct _serial *serial, uint8_t *buffer, int len);
int serial_readln(struct _serial *serial, char *buffer, int len);
void serial_close(struct _serial *serial);

#endif

