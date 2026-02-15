/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Simulate.h"

bool Simulate::stop_running = false;

Simulate::Simulate(Memory *memory) :
  memory            (memory),
  org               (0),
  cycle_count       (0),
  nested_call_count (0),
  usec              (1000000),
  break_point       (0xffffffff),
  break_io          (0xffffffff),
  step_mode         (false),
  show              (true),
  auto_run          (true),
  do_clear          (true),
  serial_in         (nullptr),
  serial_out        (nullptr),
  serial_address    (0xffffffff)
{
  enable_signal_handler();
}

Simulate::~Simulate()
{
  disable_signal_handler();

  if (serial_in  != nullptr) { fclose(serial_in);  }
  if (serial_out != nullptr) { fclose(serial_out); }
}

int Simulate::dump_ram(int start, int end)
{
  int n, count;

  count = 0;
  for (n = start; n < end; n++)
  {
    if ((count % 16) == 0) { printf("\n0x%04x: ", n); }
    printf(" %02x", memory->read8(n));
    count++;
  }

  printf("\n\n");

  return 0;
}

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

void Simulate::init_serial(
  uint32_t address,
  const char *in_name,
  const char *out_name)
{
  serial_address = address;

  if (in_name[0] != 0)
  {
    serial_in = fopen(in_name, "rb");

    if (serial_in == nullptr)
    {
      printf("Error opening infile %s\n", in_name);
      return;
    }
  }

  if (out_name[0] != 0)
  {
    serial_out = fopen(out_name, "wb");

    if (serial_out == nullptr)
    {
      printf("Error opening outfile %s\n", out_name);
      fclose(serial_in);
      return;
    }
  }
}

void Simulate::serial_write8(uint8_t data)
{
  putc(data, serial_out);
}

void Simulate::serial_write16(uint16_t data)
{
  if (memory->is_little_endian())
  {
    putc(data & 0xff, serial_out);
    putc((data >> 8) & 0xff, serial_out);
  }
    else
  {
    putc((data >> 8) & 0xff, serial_out);
    putc(data & 0xff, serial_out);
  }
}

void Simulate::serial_write32(uint32_t data)
{
  if (memory->is_little_endian())
  {
    putc(data & 0xff, serial_out);
    putc((data >> 8) & 0xff, serial_out);
    putc((data >> 16) & 0xff, serial_out);
    putc((data >> 24) & 0xff, serial_out);
  }
    else
  {
    putc((data >> 24) & 0xff, serial_out);
    putc((data >> 16) & 0xff, serial_out);
    putc((data >> 8) & 0xff, serial_out);
    putc(data & 0xff, serial_out);
  }
}

uint8_t Simulate::serial_read8()
{
  return getc(serial_in);
}

uint16_t Simulate::serial_read16()
{
  uint16_t data;

  if (memory->is_little_endian())
  {
    data = getc(serial_in);
    data |= getc(serial_in) << 8;
  }
    else
  {
    data = getc(serial_in) << 8;
    data |= getc(serial_in);
  }

  return data;
}

uint32_t Simulate::serial_read32()
{
  uint32_t data;

  if (memory->is_little_endian())
  {
    data = getc(serial_in);
    data |= getc(serial_in) << 8;
    data |= getc(serial_in) << 16;
    data |= getc(serial_in) << 24;
  }
    else
  {
    data = getc(serial_in) << 24;
    data |= getc(serial_in) << 16;
    data |= getc(serial_in) << 8;
    data |= getc(serial_in);
  }

  return data;
}

void Simulate::ram_write8(uint32_t address, uint8_t data)
{
  memory->write8(address, data);

  if (serial_out != nullptr && address == serial_address)
  {
    serial_write8(data);
  }

  if (address == break_io) { exit(data); }
}

void Simulate::ram_write16(uint32_t address, uint16_t data)
{
  memory->write16(address, data);

  if (serial_out != nullptr && address == serial_address)
  {
    serial_write16(data);
  }
}

void Simulate::ram_write32(uint32_t address, uint32_t data)
{
  memory->write32(address, data);

  if (serial_out != nullptr && address == serial_address)
  {
    serial_write32(data);
  }
}

uint8_t Simulate::ram_read8(uint32_t address)
{
  if (serial_in != nullptr && address == serial_address)
  {
    return serial_read8();
  }

  return  memory->read8(address);
}

uint16_t Simulate::ram_read16(uint32_t address)
{
  if (serial_in != nullptr && address == serial_address)
  {
    return serial_read16();
  }

  return memory->read16(address);
}

uint32_t Simulate::ram_read32(uint32_t address)
{
  if (serial_in != nullptr && address == serial_address)
  {
    return serial_read32();
  }

  return memory->read32(address);
}

