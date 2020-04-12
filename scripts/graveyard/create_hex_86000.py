#!/usr/bin/env python3

import sys, random, os

if sys.version_info < (3, 0):
  print("Can't run on Python 2")
  sys.exit(-1)

fp = open("../tests/comparison/86000_template.txt", "r")

for line in fp:
  line = line.strip()

  instruction = line

  #print(instruction)

  out = open("test.asm", "w")
  out.write("\n  " + instruction + "\n")
  out.close()

  os.system("~/temp/vms_assembler/vmsassembler_src/aslc86k test.asm")

  ops = open("test.vms", "rb")
  data = ops.read()
  ops.close()

  opcodes = ""

  checksum = len(data)

  hex_line = ":%02X000000" % (len(data))

  for c in data:
    checksum += c
    opcodes += "%02X" % (c)

  hex_line += opcodes

  checksum = -(checksum & 0xff)
  checksum = checksum & 0xff

  hex_line += "%02X" % (checksum)

  print(instruction + "|" + hex_line)

fp.close()

