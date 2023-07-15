#!/usr/bin/env python3

import sys

defines = { }

fp = open("/usbdisk/devkits/microchip/mplabx/v3.15/mpasmx/p18f45k50.inc", "r")

for line in fp:
  line = line.strip()
  if not "EQU" in line: continue
  if line.strip().startswith(";"): continue

  comment = ""

  if ";" in line:
    i = line.find(";")
    comment = line[i:]
    line = line[:i]
    comment = " " + comment.strip()

  line = line.strip()

  (name, value) = line.split("EQU")
  name = name.strip()
  value = value.strip()

  if name in defines:
    if value != defines[name]:
      print("ERROR: Redefined name " + name + " doesn't match value.")
      sys.exit(-1)
    continue

  defines[name] = value

  value = "0x" + value.replace("H'", "").replace("'", "").lower()
  #print("%-20s equ %8s%s" % (name, value, comment))
  print("%-20s equ %8s" % (name, value))

fp.close()

