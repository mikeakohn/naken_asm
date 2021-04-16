#!/usr/bin/env python

import os, sys

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write("  " + instruction + "\n")
  out.write("END\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/8051.txt", "r")
out = open("8051.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  print(instruction)
  create_asm(instruction)

  a = os.system("c51asm temp.asm")

  if a != 0:
    print("Error! Quitting.. on " + instruction)
    sys.exit(-1)

  fp1 = open("temp.hex", "r")
  hex = fp1.readline().strip()
  out.write(instruction + "|" + hex + "\n")
  fp1.close


  os.remove("temp.hex")

fp.close()
out.close()

os.remove("temp.asm")

