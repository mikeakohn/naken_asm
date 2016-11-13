#!/usr/bin/env python

import random

fp = open("../table/powerpc.c", "rb")

reg3 = [
  "OP_RD_RA_RB",
  "OP_RS_RA_RB",
  "OP_RA_RS_RB",
]

reg2 = [
  "OP_RD_RA",
  "OP_RA_RS",
]

reg1 = [
  "OP_RD",
]

branch = [
  "OP_JUMP",
  "OP_BRANCH",
]

for line in fp:
  if not "OP_" in line: continue

  tokens = tokens = line.replace("{","").replace("}","").replace("\"","").strip().split(",")

  instr = tokens[0].strip()
  op = tokens[3].strip()
  flags = tokens[4].strip()

  #print instr + " " + op + " " + flags

  operands = ""

  r1 = "r" + str(random.randint(1, 31))
  r2 = "r" + str(random.randint(1, 31))
  r3 = "r" + str(random.randint(1, 31))
  sh = random.randint(1, 31)

  if op == "OP_NONE":
    operands = ""
  elif op in reg1:
    operands = " " + str(r1)
  elif op in reg2:
    operands = " " + str(r1) + ", " + str(r2)
  elif op in reg3:
    operands = " " + str(r1) + ", " + str(r2) + ", "  + str(r3)
  elif op == "OP_BRANCH_COND_ALIAS":
    instr = "main: " + instr
    operands = " main"
  elif op in branch:
    instr = "main: " + instr
    operands = " main"
  elif op == "OP_BRANCH_COND":
    instr = "main: " + instr
    operands = " 10, 11"
  elif op == "OP_BRANCH_BD":
    instr = "main: " + instr
    operands = " main, 10, 11"
  elif op in [ "OP_BRANCH_COND_BD", "OP_JUMP_COND_BD" ]:
    instr = "main: " + instr
    operands = " 10, 11, main"
  elif op == "OP_RD_SIMM":
    operands = " " + str(r1) + ", " + str(random.randint(10, 1000))
  elif op == "OP_RD_RA_SIMM":
    operands = " " + str(r1) + ", " + str(r2) + ", " + str(random.randint(10, 1000))
  elif op == "OP_RA_RS_UIMM":
    operands = " " + str(r1) + ", " + str(r2) + ", " + str(random.randint(10, 1000))
  elif op == "OP_RA_RS_SH_MB_ME":
    operands = " " + str(r1) + ", " + str(r2) + ", " + str(sh) + ", 2, 3"
  elif op in [ "OP_RS_OFFSET_RA", "OP_RD_OFFSET_RA" ]:
    operands = " " + str(r1) + ", " + str(random.randint(-100, 100)) + "(" + str(r2) + ")"
  elif op in [ "OP_RS_RA_NB", "OP_RD_RA_NB" ]:
    operands = " " + str(r1) + ", " + str(r2) + ", 10"
  elif op == "OP_CRB_CRB_CRB":
    operands = " crb7, crb3, crb1"
  elif op == "OP_RA_RS_SH":
    operands = " " + str(r1) + ", " + str(r2) + ", " + str(sh)
  elif op == "OP_CMP":
    operands = " cr5, " + str(r1) + ", " + str(r2)
  elif op == "OP_CMPI":
    operands = " cr5, " + str(r1) + ", " + str(random.randint(1, 100))
  elif op == "OP_CRD_CRS":
    operands = " cr5, cr3"
  elif op == "OP_CRD":
    operands = " cr5"
  elif op == "OP_RD_SPR":
    operands = str(r1) + ", 10"
  elif op == "OP_SPR_RS":
    operands = "10, " + str(r1)
  elif op == "OP_CRM_RS":
    operands = "10, " + str(r1)
  elif op == "OP_RD_TBR":
    operands = str(r1) + ", tbl"
  else:
    print "Unknown OP: " + instr + " " + op + " " + flags

  if not flags == "FLAG_REQUIRE_DOT":
    print instr + operands

  if flags in [ "FLAG_REQUIRE_DOT", "FLAG_DOT" ]:
    print instr + "." + operands

fp.close()

