#!/usr/bin/env python

def print_instr(name, opcode, mask, size, op):
  print "  { \"" + instr + "\", " + opcode + ", " + mask + ", " + size + " },"

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
  tokens = line.split()[0:3]
  #print tokens

  instr = tokens[1].lower()
  opcode = calc_opcode(tokens[0])
  mask = calc_mask(tokens[0])
  size = calc_size(tokens[2])
  ops = []

  print_instr(instr, opcode, mask, size, ops)

fp.close()

print "};"
print ""



