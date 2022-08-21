#!/usr/bin/env python3

import sys
import rcp_table

def highest_bit_set(num):
  count = 0

  while num != 0:
    num = num >> 1
    count += 1

  return count - 1

def set_bits(start, stop):
  i = 0
  if stop < 0: stop = 0

  while start >= stop:
    i |= 1 << start
    start -= 1

  return i

def reciprocal(num):
  mask = num >> 31
  result = num ^ mask

  if num > -32768: result -= mask

  if result == 0: return 0x7fff_ffff
  if num == -32768: return 0xffff_0000

  shift = 31 - highest_bit_set(result)
  index = ((result << shift) & 0x7fc0_0000) >> 22
  result = rcp_table.values[index]
  result = ((0x10000 | result) << 14) >> (31 - shift)
  result = result ^ mask

  print("shift: %d" % (shift))
  print("index: %d" % (index))

  return result

def as_float(num):
  return float(num >> 16) + (float(num & 0xffff) / 65536)

# --------------------------- fold here -----------------------------

if len(sys.argv) != 2:
  print("Usage: python3 reciprocal.py <15.16 fixed point number>")
  sys.exit(0)

if sys.argv[1].startswith("0x"):
  num = int(sys.argv[1], 16)
else:
  num = int(sys.argv[1])

rcp = reciprocal(num)
f = as_float(num)

print("   num=%d 0x%04x  %.4f" % (num, num, f))
print("   rcp=%d 0x%04x  %.4f %.4f" % (rcp, rcp, as_float(rcp << 1), 1 / f))
print("   div=0x%04x" % ((rcp >> 16) & 0xffff))
print("result=0x%04x" % (rcp & 0xffff))

