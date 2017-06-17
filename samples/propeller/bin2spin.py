#!/usr/bin/env python

import sys

if len(sys.argv) != 2:
  print "Usage: python bin2spin.py <infile.bin>"
  sys.exit(0)

print "Pub Main"
print "  cognew(@code, 0)"
print
print "DAT"

data = "  code byte"
count = 0

fp = open(sys.argv[1], "rb")

while 1:
  b = fp.read(1);
  if not b: break

  if (count % 8) != 0:
    data += ","

  data += " %d" % ord(b)

  count += 1

  if (count % 8) == 0:
    print data
    data = "       byte"

if (count % 8) != 0: print data

fp.close()


