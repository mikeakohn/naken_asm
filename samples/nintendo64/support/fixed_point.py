#!/usr/bin/env python3

import sys

def get_fixed(f):
  if f < 0:
    s = 1
    f = -f
  else:
    s = 0

  i = int(f)
  d = int(65536 * (f - float(i)))

  answer = (i << 16) | d

  if s == 1:
    answer = (answer ^ 0xffffffff) + 1

  return answer

# ------------------------- fold here ------------------------

if len(sys.argv) != 3:
  print("Usage: python3 <dividend> <divisor>")
  sys.exit(0)

dividend = float(sys.argv[1])
divisor = float(sys.argv[2])

reciprocal = 1 / divisor
reciprocal_fixed = get_fixed(reciprocal)

dividend_fixed = get_fixed(dividend)
divisor_fixed = get_fixed(divisor)

answer = dividend / divisor
answer_fixed = get_fixed(answer)

print("%.2f / %.2f = %.2f" % (dividend, divisor, answer))
print("%08x / %08x = %08x" % (dividend_fixed, divisor_fixed, answer_fixed))
print("%.2f %08x" % (reciprocal, reciprocal_fixed))

