#!/usr/bin/env python

import os, sys

def create_asm(instruction):
  out = open("temp.asm", "wb")
  if instruction[-1] in [ "A","B","C","D","E","F" ]:
    reg = ord(instruction[-1]) - ord('A') + 10
    instruction = instruction[:-1] + str(reg)
    print instruction
  out.write("  CPU 1805\n")
  out.write("  " + instruction + "\n")
  out.write("  END\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("1802_template.txt", "rb")
out = open("1802.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  a = os.system("a18 temp.asm -o temp.hex")

  if a != 0:
    print "Error!"
    sys.exit(-1)

  fp1 = open("temp.hex", "rb")
  hex = fp1.readline().strip()
  out.write(instruction + "|" + hex + "\n")
  fp1.close


  os.remove("temp.hex")

fp.close()
out.close()

os.remove("temp.asm")


