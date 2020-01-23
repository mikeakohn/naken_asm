#!/usr/bin/env python

import sys

def create_table(entry):
  flags = [ ]
  bits = 32

  name = "\"" + entry["name"].lower() + "\","

  if ".f" in entry["flags"]: flags.append("F_F")
  if ".cc" in entry["flags"]: flags.append("F_CC")
  if ".d" in entry["flags"]: flags.append("F_D")

  flags = " | ".join(flags)

  a = entry["bits"]
  a = a.strip()

  if len(a) != 32:
    bits = 16
    #print "Error: " + str(len(a))

  opcode = 0
  mask = 0

  for b in a:
    opcode = opcode << 1
    mask = mask << 1

    if b == '0':
      mask = mask | 1
    elif b == '1':
      mask = mask | 1
      opcode = opcode | 1

    #print b + ": %x %x" % (opcode, mask)

  operands = "OP_" + entry["operands"].replace(",","_").upper()

  if bits == 32:
    print "  { %-9s 0x%08x, 0x%08x, %s, %s }," % (name, opcode, mask, operands, flags)
  else:
    print "  { %-9s 0x%04x, 0x%04x, %s }," % (name, opcode, mask, operands)

# --------------------------- fold here ---------------------------

if len(sys.argv) != 2:
  print "Usage: python get_mask.py <filename>"
  sys.exit(0)

fp = open(sys.argv[1], "rb")

count = 0
state = 0
entries = [ ]

for line in fp:
  if line.startswith("--"):
    state += 1
    count = 0
    continue

  line = line.strip()

  if state == 0:
    entries.append({ "bits": line })
  elif state == 1:
    if "<" in line:
      name = line.split("<")[0]
      flags = line[len(name):]
    else:
      name = line
      flags = ""
    entries[count]["name"] = name
    entries[count]["flags"] = flags
  else:
    entries[count]["operands"] = line

  count += 1

fp.close

for entry in entries:
  #print entry
  create_table(entry)

