#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write(".rsp\n")
  out.write(".create \"temp.bin\",0\n")
  out.write("  " + instruction + "\n")
  out.write(".close\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("n64_rsp_template.txt", "r")
out = open("n64_rsp.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("armips temp.asm")

  fp1 = open("temp.bin", "rb")

  data = fp1.read(4)

  checksum = 4 + data[0] + data[1] + data[2] + data[3]

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)

  code = "%02X%02X%02X%02X" % (data[0], data[1], data[2], data[3])

  out.write(instruction + "|:04000000" + code + checksum + "\n")

  fp1.close

  os.remove("temp.bin")

fp.close()
out.close()

os.remove("temp.asm")


