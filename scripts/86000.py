#!/usr/bin/env python3

import sys

if sys.version_info < (3, 0):
  print("Can't run on Python 2")
  sys.exit(-1)

unknown = []
known = []

fp = open("86000.txt", "r")

for line in fp:
  line = line.strip()
  if line == "": continue

  columns = line.split("\t")

  #print(str(columns))

  tokens =  columns[0].split()

  instruction = tokens[0].strip().lower()
  operands = ""

  if len(tokens) > 1:
    operands = "".join(tokens[1:])

  code = columns[1].split()

  if (len(columns) > 2):
    cycles = columns[2]
  else:
    cycles = "?"

  #print(cycles)

  #print(code)

  opcode = 0
  mask = 0
  skip = False

  for c in code[0]:
    if skip: skip = False; continue

    opcode = opcode << 1
    mask = mask << 1

    if c == "1": opcode |= 1
    if c in [ "0", "1" ]: mask |= 1
    if c in [ "b", "d", "i", "j" ]: skip = True

  #print(opcode)

  opcode = "0x%02x" % (opcode)
  mask = "0x%02x" % (mask)

  if operands == "":
    operands = "OP_NONE"
  elif operands == "r8":
    operands = "OP_RELATIVE8"
  elif operands == "@Ri":
    operands = "OP_AT_REG"
  elif operands == "d9":
    operands = "OP_ADDRESS"
  elif operands == "#i8":
    operands = "OP_IMMEDIATE"
  elif operands == "d9,b3":
    operands = "OP_ADDRESS_BIT"
  elif operands == "d9,r8":
    operands = "OP_ADDRESS_RELATIVE8"
  elif operands == "#i8,r8":
    operands = "OP_IMMEDIATE_RELATIVE8"
  elif operands == "@Rj,r8":
    operands = "OP_AT_REG_RELATIVE8"
  elif operands == "@Rj,#i8,r8":
    operands = "OP_AT_REG_IMMEDIATE_RELATIVE8"
  elif operands == "d9,b3,r8":
    operands = "OP_ADDRESS_BIT_RELATIVE8"
  elif operands == "r16":
    operands = "OP_RELATIVE16"
  elif operands == "a12":
    operands = "OP_ADDRESS12"
  elif operands == "a16":
    operands = "OP_ADDRESS16"
  elif operands == "@Ri,r8":
    operands = "OP_AT_REG_I_RELATIVE8"
  elif operands == "#i8,d9":
    operands = "OP_IMMEDIATE_ADDRESS"
  elif operands == "#i8,@Rj":
    operands = "OP_IMMEDIATE_AT_REG"
  else:
    if operands not in unknown: unknown.append(operands)

  print("  { \"" + instruction + "\", " + opcode + ", " + mask + ", " + operands + ", " + cycles + " },")

  if operands not in known: known.append(operands)

fp.close()

print(unknown)

for a in known:
  print(str(a) + ",")

