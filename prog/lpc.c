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

#include "prog/lpc.h"
#include "prog/lpc_parts.h"
#include "prog/serial.h"
#include "common/memory.h"

static int lpc_send_command(struct _serial *serial, char *command);

char *lpc_errors[] =
{
  "CMD_SUCCESS",            // 0
  "INVALID_COMMAND",        // 1
  "SRC_ADDR_ERROR",         // 2
  "DST_ADDR_ERROR",         // 3
  "SRC_ADDR_NOT_MAPPED",    // 4
  "DST_ADDR_NOT_MAPPED",    // 5
  "COUNT_ERROR",            // 6
  "INVALID_SECTOR",         // 7
  "SECTOR_NOT_BLANK",       // 8
  "SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION", // 9
  "COMPARE_ERROR",          // 10
  "BUSY",                   // 11
  "PARAM_ERROR",            // 12
  "ADDR_ERROR",             // 13
  "ADDR_NOT_MAPPED",        // 14
  "CMD_LOCKED",             // 15
  "INVALID_CODE",           // 16
  "INVALID_BAUD_RATE",      // 17
  "INVALID_STOP_BIT",       // 18
  "CODE_READ_PROTECTION_ENABLED",           // 19
};

static char *lpc_show_error(int err_code)
{
  if (err_code<0 || err_code>19) { return "???"; }
  return lpc_errors[err_code];
}

//FIXME - change this to a simple subtraction
//static char uuencode[64] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";

struct _uu
{
  char coded_data[64];
  int coded_ptr;
  uint32_t holding;
  uint32_t mask;
  uint32_t checksum;
  int len;
  int bytes;
  int lines;
};

static void uu_init(struct _uu *uu)
{
  uu->coded_ptr=1;
  uu->holding=0;
  uu->mask=0;
  uu->checksum=0;
  uu->len=0;
  uu->bytes=0;
  uu->lines=0;
}

static int uu_decode_bytes(struct _uu *uu, char *line, struct _memory *memory, uint32_t *address)
{
int expected;
int real=0;
uint8_t *s=(uint8_t *)line+1;

  expected=line[0]-32;

  while(*s!=0)
  {
    if (*s=='`') { *s=' '; }
    uu->holding<<=6;
    uu->mask=(uu->mask<<6)|0x3f;
    uu->holding|=((*s)-32);
    uu->len+=6;

    while (uu->len>=8)
    {
      real++;
      int data=uu->holding>>(uu->len-8);
      uu->checksum+=data;
      memory_write_m(memory, *address, data);
      memory_debug_line_set_m(memory, *address, 1);
      (*address)++;
      // printf(" holding=%x len=%d mask=%x\n", holding, len, mask);

      uu->len-=8;
      uu->mask>>=8;
      uu->holding&=uu->mask;
      if (real==expected) { break; }
    }

    if (real==expected) { break; }
    s++;
  }

  uu->bytes+=real;
  uu->lines++;

  return 0;
}

static int uu_encode_byte(struct _uu *uu, uint8_t data)
{
  uu->holding<<=8;
  uu->holding|=data;
  uu->checksum+=data;
  uu->bytes++;
  uu->mask<<=8;
  uu->mask|=0xff;
  uu->len+=8;

  while(uu->len>=6)
  {
    uu->coded_data[uu->coded_ptr++]=(uu->holding>>(uu->len-6))+32;
    uu->mask>>=6;
    uu->holding&=uu->mask;
    uu->len-=6;

    //if (uu->bytes==45) { break; }
  }

  return uu->bytes;
}

