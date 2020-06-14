#!/usr/bin/env python3

import random
import sys

fp = open("../../table/tms340.c", "r")

for line in fp:
  if not "OP_" in line: continue

  line = line.replace("{", "").replace("}", "").replace("\"", "").strip()
  tokens = line.split(",")

  for i in range(0, len(tokens)): tokens[i] = tokens[i].strip()

  instr = tokens[0]

  #print(tokens)

  count = int(tokens[3])

  if count == 0:
    print(instr)
  else:
    for i in range(0, count):
      if tokens[4 + i] == "OP_DISP":
        instr = "main: " + instr

    operands = ""
    error = False

    for i in range(0, count):
      if i != 0: operands += ", "

      if random.randint(0, 1) == 0:
        r = "a"
      else:
        r = "b"

      a = tokens[4 + i]

      if a == "OP_F":
        operands += str(random.randint(0, 1))
      elif a == "OP_FE":
        operands += str(random.randint(0, 1))
      elif a == "OP_L":
        operands += "L"
      elif a == "OP_B":
        operands += "B"
      elif a == "OP_Z":
        operands += "Z"
      elif a == "OP_XY":
        operands += "XY"
      elif a == "OP_RS" or a == "OP_RD":
        operands += r + str(random.randint(0, 14))
      elif a == "OP_P_RS" or a == "OP_P_RD":
        operands += "*" + r + str(random.randint(0, 14))
      elif a == "OP_P_RS_XY" or a == "OP_P_RD_XY":
        operands += "*" + r + str(random.randint(0, 14)) + ".XY"
      elif a == "OP_MP_RS" or a == "OP_MP_RD":
        operands += "-*" + r + str(random.randint(0, 14))
      elif a == "OP_P_RS_P" or a == "OP_P_RD_P":
        operands += "*" + r + str(random.randint(0, 14)) + "+"
      elif a == "OP_P_RS_DISP" or a == "OP_P_RD_DISP":
        operands += "*" + r + str(random.randint(0, 14)) + "(" + str(random.randint(-32768, 32767)) + ")"
      elif a == "OP_AT_ADDR":
        operands +=  "0x%05x" % (random.randint(0x10000, 0x80000))
      elif a == "OP_IW":
        operands += str(random.randint(-32768, 32767))
      elif a == "OP_IL":
        operands += str(random.randint(100000, 200000))
      elif a == "OP_K32":
        operands += str(random.randint(1, 32))
      elif a == "OP_K":
        operands += str(random.randint(0, 31))
      elif a == "OP_1K":
        operands += str(random.randint(0, 31))
      elif a == "OP_2K":
        operands += str(random.randint(0, 31))
      elif a == "OP_FS":
        operands += str(random.randint(1, 62))
      elif a == "OP_NN":
        operands += str(random.randint(0, 31))
      elif a == "OP_N":
        operands += str(random.randint(0, 31))
      elif a == "OP_LIST":
        operands += "a0, a5, a11, a15"
      elif a == "OP_ADDRESS":
        operands += str(random.randint(100000, 200000))
      elif a == "OP_DISP":
        operands += "main"
      elif a == "OP_SKIP":
        #operands += "main"
        error = True
      elif a == "OP_JUMP":
        #operands += "main"
        error = True
      else:
        print("Unknown: " + a)
        sys.exit(1)

    if error: continue

    print(instr + " " + operands)

fp.close()

