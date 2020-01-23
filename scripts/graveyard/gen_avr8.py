#!/usr/bin/env python

fp = open("table_avr8.c", "rb")

for line in fp:
  if "{ \"" in line.strip():
    line = line[:-1]
    instr = line.split("\"")[1]
    instr = "AVR8_" + instr.upper()
    #print instr
    print line.replace(" },", ", " + instr + " },")
  else:
    print line[:-1]
    pass

fp.close()

