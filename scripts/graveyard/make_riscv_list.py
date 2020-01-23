#!/usr/bin/env python

import random

rounding_methods = [ "rne", "rtz", "rdn", "rup", "rmm" ]

def add_rm(instr, operands):
  for rm in rounding_methods:
    print instr + operands + ", " + rm

# -------------------------- fold here ----------------------------

fp = open("../table/riscv.c", "rb")

branch = [
  "OP_JUMP",
  "OP_BRANCH",
]

for line in fp:
  if not "OP_" in line: continue
  if line.strip().startswith("//"): continue

  tokens = tokens = line.replace("{","").replace("}","").replace("\"","").strip().split(",")

  instr = tokens[0].strip()
  op = tokens[3].strip()

  if op == "OP_FENCE": continue

  #print instr + " " + op

  operands = ""

  r1 = "x" + str(random.randint(1, 31))
  r2 = "x" + str(random.randint(1, 31))
  r3 = "x" + str(random.randint(1, 31))
  f1 = "f" + str(random.randint(1, 31))
  f2 = "f" + str(random.randint(1, 31))
  f3 = "f" + str(random.randint(1, 31))
  f4 = "f" + str(random.randint(1, 31))
  sh = random.randint(1, 31)
  u = random.randint(1, 1000)

  if op == "OP_NONE":
    operands = ""
  elif op == "OP_FFFF":
    operands = ""
  elif op == "OP_R_TYPE":
    operands = " " + str(r1) + ", " + str(r2) + ", "  + str(r3)
  elif op == "OP_R_R":
    operands = " " + str(r1) + ", " + str(r2)
  elif op == "OP_U_TYPE":
    operands = " " + str(r1) + ", " + str(u)
  elif op == "OP_FP":
    operands = " " + str(f1)
  elif op == "OP_FP_FP_FP":
    operands = " " + str(f1) + ", " + str(f2) + ", "  + str(f3)
  elif op == "OP_FP_FP_RM":
    operands = " " + str(f1) + ", " + str(f2)
    print instr + operands
    add_rm(instr, operands)
    continue
  elif op == "OP_FP_FP_FP_RM":
    operands = " " + str(f1) + ", " + str(f2) + ", "  + str(f3)
    print instr + operands
    add_rm(instr, operands)
    continue
  elif op == "OP_FP_FP_FP_FP_RM":
    operands = " " + str(f1) + ", " + str(f2) + ", "  + str(f3) + ", " + str(f4)
    print instr + operands
    add_rm(instr, operands)
    continue
  elif op == "OP_FP_FP":
    operands = " " + str(f1) + ", " + str(f2)
  elif op == "OP_R_FP":
    operands = " " + str(r1) + ", " + str(f2)
  elif op == "OP_R_FP_RM":
    operands = " " + str(r1) + ", " + str(f2)
    print instr + operands
    add_rm(instr, operands)
    continue
  elif op == "OP_FP_R":
    operands = " " + str(f1) + ", " + str(r2)
  elif op == "OP_FP_R_RM":
    operands = " " + str(f1) + ", " + str(r2)
    print instr + operands
    add_rm(instr, operands)
    continue
  elif op == "OP_R_FP_FP":
    operands = " " + str(r1) + ", " + str(f2) + ", " + str(f3)
  elif op == "OP_STD_EXTXX":
    operands = " " + str(r1) + ", " + str(r2) + ", "  + str(r3)
    print instr + operands
    operands = ".aq " + str(r1) + ", " + str(r2) + ", "  + str(r3)
    print instr + operands
    operands = ".aq.rl " + str(r1) + ", " + str(r2) + ", "  + str(r3)
    print instr + operands
    operands = ".rl " + str(r1) + ", " + str(r2) + ", "  + str(r3)
    print instr + operands
    continue
  elif op == "OP_STD_EXT":
    operands = " " + str(r1) + ", " + str(r2) + ", ("  + str(r3) + ")"
    print instr + operands
    operands = ".aq " + str(r1) + ", " + str(r2) + ", ("  + str(r3) + ")"
    print instr + operands
    operands = ".aq.rl " + str(r1) + ", " + str(r2) + ", ("  + str(r3) + ")"
    print ";" + instr + operands
    operands = ".rl " + str(r1) + ", " + str(r2) + ", ("  + str(r3) + ")"
    print instr + operands
    continue
  elif op == "OP_LR":
    operands = " " + str(r1) + ", (" + str(r2) + ")"
    print instr + operands
    operands = ".aq " + str(r1) + ", (" + str(r2) + ")"
    print instr + operands
    operands = ".aq.rl " + str(r1) + ", (" + str(r2) + ")"
    print ";" + instr + operands
    operands = ".rl " + str(r1) + ", (" + str(r2) + ")"
    print instr + operands
    continue
  elif op == "OP_I_TYPE":
    operands = " " + str(r1) + ", " + str(r2) + ", "  + str(u)
    print instr + operands
    operands = " " + str(r1) + ", " + str(r2) + ", -"  + str(u)
    print instr + operands
    continue
  elif op in [ "OP_FD_INDEX_R", "OP_FS_INDEX_R" ]:
    operands = " " + str(f1) + ", " + str(u) + "("  + str(r2) + ")"
    print instr + operands
    operands = " " + str(f1) + ", -" + str(u) + "("  + str(r2) + ")"
    print instr + operands
    continue
  elif op == "OP_UI_TYPE":
    operands = " " + str(r1) + ", " + str(r2) + ", "  + str(u)
  #elif op == "OP_S_TYPE":
  #  operands = " " + str(r1) + ", " + str(r2) + ", "  + str(u)
  #  print instr + operands
  #  operands = " " + str(r1) + ", " + str(r2) + ", -"  + str(u)
  #  print instr + operands
  #  continue
  elif op in [ "OP_RD_INDEX_R", "OP_RS_INDEX_R" ]:
    operands = " " + str(r1) + ", " + str(u) + "(" + str(r2) + ")"
    print instr + operands
    operands = " " + str(r1) + ", -"  + str(u) + "(" + str(r2) + ")"
    print instr + operands
    continue
  elif op == "OP_SHIFT":
    operands = " " + str(r1) + ", " + str(r2) + ", "  + str(sh)
  elif op == "OP_READ":
    operands = " " + str(r1)
  elif op == "OP_SB_TYPE":
    instr = "main: " + instr
    operands = " x10, x11, main"
  elif op == "OP_UJ_TYPE":
    instr = "main: " + instr
    operands = " x10, main"
  else:
    print "Unknown OP: " + instr + " " + op

  if instr in [ "rdcycleh", "rdtimeh", "rdinstreth" ]:
    print ";" + instr + operands
  else:
    print instr + operands

fp.close()

print "fence"
print ";fence pi, so"
print ";fence si, po, pr, pw, pi, so, sr, sw"
print "or zero, ra, s0"
print "or s5, s1, s11"
print "or sp, tp, sp"
print "or a3, a0, a7"
print "or t0, t4, gp"
print "fsub.d ft0, fs1, fa1"
print "fsub.d fs1, fa0, fa7"
print "fsub.d ft0, ft3, ft5"


