#!/usr/bin/env python3

import os, sys

effects = [
  "wz",
  "wc",
  "wcz",
  "andz",
  "andc",
  "orz",
  "orc",
  "xorz",
  "xorc",
]

def create_asm(instruction):
  for effect in effects:
    if ", " + effect in instruction:
      instruction = instruction.replace(", " + effect, " " + effect)
      break

  instruction = instruction.replace("0x", "$")

  print("  -> " + instruction)

  out = open("temp.asm", "w")
  out.write("dat\n")
  out.write("  orgh 0\n")
  out.write("  org 0\n")
  out.write("main\n")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/propeller2.txt", "r")
out = open("propeller2.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  print(instruction)

  if instruction.startswith(";"): continue

  create_asm(instruction)

  a = os.system("p2asm temp.asm")

  if a != 0:
    print("Error!")
    sys.exit(-1)

  p = os.popen("hexdump -C temp.bin", "r")
  hex = p.readline().strip()

  #print(hex)

  tokens = hex.upper().split()
  checksum = 4 + \
    int(tokens[1], 16) + \
    int(tokens[2], 16) + \
    int(tokens[3], 16) + \
    int(tokens[4], 16)

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)

  if "\\" in instruction: instruction = instruction.replace("\\", "\\\\")

  line = instruction + "|:04000000" + tokens[1] + tokens[2] + tokens[3] + tokens[4] + checksum
  #print(line)
  #if instruction != "nop": sys.exit(0)

  out.write(line + "\n")
  p.close

  os.remove("temp.lst")
  os.remove("temp.bin")

fp.close()
out.close()

os.remove("temp.asm")


