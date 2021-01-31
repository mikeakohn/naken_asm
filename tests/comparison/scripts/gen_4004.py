#!/usr/bin/env python

import os

def create_asm(instruction):

  out = open("temp.asm", "w")
  out.write("                .CR     4004\n")
  out.write("                .TF     temp.hex,hex\n")

  if instruction.startswith("main:"):
    instruction = instruction.replace("main:", "")
    instruction = "main            " + instruction
  else:
    instruction = "                " + instruction

  out.write(instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("4004_template.txt", "r")
out = open("4004.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("./sbasm temp.asm")

  fp1 = open("temp.hex", "r")

  code = fp1.read(16)
  code = code.strip()

  length = int(len(code) / 2)
  checksum = length

  for i in range(0, len(code), 2):
    data = code[i:i + 2]
    checksum += int(data, 16)

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)

  byte_count = "%02x" % length

  out.write(instruction + "|:" + byte_count + "000000" + code + checksum + "\n")

  fp1.close

  os.remove("temp.hex")

fp.close()
out.close()

os.remove("temp.asm")

