#!/usr/bin/env python

import os

def create_asm(instruction):
  if instruction.startswith("main:"):
    if "ja" in instruction:
      instruction = "." + instruction
    #print(instruction)

  out = open("temp.asm", "w")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/riscv64.txt", "r")
out = open("riscv64.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  #os.system("riscv64-unknown-elf-as temp.asm")
  #os.system("riscv64-unknown-elf-objcopy -F ihex a.out riscv_gnu.hex")

  if not instruction.startswith("c."):
    os.system("as temp.asm")
  else:
    os.system("as -march=rv64imafcd temp.asm")

  os.system("objcopy -F ihex a.out riscv_gnu.hex")

  fp1 = open("riscv_gnu.hex", "r")
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("riscv_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")

