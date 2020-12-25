#!/usr/bin/env python

import os

def create_asm(instruction):
  orig = instruction

  out = open("temp.asm", "wb")
  for i in range(0, 128):
    instruction = instruction.replace("r" + str(i), str(i))

  print orig + " -> " + instruction
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("cell_template.txt", "rb")
out = open("cell.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  original = instruction

  if instruction.startswith(";"): continue
  #print instruction,

  create_asm(instruction)

  os.system("spu-as temp.asm")
  os.system("spu-objcopy -F ihex a.out cell_gnu.hex")

  fp1 = open("cell_gnu.hex", "rb")
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  if "main" in instruction: original = "main: " + original

  out.write(original + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("cell_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


