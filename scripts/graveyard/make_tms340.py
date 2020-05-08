#!/usr/bin/env python3

import sys

fp = open("tms340.txt", "r")

for line in fp:
  line = line.strip()
  if line == "": continue

  tokens = line.split("\t")

  if len(tokens) != 2:
    print("Error: " + str(tokens))
    sys.exit(1)

  #print(tokens)
  data = tokens[1].replace(" ", "")

  if " " in tokens[0]:
    (instr, operands) = tokens[0].lower().split()
  else:
    instr = tokens[0].lower()
    operands = ""

  if len(data) != 16:
    print("Error: " + str(tokens))
    sys.exit(1)

  mask = 0
  opcode = 0

  for d in data:
    mask = mask << 1
    opcode = opcode << 1

    if d == '0' or d == '1':
      mask |= 1
      if d == '1': opcode |= 1

  ops = []

  if operands != "":
    operands = operands.split(",")

    for o in operands:
      if o == "rs": ops.append("OP_RS")
      elif o == "rd": ops.append("OP_RD")
      elif o == "l": ops.append("OP_L")
      elif o == "b": ops.append("OP_B")
      elif o == "xy": ops.append("OP_XY")
      elif o == "*rd": ops.append("OP_P_RD")
      elif o == "*rs": ops.append("OP_P_RS")
      elif o == "*rd.xy": ops.append("OP_P_RD_XY")
      elif o == "*rs.xy": ops.append("OP_P_RS_XY")
      elif o == "z": ops.append("OP_Z")
      elif o == "*rd(disp)": ops.append("OP_P_RD_DISP")
      elif o == "*rs(disp)": ops.append("OP_P_RS_DISP")
      elif o == "@daddress": ops.append("OP_AT_ADDR")
      elif o == "@saddress": ops.append("OP_AT_ADDR")
      elif o == "f": ops.append("OP_F")
      elif o == "-*rd": ops.append("OP_MP_RD")
      elif o == "*rd+": ops.append("OP_P_RD_P")
      elif o == "-*rs": ops.append("OP_MP_RS")
      elif o == "*rs+": ops.append("OP_P_RS_P")
      elif o == "iw": ops.append("OP_IW")
      elif o == "il": ops.append("OP_IL")
      elif o == "k": ops.append("OP_K")
      elif o == "list": ops.append("OP_LIST")
      elif o == "fs": ops.append("OP_FS")
      elif o == "fe": ops.append("OP_FE")
      elif o == "address": ops.append("OP_ADDRESS")
      elif o == "[n]": ops.append("OP_NN")
      elif o == "n": ops.append("OP_N")
      else:
        print("Error: " + o)
        sys.exit(1)

  if len(ops) > 3:
    print("Error: \"" + str(ops) + "\"")
    sys.exit(1)

  instr = "\"" + instr + "\","

  count = len(ops)

  for i in range(0, len(ops)):
    if i == 2: break
    ops[i] = ops[i] + ","

  while len(ops) < 3:
    if len(ops) == 2:
      ops.append("OP_NONE")
    else:
      ops.append("OP_NONE,")

  print("  { %-9s 0x%04x, 0x%04x, %d, { %-13s %-13s %s }}," % (instr, opcode, mask, count, ops[0], ops[1], ops[2]))

fp.close()

