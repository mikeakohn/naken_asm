#!/usr/bin/env python

import sys

def find_bitpos(bits, letter):
  if letter == "*": return ("-1", "-1")

  length = str(bitfields.count(letter))
  pos = str(24 - (bitfields.find(letter) + int(length)))
  return (pos, length)

# -------------------------------- fold here ------------------------------

args_dict = {
  "f": [ "OP_F", "f" ],
  "{WREG}": [ "OP_WREG", "D" ],
  "WREG": [ "OP_WREG", "*" ],
  "Wn": [ "OP_WN", "s" ],
  "Wm": [ "OP_WM", "v" ],
  "Wb": ["OP_WB", "w" ],
  "Wd": ["OP_WD", "d" ],
  "Ws": ["OP_WS", "s" ],
  "Wns": ["OP_WNS", "s" ],
  "Wnd": ["OP_WND", "d" ],
  "Acc": ["OP_ACC", "A" ],
  "Expr": ["OP_EXPR", "n" ],
  #"Wd": ["OP_WS_LIT"     // Ws "s" "k"
  #"Wd": ["OP_WD_LIT"     // Wd "d" "k"
  "Wm*Wm": ["OP_MULT2", "m" ],
  "Wm*Wn": ["OP_MULT3", "m" ],
  "{[Wx]Wxd}": ["OP_PREFETCH_X", "i"],
  "Wxd": ["OP_WXD", "x" ],
  "{[Wy]Wyd}": ["OP_PREFETCH_Y", "j"],
  "Wyd": ["OP_WYD", "y" ],
  "{AWB}": ["OP_ACC_WB", "a"],

  "[Wx]": ["OP_PREFETCH_ED_X", "i"],
  "[Wy]": ["OP_PREFETCH_ED_Y", "j"],
  "[Wd]": ["OP_W_INDEX", "d"],
  "[Ws]": ["OP_W_INDEX", "s"],
}

print "#include <stdio.h>"
print "#include <stdlib.h>"
print "#include \"table_dspic.h\""
print
print "struct _dspic_table dspic_table[] = {"

fp = open("dspic.txt", "rb")

