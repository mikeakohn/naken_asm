#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("dspic_template.txt", "rb")
out = open("dspic.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("xc16-as temp.asm")
  #os.system("xc16-objcopy -F ihex a.out dspic_gnu.hex")
  os.system("xc16-bin2hex a.out")

  #fp1 = open("dspic_gnu.hex", "rb")
  fp1 = open("a.hex", "rb")
  hex = fp1.readline().strip()
  hex = fp1.readline().strip().upper()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("a.hex")
  #os.remove("dspic_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


