#!/usr/bin/env python

import random

fp = open("table/6809.c", "rb")

for line in fp:
  if not "M6809_OP_" in line: continue
  if "M6809_OP_VARIANT" in line: continue
  if "M6809_OP_ILLEGAL" in line: continue
  if line.strip().startswith("//"): continue
  line = line.replace("{","").replace("}","").replace("\"","").strip()
  tokens = line.split(",")

  for i in range(0,len(tokens)): tokens[i] = tokens[i].strip()

  if tokens[2] == "M6809_OP_IMMEDIATE":
    print tokens[0] + " #" + str(random.randint(0,255))
  elif tokens[2] == "M6809_OP_DIRECT":
    print tokens[0] + " " + str(random.randint(0,255))
  elif tokens[2] == "M6809_OP_EXTENDED":
    print tokens[0] + " " + str(random.randint(0,65535))
  elif tokens[2] == "M6809_OP_INHERENT":
    print tokens[0]
  elif tokens[2] == "M6809_OP_RELATIVE":
    print "main: " + tokens[0] + " main"
  elif tokens[2] == "M6809_OP_TWO_REG":
    print tokens[0] + " X, Y"
    print tokens[0] + " B, A"
  elif tokens[2] == "M6809_OP_INDEXED":
    print tokens[0] + " " + str(random.randint(0,255)) + ", X"
    print tokens[0] + " " + str(random.randint(0,255)) + ", Y"
  elif tokens[2] == "M6809_OP_STACK":
    print tokens[0] + " X, Y"
    print tokens[0] + " A, B"
  else:
    print "Error:" + str(tokens)
    break

fp.close()

