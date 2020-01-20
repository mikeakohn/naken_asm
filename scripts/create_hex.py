#!/usr/bin/env python3

fp = open("mcs48_start.txt", "r")

for line in fp:
  line = line.strip()

  tokens = line.split("|")

  instruction = tokens[0].strip()
  opcodes = tokens[1].strip().split()[1:]

  #print(instruction + " " + str(opcodes))

  hex_line = ":%02X000000" % (len(opcodes))

  checksum = len(opcodes)

  for opcode in opcodes:
    hex_line += opcode.upper()
    value = int(opcode, 16)
    checksum += value

  checksum = -(checksum & 0xff)
  checksum = checksum & 0xff

  hex_line += "%02X" % (checksum)

  print(instruction + "|" + hex_line)

fp.close()