for line in fp:
  line = line.strip()
  if line == "": continue
  tokens = line.split(";")

  tokens = [ token.strip() for token in tokens ]
  #tokens[0] = tokens[0].strip()
  #tokens[1] = tokens[1].strip()
  #tokens[2] = tokens[2].strip()
  #tokens[3] = tokens[3].strip()

  bitfields = tokens[1].replace(" ", "")
  mask = ""
  mask48 = ""
  opcode = ""
  opcode48 = ""
  bitlen_tot = 24

  for i in range(0,len(bitfields)):
    if bitfields[i] != "1": opcode += "0"
    else: opcode += "1"
    if bitfields[i] == '0' or bitfields[i] == '1': mask += "1"
    else: mask += "0"
 

  if len(mask) == 48:
    mask48 = mask[24:]
    mask = mask[:24]
    opcode48 = opcode[24:]
    opcode = opcode[:24]
    bitlen_tot = 48
  if len(mask) != 24:
    print "ERROR: " + mask + " is not 24 or 48 bits"
    sys.exit(1)

  mask = "0x%06x" % int(mask, 2)
  opcode = "0x%06x" % int(opcode, 2)
  if mask48 == "":
    mask48 = "0x000000"
    opcode48 = "0x000000"
  else:
    mask48 = "0x%06x" % int(mask48, 2)
    opcode48 = "0x%06x" % int(opcode48, 2)

  if "," in tokens[2]:
    args = len(tokens[2].split(","))
  elif tokens[2].replace(tokens[0].upper(), "").strip() != "":
    args = 1
  else:
    args = 0

  if "-" in tokens[3]:
    (cycles_min,cycles_max) = tokens[3].split("-")
  else:
    cycles_min = tokens[3]
    cycles_max = tokens[3]

  #if not tokens[0].startswith("bra"): continue
  if tokens[0].startswith("bra") and tokens[0] != "bra":
    tokens[2] = tokens[2].replace(tokens[0].upper().replace("BRA ","")+", ", "")
    args-=1
    #print tokens[2]

  temp = tokens[2].split()
  instr = temp[0]
  instr_args = temp[1:]

  if "{.B}" in instr:
    flag = "FLAG_B|FLAG_W"
    default_flag = "FLAG_W"
    flag_pos = 23 - bitfields.find("B")
  elif "{C|Z}" in instr:
    flag = "FLAG_C|FLAG_Z"
    default_flag = "FLAG_NONE"
    flag_pos = 23 - bitfields.find("Z")
  elif instr.endswith(".B"):
    flag = "FLAG_B"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif "{W|D}" in instr:
    flag = "FLAG_S|FLAG_SW|FLAG_SD"
    default_flag = "FLAG_NONE"
    flag_pos = 23 - bitfields.find("W")
  elif instr.endswith(".D"):
    flag = "FLAG_D"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".N"):
    flag = "FLAG_N"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".SS"):
    flag = "FLAG_SS"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".SU"):
    flag = "FLAG_SU"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".US"):
    flag = "FLAG_US"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".UU"):
    flag = "FLAG_UU"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".S"):
    flag = "FLAG_S"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  elif instr.endswith(".R"):
    flag = "FLAG_R"
    default_flag = "FLAG_NONE"
    flag_pos = -1;
  else:
    if "." in instr: print "Error: unknown flag: " + line; sys.exit(1)
    flag = "FLAG_NONE"
    default_flag = "FLAG_NONE"
    flag_pos = 0

  operand = []
  operand.append("{ OP_NONE, 0, 0, 0, 0, 0 }")
  operand.append("{ OP_NONE, 0, 0, 0, 0, 0 }")
  operand.append("{ OP_NONE, 0, 0, 0, 0, 0 }")
  operand.append("{ OP_NONE, 0, 0, 0, 0, 0 }")
  operand.append("{ OP_NONE, 0, 0, 0, 0, 0 }")

  count = 0
  for arg in instr_args:
    optype = "OP_NONE"
    bitlen = "0"
    bitpos = "0"
    optional = "0"
    attrlen = "0"
    attrpos = "0"

    bitletter = "?"

    arg_key = arg.replace(",", "")

    #if tokens[0] in [ "div", "divf" ]:
    #  # FIXME - what about t for div???
    #  optype = "OP_WN"
    #  if count == 0: bitletter = "v"
    #  elif count == 1: bitletter = "s"
    if arg_key in args_dict:
      optype = args_dict[arg_key][0]
      bitletter = args_dict[arg_key][1]

      if arg_key == "Ws":
        (attrpos, attrlen) = find_bitpos(bitfields, "p")
      elif arg_key == "Wd":
        (attrpos, attrlen) = find_bitpos(bitfields, "q")

    elif "#lit" in arg:
      optype = "OP_LIT"
      bitletter = "k"
    elif "#Slit" in arg:
      optype = "OP_LIT"
      bitletter = "r"
    elif "#bit" in arg:
      optype = "OP_LIT"
      bitletter = "b"
    elif arg_key == "[Ws+Slit10]":
      optype = "OP_WS_LIT"
      bitletter = "s"
      (attrpos, attrlen) = find_bitpos(bitfields, "k")
    elif arg_key == "[Wd+Slit10]":
      optype = "OP_WD_LIT"
      bitletter = "d"
      (attrpos, attrlen) = find_bitpos(bitfields, "k")
    else:
      print "Error: " + arg + " " + tokens[0]
      sys.exit(1)

    if tokens[0].startswith("bra") and optype == "OP_EXPR":
      optype = "OP_BRA"
      #bitletter = "n"
    elif tokens[0] == "do" and optype == "OP_EXPR":
      optype = "OP_EXPR_DO"
      #bitletter = "n"
    elif (tokens[0] == "goto" or tokens[0] == "call") and optype == "OP_EXPR":
      optype = "OP_EXPR_GOTO"
      #bitletter = "n"

    if "{" in arg: optional = "1"
    else: optional = "0"

    (bitpos, bitlen) = find_bitpos(bitfields, bitletter)

    if int(bitpos) > 23:
      if arg_key == "Wn" and not "s" in bitfields and "d" in bitfields:
        (bitpos, bitlen) = find_bitpos(bitfields, "d")
      elif arg_key == "Wm" and not "v" in bitfields and "t" in bitfields:
        (bitpos, bitlen) = find_bitpos(bitfields, "t")
      elif arg_key == "#Slit6" and not "r" in bitfields and "k" in bitfields:
        (bitpos, bitlen) = find_bitpos(bitfields, "k")
      elif arg_key == "Wb" and not "w" in bitfields and "s" in bitfields:
        (bitpos, bitlen) = find_bitpos(bitfields, "s")
      else:
        print "\n>>ERROR: bitpos is more than 23"
        print arg_key + " " + bitletter
        print line
        sys.exit(1)

    operand[count] = "{ " + \
      optype + ", " + \
      bitlen + ", " + \
      bitpos + ", " + \
      optional + ", " + \
      attrlen + ", " + \
      attrpos + \
      " }"
    count += 1

  print "  { \"" + tokens[0] + "\", " + \
        opcode + ", " + opcode48 + ", " + \
        mask + ", " + mask48 + ", " + \
        str(bitlen_tot) + ", " + \
        str(args) + ", " + \
        cycles_min + ", " + cycles_max + ", " + \
        flag + ", " + default_flag + ", " + str(flag_pos) + ", " + \
        " { " + \
        operand[0] + ", " + \
        operand[1] + ", " + \
        operand[2] + ", " + \
        operand[3] + \
        " } " + \
        " },"

print "  { NULL, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  {{ 0 }} }"
print "};"
print
print




