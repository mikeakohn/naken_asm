#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("sh4_template.txt", "rb")
out = open("sh4.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  #os.system("sh4-linux-gnu-as --isa=sh4-up temp.asm")
  os.system("sh4-linux-gnu-as temp.asm")
  os.system("sh4-linux-gnu-objcopy -F ihex a.out sh4_gnu.hex")

  fp1 = open("sh4_gnu.hex", "rb")
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("sh4_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


