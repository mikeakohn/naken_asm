#!/usr/bin/env python

parts = {}

fp = open("lpc.txt","rb")

for line in fp:
  tokens = line.split()
  name = tokens[0]
  numbers = "".join(tokens[1:])
  for number in numbers.split(";"):
    number = number.lower()
    if number in parts:
      parts[number] += "," + name
    else:
      parts[number] = name

    #print "  { " + number.lower() + ", \"" + name + "\" },"

for part in parts:
  print "  { " + part + ", \"" + parts[part] + "\" },"
fp.close()

