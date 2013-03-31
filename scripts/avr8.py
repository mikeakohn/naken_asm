#!/usr/bin/env python

fp = open("avr8.txt","r")

for line in fp:
  line = line.strip()
  if not line: continue

  tokens = line.split()
  instr = tokens[0]
  cycles = tokens[-1]
  line = line.replace(instr,"").replace(cycles,"").strip()

  if "/" in cycles:
    tokens = cycles.split("/")
    min = tokens[0]
    max = tokens[-1]
  else:
    min = cycles
    max = cycles

  line = line.split()
  line = "".join(line)

  if line == "none": line = "OP_NONE"
  elif line == "Rd,b": line = "OP_RD_B"
  elif line == "Rr,b": line = "OP_RR_B"
  elif line == "P,b": line = "OP_P_B"
  elif line == "P,Rr": line = "OP_P_Rr"
  elif line == "s": line = "OP_S"
  elif line == "Rd": line = "OP_RD"
  elif line == "Rd,P": line = "OP_RD_P"
  elif line == "Rr": line = "OP_RR"
  elif line == "k": line = "OP_K"
  elif line == "k,Rr": line = "OP_K_RR"
  elif line == "Rd,Rr": line = "OP_RD_RR"
  elif line == "Rd,K": line = "OP_RD_K"
  elif line == "Rd,X": line = "OP_RD_X"
  elif line == "Rd,-X": line = "OP_RD_MINUS_X"
  elif line == "Rd,k": line = "OP_RD_K"
  elif line == "Rd,Z": line = "OP_RD_Z"
  elif line == "Rd,Y": line = "OP_RD_Y"
  elif line == "s,k": line = "OP_S_K"
  elif line == "Rd,X+": line = "OP_RD_X_PLUS"
  elif line == "Z+q,Rr": line = "OP_Z_PLUS_Q_RR"
  elif line == "-Z,Rr": line = "OP_MINUS_Z_RR"
  elif line == "-Y,Rr": line = "OP_MINUS_Y_RR"
  elif line == "-X,Rr": line = "OP_MINUS_X_RR"
  elif line == "X,Rr": line = "OP_X_RR"
  elif line == "Y,Rr": line = "OP_Y_RR"
  elif line == "Z,Rr": line = "OP_Z_RR"
  elif line == "X+,Rr": line = "OP_X_PLUS_RR"
  elif line == "Y+,Rr": line = "OP_Y_PLUS_RR"
  elif line == "Z+,Rr": line = "OP_Z_PLUS_RR"
  elif line == "Rd,-Z": line = "OP_RD_MINUS_Z"
  elif line == "Rd,-Y": line = "OP_RD_MINUS_Y"
  elif line == "Rd,Z+": line = "OP_RD_Z_PLUS"
  elif line == "Rd,Y+": line = "OP_RD_Y_PLUS"
  elif line == "Rd,Z+q": line = "OP_RD_Z_PLUS_Q"
  elif line == "Rd,Y+q": line = "OP_RD_Y_PLUS_Q"
  elif line == "Y+q,Rr": line = "OP_Y_PLUS_Q_RR"

  print "  { \"" + instr + "\", 0x0000, 0x0000, " + line + ", " + min + ", "+ max + " },"

fp.close()

