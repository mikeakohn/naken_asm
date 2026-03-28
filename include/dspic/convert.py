#!/usr/bin/env python

import sys

print("; Convert Microchip include files to naken_asm readable.")
print

if len(sys.argv) != 3:
  print("Usage: python " + sys.argv[0] + " <microchip .inc file> <microchip .gld file>")
  print
  print("Example: python convert.py p30F3012.inc p30F3012.gld > p30f3012.inc")
  print
  print
  sys.exit(0)

fp = open(sys.argv[2], "r")

# Read gld file first
for line in fp:
  line = line.strip()
  tokens = line.split()

  if len(tokens) < 3: continue

  if tokens[0] == "WREG0": continue

  if tokens[1] == "=":
    if line.endswith(";"): line = line.replace(";","")
    print(line.replace("=", "equ"))

fp.close()

# Read inc
fp = open(sys.argv[1], "r")

for line in fp:
  line = line.strip()
  if line.startswith(".equiv"):
    tokens = line.split(",")
    print(tokens[0].replace(".equiv","") + " equ " + tokens[1])

fp.close()

