#!/usr/bin/env python

import sys

instructions = []

fp = open("6800.txt", "rb")

for line in fp:
  line = line.replace("  nn"," nn")
  line = line.replace("  #"," #")
  line = line.replace("  <dp"," <dp")
  line = line.replace("  >ext"," >ext")
  line = line.replace("  rel"," rel")
  columns = line.strip().split("  ")
  for column in columns:
    column = column.strip()
    if column != "": instructions.append(column.lower())
fp.close()

print """
/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table_6800.h"

struct _m6800_table m6800_table[] = {"""

count = 0

instructions.sort()
for instruction in instructions:
  #print instruction
  tokens = instruction.split()
  i = int("0x" + tokens[0], 16)
  #print str(count) + " " + str(i)
  if i != count: print "ERROR!"; sys.exit(1)
  operand = "NONE"
  if tokens[1] == "-": tokens[1] = "NULL"; operand = "UNDEF"
  else: tokens[1] = "\"" + tokens[1] + "\""

  if len(tokens) > 2:
    if tokens[2] == "nn,x": operand = "NN_X"
    elif tokens[2] == "#imm": operand = "IMM8"
    elif tokens[2] == "#n16": operand = "IMM16"
    elif tokens[2] == "<dp": operand = "DIR_PAGE_8"
    elif tokens[2] == ">ext": operand = "ABSOLUTE_16"
    elif tokens[2] == "rel": operand = "REL_OFFSET"
    else: operand = "??" + tokens[2]

  print "  { " + tokens[1] + ", M6800_OP_" + operand + " },  // " + ("0x%02x" % count)
  count += 1

print "};"
print

