#!/usr/bin/env python

import os

def create_asm(instruction):

  out = open("temp.asm", "w")
  out.write("        ROMW    16\n")
  out.write("        ORG     $0000\n")

  instruction = instruction.replace("0x", "$")

  if instruction.startswith("main:"):
    instruction = instruction.replace("main:", "")
    instruction = "main    " + instruction
  else:
    instruction = "        " + instruction

  out.write(instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("cp1610_template.txt", "r")
out = open("cp1610.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("bin/as1600 -o temp.bin temp.asm")

  fp1 = open("temp.bin", "rb")

  data = fp1.read(16)

  length = len(data)
  checksum = length

  code = ""

  for i in range(0, len(data)):
    checksum += int(data[i])
    code += "%02X" % (int(data[i]))

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)

  byte_count = "%02x" % length

  out.write(instruction + "|:" + byte_count + "000000" + code + checksum + "\n")

  fp1.close

  os.remove("temp.bin")

fp.close()
out.close()

os.remove("temp.asm")

