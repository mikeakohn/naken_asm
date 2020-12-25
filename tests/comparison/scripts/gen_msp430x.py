#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("msp430x_template.txt", "rb")
out = open("msp430x.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("msp430-as temp.asm -mmsp430x2619 -mcpu=430x")
  os.system("msp430-objcopy -F ihex a.out msp430x_gnu.hex")

  fp1 = open("msp430x_gnu.hex", "rb")
  hex = fp1.readline().strip()
  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("msp430x_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


