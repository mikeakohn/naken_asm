#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("propeller_template.txt", "rb")
out = open("propeller.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("propeller-elf-as temp.asm")
  os.system("propeller-elf-objcopy -F ihex a.out propeller_gnu.hex")

  fp1 = open("propeller_gnu.hex", "rb")
  hex = fp1.readline().strip()

  effects = []

  for b in [ "wc", "wz", "wr", "nr" ]:
    a = instruction.find(b)
    if a > 0: effects.append(a)

  if len(effects) != 0:
    effects.sort()
    a = effects[0] - 1
    instruction = instruction[:a] + "," + instruction[a:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("propeller_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


