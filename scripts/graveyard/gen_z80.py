#!/usr/bin/env python

fp = open("table_z80.c", "rb")

for line in fp:
  if "{ \"" in line.strip():
    line = line[:-1]
    instr = line.split("\"")[1]
    instr = "Z80_" + instr.upper()
    tokens = line.split(",")
    print ",".join(tokens[0:4]) + ", " + instr + "," + ",".join(tokens[4:])
  else:
    print line[:-1]
    pass

fp.close()

