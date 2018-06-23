#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int get_int32_be(uint8_t *buffer)
{
  uint32_t n;

  n = (buffer[0] << 24) |
      (buffer[1] << 16) |
      (buffer[2] << 8) |
       buffer[3];

  return n;
}

int get_int16_be(uint8_t *buffer)
{
  uint32_t n;

  n = (buffer[0] << 8) | buffer[1];

  return n;
}

int get_int32_le(uint8_t *buffer)
{
  uint32_t n;

  n = (buffer[3] << 24) |
      (buffer[2] << 16) |
      (buffer[1] << 8) |
       buffer[0];

  return n;
}

int get_int16_le(uint8_t *buffer)
{
  uint32_t n;

  n = (buffer[1] << 8) | buffer[0];

  return n;
}

