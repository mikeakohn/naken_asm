#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  if instruction.startswith("main:"):
    out.write(instruction + "\n\n")
  else:
    out.write("  " + instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/f8.txt", "r")
out = open("f8.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("asmx -o temp.hex -C f8 temp.asm")

  fp1 = open("temp.hex", "r")
  hex = fp1.readline().strip()
  #print(hex)
  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("temp.hex")

fp.close()
out.close()

os.remove("temp.asm")

