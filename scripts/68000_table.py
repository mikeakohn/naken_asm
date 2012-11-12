#!/usr/bin/env python

import sys

op_table = \
{
  "<ea>": [ "OP_EA", "EAMEAR" ],
  "<sea>": [ "OP_SEA", "SAMSRE" ],
  "<dea>": [ "OP_DEA", "DREDAM" ],
  "label": [ "OP_LABEL", "" ],
  "#imm": [ "OP_IMM", "" ],
  "#imm8": [ "OP_IMM8", "" ],
  "#imm16": [ "OP_IMM16", "" ],
  "#imm256": [ "OP_IMM256", "" ],
  "Dn": [ "OP_D", "REd" ],
  "An": [ "OP_A", "REa" ],
  "Dx": [ "OP_DX", "REx" ],
  "Dy": [ "OP_DY", "REy" ],
  "Ax": [ "OP_AX", "REx" ],
  "Ay": [ "OP_AY", "REy" ],
  "CCR": [ "OP_CCR", "" ],
  "SR": [ "OP_SR", "" ],
  "(Ax)+": [ "OP_AX_PLUS", "" ],
  "(Ay)+": [ "OP_AY_PLUS", "" ],
  "8bitdisp": [ "OP_8BITDISP", "8bitdisp" ],
  "Dq/Dr:Dq": [ "OP_DQ_DR_DQ", "" ],
  "Dr:Dq": [ "OP_DR_DQ", "" ],
  "{reglist}": [ "OP_REG_LIST", "" ],
  "(d16_Ay)": [ "OP_D16_AY", "REy" ],
  "Dl/Dh-Dl": [ "OP_DL_DH_DL", "" ],
}

def print_instr(name, opcode, mask, size, mode, ops, cond_pos):
  s = "{ "
  count = 0

  #print ops

  for op in ops:
    s += "{ " + op[0] + ", " + str(op[1]) + ", " + str(op[2]) + "}"
    if count != 3: s += ", "
  s += "}"

  print "  { \"" + instr + "\", " + opcode + ", " + mask + ", " + str(size) + ", " + str(cond_pos) + ", " + str(mode) + ", " + s + " },"

def calc_opcode(s):
  n = ""

  for a in s:
    if a in [ "0", "1" ]: n += a
    else: n += "0"

  return "0x%04x" % (int(n, 2))

def calc_mask(s):
  n = ""

  for a in s:
    if a in [ "0", "1" ]: n += "1"
    else: n += "0"

  return "0x%04x" % (int(n, 2))

def calc_size(s):
  n = 0

  for a in s:
    if a == "B": n |= 1
    elif a == "W": n |= 2
    elif a == "L": n |= 4
  return "0x%x" % n

# --------------------------------- fold here -------------------------------

print "#include <stdio.h>"
print "#include <stdlib.h>"
print "#include \"table_680x0.h\""
print

print "struct _table_680x0 table_680x0[] ="
print "{"

fp = open("68000.txt", "rb")

for line in fp:
  tokens = line.split("\t")[0:4]
  #print tokens

  # Let's skip coprocessor stuff for now
  if tokens[1].startswith("cp"): continue
  if tokens[1] in [ "MOVE16", "BFCHG", "BFCLR", "BFEXTS", "BFEXTU", "BFFFO", "BFINS", "BFSET", "BFTST", "CALLM", "CAS", "CAS2", "CHK2", "CMP2" ]: continue

  instr = tokens[1].lower()
  opcode = calc_opcode(tokens[0])
  mask = calc_mask(tokens[0])
  size = calc_size(tokens[2])
  ops = []

  mode = tokens[0].find("m")
  if mode != -1: mode = 15 - mode

  if "COND" in tokens[0]: cond_pos = tokens[1].find("cc")
  else: cond_pos = 0

  count = 3
  if tokens[3] == "":
    operands = []
  else:
    operands = tokens[3].replace("\"", "").split(",")

  for operand in operands:
    operand = operand.strip()

    if operand in op_table:
      op = op_table[operand][0]
      if op_table[operand][1] == "":
        pos = -1
        size = 0
      else:
        pos = tokens[0].find(op_table[operand][1])
        size = len(op_table[operand][1])
        if pos < 0:
          print "Error operand ! " + tokens[1] + " '" + operand + "' " + op_table[operand][1]
          sys.exit(1)
    else:
      print "Error operand " + tokens[1] + " '" + operand + "' " + str(operands)
      sys.exit(1)

    ops.append([ op, pos, size ])
    count -= 1

  if count < 0:
    print "operand error " + str(operand)
    sys.exit(1)

  while count != 0:
    ops.append([ "OP_NONE", 0, 0 ])
    count -= 1

  print_instr(instr, opcode, mask, size, mode, ops, cond_pos)

fp.close()

print "};"
print ""



