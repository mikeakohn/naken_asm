#!/usr/bin/env python

alu = [ "add", "addc", "and", "asr", "ior", "lsr", "rlc", "rlnc", "rrc",
        "rrnc", "sl", "sub", "subb", "subbr", "subr", "xor" ]
maybe = [ "clr", "dec", "dec2", "inc", "inc2", "neg" ]

#alu_ops = {}
alu_ops = []
accum = []
allops = {}

args_list = []

fp = open("dspic.txt", "rb")

for line in fp:
  line = line.strip()
  if line == "": continue
  tokens = line.split(";")

  tokens = [ token.strip() for token in tokens ]

  args = tokens[2]
  b = args.find("{")
  d = args.find(".")

  if b == -1 and d == -1:
    args = "none"
  elif b == -1 or d < b:
    args = args[d:]
  elif d == -1 or b < d:
    args = args[b:]
  else:
    args = args[b:]

  if args.startswith("."):
    #print " ".join(args.split()[1:])
    args = " ".join(args.split()[1:])
    if args == "": args = "none"

  if not args in args_list: args_list.append(args)

  #opkey = tokens[1].replace("1", "?").replace("0", "?")
  #if not opkey in allops: allops[opkey] = []
  #allops[opkey].append(tokens)

  #if tokens[0] in alu:
  #  if "Acc" in tokens[2]:
  #    if not opkey in accum: accum.append(opkey)
  #  else:
  #    if not opkey in alu_ops: alu_ops.append(opkey)

  #if tokens[0] in alu:
  #  if "Acc" in tokens[2]:
  #    print line
  #  else:
  #    alu_ops[tokens[1]] = tokens

fp.close()

print " ----------------"

for args in args_list:
  print args

#ops = alu_ops.keys()
#ops.sort()

#for op in ops:
#  print op + " " + alu_ops[op][0] + " " + alu_ops[op][1]

#for opkey in alu_ops:
#  print opkey
#  curr_ops = {}
#  for op in allops[opkey]:
#    #print "  " + str(op)
#    curr_ops[op[1]] = op
#
#  ops_sorted = curr_ops.keys()
#  ops_sorted.sort()
#  for op in ops_sorted:
#    print curr_ops[op]


