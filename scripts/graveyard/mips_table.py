#!/usr/bin/env python

def parse_file(filename, struct_name):
  print "struct _mips_instr " + struct_name + "[] = {"

  fp = open(filename, "rb")

  for line in fp:
    line = line.strip()
    if line == "": continue

    tokens = line.split()

    arg_count = 0
    instr = tokens.pop(0)
    function = tokens.pop()
    null_params = (3 - len(tokens))
    params = ""

    for token in tokens:
      token = token.replace(",","").strip()
      if token == "rd": params += "MIPS_OP_RD, "
      elif token == "rs": params += "MIPS_OP_RS, "
      elif token == "rt": params += "MIPS_OP_RT, "
      elif token == "sa": params += "MIPS_OP_SA, "
      elif token == "label": params += "MIPS_OP_LABEL, "
      elif token == "immediate": params += "MIPS_OP_IMMEDIATE, "
      elif token == "immediate(rs)": params += "MIPS_OP_IMMEDIATE_RS, "
      elif token == "rt=00000": params += "MIPS_OP_RT_IS_0, "; arg_count-=1
      elif token == "rt=00001": params += "MIPS_OP_RT_IS_1, "; arg_count-=1
      else: print "ERROR! " + token
      arg_count += 1

    params = (params + ("MIPS_OP_NONE, " * null_params)).strip()
    params = params[:-1].strip()

    print "  { \"" + instr + "\", { " + params + " }, " + str("0x%02x" % int(function, 2)) + ", " + str(arg_count) +  " },"

  print "  { NULL, { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x00, 0 }"
  print "};\n"

def parse_co_file(filename, struct_name):
  print "struct _mips_cop_instr " + struct_name + "[] = {"

  fp = open(filename, "rb")

  for line in fp:
    line = line.strip()
    if line == "": continue

    tokens = line.split()

    arg_count = 0
    instr = tokens.pop(0)
    formt = tokens.pop()
    function = tokens.pop()
    null_params = (3 - len(tokens))
    params = ""

    for token in tokens:
      token = token.replace(",","").strip()
      if token == "fd": params += "MIPS_COP_FD, "
      elif token == "fs": params += "MIPS_COP_FS, "
      elif token == "ft": params += "MIPS_COP_FT, "
      else: print "ERROR! " + token
      arg_count += 1

    params = (params + ("MIPS_OP_NONE, " * null_params)).strip()
    params = params[:-1].strip()

    print "  { \"" + instr + "\", { " + params + " }, " + str("0x%02x" % int(function, 2)) + ", " + str("0x%02x" % int(formt, 2)) + ", " + str(arg_count) +  " },"

  print "  { NULL, { MIPS_COP_NONE, MIPS_COP_NONE, MIPS_COP_NONE }, 0x00, 0x00, 0 }"
  print "};\n"


# ----------------------------- fold here ----------------------------------

print "#include <stdio.h>"
print "#include <stdlib.h>"
print "#include \"disasm_mips32.h\""
print

parse_file("mips_r.txt", "mips_r_table")
parse_file("mips_i.txt", "mips_i_table")
parse_co_file("mips_co.txt", "mips_cop_table")


