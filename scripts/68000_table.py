#!/usr/bin/env python

import sys

op_table = 
{
  "<ea>": [ "OP_EA", "EAMEAR" ] 
  "dest": [ "OP_EA", "EAMEAR" ] 
  "src": [ "OP_EA", "EAMEAR" ] 
  "label": [ "OP_LABEL", "8bitdisp" ] 
  "#imm": [ "OP_IMM", "" ] 
  "Dn": [ "OP_D", "REn" ] 
  "An": [ "OP_A", "REn" ] 
}

def print_instr(name, opcode, mask, size, op, cond_pos):
  print "  { \"" + instr + "\", " + opcode + ", " + mask + ", " + size + ", " + str(cond_pos) + " },"

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
  tokens = line.split()[0:4]
  #print tokens

  # Let's skip coprocessor stuff for now
  if tokens[1].startswith("cp"): continue

  instr = tokens[1].lower()
  opcode = calc_opcode(tokens[0])
  mask = calc_mask(tokens[0])
  size = calc_size(tokens[2])
  ops = []

  if "COND" in tokens[0]: cond_pos = tokens[1].find("cc")
  else: cond_pos = 0

  count = 3
  operands = tokens[3].spilt(",")
  for operand in operands:
    if operand == "<ea>":
      op = "OP_EA"

  if count < 0:
    print "operand error"
    sys.exit(1)

  while count != 0:
    ops.push([ "OP_NONE", 0, 0 ])
    count -= 1

  print_instr(instr, opcode, mask, size, ops, cond_pos)

fp.close()

print "};"
print ""



