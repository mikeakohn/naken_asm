#!/usr/bin/env python

import os, sys

def create_asm(instruction):
  out = open("temp.asm", "wb")
  out.write(".orig 0\n")
  out.write("  " + instruction + "\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("lc3_template.txt", "rb")
out = open("lc3.txt", "wb")

for instruction in fp:
  instruction = instruction.strip()
  print instruction

  if instruction.startswith(";"): continue

  create_asm(instruction)

  a = os.system("lc3as temp.asm")

  if a != 0:
    print "Error!"
    sys.exit(-1)

  p = os.popen("hexdump -C temp.obj", "rb")
  #fp1 = open("temp.obj", "rb")
  hex = p.readline().strip()
  tokens = hex.upper().split()
  # :02000000 1947 00
  # :040000000000 1705 E0
  checksum = 4 + int(tokens[3], 16) + int(tokens[4], 16)
  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % (checksum)
  #out.write(instruction + "|:02000000" + tokens[3] + tokens[4] + checksum + "\n")
  out.write(instruction + "|:040000000000" + tokens[3] + tokens[4] + checksum + "\n")
  p.close

  os.remove("temp.obj")
  os.remove("temp.sym")

fp.close()
out.close()

os.remove("temp.asm")


