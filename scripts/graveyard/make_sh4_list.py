#!/usr/bin/env/python

import sys
import random

operand_types = [ ]
simple = [
  "OP_REG",
  "OP_FREG",
  "OP_NONE",
  "OP_XDREG_DREG",
  "OP_XDREG_XDREG",
  "OP_XMTRX_FVREG",
  "OP_FREG_FREG",
  "OP_FVREG_FVREG",
  "OP_DREG",
  "OP_REG_REG",
  "OP_DREG_DREG",
  "OP_DREG_FPUL",
  "OP_DREG_XDREG",
  "OP_FPUL_DREG",
  "OP_FPUL_FREG",
  "OP_FREG_FPUL",
  "OP_FR0_FREG_FREG",
  "OP_DREG_AT_REG",
  "OP_DREG_AT_MINUS_REG",
  "OP_DREG_AT_R0_REG",
  "OP_FREG_AT_REG",
  "OP_FREG_AT_MINUS_REG",
  "OP_FREG_AT_R0_REG",
  "OP_XDREG_AT_REG",
  "OP_XDREG_AT_MINUS_REG",
  "OP_XDREG_AT_R0_REG",
  "OP_AT_REG_DREG",
  "OP_AT_REG_PLUS_DREG",
  "AT_REG_PLUS",
  "OP_AT_R0_REG_DREG",
  "OP_AT_REG_FREG",
  "OP_AT_REG_PLUS_FREG",
  "OP_AT_R0_REG_FREG",
  "OP_AT_REG_XDREG",
  "OP_AT_REG_PLUS_XDREG",
  "OP_AT_R0_REG_XDREG",
  "OP_AT_REG",
  "OP_REG_AT_REG",
  "OP_REG_AT_MINUS_REG",
  "OP_REG_AT_R0_REG",
  "OP_R0_AT_DISP_GBR",
  "OP_R0_AT_DISP_REG",
  "OP_AT_DISP_GBR_R0",
  "OP_AT_DISP_REG_R0",
  "OP_AT_DISP_PC_R0",
  "OP_R0_AT_REG",
]

specials = [
  "FPUL",
  "XMTRX",
]

def get_operand(operand):
  if operand in specials: return operand
  if operand == "REG": return "r" + str(random.randint(1, 15))
  if operand == "FREG": return "fr" + str(random.randint(1, 15))
  if operand == "DREG": return "dr" + str(random.randint(1, 7) * 2)
  if operand == "XDREG": return "xd" + str(random.randint(1, 7) * 2)
  if operand == "FVREG": return "fv" + str(random.randint(1, 3) * 4)
  if operand == "FR0": return "fr0"
  if operand == "R0": return "r0"
  if operand == "AT_REG": return "@r" + str(random.randint(1, 15))
  if operand == "AT_REG_PLUS": return "@r" + str(random.randint(1, 15)) + "+"
  if operand == "AT_MINUS_REG": return "@-r" + str(random.randint(1, 15))
  if operand == "AT_R0_REG": return "@(r0,r" + str(random.randint(1, 15)) + ")"
  if operand == "AT_DISP_GBR": return "@(" + str(random.randint(1, 16) * 4) + ", GBR)"
  if operand == "AT_DISP_PC": return "@(" + str(random.randint(1, 16) * 4) + ", PC)"
  if operand == "AT_DISP_REG": return "@(" + str(random.randint(1, 16) * 4) + ", r" + str(random.randint(1, 15)) + ")"

  print("operand error: " + operand)
  sys.exit(1)

# ----------------------------- fold here ----------------------------

fp = open("../table/sh4.c", "r")

