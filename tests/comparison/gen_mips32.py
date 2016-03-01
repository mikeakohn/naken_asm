#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("mips32_template.txt", "rb")
out = open("mips32.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("pic32-as temp.asm")
  os.system("pic32-objcopy -F ihex a.out mips32_gnu.hex")

  fp1 = open("mips32_gnu.hex", "rb")
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("mips32_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


