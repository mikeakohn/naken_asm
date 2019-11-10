/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifndef WIN32
#include <termios.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <io.h>
#endif

#include "prog/serial.h"

int serial_open(struct _serial *serial, char *device)
{
#ifndef WIN32
  serial->fd = open(device, O_RDWR|O_NOCTTY);

  if (serial->fd == -1)
  {
    printf("Couldn't open serial device.\n");
  }
    else
  {
    tcgetattr(serial->fd, &serial->oldtio);

    memset(&serial->newtio, 0, sizeof(struct termios));
    serial->newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    serial->newtio.c_iflag = IGNPAR;
    serial->newtio.c_oflag = 0;
    serial->newtio.c_lflag = 0;
    serial->newtio.c_cc[VTIME] = 0;
    serial->newtio.c_cc[VMIN] = 1;

    tcflush(serial->fd, TCIFLUSH);
    tcsetattr(serial->fd, TCSANOW, &serial->newtio);
  }
#endif

  return 0;
}

int serial_send(struct _serial *serial, uint8_t *buffer, int len, int do_flow)
{
#ifndef WIN32
  struct timeval timeout;
  fd_set readset;
  int i, n;
  int wait_for_chip = 0;

  i = 0;

  while(i < len)
  {
    // If XON/XOFF flow control is needed.
    while (do_flow)
    {
      memset(&timeout, 0, sizeof(timeout));
      FD_ZERO(&readset);
      FD_SET(serial->fd, &readset);
      n = select(serial->fd+1, &readset, NULL, NULL, &timeout);

      if (n < 0)
      {
        perror("select() failed\n");
        return -1;
      }

      if (n == 1)
      {
        char ch;
        n = read(serial->fd, &ch, 1);

        if (ch == 0x13)
        {
          // DC3 XOFF
          wait_for_chip = 1;
        }
        else if (ch == 0x11)
        {
          // DC1 XON
          wait_for_chip = 0;
        }

        // DEBUG - Remove this
        printf("wait_for_chip=%d ch=%02x\n", wait_for_chip, ch);
      }

      if (wait_for_chip == 0) { break; }
    };

    n = write(serial->fd, buffer + i, len - i);

    if (n < 0) { return -1; }
    i += n;
  }
#endif

  return 0;
}

int serial_readln(struct _serial *serial, char *buffer, int len)
{
int i, n;

  // FIXME - Don't leave this like this
  i = 0;

  while(1)
  {
    n = read(serial->fd, buffer + i, 1);
    // printf("n=%d i=%d 0x%02x\n", n, i, buffer[i]);

    i++;

    if (n < 0) { return -1; }
    if (i == len) { break; }
    if (buffer[i - 1] == '\n') { break; }
  }

  buffer[i]=0;

  return i;
}

void serial_close(struct _serial *serial)
{
  if (serial->fd == 0) { return; }

#ifndef WIN32
  tcsetattr(serial->fd, TCSANOW, &serial->oldtio);
  close(serial->fd);
#endif

  serial->fd = 0;
}

