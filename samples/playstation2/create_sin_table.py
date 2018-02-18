#!/usr/bin/env python

import math

table_cos = []
table_sin = []

for i in range(0,512):
 table_cos.append(math.cos((i / float(512)) * 6.28))
 table_sin.append(math.sin((i / float(512)) * 6.28))

print "_sin_table:"

for i in range(0,512,4):
  s = "  dc32"
  for j in range(i, i + 4):
    s += "%9s" % (" %.4f," % (table_sin[j]))
  print s

print
print "_cos_table:"

for i in range(0,512,4):
  s = "  dc32"
  for j in range(i, i + 4):
    s += "%9s" % (" %.4f," % (table_cos[j]))
  print s

print

