#!/usr/bin/env python3

import os

def create_asm(instruction):
  out = open("temp.asm", "w")

  tokens = instruction.split()

  if len(tokens) > 1:
    if "." in tokens[1]:
      tokens[1] = tokens[1].replace(".", ", #")
      instruction = tokens[0] + " " + tokens[1]

    if instruction.startswith("ret"):
      instruction = tokens[0] + " #" + tokens[1]

  out.write(instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/pdk16.txt", "r")
out = open("pdk16.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("sdaspdk16 -l temp.asm")

  fp1 = open("temp.lst", "r")

  for line in fp1:
    line = line.strip()
    if line.startswith("000000"): break

  tokens = line.split()
  data = [ tokens[1], tokens[2] ]

  hex = ""
  code = ""
  l = 0
  checksum = 0

  for a in data:
    a = int(a, 16)
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

  os.remove("temp.lst")

fp.close()
out.close()

os.remove("temp.asm")

