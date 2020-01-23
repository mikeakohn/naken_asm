#!/usr/bin/env python

import sys

instructions = []
instructions16 = []

bit_stuff = [
  "0,opr8a",
  "1,opr8a",
  "2,opr8a",
  "3,opr8a",
  "4,opr8a",
  "5,opr8a",
  "6,opr8a",
  "7,opr8a"
]

fp = open("cpu08.txt", "rb")

for line in fp:
  columns = line.strip().split(" ")

  #print columns

  f = columns[1][0]

  if "INH" in line and columns[1] != "rel":
    instruction = columns[2] + "|" + columns[0] + "|NONE|"  + columns[-1]
  elif columns[1] in [ "opr", "opr8a" ]:
    instruction = columns[3] + "|" + columns[0] + "|OPR8|"  + columns[-1]
  elif columns[1] == "#opr8i":
    instruction = columns[3] + "|" + columns[0] + "|NUM8|"  + columns[-1]
  elif columns[1] == "#opr":
    instruction = columns[3] + "|" + columns[0] + "|NUM16|"  + columns[-1]
  elif columns[1] == "opr16a":
    instruction = columns[3] + "|" + columns[0] + "|OPR16|"  + columns[-1]
  elif columns[1] == ",X":
    instruction = columns[3] + "|" + columns[0] + "|COMMA_X|"  + columns[-1]
  elif columns[1] == "oprx8,X":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_X|"  + columns[-1]
  elif columns[1] == "oprx16,X":
    instruction = columns[3] + "|" + columns[0] + "|OPR16_X|"  + columns[-1]
  elif columns[1] == "oprx8,SP":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_SP|"  + columns[-1]
  elif columns[1] == "oprx16,SP":
    instruction = columns[3] + "|" + columns[0] + "|OPR16_SP|"  + columns[-1]
  elif columns[1] == "rel":
    instruction = columns[3] + "|" + columns[0] + "|REL|"  + columns[-1]
  elif columns[1] == "opr8a,rel":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_REL|"  + columns[-1]
  elif columns[1] == "#opr8i,rel":
    instruction = columns[3] + "|" + columns[0] + "|NUM8_REL|"  + columns[-1]
  elif columns[1] == "oprx8,X+,rel":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_X_PLUS_REL|"  + columns[-1]
  elif columns[1] == ",X+,rel":
    instruction = columns[3] + "|" + columns[0] + "|X_PLUS_REL|"  + columns[-1]
  elif columns[1] in [ "opr8x,X,rel", "oprx8,X,rel" ]:
    instruction = columns[3] + "|" + columns[0] + "|OPR8_X_REL|"  + columns[-1]
  elif columns[1] == "oprx8,SP,rel":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_SP_REL|"  + columns[-1]
  elif columns[1] == "opr8x,SP,rel":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_SP_REL|"  + columns[-1]
  elif columns[1] == "opr8a,opr8a":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_OPR8|"  + columns[-1]
  elif columns[1] == "opr8a,X+":
    instruction = columns[3] + "|" + columns[0] + "|OPR8_X_PLUS|"  + columns[-1]
  elif columns[1] == "#opr8i,opr8a":
    instruction = columns[3] + "|" + columns[0] + "|NUM8_OPR8|"  + columns[-1]
  elif columns[1] == ",X+,opr8a":
    instruction = columns[3] + "|" + columns[0] + "|X_PLUS_OPR8|"  + columns[-1]
  elif columns[1] == ",X,rel":
    instruction = columns[3] + "|" + columns[0] + "|X_REL|"  + columns[-1]
  elif columns[1] == "X":
    instruction = columns[3] + "|" + columns[0] + "|X|"  + columns[-1]
  elif columns[1][1:] == ",opr8a" and f >= '0' and f <= '9':
    instruction = columns[4] + "|" + columns[0] + "|" + f + "_COMMA_OPR|"  + columns[-1]
  elif columns[1][1:] == ",opr8a,rel" and f >= '0' and f <= '9':
    instruction = columns[4] + "|" + columns[0] + "|" + f + "_COMMA_OPR_REL|"  + columns[-1]
  else:
    print "ERROR: " + str(columns)
    sys.exit(1)

  #print instruction

  columns = instruction.split("|")
  if len(columns[0]) == 2:
    instructions.append(instruction)
  elif len(columns[0]) == 4:
    instructions16.append(instruction)
  else:
    print "ERROR: " + instruction
    sys.exit(1)

fp.close()

print """/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table_68hc08.h"

struct _m68hc08_table m68hc08_table[] = {"""


instructions.sort()
instructions16.sort()

count = 0
previous = -1

for instruction in instructions:

  #print instruction
  tokens = instruction.split("|")
  tokens[1] = "\"" + tokens[1].lower() + "\""

  opcode = int(" 0x" + tokens[0], 16)
  if opcode == previous:
    print "  // " + tokens[1]
    continue

  if opcode != count:
    for n in range(count, opcode):
      print "  { NULL, CPU08_OP_NONE, 0 },  // " + ("0x%02x" % n) + " NONE"
    count = opcode

  print "  { " + tokens[1] + ", CPU08_OP_" + tokens[2] + ", " + tokens[3] + " },  // " + ("0x%02x" % count) + " 0x" + tokens[0].lower()

  previous = opcode
  count += 1

print "};"
print

print "struct _m68hc08_16_table m68hc08_16_table[] = {"
for instruction in instructions16:
  #print instruction
  tokens = instruction.split("|")
  tokens[1] = "\"" + tokens[1].lower() + "\""

  print "  { " + tokens[1] + ", 0x" + tokens[0].lower() + ", CPU08_OP_" + tokens[2] + ", " + tokens[3] + " },"

print "  { NULL, 0, CPU08_OP_NONE, 0 },"
print "};"
print

