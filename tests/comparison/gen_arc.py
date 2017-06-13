#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("arc_template.txt", "rb")
out = open("arc.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("arc-linux-as temp.asm")
  os.system("arc-linux-objcopy -F ihex a.out arc_gnu.hex")

  fp1 = open("arc_gnu.hex", "rb")
  hex = fp1.readline().strip()
  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("arc_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


