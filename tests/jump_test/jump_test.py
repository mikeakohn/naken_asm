#!/usr/bin/env python

import sys,os

def test_jump(arch, instruction, distance, jump, start, end):

  print instruction + " / " + jump + " ... ",

  out = open("test.asm", "wb")

  out.write("." + arch + "\n")
  out.write("  " + jump + "\n")
  out.write("  " + instruction + "\n")
  out.write("blah:\n\n")
  out.close()

  os.system("../../naken_asm -l test.asm > /dev/null")

  fp2 = open("out.hex")
  code = fp2.readline()
  fp2.close()

  code = code[start:end]

  #print code

  if int(code,16) != distance:
    print "\x1b[31mERROR! " + str(int(code,16)) + " " + str(distance) + "\x1b[0m"
    return 1
  else:
    print "\x1b[32mPASS\x1b[0m"
    return 0

# ------------------------------- fold here ---------------------------------

errors = 0

if len(sys.argv) != 2:
  print "Usage: jump_test.py <arch>"
  sys.exit(0)

arch = sys.argv[1]

fp = open("../comparison/" + arch + ".txt", "rb")

for line in fp:
  tokens = line.strip().split("|")
  code = tokens[1]
  code = code[:-2]
  code = code[9:]
  distance = len(code) / 2
  #print str(tokens) + " " + code + " " + str(distance)

  errors += test_jump(arch, tokens[0], distance + 6, "jmp (blah)", 17, 21)
  errors += test_jump(arch, tokens[0], distance + 4, "jmp (blah).w", 13, 17)
  errors += test_jump(arch, tokens[0], distance + 6, "jmp (blah).l", 17, 21)
  errors += test_jump(arch, tokens[0], distance, "jmp blah", 13, 17)
  errors += test_jump(arch, tokens[0], distance, "beq.s blah", 11, 13)

fp.close()

print "Total errors " + str(errors)

os.unlink("test.asm")
os.unlink("out.hex")
os.unlink("out.lst")

