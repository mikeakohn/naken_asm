#!/usr/bin/env python3

import sys
import random

fp = open("table/mips.c", "r")

for line in fp:
  if not "OP_MIPS_" in line: continue
  line = line.strip()

  if line.startswith("//"): continue

  line = line.replace("{", "").replace("}", "").strip()

  tokens = line.split(",")

  for i in range(0, len(tokens)):
    tokens[i] = tokens[i].replace("\"", "").strip()

  #print(tokens)

  shift = int(tokens[3])
  element_max = int(tokens[4])
  element_step = int(tokens[5])

  if tokens[1] == "OP_MIPS_RSP_LOAD_STORE":
    element = random.randint(0, element_max)
    offset = random.randint(-20, 20)
    vd = random.randint(0, 31)
    rt = random.randint(0, 31)

    mask = 0xff >> element_step
    mask <<= element_step
    element &= mask

    offset <<= shift

    vd = "$v" + str(vd) + "[" + str(element) + "]"
    rt = str(offset) + "($" + str(rt) + ")"

    print(tokens[0] + " " + vd + ", " + rt)
  elif tokens[1] == "OP_MIPS_RSP_REG_MOVE":
    vd = random.randint(0, 31)
    rt = random.randint(0, 31)
    element = random.randint(0, 3) * 4

    vd = "$v" + str(vd) + "[" + str(element) + "]"
    rt = "$" + str(rt)

    print(tokens[0] + " " + rt + ", " + vd)
  elif tokens[1] == "OP_MIPS_RSP_ALU":
    vd = random.randint(0, 31)
    vs = random.randint(0, 31)
    vt = random.randint(0, 31)

    vd = "$v" + str(vd)
    vs = "$v" + str(vs)
    vt = "$v" + str(vt)

    if random.randint(0, 1) == 0:
      element = random.randint(0, 15)
      vt = vt + "[" + str(element) + "]"

    print(tokens[0] + " " + vd + ", " + vs + ", " + vt)
  elif tokens[1] == "OP_MIPS_RSP_REG_2":
    vd = random.randint(0, 31)
    vt = random.randint(0, 31)

    vd = "$v" + str(vd)
    vt = "$v" + str(vt)

    element = random.randint(0, 15)
    vt = vt + "[" + str(element) + "]"

    element = random.randint(0, 15)
    vd = vd + "[" + str(element) + "]"

    print(tokens[0] + " " + vd + ", " + vt)
  elif tokens[1] == "OP_MIPS_RSP_NONE":
    print(tokens[0])
  else:
    print(tokens[1])
    sys.exit(1)

fp.close()

