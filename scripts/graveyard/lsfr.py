#!/usr/bin/env python

address_to_lsfr = { }
lsfr_to_address = { }

lsfr = 0

for address in range(0, 64):
  address_to_lsfr[address] = lsfr
  lsfr_to_address[lsfr] = address

  b5 = (lsfr >> 5) & 1
  b4 = (lsfr >> 4) & 1
  b0 = (b5 ^ b4) ^ 1

  if lsfr == 0x1f: b0 = 1
  elif lsfr == 0x3f: b0 = 0

  #print "%02x %d %d -> %d" % (lsfr, b5, b4, b0)

  lsfr = (lsfr << 1) & 0x3f
  lsfr |= b0

#lsfr_to_address[lsfr] = address

print "uint8_t address_to_lsfr[] ="
print "{"

for i in range(0, 64):
  if (i % 8) == 0: print
  print "0x%02x," % address_to_lsfr[i],

print "};"

print "uint8_t lsfr_to_address[] ="
print "{"

for i in range(0, 64):
  if (i % 8) == 0: print
  print "0x%02x," % lsfr_to_address[i],

print "};"


