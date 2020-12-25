#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  instruction = instruction.replace("0x","$")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("sweet16_template.txt", "rb")
out = open("sweet16.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print instruction
  create_asm(instruction)

  os.system("ca65 --cpu sweet16 -l temp.list temp.asm")

  fp1 = open("temp.list", "rb")

  hex = ""
  code = ""
  l = 0
  checksum = 0

  for line in fp1:
    if line.startswith("000000r") and not line.endswith(" "):
      tokens = line[11:24].strip().split()
      print tokens

      for token in tokens:
        a = int(token, 16)
        print a
        checksum += a

      code = "".join(tokens)
      code_len = "%02X" % len(tokens)
      checksum += len(tokens)
      checksum = ((checksum ^ 0xff) + 1) & 0xff
      checksum = "%02X" % checksum
      hex = ":" + code_len + "000000" + code + checksum
      break

  out.write(instruction + "|" + hex + "\n")

  fp1.close

  os.remove("temp.list")
  os.remove("temp.o")

fp.close()
out.close()

os.remove("temp.asm")

