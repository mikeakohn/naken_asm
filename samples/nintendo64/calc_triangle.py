#!/usr/bin/env python3

import math

def show_triangle(triangle):
  print("Triangle:")

  i = 0

  for value in triangle["vertex"]:
    print("  vertex " + str(i) + ": " + str(value))
    i += 1

  print("  is_left_major: " + str(triangle["is_left_major"]))
  print("           YH: " + str(triangle["YH"]) + "/" + str(triangle["YH_2"]))
  print("           YM: " + str(triangle["YM"]) + "/" + str(triangle["YM_2"]))
  print("           YL: " + str(triangle["YL"]) + "/" + str(triangle["YL_2"]))
  print("           XH: " + str(triangle["XH"]) + "/" + str(triangle["XH_frac"]))
  print("           XM: " + str(triangle["XM"]) + "/" + str(triangle["XM_frac"]))
  print("           XL: " + str(triangle["XL"]) + "/" + str(triangle["XL_frac"]))
  print("        DxHDy: " + str(triangle["DxHDy"]) + "/" + str(triangle["DxHDy_frac"]))
  print("        DxMDy: " + str(triangle["DxMDy"]) + "/" + str(triangle["DxMDy_frac"]))
  print("        DxHDy: " + str(triangle["DxLDy"]) + "/" + str(triangle["DxLDy_frac"]))
  print("   DxHDy_real: " + str(triangle["DxHDy_real"]))
  print("   DxMDy_real: " + str(triangle["DxMDy_real"]))
  print("   DxLDy_real: " + str(triangle["DxLDy_real"]))

  print()

  #for i in range(0, 4):
  #  print("  .dc64 0x%016lx" % (triangle["words"][i]))

  # Non-Shaded Triangle = 8
  command = (8 << 8) | ((triangle["is_left_major"] ^ 1) << 7)

  YH = (triangle["YH"] << 2) | (triangle["YH_2"])
  YM = (triangle["YM"] << 2) | (triangle["YM_2"])
  YL = (triangle["YL"] << 2) | (triangle["YL_2"])

  print("  .dc64 0x%04x%04x%04x%04x" % (command, YL, YM, YH))
  print("  .dc64 0x%04x%04x%04x%04x" % (triangle["XL"], triangle["XL_frac"], triangle["DxLDy"], triangle["DxLDy_frac"]))
  print("  .dc64 0x%04x%04x%04x%04x" % (triangle["XH"], triangle["XH_frac"], triangle["DxHDy"], triangle["DxHDy_frac"]))
  print("  .dc64 0x%04x%04x%04x%04x" % (triangle["XM"], triangle["XM_frac"], triangle["DxMDy"], triangle["DxMDy_frac"]))

def compute_frac(value):
  #whole = math.trunc(value)
  #frac = abs(math.trunc((value - whole) * 0xffff))
  whole = int(math.trunc(value))
  frac = abs(int((value - whole) * 0xffff))
  whole = whole & 0xffff

  return (whole, frac)

def calc_triangle(vertex):
  triangle = { }

  # Sort vertexes so the Y values go from top of the screen to the bottom.
  vertex.sort(key = lambda x : x[1])

  triangle["vertex"] = vertex

  x0 = vertex[0][0]
  y0 = vertex[0][1]
  x1 = vertex[1][0]
  y1 = vertex[1][1]
  x2 = vertex[2][0]
  y2 = vertex[2][1]

  # Middle vertex leans to the right (left_major).
  triangle["is_left_major"] = x1 > x0

  # The triangle is rendered from the top (YH) to the bottom (YL) with the
  # slope of the line changing at YM. Format is 11 bit whole 2 bit fraction
  # fixed point.
  triangle["YH"] = int(y0)
  triangle["YM"] = int(y1)
  triangle["YL"] = int(y2)

  YH_fraction = y0 - triangle["YH"]
  YM_fraction = y1 - triangle["YM"]
  YL_fraction = y2 - triangle["YL"]

  triangle["YH_2"] = int(YH_fraction * 4)
  triangle["YM_2"] = int(YM_fraction * 4)
  triangle["YL_2"] = int(YL_fraction * 4)

  # Slope: y = dy/dx * x + y0
  # Inverse Slope: x = dx/dy * y + x0
  # XM is the X coordinate where the mid minor edge hits trunc(y0).
  # XH is the X coordinate where the major edge hits trunc(y0).
  # XL is the X coordinate where the mid monor edge hits YH.YH_2.

  if triangle["is_left_major"]:
    DxHDy_real = (x0 - x2) / (y0 - y2)
    DxMDy_real = (x0 - x1) / (y0 - y1)
    DxLDy_real = (x1 - x2) / (y1 - y2)

    XH_real = x0 - (DxHDy_real * YH_fraction)
    XM_real = x0 - (DxMDy_real * YH_fraction)
    XL_real = x0 - (DxMDy_real * y1)

  else:
    DxHDy_real = (x0 - x2) / (y0 - y2)
    DxMDy_real = (x0 - x1) / (y0 - y1)
    DxLDy_real = (x1 - x2) / (y1 - y2)

    XH_real = x0 - (DxHDy_real * YH_fraction)
    XM_real = x0 - (DxMDy_real * YH_fraction)
    XL_real = x0 - (DxMDy_real * y1)

  triangle["DxHDy_real"] = DxHDy_real
  triangle["DxMDy_real"] = DxMDy_real
  triangle["DxLDy_real"] = DxLDy_real

  (triangle["DxHDy"], triangle["DxHDy_frac"]) = compute_frac(DxHDy_real)
  (triangle["DxMDy"], triangle["DxMDy_frac"]) = compute_frac(DxMDy_real)
  (triangle["DxLDy"], triangle["DxLDy_frac"]) = compute_frac(DxLDy_real)

  (triangle["XH"], triangle["XH_frac"]) = compute_frac(XH_real)
  (triangle["XM"], triangle["XM_frac"]) = compute_frac(XM_real)
  (triangle["XL"], triangle["XL_frac"]) = compute_frac(XL_real)

  # Non-Shaded Triangle = 8
  command = 8

  triangle["words"] = [ ]

  word = \
    (command << 56) | \
    ((triangle["is_left_major"] ^ 1) << 55) | \
    (((triangle["YH"] << 2) | (triangle["YH"])) << 32) | \
    (((triangle["YM"] << 2) | (triangle["YM"])) << 32) | \
    (((triangle["YL"] << 2) | (triangle["YL"])) << 32)

  triangle["words"].append(word)

  word = \
    (triangle["XL"] << 48) | \
    (triangle["XL_frac"] << 32) | \
    (triangle["DxLDy"] << 16) | \
     triangle["DxLDy_frac"]

  triangle["words"].append(word)

  word = \
    (triangle["XH"] << 48) | \
    (triangle["XH_frac"] << 32) | \
    (triangle["DxHDy"] << 16) | \
     triangle["DxHDy_frac"]

  triangle["words"].append(word)

  word = \
    (triangle["XM"] << 48) | \
    (triangle["XM_frac"] << 32) | \
    (triangle["DxMDy"] << 16) | \
     triangle["DxMDy_frac"]

  triangle["words"].append(word)

  show_triangle(triangle)

# ----------------------------- fold here ----------------------------

vertex = [
  [ 300, 100.15 ],
  [ 200, 150.50 ],
  [ 320, 110.75 ],
]

calc_triangle(vertex)

