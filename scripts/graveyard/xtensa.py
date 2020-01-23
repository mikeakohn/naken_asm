#!/usr/bin/env python

import random
import sys

c4 = [ -1, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 32, 64, 128, 256 ]


fp = open("../../table/xtensa.c", "rb")

for line in fp:
  line = line.strip()
  if not ", XTENSA_OP" in line: continue

  tokens = line.split(",")
  tokens[0] = tokens[0].replace("{", "").replace("\"", "").strip()
  tokens[1] = tokens[1].strip()
  tokens[2] = tokens[2].strip()
  tokens[3] = tokens[3].replace("}", "").strip()

  operands = tokens[3].replace("XTENSA_OP_", "").split("_")

  instr = tokens[0]
  count = 0
  s = ""
  skip = False 
  narrow = False
  prefix = ""

  if operands[0] in [ "BRANCH", "CALL", "LOOP" ]:
    for i in range(1, len(operands)):

      if count != 0: s += ","

      if skip: skip = False; continue

      op = operands[i]

      if op == "N":
        narrow = True
        continue

      if op == "I8": a = "main"
      elif op == "I18": a = "main"
      elif op == "I12": a = "main"
      elif op == "LABEL": a = "main"
      elif op == "B5": a = str(random.randint(16, 31))
      elif op == "C4": a = str(c4[random.randint(0, 15)])
      elif op == "I6": a = "8"
      elif op in [ "AR", "AS", "AT" ]: a = "a" + str(random.randint(0, 16))
      elif op in [ "FR", "FS", "FT" ]: a = "f" + str(random.randint(0, 16))
      elif op in [ "BR", "BS", "BT" ]: a = "b" + str(random.randint(0, 16))
      else:
        print "ERROR" + instr + " " + str(operands)
        sys.exit()

      s += " " + a
      count += 1

    print "main: " + instr + s
  else:
    for i in range(0, len(operands)):
      if skip: skip = False; continue

      op = operands[i]

      if op == "N":
        narrow = True
        continue

      if op == "NONE": break

      if count != 0: s += ","

      if op == "N128" and operands[i + 1] == "127":
        a = str(random.randint(-128, -1))
        skip = True
      elif op == "N1" and operands[i + 1] == "15":
        a = random.randint(0, 15)
        if a == 0: a = -1
        a = str(a)
        skip = True
      elif op == "N32768" and operands[i + 1] == "32512":
        a = str(random.randint(-128, 127) << 8)
        skip = True
      elif op == "0" and operands[i + 1] == "32760":
        a = str(random.randint(0, 4096) << 3)
        skip = True
      elif op == "0" and operands[i + 1] == "15":
        a = str(random.randint(0, 15))
        skip = True
      elif op == "7" and operands[i + 1] == "22":
        a = str(random.randint(0, 15) + 7)
        skip = True
      elif op == "0" and operands[i + 1] == "1020":
        a = str(random.randint(0, 8) << 4)
        skip = True
      elif op == "0" and operands[i + 1] == "240":
        a = str(random.randint(0, 15) << 4)
        skip = True
      elif op == "0" and operands[i + 1] == "510":
        a = str(random.randint(0, 7) << 2)
        skip = True
      elif op == "0" and operands[i + 1] == "4096":
        a = str(random.randint(0, 15) << 3)
        skip = True
      elif op == "0" and operands[i + 1] == "255":
        a = str(random.randint(0, 255))
        skip = True
      elif op == "N64" and operands[i + 1] == "N4":
        a = str(random.randint(-64, -4))
        skip = True
      elif op == "0" and operands[i + 1] == "60":
        a = str(random.randint(0, 60))
        skip = True
      elif op == "N2048" and operands[i + 1] == "2047":
        a = str(random.randint(-2048, 2047))
        skip = True
      elif op == "N8" and operands[i + 1] == "7":
        a = str(random.randint(-8, 7))
        skip = True
      elif op == "1" and operands[i + 1] == "31":
        a = str(random.randint(1, 31))
        skip = True
      elif op == "0" and operands[i + 1] == "31":
        a = str(random.randint(0, 31))
        skip = True
      elif op in [ "AR", "AS", "AT" ]: a = "a" + str(random.randint(0, 15))
      elif op in [ "FR", "FS", "FT" ]: a = "f" + str(random.randint(0, 15))
      elif op in [ "BR", "BS", "BT" ]: a = "b" + str(random.randint(0, 15))
      elif op == "BS4": a = "b" + str(random.randint(0, 3) * 4)
      elif op == "BS8": a = "b" + str(random.randint(0, 1) * 8)
      elif op == "NUM": a = str(random.randint(0, 15))
      elif op == "MW": a = "m" + str(random.randint(0, 3))
      elif op == "MX": a = "m1"
      elif op == "MY": a = "m3"
      elif op == "SPR": a = "SAR"
      elif op == "SHIFT": a = "19"
      elif op == "MASK": a = "7"
      elif op == "I16": a = "main"; prefix = "main: "
      else:
        print "ERROR " + instr + " " + str(operands)
        sys.exit()

      s += " " + a
      count += 1

    print prefix + instr + s

fp.close()