for line in fp:
  if not ", OP_" in line: continue

  line = line.replace("{","").replace("},","").replace(" ", "").strip()
  tokens = line.replace("\"","").split(",")

  #print(tokens)

  special = ""

  if tokens[4] != "0": special = tokens[4].split("_")[-1]
  instruction = tokens[0]

  if not tokens[3] in operand_types and tokens[3] not in simple:
    operand_types.append(tokens[3])

  operand_type = tokens[3]
  operands = operand_type.replace("OP_","").split("_")

  if len(operands) > 1 and operands[1] == "AT":
    a = [ ]
    a.append(operands[0])
    a.append("_".join(operands[1:]))
    operands = a

  if len(operands) > 1 and operands[0] == "AT":
    if operands[-1] == "PLUS" or operands[-1] == "REG":
      a = [ ]
      a.append("_".join(operands))
      operands = a
    else:
      a = [ ]
      a.append("_".join(operands[0:-1]))
      a.append(operands[-1])
      #print(a)
      operands = a

  if operand_type == "OP_NONE":
      print(instruction)
  elif operand_type in simple:
    if len(operands) == 1:
      print(instruction + " " + get_operand(operands[0]))
    elif len(operands) == 2:
      print(instruction + " " + get_operand(operands[0]) + ", " + \
                                get_operand(operands[1]))
    else:
      print(instruction + " " + get_operand(operands[0]) + ", " + \
                                get_operand(operands[1]) + ", " + \
                                get_operand(operands[2]))
  elif operand_type == "OP_IMM_REG":
    print(instruction + " #" + str(random.randint(-127, 128)) + ", " + \
                         "r" + str(random.randint(1, 15)))
  elif operand_type == "OP_IMM":
    print(instruction + " #" + str(random.randint(0, 255)))
  elif operand_type == "OP_IMM_R0":
    print(instruction + " #" + str(random.randint(0, 255)) + ", r0")
  elif operand_type == "OP_IMM_AT_R0_GBR":
    print(instruction + " #" + str(random.randint(0, 255)) + ", @(r0,GBR)")
  elif operand_type == "OP_BRANCH_S9":
    print("main: " + instruction + " main")
  elif operand_type == "OP_BRANCH_S13":
    print("main: " + instruction + " main")
  elif operand_type == "OP_REG_SPECIAL":
    print(instruction + " r" + str(random.randint(1, 15)) + ", " + special)
  elif operand_type == "OP_SPECIAL_REG":
    print(instruction + " " + special + ", r" + str(random.randint(1, 15)))
  elif operand_type == "OP_REG_REG_BANK":
    print(instruction + " r" + str(random.randint(1, 15)) + ", " + \
                        " r" + str(random.randint(1, 7)) + "_bank")
  elif operand_type == "OP_REG_PLUS_SPECIAL":
    print(instruction + " r" + str(random.randint(1, 15)) + "+, " + special)
  elif operand_type == "OP_AT_REG_PLUS_SPECIAL":
    print(instruction + " @r" + str(random.randint(1, 15)) + "+, " + special)
  elif operand_type == "OP_AT_REG_PLUS_REG_BANK":
    print(instruction + " @r" + str(random.randint(1, 15)) + "+," + \
                         " r" + str(random.randint(1, 7)) + "_bank")
  elif operand_type == "OP_REG_BANK_REG":
    print(instruction + " r" + str(random.randint(1, 7)) + "_bank," + \
                        " r" + str(random.randint(1, 15)))
  elif operand_type == "OP_REG_BANK_AT_MINUS_REG":
    print(instruction + " r" + str(random.randint(1, 7)) + "_bank," + \
                      " @-r" + str(random.randint(1, 15)))
  elif operand_type == "OP_AT_REG_PLUS_AT_REG_PLUS":
    print(instruction + " @r" + str(random.randint(1, 15)) + "+," + \
                        " @r" + str(random.randint(1, 15)) + "+")
  elif operand_type == "OP_AT_REG_REG":
    print(instruction + " @r" + str(random.randint(1, 15)) + "," + \
                         " r" + str(random.randint(1, 15)))
  elif operand_type == "OP_AT_REG_PLUS_REG":
    print(instruction + " @r" + str(random.randint(1, 15)) + "+," + \
                         " r" + str(random.randint(1, 15)))
  elif operand_type == "OP_AT_R0_REG_REG":
    print(instruction + " @(r0,r" + str(random.randint(1, 15)) + "), " + \
                         " r" + str(random.randint(1, 15)))
  elif operand_type == "OP_AT_DISP_PC_REG":
    print(instruction + " @(" + str(random.randint(1, 16) * 4) + ",PC)," + \
                         " r" + str(random.randint(1, 15)))
  elif operand_type == "OP_AT_DISP_REG_REG":
    print(instruction + " @(" + str(random.randint(1, 16) * 4) + "," + \
                          "r" + str(random.randint(1, 15)) + ")," + \
                         " r" + str(random.randint(1, 15)))
  elif operand_type == "OP_SPECIAL_AT_MINUS_REG":
    print(instruction + " " + special + ", @-r" + str(random.randint(1, 15)))
  else:
    print(operand_type)
    print(operands)
    sys.exit(1)

fp.close()

#operand_types.sort()

#for operand_type in operand_types:
#  print(operand_type)

