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
  print("      XH_real: " + str(triangle["XH_real"]))
  print("      XM_real: " + str(triangle["XM_real"]))
  print("      XL_real: " + str(triangle["XL_real"]))
  print("           XH: " + str(triangle["XH"]) + "/" + str(triangle["XH_frac"]))
  print("           XM: " + str(triangle["XM"]) + "/" + str(triangle["XM_frac"]))
  print("           XL: " + str(triangle["XL"]) + "/" + str(triangle["XL_frac"]))
  print("        DxHDy: " + str(triangle["DxHDy"]) + "/" + str(triangle["DxHDy_frac"]))
  print("        DxMDy: " + str(triangle["DxMDy"]) + "/" + str(triangle["DxMDy_frac"]))
  print("        DxLDy: " + str(triangle["DxLDy"]) + "/" + str(triangle["DxLDy_frac"]))
  print("   DxHDy_real: " + str(triangle["DxHDy_real"]))
  print("   DxMDy_real: " + str(triangle["DxMDy_real"]))
  print("   DxLDy_real: " + str(triangle["DxLDy_real"]))

  print()

  # Non-Shaded Triangle = 8. According to the docs, if it's a left major
  # then the flag should be 0, but for some reason this is the opposite
  # for me.
  #command = (8 << 8) | ((triangle["is_left_major"] ^ 1) << 7)
  command = (8 << 8) | ((triangle["is_left_major"]) << 7)

  YH = (triangle["YH"] << 2) | (triangle["YH_2"])
  YM = (triangle["YM"] << 2) | (triangle["YM_2"])
  YL = (triangle["YL"] << 2) | (triangle["YL_2"])

  print("  .dc64 0x%04x%04x%04x%04x" % (command, YL, YM, YH))
  print("  .dc64 0x%04x%04x%04x%04x" % (triangle["XL"], triangle["XL_frac"], triangle["DxLDy"], triangle["DxLDy_frac"]))
  print("  .dc64 0x%04x%04x%04x%04x" % (triangle["XH"], triangle["XH_frac"], triangle["DxHDy"], triangle["DxHDy_frac"]))
  print("  .dc64 0x%04x%04x%04x%04x" % (triangle["XM"], triangle["XM_frac"], triangle["DxMDy"], triangle["DxMDy_frac"]))

def compute_frac(value):
  if value < 0:
    negative = True
    value = -value
  else:
    negative = False

  whole = int(value)
  frac = abs(int((value - whole) * 0x10000))

  if negative:
    frac ^= 0xffff
    whole ^= 0xffff

  whole &= 0xffff

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

  if y0 == y1: y1 += 0.15

  # Middle vertex leans to the right (left_major).
  triangle["is_left_major"] = int(x1 > x0)

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
  dx_h = x0 - x2
  dx_m = x0 - x1
  dx_l = x1 - x2
  dy_h = y0 - y2
  dy_m = y0 - y1
  dy_l = y1 - y2

  if dy_h == 0: dy_h = 1.0
  if dy_m == 0: dy_m = 1.0
  if dy_l == 0: dy_l = 0.0000001

  DxHDy_real = dx_h / dy_h
  DxMDy_real = dx_m / dy_m
  DxLDy_real = dx_l / dy_l

  # XM is the X coordinate where the middle minor edge hits trunc(y0).
  # XH is the X coordinate where the major edge hits trunc(y0).
  # XL is the X coordinate where the middle minor edge hits YH.YH_2.
  XH_real = x0 - (DxHDy_real * YH_fraction)
  XM_real = x0 - (DxMDy_real * YH_fraction)
  XL_real = x0 + (DxMDy_real * (y1 - y0))

  triangle["DxHDy_real"] = DxHDy_real
  triangle["DxMDy_real"] = DxMDy_real
  triangle["DxLDy_real"] = DxLDy_real

  triangle["XH_real"] = XH_real
  triangle["XM_real"] = XM_real
  triangle["XL_real"] = XL_real

  (triangle["DxHDy"], triangle["DxHDy_frac"]) = compute_frac(DxHDy_real)
  (triangle["DxMDy"], triangle["DxMDy_frac"]) = compute_frac(DxMDy_real)
  (triangle["DxLDy"], triangle["DxLDy_frac"]) = compute_frac(DxLDy_real)

  (triangle["XH"], triangle["XH_frac"]) = compute_frac(XH_real)
  (triangle["XM"], triangle["XM_frac"]) = compute_frac(XM_real)
  (triangle["XL"], triangle["XL_frac"]) = compute_frac(XL_real)

  # Non-Shaded Triangle = 8
  command = 8

  show_triangle(triangle)

# ----------------------------- fold here ----------------------------

triangle_left_major = [
  [ 150, 120.15 ],
  [ 170, 170.50 ],
  [ 110, 190.75 ],
]

triangle_right_major = [
  [ 250, 120.15 ],
  [ 230, 170.50 ],
  [ 290, 190.75 ],
]

triangle_isosceles = [
  [ 250, 50.15 ],
  [ 230, 90.15 ],
  [ 270, 90.15 ],
]

triangle_isosceles_upside_down = [
  [ 130, 50.15 ],
  [ 170, 50.15 ],
  [ 150, 90.15 ],
]

calc_triangle(triangle_left_major)
calc_triangle(triangle_right_major)
calc_triangle(triangle_isosceles)
calc_triangle(triangle_isosceles_upside_down)

