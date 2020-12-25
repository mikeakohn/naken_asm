#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  processor 16f877\n")
  if instruction.startswith("main:"):
    out.write("main:\n  " + instruction.replace("main: ","") + "\n")
  else:
    out.write("  " + instruction + "\n")
  out.write("  end\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("pic14_template.txt", "rb")
out = open("pic14.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("gpasm temp.asm")

  fp1 = open("temp.hex", "rb")
  hex = fp1.readline().strip()
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("temp.hex")

fp.close()
out.close()

os.remove("temp.asm")


