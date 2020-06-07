#!/usr/bin/env python3

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  if instruction.startswith("main:"):
    instruction = instruction.replace("main:","main").strip()
  else:
    instruction = "  " + instruction

  out.write("cpu 6800\n")
  out.write("* = 0\n")
  out.write(instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("6800_template.txt", "r")
out = open("6800.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("crasm -o temp.txt temp.asm")

  fp1 = open("temp.txt", "r")

  hex = ""
  code = ""
  l = 0
  checksum = 0

  line = fp1.readline().strip()
  if line == "": break 

  line = line[8:-2]
  print(line)

  for i in range(0, len(line), 2):
    a = int(line[i:i+2], 16)
    l += 1
    checksum += a
    code = code + ("%02X" % a)

  code_len = "%02X" % l
  checksum += l

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % checksum
  hex = ":" + code_len + "000000" + code + checksum

  out.write(instruction + "|" + hex + "\n")

  fp1.close

  os.remove("temp.txt")

fp.close()
out.close()

os.remove("temp.asm")

