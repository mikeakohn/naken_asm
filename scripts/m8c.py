#!/usr/bin/env python3

import sys

def get_argument(argument):
  argument = argument.strip()

  if argument == "A": return "OP_A"
  if argument == "X": return "OP_X"
  if argument == "F": return "OP_F"
  if argument == "SP": return "OP_SP"
  if argument == "expr": return "OP_EXPR"
  if argument == "[expr]": return "OP_INDEX_EXPR"
  if argument == "[X+expr]": return "OP_INDEX_X_EXPR"
  if argument == "REG[expr]": return "OP_REG_INDEX_EXPR"
  if argument == "REG[X+expr]": return "OP_REG_INDEX_X_EXPR"
  if argument == "[[expr]++]": return "OP_INDEX_EXPR_INC"
  else:
    print("Unknown argument: '" + argument + "'")
    sys.exit(1)


# ------------------------------ fold here ------------------------------

fp = open("m8c.txt", "r")

while 1:
  instruction = fp.readline()
  instruction = instruction.strip()

  if not instruction: break

  arguments = fp.readline().strip()
  operation = fp.readline().strip()
  opcode = fp.readline().strip().lower()
  cycles = fp.readline().strip()
  count = fp.readline().strip()
  mask = "0xff"

  if arguments == "":
    arguments = [ "OP_NONE", "OP_NONE" ]
  else:
    arguments = arguments.split(",")

    if len(arguments) == 1:
      arguments[0] = get_argument(arguments[0])
      arguments.append("OP_NONE")
    elif len(arguments) == 2:
      arguments[0] = get_argument(arguments[0])
      arguments[1] = get_argument(arguments[1])
    else:
      print("Unknown number of arguments " + str(arguments))
      sys.exit(1)

  if opcode.endswith("x"):
    opcode = opcode[:-1] + "0"
    mask = "0xf0"

  instruction = "\"" + instruction.lower() + "\","
  instruction = "%-8s" % (instruction)

  print("  { " + instruction + \
        opcode + ", " + \
        mask + ", " + \
        arguments[0] + ", " + \
        arguments[1] + ", " + \
        count + ", " + \
        cycles + " },")

fp.close()

