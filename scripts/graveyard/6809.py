#!/usr/bin/env python

# http://techheap.packetizer.com/processors/6809/6809Instructions.html

aliases = []
ops = {}

fp = open("6809.txt", "rb")

for line in fp:
  line = line.strip()

  if line == "": continue

  tokens = line.split("|")
  tokens[1] = "0x" + tokens[1].split()[0].lower().strip()
  instruction = tokens[2].lower().strip()
  tokens[3] = "M6809_OP_" + tokens[3].strip()
  tokens[5] = tokens[5].strip()

  ops[tokens[3]] = 1

  if "/" in instruction:
    a = instruction.split("/")
    instruction = a[0]
    aliases.append(a[1] + "=" + a[0])

  if "(" in tokens[4]: tokens[4] = tokens[4].replace("(","/").replace(")","")
  cycles = tokens[4].strip().split("/")

  if len(cycles) == 1:
    cycles_min = cycles[0]
    cycles_max = cycles[0]
  else:
    cycles_min = cycles[0]
    cycles_max = cycles[1]

  if tokens[3].endswith("_ILLEGAL"): continue

  print "  { \"" + instruction + "\", " + \
                   tokens[1] + ", " + \
                   tokens[3] + ", " + \
                   tokens[5] + ", " + \
                   cycles_min + ", " + \
                   cycles_max + " },"

fp.close()

for alias in aliases:
  print alias

for op in ops:
  print op + ","


