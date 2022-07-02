#!/usr/bin/env python3

import math

def get_real(value):
  # Convert fixed point to floating point.
  whole = value >> 4
  frac = value & 0xf

  return float(whole) + (frac / 16)

def show_fixed(value):
  return str(value >> 4) + " : " + str(value & 0xf)

def show_triangle(triangle):
  print("Triangle:")

  i = 0

  for value in triangle["vertex"]:
    print("  vertex " + str(i) + ": " + str(value))
    i += 1

  print("  is_left_major: " + str(triangle["is_left_major"]))
  print("           YH: " + show_fixed(triangle["YH"]))
  print("           YM: " + show_fixed(triangle["YM"]))
  print("           YL: " + show_fixed(triangle["YL"]))
  print("      XH_real: " + str(get_real(triangle["XH"])))
  print("      XM_real: " + str(get_real(triangle["XM"])))
  print("      XL_real: " + str(get_real(triangle["XL"])))
  print("           XH: " + show_fixed(triangle["XH"]))
  print("           XM: " + show_fixed(triangle["XM"]))
  print("           XL: " + show_fixed(triangle["XL"]))
  print("        DxHDy: " + show_fixed(triangle["DxHDy"]))
  print("        DxMDy: " + show_fixed(triangle["DxMDy"]))
  print("        DxLDy: " + show_fixed(triangle["DxLDy"]))
  print("   DxHDy_real: " + str(get_real(triangle["DxHDy"])))
  print("   DxMDy_real: " + str(get_real(triangle["DxMDy"])))
  print("   DxLDy_real: " + str(get_real(triangle["DxLDy"])))

  print()

  # Non-Shaded Triangle = 8. According to the docs, if it's a left major
  # then the flag should be 0, but for some reason this is the opposite
  # for me.
  #command = (8 << 8) | ((triangle["is_left_major"] ^ 1) << 7)
  command = (8 << 8) | ((triangle["is_left_major"]) << 7)

  YH = triangle["YH"] >> 2
  YM = triangle["YM"] >> 2
  YL = triangle["YL"] >> 2

  XL = triangle["XL"] << 12
  XH = triangle["XH"] << 12
  XM = triangle["XM"] << 12

  DxLDy = (triangle["DxLDy"] << 12) & 0xffffffff
  DxHDy = (triangle["DxHDy"] << 12) & 0xffffffff
  DxMDy = (triangle["DxMDy"] << 12) & 0xffffffff

  print("  .dc64 0x%04x%04x%04x%04x" % (command, YL, YM, YH))
  print("  .dc64 0x%08x%08x" % (XL, DxLDy))
  print("  .dc64 0x%08x%08x" % (XH, DxHDy))
  print("  .dc64 0x%08x%08x" % (XM, DxMDy))
  print()

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

  if y0 == y1: y1 += 1

  # Middle vertex leans to the right (left_major).
  triangle["is_left_major"] = int(x1 > x0)

  # The triangle is rendered from the top (YH) to the bottom (YL) with the
  # slope of the line changing at YM. Format is 11 bit whole 2 bit fraction
  # fixed point.
  triangle["YH"] = y0
  triangle["YM"] = y1
  triangle["YL"] = y2

  # Just YH_fraction is being used.
  YH_fraction = y0 & 0xf
  YM_fraction = y1 & 0xf
  YL_fraction = y2 & 0xf

  # Slope: y = dy/dx * x + y0
  # Inverse Slope: x = dx/dy * y + x0
  dx_h = x0 - x2
  dx_m = x0 - x1
  dx_l = x1 - x2
  dy_h = y0 - y2
  dy_m = y0 - y1
  dy_l = y1 - y2

  if dy_h == 0: dy_h = 1 << 4
  if dy_m == 0: dy_m = 1 << 4
  if dy_l == 0: dy_l = 1

  DxHDy = int((dx_h << 4) / dy_h)
  DxMDy = int((dx_m << 4) / dy_m)
  DxLDy = int((dx_l << 4) / dy_l)

  # XM is the X coordinate where the middle minor edge hits trunc(y0).
  # XH is the X coordinate where the major edge hits trunc(y0).
  # XL is the X coordinate where the middle minor edge hits YH.YH_2.
  XH = x0 - ((DxHDy * YH_fraction) >> 4)
  XM = x0 - ((DxMDy * YH_fraction) >> 4)
  XL = x0 + ((DxMDy * (y1 - y0)) >> 4)

  triangle["XH"] = get_real(XH)
  triangle["XM"] = get_real(XM)
  triangle["XL"] = get_real(XL)

  triangle["DxHDy"] = DxHDy
  triangle["DxMDy"] = DxMDy
  triangle["DxLDy"] = DxLDy

  triangle["XH"] = XH
  triangle["XM"] = XM
  triangle["XL"] = XL

  # Non-Shaded Triangle = 8
  command = 8

  show_triangle(triangle)

# ----------------------------- fold here ----------------------------

triangle_left_major = [
  [ 150 << 4, (120 << 4) | int(0xf * 15 / 100) ],
  [ 170 << 4, (170 << 4) | int(0xf * 50 / 100) ],
  [ 110 << 4, (190 << 4) | int(0xf * 75 / 100) ],
]

triangle_right_major = [
  [ 250 << 4, (120 << 4) | int(0xf * 15 / 100) ],
  [ 230 << 4, (170 << 4) | int(0xf * 50 / 100) ],
  [ 290 << 4, (190 << 4) | int(0xf * 75 / 100) ],
]

triangle_isosceles = [
  [ 250 << 4, (50 << 4) | int(0xf * 15 / 100) ],
  [ 230 << 4, (90 << 4) | int(0xf * 15 / 100) ],
  [ 270 << 4, (90 << 4) | int(0xf * 15 / 100) ],
]

triangle_isosceles_upside_down = [
  [ 130 << 4, (50 << 4) | int(0xf * 15 / 100) ],
  [ 170 << 4, (50 << 4) | int(0xf * 15 / 100) ],
  [ 150 << 4, (90 << 4) | int(0xf * 15 / 100) ],
]

calc_triangle(triangle_left_major)
calc_triangle(triangle_right_major)
calc_triangle(triangle_isosceles)
calc_triangle(triangle_isosceles_upside_down)

