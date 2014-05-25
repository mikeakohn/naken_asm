#!/usr/bin/env python

# This can be used to make a naken_asm compatible include file from
# the include files that come with msp430-gcc.  On Ubuntu do:
#
# sudo apt-get install msp430mcu
#
# All the include files will be installed in /usr/msp430/include
# Change the name of the include file below and redirect to a file
# such as msp430f5529.inc

defines = {}
ignore = False
ndef_stdc = False

fp = open("/usr/msp430/include/msp430f5529.h", "rb")

for line in fp:
  if ndef_stdc == True:
    if line.startswith("#else"):
      ignore = True
      continue
    elif line.startswith("#endif"):
      ignore = False
      ndef_stdc = False
      continue

  if ignore == True: continue

  if line.startswith("#define"):
    line = "." + line[1:]
    line = line.replace("/*",";")
    line = line[0:line.find("*/")].strip()
    tokens = line.split()
    if len(tokens) < 3 or tokens[2] == ";": continue
    if tokens[1] not in defines:
      defines[tokens[1]] = tokens[2]
      print line
  elif line.startswith("#ifndef __STDC__"):
    ndef_stdc = True
  elif line.startswith("sfrb") or line.startswith("sfrw"):
    line = line[4:].replace("(", "").replace(");","").strip()
    tokens = line.split(",")
    tokens[0] = tokens[0].strip()
    tokens[1] = tokens[1].strip()
    print ".define " + tokens[0] + " " + tokens[1]

fp.close()

#print defines

