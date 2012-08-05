#!/usr/bin/env python

name="SELM"
bit=3
size=2

for i in range(0,2**size):
  print name+"_"+str(i)+" equ 0x%02x" % (i<<bit)

