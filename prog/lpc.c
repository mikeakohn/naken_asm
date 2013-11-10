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

static char uuencode[64] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";

static int lpc_send_command(struct _serial *serial, char *command)
{
char buffer[128];
int len;

  printf("lpc_send_command command='%s'\n", command);

  sprintf(buffer, "%s\r\n", command);

  if (serial_send(serial, (uint8_t *)buffer, strlen(buffer))<0)
  {
    printf("Error: serial port problem\n");
    return -1;
  }

  len=serial_readln(serial, buffer, 128);
  if (len==128) { printf("Error: read error (128)\n"); return -1; }
  buffer[len-2]=0;
  printf("buffer='%s'\n", buffer);

  if (strcmp(buffer, "0")!=0)
  {
    printf("Error: lpc_send_command(%s) error status=%s.\n", command, buffer);
    return -1; 
  }

  return 0;
}

static int lpc_read(struct _serial *serial, char *buffer, int len)
{
int count;

  count=serial_readln(serial, buffer, len);
  if (count==len) { printf("Error: read error(128)\n"); return -1; }
  count-=2;
  buffer[count]=0;
  printf("buffer='%s'\n", buffer);

  return count;
}

static int lpc_synchronize(struct _serial *serial)
{
char buffer[128];
//int len;

  // Send a ? to start autobaud, wait for Synchronized
  if (serial_send(serial, (uint8_t *)"?", 1)<0) { return -1; }
  lpc_read(serial, buffer, 128);

  if (strcmp(buffer, "Synchronized")!=0)
  {
    printf("Error: Cannot synchronize.  Try resetting chip.\n");
    return -1;
  }

  // Respond with Synchronized and wait for OK
  if (serial_send(serial, (uint8_t *)"Synchronized\r\n", sizeof("Synchronized\r\n")-1)<0) {  return -1; }

  serial_readln(serial, buffer, 128);
  lpc_read(serial, buffer, 128);

  if (strcmp(buffer, "OK")!=0)
  {
    printf("Error: Cannot synchronize (No sync OK).  Try resetting chip.\n");
    return -1; 
  }

  // Respond with clock frequency and wait for OK
  if (serial_send(serial, (uint8_t *)"10000\r\n", sizeof("10000\r\n")-1)<0) {  return -1; }

  serial_readln(serial, buffer, 128);
  lpc_read(serial, buffer, 128);

  if (strcmp(buffer, "OK")!=0)
  {
    printf("Error: Cannot synchronize (No freq OK).  Try resetting chip.\n");
    return -1; 
  }

  // Turn off echo and wait for OK
  if (serial_send(serial, (uint8_t *)"A 0\r\n", sizeof("A 0\r\n")-1)<0) {  return -1; }

  serial_readln(serial, buffer, 128);
  lpc_read(serial, buffer, 128);

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
int major,minor;
//int len;
int n;

  if (serial_open(&serial, device)!=0)
  {
    printf("Error: Cannot open serial port %s\n", device);
    return -1;
  }

  do
  {
    if (lpc_synchronize(&serial)!=0) { break; }

    // Read part number and translate from number to string
    if (lpc_send_command(&serial, "J")!=0) { break; }
    lpc_read(&serial, buffer, 128);

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

    // Read boot code version
    if (lpc_send_command(&serial, "K")!=0) { break; }
    lpc_read(&serial, buffer, 128);
    major=atoi(buffer);
    lpc_read(&serial, buffer, 128);
    minor=atoi(buffer);
    printf("Boot code version: %d.%d\n", major, minor);

    // Read UART ISP UID
    if (lpc_send_command(&serial, "N")!=0) { break; }
    lpc_read(&serial, buffer, 128);
    printf("UART ISP UID: 0x%08x\n", atoi(buffer));

  } while(0);

  serial_close(&serial);

  return 0;
}

int lpc_memory_read(char *device, uint32_t address, uint32_t count)
{
struct _serial serial;
char uudecode[256];
char buffer[128];
char command[128];
uint32_t holding;
uint32_t mask;
uint32_t len;
uint32_t bytes;
uint32_t checksum;
int lines,bytes_written;
int n,c;

  memset(uudecode, 0, sizeof(uudecode));
  for (n=0; n<64; n++)
  {
    uudecode[(int)uuencode[n]] = n;
  }

  if (serial_open(&serial, device)!=0)
  {
    printf("Error: Cannot open serial port %s\n", device);
    return -1;
  }

  do
  {
    if (lpc_synchronize(&serial)!=0) { break; }

    // Read memory
    sprintf(command, "R %u %u", address, count);
    if (lpc_send_command(&serial, command)!=0) { break; }

    bytes=0; lines=0; // bytes read in, lines read in
    bytes_written=0;  // to know when to print a \n
    checksum=0;
    holding=0; len=0; // for uudecode
    mask=0;

    while(bytes<count)
    {
      c=lpc_read(&serial, buffer, 128);
      bytes+=uudecode[(int)buffer[0]];

      for (n=1; n<c; n++)
      {
        holding<<=6;
        mask=(mask<<6)|0x3f;
        holding|=uudecode[(int)buffer[n]];
        len+=6;

        while (len>=8)
        {
          if ((bytes_written%16)==0) { printf("\n"); }
          if (bytes_written<count)
          {
            int data=holding>>(len-8);
            checksum+=data;
            printf(" %02x", data);
            bytes_written++;
          }
          // printf(" holding=%x len=%d mask=%x\n", holding, len, mask);
          len-=8;
          mask>>=8;
          holding&=mask;
        }
      }

      lines++;

      // Every 20 lines we are supposed to send OK to say the
      // the checksum matches.
      if ((lines%20)==0)
      {
        lpc_read(&serial, buffer, 128);

        if (atoi(buffer)!=checksum)
        {
          // FIXME - Add the resend code
          printf("Error: Checksum failure %s %d\n", buffer, checksum);
          break;
        }

        if (lpc_send_command(&serial, "OK")!=0) { break; }
        checksum=0;
      }
    }

#if 0
    while (len>0)
    {
      if ((bytes_written%16)==0) { printf("\n"); }
      int data=holding>>(len-8);
      checksum+=data;
      printf(" %02x", data);
      bytes_written++;
      // printf(" holding=%x len=%d mask=%x\n", holding, len, mask);
      len-=8;
      mask>>=8;
      holding&=mask;
    }
#endif

    lpc_read(&serial, buffer, 128);
    //printf("%s %d\n", buffer, checksum);
    if (atoi(buffer)!=checksum)
    {
      // FIXME - Add the resend code
      printf("Error: Checksum failure %s %d\n", buffer, checksum);
    }
  } while(0);

  serial_close(&serial);

  return 0;
}

int lpc_memory_write(char *device)
{
  return -1;
}



