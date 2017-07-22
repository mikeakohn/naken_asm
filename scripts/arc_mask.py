#!/usr/bin/env python

import sys

if len(sys.argv) != 2:
  print "Usage: python get_mask.py <code>"
  sys.exit(0)

a = sys.argv[1]
a = a.strip()

if len(a) != 32:
  print "Error: " + str(len(a))

opcode = 0
mask = 0

for b in a:
  opcode = opcode << 1
  mask = mask << 1

  if b == '0':
    mask = mask | 1
  elif b == '1':
    mask = mask | 1
    opcode = opcode | 1

  #print b + ": %x %x" % (opcode, mask)

print "0x%08x, 0x%08x" % (opcode, mask)

