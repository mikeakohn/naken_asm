#!/usr/bin/env python3

unique = { }

fp = open("scripts/graveyard/propeller2_cz.txt", "r")

for line in fp:
  tokens = line.strip().split("=")
  tokens[0] = tokens[0].lower().strip()
  tokens[1] = tokens[1].strip()[1:]

  code = int(tokens[1], 2)
  tokens.append(code)

  print("  { \"" + "%-12s" % (tokens[0] + "\",") + " " + "0x%x" % (tokens[2]) + " },")

  if not tokens[2] in unique: unique[tokens[2]] = tokens[0]

fp.close()

for n in range(0, 16):
  print("  \"" + unique[n] + "\",")

