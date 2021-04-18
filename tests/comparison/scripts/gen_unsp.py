#!/usr/bin/env python3

import os

stack = [ "push", "pop" ]
alu = {
  "add": "+",
  "adc": "+",
  "sub": "-",
  "sbc": "-",
  "xor": "^",
  "or": "|",
  "and": "&",
}

carry = [ "adc", "sbc" ]

def create_asm(instruction):
  out = open("temp.asm", "w")

  use_rs_form = False

  instruction = instruction.replace("#", "")

  if " " in instruction:
    name = instruction.split(" ")[0]
    operands = instruction.replace(name, "").strip().split(",")

    if len(operands) > 1 and name not in [ "st", "mac.ss", "mac.us", "mac.uu" ]:
      operands[1] = operands[1].replace("#", "")

      if operands[0].startswith("[") and operands[0].endswith("]"):
        a = operands[0].replace("[", "").replace("]", "")
        if a.isnumeric(): use_rs_form = True
        result = operands[0]
        operands[0] = operands[1]
        operands[1] = operands[2]
  else:
    name = ""
    operands = ""

  #if name == "mac":
  #  instruction = "mr = " + operands[0] + " * " + operands[1] + "," + operands[2]
  #  print(instruction)
  #  sys.exit(1)

  if name in stack:
    if name == "push": instruction = instruction.replace(",", " to ")
    elif name == "pop": instruction = instruction.replace(",", " from ")
    instruction = instruction.replace("-", ",") 
  elif name in alu:
    if use_rs_form:
      instruction = result + " = " + operands[0] + " " + alu[name] + " " + operands[1]
    elif len(operands) == 3:
      instruction = operands[0] + " =  " + operands[1] + " " + alu[name] + " " + operands[2]
    else:
      instruction = operands[0] + " " + alu[name] + "= " + operands[1]
  elif name == "ld":
    instruction = operands[0] + " = " + operands[1]
  elif name == "st":
    instruction = operands[1] + " = " + operands[0]
  elif name == "neg":
    instruction = operands[0] + " = -" + operands[1]
  elif name == "mul.ss":
    instruction = "mr = " + operands[0] + " * " + operands[1]
  elif name == "mul.us":
    instruction = "mr = " + operands[0] + " * " + operands[1] + ",us"
  elif name == "mul.uu":
    instruction = "mr = " + operands[0] + " * " + operands[1] + ",uu"
  elif name == "mac.ss":
    instruction = "mr = " + operands[0] + " * " + operands[1] + "," + operands[2]
  elif name == "mac.us":
    instruction = "mr = " + operands[0] + " * " + operands[1] + ",us," + operands[2]
  elif name == "mac.uu":
    instruction = "mr = " + operands[0] + " * " + operands[1] + ",uu," + operands[2]

  if name in carry: instruction += ",carry"

  print("  " + instruction)

  out.write(".code\n")
  out.write(instruction + "\n\n")
  out.close()

# --------------------------------- fold here -------------------------------

fp = open("template/unsp.txt", "r")
out = open("unsp.txt", "w")

for instruction in fp:
  instruction = instruction.strip()
  if instruction.startswith(";"): continue
  print(instruction)
  create_asm(instruction)

  os.system("wine /dist/xasm16.exe -t3 temp.asm")

  length = os.path.getsize("temp.obj")

  fp1 = open("temp.obj", "rb")

  if "goto" in instruction or "call" in instruction:
    code_size = 4
    fp1.seek(0x74e)
  elif instruction.startswith("main:"):
    code_size = 2
    fp1.seek(0x74e)
  else:
    code_size = length - 1826
    fp1.seek(0x71d)

  data = fp1.read(code_size)

  hex = ""
  code = ""
  l = 0
  checksum = 0

  for i in data:
    #print(" %02x" % i)
    l += 1
    checksum += i
    code = code + ("%02X" % i)

  code_len = "%02X" % l
  checksum += l

  checksum = ((checksum ^ 0xff) + 1) & 0xff
  checksum = "%02X" % checksum
  hex = ":" + code_len + "000000" + code + checksum

  out.write(instruction + "|" + hex + "\n")

  fp1.close

  os.remove("temp.obj")

fp.close()
out.close()

os.remove("temp.asm")

