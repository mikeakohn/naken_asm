#!/usr/bin/env python3

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write("  " + instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("tms340_template.txt", "r")
out = open("tms340.txt", "w")

for instruction in fp:
  instruction = str(instruction.strip())
  original = instruction

  if instruction.startswith(";"): continue
  print(instruction)

  print(" -> " + instruction)

  create_asm(instruction)

  p = os.popen("/home/mike/gspa temp.asm", "r")
  hex = p.readline().strip().replace(" ", "")
  p.close()

  code = ""
  l = 0
  checksum = 0

  for i in range(0, len(hex), 2):
    value = hex[i:i+2]
    value = int(value, 16)
    l += 1
    checksum += value
    code = code + ("%02X" % value)

  code_len = "%02X" % l
  checksum += l

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % checksum

  out.write(original + "|:" + code_len + "000000" + code + checksum + "\n")

fp.close()
out.close()

os.remove("temp.asm")

