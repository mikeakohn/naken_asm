#!/usr/bin/env python3

import sys

def get_opcode(s):
  n = 0
  count = 0

  for c in s:
    if c == ' ': continue

    n = n << 1;

    if c == '1': n |= 1
    count += 1

  if count != 4:
    print("Error " + str(count))
    sys.exit(1)

  return n

# ------------------------ cut here ----------------------------

conditions = { }

fp = open("propeller2.tsv", "r")

for line in fp:
  if line.startswith("order"): continue

  tokens = line.strip().split("\t")

  tokens[0] = int(tokens[0])

  if tokens[1].startswith("<empty>"): continue
  if not tokens[2].startswith("Instruction Prefix"): continue

  instr = "\"" + tokens[1].split()[0].lower() + "\","

  opcode = tokens[3][:4]

  opcode = get_opcode(opcode)

  if not opcode in conditions:
    conditions[opcode] = instr

  opcode = "0x%x" % (opcode)
  cond = "%-16s" % (instr)

  print("  { " + cond + opcode + " },")

fp.close()

for n in range(0, 16):
  print("  " + conditions[n])

