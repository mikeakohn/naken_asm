#/usr/bin/env python

import sys

instructions = {}

print "#include <stdio.h>"
print "#include <stdlib.h>"
print "#include \"table_8051.h\""
print

print "struct _table_8051 table_8051[] ="
print "{"

fp = open("8052.txt", "rb")

for line in fp:
  tokens = line.strip().split("\t")
  #print tokens
  instr = tokens[0].lower().split()[0]
  ops = tokens[0][len(instr):].strip()
  if ops == "":
    ops = []
  else:
    ops = ops.split(",")

  operands = []
  operands.append("OP_NONE")
  operands.append("OP_NONE")
  operands.append("OP_NONE")
  _range = -1

  if len(ops) > 3:
    print "Error" + line
    sys.exit(1)

  count = 0
  for op in ops:
    if op.startswith("page"):
      operands[count] = "OP_PAGE"
      if _range != -1: print "ERROR"; sys.exit(1)
      _range = int(op[4:])
    elif op.startswith("R"):
      operands[count] = "OP_REG"
      if _range != -1: print "ERROR"; sys.exit(1)
      _range = int(op[1:])
    elif op.startswith("@R"):
      operands[count] = "OP_AT_REG"
      if _range != -1: print "ERROR"; sys.exit(1)
      _range = int(op[2:])
    elif op == "A": operands[count] = "OP_A" 
    elif op == "#data": operands[count] = "OP_DATA" 
    elif op == "iram addr": operands[count] = "OP_IRAM_ADDR" 
    elif op == "C": operands[count] = "OP_C" 
    elif op == "bit addr": operands[count] = "OP_BIT_ADDR" 
    elif op == "/bit addr": operands[count] = "OP_SLASH_BIT_ADDR" 
    elif op == "reladdr": operands[count] = "OP_RELADDR" 
    elif op == "AB": operands[count] = "OP_AB" 
    elif op == "DPTR": operands[count] = "OP_DPTR" 
    elif op == "@A+DPTR": operands[count] = "OP_AT_A_PLUS_DPTR" 
    elif op == "code addr": operands[count] = "OP_CODE_ADDR" 
    elif op == "#data16": operands[count] = "OP_DATA_16" 
    elif op == "@A+PC": operands[count] = "OP_AT_A_PLUS_PC" 
    elif op == "@DPTR": operands[count] = "OP_AT_DPTR" 
    else: print "op error: " + op + ":" + line; sys.exit(1)
    count += 1

  instructions[int(tokens[1], 16)] = "  { \"" + instr + "\", { "+operands[0]+", "+operands[1]+", "+operands[2]+" }, "+str(_range)+" }, // " + tokens[1]

fp.close()

instr_keys = instructions.keys()
instr_keys.sort()

for i in instr_keys:
  print instructions[i]

print "};"
print ""


