#!/usr/bin/env python3

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write("  list p=18f452\n")
  out.write("  include \"p18f452.inc\"\n")

  instruction = instruction.replace(", a", ", 1")

  if instruction.endswith(" s"):
    instruction = instruction.replace(" s", " 1")

  if instruction.startswith("main:"):
    out.write("main:\n  " + instruction.replace("main: ","") + "\n")
  else:
    out.write("  " + instruction + "\n")
  out.write("  end\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/pic18.txt", "r")
out = open("pic18.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("mpasmx temp.asm")

  fp1 = open("temp.HEX", "r")
  hex = fp1.readline().strip()
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("temp.HEX")

fp.close()
out.close()

os.remove("temp.asm")

