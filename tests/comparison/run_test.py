#!/usr/bin/env python

import sys, os

def little_endian_32(code):
  data = ""

  for i in range(0, len(code), 8):
    a = code[i:i+8]
    data += a[6:8] + a[4:6] + a[2:4] + a[0:2]

  return data

# ----------------------------- fold here -------------------------------

if len(sys.argv) != 2:
  print "Usage: " + sys.argv[0] + " <cpu_type>"
  sys.exit(0)

instructions = []

cpu_type = sys.argv[1]
count = 0
need_nop = False
multiline = False
errors = 0

if cpu_type in [ "pic32", "ps2_ee" ]:
  need_nop = True

if cpu_type in [ "avr8", "msp430", "msp430x" ]:
  multiline = True

fp = open(cpu_type + ".txt", "rb")
out = open("test.asm", "wb")

out.write("." + cpu_type + "\n")

for line in fp:
  if line.startswith("main:"):
    line = line.replace("main", "main" + str(count))
    out.write(".org 0\n")
    count += 1

  tokens = line.strip().split("|")

  out.write(tokens[0] + "\n")

  code = tokens[1][:-2][9:].lower()
  code = little_endian_32(code)

  if len(code) == 16 and need_nop and \
    (line.startswith("main") or line[0] == 'j'):
    out.write("nop\n")

  instructions.append([ tokens[0], code ])

fp.close()
out.close()

os.system("../../naken_asm -l test.asm > /dev/null")

index = 0

fp = open("out.lst", "rb")

while(1):
  line = fp.readline()
  if not line: break

  if line.startswith("0x"):
    code = line.split()[1]
    if code.startswith("0x"): code = code[2:]

    if len(instructions[index][1]) == 16:
      while(1):
        line = fp.readline()
        if line.startswith("0x"):
          a = line.split()[1]
          if a.startswith("0x"): a = a[2:]
          code += a
          break

    if multiline == True:
      while len(instructions[index][1]) > len(code):
        a = fp.readline().strip()
        if " " in a:
          a = a.split()[1]
          if a.startswith("0x"): a = a[2:]
        code = a + code

    print "[" + cpu_type + "]testing " + instructions[index][0] + " ...",

    if instructions[index][1] == code:
      print "\x1b[32mPASS\x1b[0m"
    else:
      print "\x1b[31mFAIL " + instructions[index][1] + " >>" + code + "\x1b[0m"
      errors += 1
      #print instructions[index][1],
      #print code
    index += 1

fp.close()

os.unlink("out.hex")
os.unlink("out.lst")
os.unlink("test.asm")

if errors != 0:
  print "errors: " + str(errors)
  print "FAILED"
  sys.exit(1)



