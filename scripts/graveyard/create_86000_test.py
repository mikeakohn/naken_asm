#!/usr/bin/env python3

import sys, random

if sys.version_info < (3, 0):
  print("Can't run on Python 2")
  sys.exit(-1)

fp = open("graveyard/86000.txt", "r")

for line in fp:
  line = line.strip()
  if line == "": continue

  tokens = line.lower().split("\t")
  tokens = tokens[0].split()

  instruction = tokens[0]

  if len(tokens) > 1:
    operands = tokens[1]
  else:
    operands = ""

  if operands == "d9":
    operands = "0x%02x" % (random.randint(1, 0x1ff))
  elif operands == "@ri":
    operands = "@r%d" % (random.randint(0, 3))
  elif operands == "#i8":
    operands = "#0x%02x" % (random.randint(1, 0xff))
  elif operands == "#i8,@rj":
    operands = "#0x%02x,@r%d" % (random.randint(1, 0xff), random.randint(0, 3))

  print(instruction + " " + operands)

fp.close()

