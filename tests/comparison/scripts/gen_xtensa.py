#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write("  " + instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("xtensa_template.txt", "r")
out = open("xtensa.txt", "w")

for instruction in fp:
  instruction = str(instruction.strip())
  original = instruction

  if instruction.startswith(";"): continue
  print(instruction)

  print(" -> " + instruction)

  create_asm(instruction)

  os.system("xtensa-esp32-elf-as temp.asm")
  os.system("xtensa-esp32-elf-objcopy -F ihex a.out xtensa_gnu.hex")

  fp1 = open("xtensa_gnu.hex", "r")
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(original + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("xtensa_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


