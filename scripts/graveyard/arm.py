#!/usr/bin/env python

fp = open("arm_alu.txt", "rb")

count = 0
for line in fp:
  tokens = line.split()
  if count != int(tokens[0],2): print "WTF" 
  print "  { \"" + tokens[1].lower() + "\", 0x%08x, 0x0de00000, OP_ALU, 3, 2 }," % (0x00000000 | (count << 21))
  #print str(count) + " " + str(int(token[0],2))
  count += 1

fp.close()

