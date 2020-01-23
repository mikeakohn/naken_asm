#!/usr/bin/env python

fp = open("propeller.txt", "rb")

for line in fp:
  line = line.strip()
  if len(line) == 0: continue

  opcode = 0
  mask = 0
  cycles_min = 4
  cycles_max = 4

  op = "?"

  bits = line[:36]
  tokens = line[36:].split()

  instruction = tokens[0].lower()

  for a in bits:
    if a == " ": continue
    if a == "1":
      opcode = (opcode << 1) | 1
      mask = (mask << 1) | 1
    elif a == "0":
      opcode = opcode << 1
      mask = (mask << 1) | 1
    else:
      opcode = opcode << 1
      mask = mask << 1

  if "ddddddddd" in bits and "sssssssss" in bits: op = "DS"
  elif "ddddddddd" in bits: op = "D"
  elif "sssssssss" in bits:
    if not "i" in bits: op = "IMMEDIATE"
    else: op = "S"
  else: op = "NONE"

  if len(tokens) > 1:
    if "-" in tokens[1]:
      (cycles_min, cycles_max) = tokens[1].split("-")
      cycles_min = int(cycles_min)
      cycles_max = int(cycles_max)
    elif tokens[1].endswith("+"):
      cycles_min = int(tokens[1][0])
      cycles_max = 0 
    elif "," in tokens[1]:
      (cycles_min, cycles_max) = tokens[1].split(",")
      cycles_min = int(cycles_min)
      cycles_max = -int(cycles_max)
    elif tokens[1] == "?":
      cycles_min = 0
      cycles_max = 0

  #print line
  #print "'" + bits + "'"
  #print tokens

  print "  { \"%s\", 0x%08x, 0x%08x, PROPELLER_OP_%s, %d, %d }," % \
    (instruction, opcode, mask, op, cycles_min, cycles_max)

fp.close()

