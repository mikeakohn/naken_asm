#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  if instruction.startswith("main:"):
    instruction = instruction.replace("main:","main").strip()
  else:
    instruction = "  " + instruction
  out.write(instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("6809_template.txt", "rb")
out = open("6809.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("asm6809 -o temp.bin temp.asm")

  fp1 = open("temp.bin", "rb")

  hex = ""
  code = ""
  l = 0
  checksum = 0

  while 1:
    a = fp1.read(1)
    if a == "": break 
    a = ord(a)
    l += 1
    checksum += a
    code = code + ("%02X" % a)

  code_len = "%02X" % l
  checksum += l

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % checksum
  hex = ":" + code_len + "000000" + code + checksum

  out.write(instruction + "|" + hex + "\n")

  fp1.close

  os.remove("temp.bin")

fp.close()
out.close()

os.remove("temp.asm")

