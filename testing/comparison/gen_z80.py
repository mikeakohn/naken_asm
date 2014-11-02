#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  cpu z80\n")
  out.write("  output hex\n")
  out.write("  *=0\n")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("z80_template.txt", "rb")
out = open("z80.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  print instruction
  create_asm(instruction)

  #os.system("xa temp.asm")
  os.system("crasm -o temp.hex temp.asm > /dev/null")

  fp1 = open("temp.hex", "rb")
  hex = fp1.readline().strip()

  out.write(instruction + "|" + hex + "\n")

  fp1.close

  os.remove("temp.hex")

fp.close()
out.close()

os.remove("temp.asm")


