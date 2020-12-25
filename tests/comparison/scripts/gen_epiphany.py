#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("epiphany_template.txt", "rb")
out = open("epiphany.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  # buildroot/esdk.2014.11/tools/e-gnu.x86_64/bin

  os.system("epiphany-elf-as temp.asm")
  os.system("epiphany-elf-objcopy -F ihex a.out epiphany_gnu.hex")

  fp1 = open("epiphany_gnu.hex", "rb")
  hex = fp1.readline().strip()
  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("epiphany_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


