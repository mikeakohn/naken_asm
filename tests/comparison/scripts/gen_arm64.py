#!/usr/bin/env python3

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("arm64_template.txt", "r")
out = open("arm64.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("as temp.asm")
  os.system("objcopy -F ihex a.out arm64_gnu.hex")

  fp1 = open("arm64_gnu.hex", "r")
  hex = fp1.readline().strip()
  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("arm64_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


