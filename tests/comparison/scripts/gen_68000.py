#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  #out.write("  cpu 680x0\n")
  #out.write("  output hex\n")
  #out.write("  *=0\n")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("68000_template.txt", "rb")
out = open("68000.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("m68k-coff-as temp.asm")
  os.system("m68k-coff-objcopy -F ihex a.out m68k_gnu.hex")

  #fp1 = open("temp.hex", "rb")
  #hex = fp1.readline().strip()
  #out.write(instruction + "|" + hex + "\n")
  #fp1.close
  #os.remove("temp.hex")

  fp1 = open("m68k_gnu.hex", "rb")
  hex = fp1.readline().strip()
  out.write(instruction.replace("%","") + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("m68k_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


