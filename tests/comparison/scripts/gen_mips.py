#!/usr/bin/env python3

import os

# Using binutils-2.37.tar.gz:
# ./configure --target==mips-netbsd-elf

def create_asm(instruction):
  out = open("temp.asm", "w")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/mips.txt", "r")
out = open("mips.txt", "w")

for instruction in fp:
  version = "-mips4"
  instruction = instruction.strip()

  if instruction.endswith("[2]"):
    version = "-mips2"
    instruction = instruction.replace("[2]", "").strip()

  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  # For Imagination Creator Ci20 board (Ingenic).
  #os.system("as-new -march=r4000 -EB temp.asm")
  #os.system("objcopy --only-section=.text -F ihex a.out mips_gnu.hex")

  # For binutils cross compiler.
  os.system("as-new " + version + " temp.asm")
  os.system("objcopy --only-section=.text -F ihex a.out mips_gnu.hex")

  fp1 = open("mips_gnu.hex", "r")
  hex = fp1.readline().strip()

  data = [
    int(hex[9:11], 16),
    int(hex[11:13], 16),
    int(hex[13:15], 16),
    int(hex[15:17], 16) ]

  checksum = 4 + data[0] + data[1] + data[2] + data[3]

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)

  code = "%02X%02X%02X%02X" % (data[0], data[1], data[2], data[3])

  out.write(instruction + "|:04000000" + code + checksum + "\n")

  #out.write(instruction + "|" + hex + "\n")
  fp1.close

  os.remove("a.out")
  os.remove("mips_gnu.hex")

fp.close()
out.close()

os.remove("temp.asm")

