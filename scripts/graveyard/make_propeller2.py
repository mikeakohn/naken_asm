#!/usr/bin/env python3

import sys

def get_mask(s):
  n = 0
  count = 0

  for c in s:
    if c == ' ': continue

    n = n << 1;

    if c in [ '0', '1' ]: n |= 1
    count += 1

  if count != 32:
    print("Error " + str(count))
    sys.exit(1)

  return "0x%08x" % (n)

def get_opcode(s):
  n = 0
  count = 0

  for c in s:
    if c == ' ': continue

    n = n << 1;

    if c == '1': n |= 1
    count += 1

  if count != 32:
    print("Error " + str(count))
    sys.exit(1)

  return "0x%08x" % (n)

def parse_cycles(token):
  if "FIFO IN USE" in token: return (-1, -1)
  if "ILLEGAL" in token: return (-1, -1)
  if "2 + D" in token: return (-1, -1)
  if token == "2+": return (-1, -1)

  result = 0

  if "if result" in token:
    #token = token.split(",")[0].split()
    a = token.split(",")
    token = a[0].strip()
    result = a[1].strip()
    result = int(result.split()[0])

  if "or" in token or "..." in token:
    token = token.replace("...", " ")
    token = token.replace("or", " ")
    a = token.split()
    cycles_min = a[0]
    cycles_max = int(a[-1]) + result
  else:
    cycles_min = token
    cycles_max = token

  if "WRFAST" in token: cycles_max = -1

  return (cycles_min, cycles_max)

# ------------------------ cut here ----------------------------

print("""
/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/propeller2.h"

struct _table_propeller2 table_propeller2[] =
{""")

fp = open("propeller2.tsv", "r")

for line in fp:
  if line.startswith("order"): continue

  tokens = line.strip().split("\t")

  tokens[0] = int(tokens[0])

  if tokens[1].startswith("<empty>"): continue
  if tokens[2].startswith("Instruction Prefix"): continue

  instr = "\"" + tokens[1].split()[0].lower() + "\","
  op_type = " ".join(tokens[1].split()[1:])
  opcode = get_opcode(tokens[3])
  mask = get_mask(tokens[3])

  tokens[7] = tokens[7].replace("*", "").strip()
  tokens[8] = tokens[8].replace("*", "").strip()
  tokens[9] = tokens[9].replace("*", "").strip()

  (cycles8_min, cycles8_max) = parse_cycles(tokens[7])
  (cycles8hub_min, cycles8hub_max) = parse_cycles(tokens[8])
  (cycles16_min, cycles16_max) = parse_cycles(tokens[9])

  if cycles8hub_min == "same":
    cycles8hub_min = cycles8_min
    cycles8hub_max = cycles8_max

  #print(tokens[3] + " " + mask))

  count = 0

  wz = 0
  wc = 0
  wcz = 0
  logic = 0
  required = 0

  ops = [ "OP_NONE", "OP_NONE", "OP_NONE" ]

  if op_type != "":
    a = op_type.split(",")

    if " " in a[-1]:
      t = a[-1].split()
      a[-1] = t[0]
      a.append(t[1])

    for op in a:
      if op == "D": ops[count] = "OP_D"
      elif op == "{#}D": ops[count] = "OP_NUM_D"
      elif op == "{#}S": ops[count] = "OP_NUM_S"
      elif op == "{#}S/P": ops[count] = "OP_NUM_SP"
      elif op == "{WC/WZ/WCZ}": wc = 1; wz = 1; wcz = 1; continue
      elif op == "WC/WZ/WCZ": wc = 1; wz = 1; wcz = 1; required = 1; continue
      elif op == "WC/WZ": wc = 1; wz = 1; required = 1; continue
      elif op == "{WCZ}": wcz = 1; continue
      elif op == "{WZ}": wz = 1; continue
      elif op == "{WC}": wc = 1; continue
      elif op == "ANDC/ANDZ": wc = 1; wz = 1; logic = 1; required = 1; continue
      elif op == "ORC/ORZ": wc = 1; wz = 1; logic = 2; required = 1; continue
      elif op == "XORC/XORZ": wc = 1; wz = 1; logic = 3; required = 1; continue
      elif op == "c": ops[count] = "OP_C"
      elif op == "z": ops[count] = "OP_Z"
      elif op == "#{\}A": ops[count] = "OP_A"
      elif op == "PA/PB/PTRA/PTRB": ops[count] = "OP_P"
      elif op == "#N" or op == "#n":
        n = 0
        for b in tokens[3]:
          if b == "N" or b == "n": n += 1

        if n == 1: ops[count] = "OP_N_1"
        elif n == 2: ops[count] = "OP_N_2"
        elif n == 3: ops[count] = "OP_N_3"
        elif n == 23: ops[count] = "OP_N_23"
        else:
          print("Unknown op " + op + " " + instr + " " + str(n))
          sys.exit(1)
      else:
        print("Unknown op '" + op + "' " + instr)
        sys.exit(1)

      count += 1

  operands = "{%-10s %-10s %-7s }, " % (ops[0] + ",", ops[1] + ",", ops[2])

  flags = \
    ""   + str(wz) + \
    ", " + str(wc) + \
    ", " + str(wcz) + \
    ", " + str(logic)
    #", " + str(required)

  cycles = \
    ", " + str(cycles8_min) + \
    ", " + str(cycles8_max) + \
    ", " + str(cycles8hub_min) + \
    ", " + str(cycles8hub_max) + \
    ", " + str(cycles16_min) + \
    ", " + str(cycles16_max)

  print("  { %-10s " % (instr) + opcode + ", " + mask + ", " + str(count) + ", " + operands + flags + cycles + " },")

fp.close()

print("  { NULL }")
print("};\n")

