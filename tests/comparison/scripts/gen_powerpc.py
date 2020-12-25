#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("powerpc_template.txt", "rb")
out = open("powerpc.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  original = instruction

  if instruction.startswith(";"): continue
  print instruction,

  for i in range(0,32):
    if "tbl" in instruction:
      instruction = instruction.replace("tbl", "268")
    if "tbh" in instruction:
      instruction = instruction.replace("tbl", "269")
    if ("crb" + str(i)) in instruction:
      instruction = instruction.replace("crb" + str(i), str(i))
    if ("(r" + str(i) + ")") in instruction:
      instruction = instruction.replace("(r" + str(i) + ")", "(" + str(i) + ")")
    if (" r" + str(i)) in instruction:
      instruction = instruction.replace(" r" + str(i), " " + str(i))
    if (" v" + str(i)) in instruction:
      instruction = instruction.replace(" v" + str(i), " " + str(i))
    if (" fp" + str(i)) in instruction:
      instruction = instruction.replace(" fp" + str(i), " " + str(i))

  if (" fl") in instruction:
    instruction = instruction.replace(" fl", " 0")
  if (" fg") in instruction:
    instruction = instruction.replace(" fg", " 1")
  if (" fe") in instruction:
    instruction = instruction.replace(" fe", " 2")
  if (" fu") in instruction:
    instruction = instruction.replace(" fu", " 3")

  print " -> " + instruction

  create_asm(instruction)

  os.system("as -maltivec temp.asm")
  os.system("objcopy -F ihex a.out powerpc_gnu.hex")

  fp1 = open("powerpc_gnu.hex", "rb")
  hex = fp1.readline().strip()

  #if instruction.startswith("b"):
    #l = len(hex)
    #old = hex + " " + hex[:l-10] + " " + hex[-2:]
    #out.write(old + "\n")
    #hex = hex[0:l-10] + hex[-2:]

  out.write(original + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("powerpc_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")


