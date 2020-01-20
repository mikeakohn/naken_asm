#!/usr/bin/env python3

fp = open("m8c_start.txt", "r")

for line in fp:
  line = line.strip()

  instruction = line.split()[2:]
  data = line.split()[1]

  instruction = " ".join(instruction)

  #print(instruction + " " + data)

  opcodes = []

  for a in range(0, len(data), 2):
    opcodes.append(data[a:a+2])

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

