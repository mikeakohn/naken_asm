#!/usr/bin/env python

import os

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write("stm8/\n")
  out.write("  segment 'rom'\n")
  out.write("  " + instruction + "\n")
  out.write("  end\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("stm8_template.txt", "rb")
out = open("stm8.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  print instruction
  create_asm(instruction)

  os.system("wine asm.exe -li temp.asm")

  fp1 = open("temp.lst", "rb")

  hex = ""

  for line in fp1:
    line = line.strip()
    if not "000000" in line: continue
    code = line.split()[2]
    if code == "R": code = line.split()[3]

    l = (len(code) / 2)
    code_len = "%02X" % l
    checksum = l

    for i in range(0, len(code), 2):
      a = code[i:i+2]
      a = int(a,16)
      #print "%02x" % ((0 - a) & 0xff)
      #print "%02x" % (((a ^ 0xff) + 1) & 0xff)
      checksum += a

    checksum = ((checksum ^ 0xff) + 1) & 0xff
    checksum = "%02X" % checksum
    hex = ":" + code_len + "000000" + code + checksum
    #print hex
    break

  out.write(instruction + "|" + hex + "\n")

  fp1.close

fp.close()
out.close()


os.remove("cbe.err")
os.remove("temp.obj")
os.remove("temp.lst")
os.remove("temp.asm")


