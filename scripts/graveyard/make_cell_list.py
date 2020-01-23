#!/usr/bin/env python

import random, sys

def get_register():
  return "r" + str(random.randint(1, 127))

# -------------------------- fold here -----------------------------------

fp = open("../table/cell.c", "rb")

for line in fp:
  if not "OP_" in line: continue

  tokens = tokens = line.replace("{","").replace("}","").replace("\"","").strip().split(",")

  instr = tokens[0].strip()
  op = tokens[3].strip()
  flags = tokens[4].strip()

  #print instr + " " + op + " " + flags

  operands = ""
  address = False

  operand_list = [ ]

  if op == "OP_NONE":
    operands = ""
  elif op == "OP_HINT_RELATIVE_RO_RA":
    operand_list = [ "main", get_register() ]
  elif op in [ "OP_HINT_ABSOLUTE_RO_I16", "OP_HINT_RELATIVE_RO_I16" ]:
    operand_list = [ "main", "main" ]
  elif op in [ "OP_BRANCH_RELATIVE_RT", "OP_BRANCH_ABSOLUTE_RT" ]:
    operand_list = [ get_register(), "main" ]
  elif op == "OP_RT_S10_RA":
    s = random.randint(-512, 511) / 16 * 16
    offset_reg = str(s) + "(" + get_register() + ")"
    operand_list = [ get_register(), offset_reg ]
  elif op == "OP_RT_S7_RA":
    s = random.randint(-64, 63) / 16 * 16
    offset_reg = str(s) + "(" + get_register() + ")"
    operand_list = [ get_register(), offset_reg ]
  else:
    operands = op.split("_")

    for operand in operands[1:]:
      if operand in [ "BRANCH", "HINT" ]: continue

      if operand in [ "RT", "RA", "RB", "RC", ]:
        operand_list.append(get_register())
      elif operand == "SA":
        operand_list.append(str(random.randint(1, 127)))
      elif operand == "CA":
        operand_list.append(str(random.randint(1, 127)))
      elif operand in [ "ADDRESS", "ABSOLUTE", "RELATIVE" ]:
        operand_list.append("main")
      elif operand.startswith("SCALE"):
        operand_list.append(str(random.randint(1, 50)))
      elif operand.startswith("S"):
        bits = int(operand[1:]) - 1
        operand_list.append(str(random.randint(-(1 << bits), (1 << bits) - 1)))
      elif operand.startswith("U"):
        bits = int(operand[1:])
        operand_list.append(str(random.randint(0, (1 << bits) - 1)))
      else:
        print "Unknown OP: " + instr + " " + op + " " + flags
        sys.exit(1)

  if address == True:
    print "main: " + instr + " " + ", ".join(operand_list)
  else:
    print instr + " " + ", ".join(operand_list)


fp.close()