static int uu_encode_write(struct _uu *uu, struct _serial *serial, int send_checksum)
{
  // printf("holding=%08x len=%d coded_ptr=%d\n", uu->holding, uu->len, uu->coded_ptr);
  if (uu->len>0)
  {
//printf(">> holding=%08x len=%d\n", uu->holding, uu->len);
    uu->holding<<=(6-uu->len);
    uu->coded_data[uu->coded_ptr++]=uu->holding+32;
    //uu->coded_data[uu->coded_ptr++]=(uu->holding>>(uu->len-6))+32;
    //uu->mask>>=6;
    //uu->holding&=uu->mask;
    //uu->len-=6;
    //uu->len=0;
  }
//printf("holding=%08x len=%d\n", uu->holding, uu->len);

  uu->coded_data[0]=(uu->bytes)+32;
  uu->coded_data[uu->coded_ptr+0]='\r';
  uu->coded_data[uu->coded_ptr+1]='\n';
  uu->coded_data[uu->coded_ptr+2]=0;

  printf("coded_data=%s", uu->coded_data);
  int len=strlen(uu->coded_data);
  if (serial_send(serial, (uint8_t *)uu->coded_data, len, 1)!=0)
  {
    printf("Error: Can't send data over serial.\n");
    //break;
  }

  uu->coded_ptr=1;
  uu->lines++;
  uu->bytes=0;

  if (uu->lines==20 || send_checksum==1)
  {
    printf("checksum=%d\n", uu->checksum);
    sprintf(uu->coded_data, "%d\r\n", uu->checksum);
    //if (lpc_send_command(serial, uu->coded_data)!=0) { return -1; }
    if (serial_send(serial, (uint8_t *)uu->coded_data, strlen((char *)uu->coded_data), 1)!=0) { return -1; }
    uint8_t buffer[128];
    int len=serial_readln(serial, (char *)buffer, 128);
    if (len<0) { return -1; }
    if (strcmp((const char *)buffer, "RESEND\r\n")==0)
    {
      // FIXME - This is wrong.
      //uu->address-=(45*20);
    }
printf("Received: %s", buffer);

    uu->checksum=0;
    uu->lines=0;
  }

  return 0;
}

