#!/usr/bin/env python

import sys

instructions = []
instructions16 = []

fp = open("cpu08.txt", "rb")

for line in fp:
  columns = line.strip().split(" ")

  #print columns

  if "IMM" in columns:
    if columns[1] == "#opr8i,rel":
      instruction = columns[3]+"|"+columns[0]+"|IMM8_REL|"+columns[6]
    elif columns[1] == "#opr8i,X+,rel":
      instruction = columns[3]+"|"+columns[0]+"|IMM8_X_PLUS_REL|"+columns[6]
    else:
      instruction = columns[3]+"|"+columns[0]+"|IMM8|"+columns[5]
  elif "DIR" in columns:
    if columns[1] == "opr8a,rel":
      instruction = columns[3]+"|"+columns[0]+"|DIR_REL|"+columns[6]
    elif ",rel" in columns[1]:
      instruction = columns[4]+"|"+columns[0]+"|DIR_"+columns[1][0]+"_REL|"+columns[7]
    elif columns[1][1] == ",":
      instruction = columns[4]+"|"+columns[0]+"|DIR_"+columns[1][0]+"|"+columns[6]
    else:
      instruction = columns[3]+"|"+columns[0]+"|DIR|"+columns[5]
  elif "EXT" in columns:
    instruction = columns[3]+"|"+columns[0]+"|EXT|"+columns[6]
  elif "IX2" in columns:
    instruction = columns[3]+"|"+columns[0]+"|IX2|"+columns[6]
  elif "IX1" in columns:
    instruction = columns[3]+"|"+columns[0]+"|IX1|"+columns[5]
  elif "IX" in columns:
    if columns[1] == ",X,rel":
      instruction = columns[3]+"|"+columns[0]+"|IX_REL|"+columns[4]
    else:
      instruction = columns[3]+"|"+columns[0]+"|IX|"+columns[4]
  elif "IX1+" in columns:
    if columns[1] == "oprx8,X+,rel":
      instruction = columns[3]+"|"+columns[0]+"|DIR_X_PLUS_REL|"+columns[6]
    else:
      print "ERROR:" + str(columns)
      sys.exit(1)
  elif "IX+" in columns:
    if columns[1] == ",X+,rel":
      instruction = columns[3]+"|"+columns[0]+"|X_PLUS_REL|"+columns[5]
    else:
      print "ERROR:" + str(columns)
      sys.exit(1)
  elif "SP2" in columns:
    instruction = columns[3]+"|"+columns[0]+"|SP2|"+columns[6]
  elif "SP1" in columns:
    if columns[1] == "opr8i,SP,rel":
      instruction = columns[3]+"|"+columns[0]+"|SP1_REL|"+columns[6]
    elif columns[1] == "oprx8,SP,rel":
      instruction = columns[3]+"|"+columns[0]+"|SP1_REL|"+columns[6]
    else:
      instruction = columns[3]+"|"+columns[0]+"|SP2|"+columns[5]
  elif "INH" in columns:
    if columns[1] == "rel":
      instruction = columns[-3]+"|"+columns[0]+"|REL|"+columns[-1]
    else:
      instruction = columns[-2]+"|"+columns[0]+"|NONE|"+columns[-1]
  elif "REL" in columns:
    instruction = columns[3]+"|"+columns[0]+"|REL|"+columns[5]
  elif "DIR/DIR" in columns:
    instruction = columns[3]+"|"+columns[0]+"|DIR_DIR|"+columns[6]
  elif "DIR/IX+" in columns:
    instruction = columns[3]+"|"+columns[0]+"|DIR_IX_PLUS|"+columns[5]
  elif "IMM/DIR" in columns:
    instruction = columns[3]+"|"+columns[0]+"|IMM_DIR|"+columns[6]
  elif "IX+/DIR" in columns:
    instruction = columns[3]+"|"+columns[0]+"|IX_PLUS_DIR|"+columns[5]
  else:
    print "ERROR: " + str(columns)
    sys.exit(1)

  columns = instruction.split("|")
  if len(columns[0]) == 2:
    instructions.append(instruction)
  elif len(columns[0]) == 4:
    instructions16.append(instruction)
  else:
    print "ERROR: " + instruction

fp.close()

print """
/**
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

for instruction in instructions:
  #print instruction
  tokens = instruction.split("|")
  tokens[1] = "\"" + tokens[1] + "\""

  print "  { " + tokens[1] + ", CPU08_OP_" + tokens[2] + ", " + tokens[3] + " },  // " + ("0x%02x" % count) + " 0x" + tokens[0].lower()
  count += 1

print "};"
print

print "struct _m68hc08_table m68hc08_table[] = {"
for instruction in instructions16:
  #print instruction
  tokens = instruction.split("|")
  tokens[1] = "\"" + tokens[1] + "\""

  print "  { 0x" + tokens[0].lower() + ", " + tokens[1] + ", M6800_OP_" + tokens[2] + ", " + tokens[3] + " },"
print "};"
print

