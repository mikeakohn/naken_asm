#!/usr/bin/env python3

import os, sys

reg_nums = []
instructions = []
errors = 0

alias = [ "move", "negu", "not" ]

# -------------------------- fold here -----------------------------

print("Disassembler: PIC32")

for i in range(0, 32):
  reg_nums.append("$" + str(i))

fp = open("../comparison/pic32.txt", "r")
out = open("test.asm", "w")

out.write(".pic32\n")

for line in fp:
  instruction = line.split("|")[0].strip()
  if instruction.startswith("main:"): continue
  if instruction.startswith("li"): continue
  if instruction.startswith("la"): continue

  ignore = False
  for reg_num in reg_nums:
    if reg_num in instruction: ignore = True
  if ignore == True: continue

  if " " in instruction: name = instruction.split()[0]
  else: name = instruction

  if name in alias:
    #print("Skipping: " + name)
    continue

  out.write(instruction + "\n")
  instructions.append(instruction)

fp.close()
out.close()

os.system("../../naken_asm -l test.asm > /dev/null")

fp = open("out.lst", "r")

i = 0

for line in fp:
  if not line.startswith("0x"): continue

  line = line[23:64].strip()

  if line != instructions[i]:
    a = instructions[i].split(",")
    b = line.split(",")

    name_a = a[0].split()[0]
    name_b = b[0].split()[0]

    if a[0] in [ "di", "ei" ]: continue

    a[0] = a[0].split()[1]
    b[0] = b[0].split()[1]

    broken = False

    if len(a) != len(b) or name_a != name_b:
      print(name_a + " " + name_b)
      broken = True
    else:
      for j in range(0, len(a)):
        a[j] = a[j].strip()
        b[j] = b[j].strip()

        if a[j] != b[j]:
          if not " " in a[j] and "(" in a[j] and a[j][0] != '(' and \
             not " " in b[j] and "(" in b[j] and b[j][0] != '(':
            a[j] = a[j].replace("(", " (")
            b[j] = b[j].replace("(", " (")
            if a[j].split()[1] == b[j].split()[1]:
              value_a = int(a[j].split()[0], 0)
              value_b = int(b[j].split()[0], 0)

              if value_a < 0: value_a = value_a + 1 + 0xffff
              if value_b < 0: value_b = value_b + 1 + 0xffff

              if value_a == value_b: continue

          operands = b[j].replace("(","").replace(")","").split()

          if len(operands) == 2:
            if str(a[j]) in operands: continue

          broken = True

    if broken == True:
      print(str(i) + ") " + line + " " + instructions[i])
      errors += 1

  i += 1

fp.close()

os.unlink("test.asm")
os.unlink("out.hex")
os.unlink("out.lst")

if errors != 0:
  print("Total errors: " + str(errors))
  print("Failed!")
  sys.exit(-1)
else:
  print("Passed!")