static int lpc_send_command(struct _serial *serial, char *command)
{
char buffer[128];
int err_code;
int len;

  printf("lpc_send_command command='%s'\n", command);

  sprintf(buffer, "%s\r\n", command);

  if (serial_send(serial, (uint8_t *)buffer, strlen(buffer), 1)<0)
  {
    printf("Error: serial port problem\n");
    return -1;
  }

  len=serial_readln(serial, buffer, 128);
  if (len==128) { printf("Error: read error (128)\n"); return -1; }
  buffer[len-2]=0;
  err_code=atoi(buffer);
  printf("buffer='%s'  %s (%d)\n", buffer, lpc_show_error(err_code), err_code);

  if (strcmp(buffer, "0")!=0)
  {
    printf("Error: lpc_send_command(%s) error status='%s' %s (%d).\n", command, buffer, lpc_show_error(err_code), err_code);
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
  if (serial_send(serial, (uint8_t *)"?", 1, 1)<0) { return -1; }
  lpc_read(serial, buffer, 128);

  if (strcmp(buffer, "Synchronized")!=0)
  {
    printf("Error: Cannot synchronize.  Try resetting chip.\n");
    return -1;
  }

  // Respond with Synchronized and wait for OK
  if (serial_send(serial, (uint8_t *)"Synchronized\r\n", sizeof("Synchronized\r\n")-1, 1)<0) {  return -1; }

  serial_readln(serial, buffer, 128);
  lpc_read(serial, buffer, 128);

  if (strcmp(buffer, "OK")!=0)
  {
    printf("Error: Cannot synchronize (No sync OK).  Try resetting chip.\n");
    return -1; 
  }

  // Respond with clock frequency and wait for OK
  if (serial_send(serial, (uint8_t *)"10000\r\n", sizeof("10000\r\n")-1, 1)<0) {  return -1; }

  serial_readln(serial, buffer, 128);
  lpc_read(serial, buffer, 128);

  if (strcmp(buffer, "OK")!=0)
  {
    printf("Error: Cannot synchronize (No freq OK).  Try resetting chip.\n");
    return -1; 
  }

  // Turn off echo and wait for OK
  if (serial_send(serial, (uint8_t *)"A 0\r\n", sizeof("A 0\r\n")-1, 1)<0) {  return -1; }

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

int lpc_memory_read(char *device, struct _memory *memory, uint32_t address, uint32_t count)
{
struct _serial serial;
struct _uu uu;
char buffer[128];
char command[128];
int c;

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

    //bytes_written=0;  // to know when to print a \n

    uu_init(&uu);

    //while(bytes<count)
    while(1)
    {
      c=lpc_read(&serial, buffer, 128);
      if (c<0) { break; }
      uu_decode_bytes(&uu, buffer, memory, &address);

      // Every 20 lines we are supposed to send OK to say the
      // the checksum matches.
      if ((uu.lines%20)==0 || uu.bytes==count)
      {
        lpc_read(&serial, buffer, 128);

        if (atoi(buffer)!=uu.checksum)
        {
          // FIXME - This isn't quite right.  Should only subtract the
          // number of bytes written since the last checksum.
          printf("Error: Checksum failure %s %d\n", buffer, uu.checksum);
          serial_send(&serial, (uint8_t *)"RESEND\r\n", 8, 1);
          address-=(45*20);
        }
          else
        {
          serial_send(&serial, (uint8_t *)"OK\r\n", 4, 1);
          uu.checksum=0;
        }
      }

      if (uu.bytes==count) { break; }
    }
  } while(0);

  serial_close(&serial);

  return 0;
}

int lpc_memory_write(char *device, struct _memory *memory)
{
struct _serial serial;
struct _uu uu;
uint32_t i;
uint8_t data;
uint32_t count;
int sectors;

  count=memory->high_address-memory->low_address+1;
  sectors=count/256;
  if ((count%256)!=0) { sectors++; }

  if ((count&0x3)!=0)
  {
    printf("Error: Data chunk isn't 4 byte aligned.\n");
    return -1;
  }

  if (serial_open(&serial, device)!=0)
  {
    printf("Error: Cannot open serial port %s\n", device);
    return -1;
  }

  do
  {
    if (lpc_synchronize(&serial)!=0) { break; }

    // Send unlock code
    if (lpc_send_command(&serial, "U 23130")!=0) { break; }

    // Send data to write to RAM so it can be transferred to FLASH
    //sprintf(coded_data, "W %d %d", memory->low_address, memory->high_address-memory->low_address + 1);
    sprintf(uu.coded_data, "W %d %d", 0x10000000, count);
    if (lpc_send_command(&serial, uu.coded_data)!=0) { break; }

    uu_init(&uu);

    for (i=memory->low_address; i<=memory->high_address; i++)
    {
      data=memory_read_m(memory, i);
      if (uu_encode_byte(&uu, data)==45)
      {
        uu_encode_write(&uu, &serial, 0);
      }
    }

    uu_encode_write(&uu, &serial, 1);

    // Prepare sectors for write.
    sprintf(uu.coded_data, "P %d %d", 0, sectors-1);
    if (lpc_send_command(&serial, uu.coded_data)!=0) { break; }

    // Erase sectors?  FIXME - what to do?
#if 0
    sprintf(coded_data, "E %d %d", 0, 0);
    if (lpc_send_command(&serial, coded_data)!=0) { break; }
#endif

    // Copy RAM to FLASH:  copy(flash_addr, ram_addr, size);
    sprintf(uu.coded_data, "C %d %d %d", 0, 0x10000000, sectors*256);
    if (lpc_send_command(&serial, uu.coded_data)!=0) { break; }

  } while(0);

  serial_close(&serial);

  return 0;
}

int lpc_run(char *device, uint32_t address)
{
struct _serial serial;
char buffer[128];

  if (serial_open(&serial, device)!=0)
  {
    printf("Error: Cannot open serial port %s\n", device);
    return -1;
  }

  do
  {
    if (lpc_synchronize(&serial)!=0) { break; }

    // Send unlock code
    if (lpc_send_command(&serial, "U 23130")!=0) { break; }

    // Send GO command
    sprintf(buffer, "G %d T", address);
    if (lpc_send_command(&serial, buffer)!=0) { break; }

  } while(0);

  serial_close(&serial);

  return 0;
}


