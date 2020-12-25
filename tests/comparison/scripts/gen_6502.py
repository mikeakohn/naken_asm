#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  *=0\n")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("6502_template.txt", "rb")
out = open("6502.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("xa temp.asm")

  fp1 = open("a.o65", "rb")

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

  os.remove("a.o65")

fp.close()
out.close()

os.remove("temp.asm")


