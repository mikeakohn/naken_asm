#!/usr/bin/env python3

import math

cos_table = [ ]

pi = 3.14

for i in range(0, 512):
  c = pi * 2
  d = math.cos((float(i) / 512) * c)

  if d >= 0:
    value = int(65536 * d)
  else:
    value = int(65536 * -d)
    value ^= 0xffffffff
    value += 1

  print(str(i) + " " + str(d) + (" %08x" % (value)))

  cos_table.append(value)

count = 0

for value in cos_table:
  if (count % 6) == 0:
    print("\n  .dc32", end = "")
  print(" 0x%08x," % (value), end = "")

  count += 1

print()

