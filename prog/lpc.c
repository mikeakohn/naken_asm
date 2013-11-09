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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lpc.h"
#include "lpc_parts.h"
#include "serial.h"

static int synchronize(struct _serial *serial)
{
char buffer[128];
int len;

  // Send a ? to start autobaud, wait for Synchronized
  if (serial_send(serial, (uint8_t *)"?", 1)<0) { return -1; }

  len=serial_readln(serial, buffer, 128);
  if (len==128) { return -1; }
  buffer[len-2]=0;
  printf("buffer='%s'\n", buffer);

  if (strcmp(buffer, "Synchronized")!=0)
  {
    printf("Error: Cannot synchronize.  Try resetting chip.\n");
    return -1;
  }

  // Respond with Synchronized and wait for OK
  if (serial_send(serial, (uint8_t *)"Synchronized\r\n", sizeof("Synchronized\r\n")-1)<0) {  return -1; }

  len=serial_readln(serial, buffer, 128);
  len=serial_readln(serial, buffer, 128);
  if (len==128) { return -1; }
  buffer[len-2]=0;
  printf("buffer='%s'\n", buffer);

  if (strcmp(buffer, "OK")!=0)
  {
    printf("Error: Cannot synchronize (No sync OK).  Try resetting chip.\n");
    return -1; 
  }

  // Respond with clock frequency and wait for OK
  if (serial_send(serial, (uint8_t *)"10000\r\n", sizeof("10000\r\n")-1)<0) {  return -1; }

  len=serial_readln(serial, buffer, 128);
  len=serial_readln(serial, buffer, 128);
  if (len==128) { return -1; }
  buffer[len-2]=0;
  printf("buffer='%s'\n", buffer);

  if (strcmp(buffer, "OK")!=0)
  {
    printf("Error: Cannot synchronize (No freq OK).  Try resetting chip.\n");
    return -1; 
  }

  // Turn off echo and wait for OK
  if (serial_send(serial, (uint8_t *)"A 0\r\n", sizeof("A 0\r\n")-1)<0) {  return -1; }

  len=serial_readln(serial, buffer, 128);
  len=serial_readln(serial, buffer, 128);
  if (len==128) { return -1; }
  buffer[len-2]=0;
  printf("buffer='%s'\n", buffer);

  if (strcmp(buffer, "0")!=0)
  {
    printf("Error: Cannot synchronize (No echo OK).  Try resetting chip.\n");
    return -1; 
  }

  return 0;
}

int lpc_info(char *device)
{
struct _serial serial;
char buffer[128];
uint32_t id;
int len;
int n;

  if (serial_open(&serial, device)!=0)
  {
    printf("Error: Cannot open serial port %s\n", device);
    return -1;
  }

  do
  {
    if (synchronize(&serial)!=0) { break; }
printf("asking for part number\n");
    if (serial_send(&serial, (uint8_t *)"J\r\n", 3)<0) { break; }

    len=serial_readln(&serial, buffer, 128);
    if (len==128) { return -1; }
    buffer[len-2]=0;
    printf("buffer='%s'\n", buffer);

    if (strcmp(buffer, "0")!=0)
    {
      printf("Error: Read part identification failed.\n");
      return -1; 
    }

    len=serial_readln(&serial, buffer, 128);
    if (len==128) { return -1; }
    buffer[len-2]=0;
    printf("buffer='%s'\n", buffer);

    id=atoi(buffer);
    printf("Part: %08x", id);

    n=0;
    while(lpc_parts[n].id!=0)
    {
      if (lpc_parts[n].id==id)
      {
        printf(" (%s)", lpc_parts[n].part);
        break;
      }
      n++;
    }

    if (lpc_parts[n].id==0) { printf(" (Unknown)\n"); }
    printf("\n");

  } while(0);


  serial_close(&serial);

  return 0;
}


