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

  data = tokens[1] + tokens[2] + tokens[3] + tokens[4]

  count = 4

  if int(tokens[4], 16) == 0xff and int(tokens[8], 16) != 0x00:
    data += tokens[5] + tokens[6] + tokens[7] + tokens[8]
    count = 8

  checksum = count

  for i in range(0, count):
    checksum += int(tokens[i + 1], 16)

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)

  if "\\" in instruction: instruction = instruction.replace("\\", "\\\\")

  line = instruction + "|:0" + str(count) + "000000" + data + checksum
  #print(line)
  #if instruction != "nop": sys.exit(0)

  out.write(line + "\n")
  p.close

  os.remove("temp.lst")
  os.remove("temp.bin")

fp.close()
out.close()

os.remove("temp.asm")


