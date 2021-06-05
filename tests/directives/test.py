#!/usr/bin/env python3

import os,sys

p = os.popen("../../naken_asm endian.asm")
while 1:
  line = p.readline()
  if line == "": break
p.close()

fp = open("out.hex", "r")
line = fp.readline().strip()
fp.close()

os.remove("out.hex")

print("Endian test: ", end = '')

if line[9:21] == "123478561234":
  print("\x1b[32mPASS\x1b[0m")
else:
  print("\x1b[31mFAIL\x1b[0m")
  sys.exit(-1)

