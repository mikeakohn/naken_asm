#!/usr/bin/env python

import sys, struct

reg_desc = {
  0x00: "PRIM",
  0x01: "RGBAQ",
  0x02: "ST",
  0x03: "UV",
  0x04: "XYZF2",
  0x05: "XYZ2",
  0x06: "TEX0_1",
  0x07: "TEX0_2",
  0x08: "CLAMP_1",
  0x09: "CLAMP_2",
  0x0a: "FOG",
  0x0c: "XYZF3",
  0x0d: "XYZ3",
  0x0e: "A_D",
  0x0f: "NOP",
  0x14: "TEX1_1",
  0x15: "TEX1_2",
  0x16: "TEX2_1",
  0x17: "TEX2_2",
  0x18: "XYOFFSET_1",
  0x19: "XYOFFSET_2",
  0x1a: "PRMODECONT",
  0x1b: "PRMODE",
  0x1c: "TEXCLUT",
  0x22: "SCANMSK",
  0x34: "MIPTBP1_1",
  0x35: "MIPTBP1_2",
  0x36: "MIPTBP2_1",
  0x37: "MIPTBP2_2",
  0x3b: "TEXA",
  0x3d: "FOGCOL",
  0x3f: "TEXFLUSH",
  0x40: "SCISSOR_1",
  0x41: "SCISSOR_2",
  0x42: "ALPHA_1",
  0x43: "ALPHA_2",
  0x44: "DIMX",
  0x45: "DTHE",
  0x46: "COLCLAMP",
  0x47: "TEST_1",
  0x48: "TEST_2",
  0x49: "PABE",
  0x4a: "FBA_1",
  0x4b: "FBA_2",
  0x4c: "FRAME_1",
  0x4d: "FRAME_2",
  0x4e: "ZBUF_1",
  0x4f: "ZBUF_2",
  0x50: "BITBLTBUF",
  0x51: "TRXPOS",
}

flg_desc = [
  "PACKED",
  "REGLIST",
  "IMAGE",
  "Disabled",
]

pre_desc = [
  "Ignores PRIM field",
  "Outputs PRIM field to PRIM register",
]

eop_desc = [
  "End of packet with primitive",
  "End of packet without primitive",
]

prim_desc = [
  "POINT",            #  0
  "LINE",             #  1
  "LINE_STRIP",       #  2
  "TRIANGLE",         #  3
  "TRIANGLE_STRIP",   #  4
  "TRIANGLE_FAN",     #  5
  "SPRITE",           #  6
]

psm_desc = {
  0x0: "PSMCT32",
  0x1: "PSMCT24",
  0x2: "PSMCT16",
  0xa: "PSMCT16S",
  0x30: "PSMZ32",
  0x31: "PSMZ24",
  0x32: "PSMZ16",
  0x3a: "PSMZ16S",
}

zpsm_desc = {
  0x0: "PSMZ32",
  0x1: "PSMZ24",
  0x2: "PSMZ16",
  0xa: "PSMZ16S",
}

def bytes_to_64bit(tokens):
  tokens.reverse()
  return "".join(tokens)

def show_giftag(data, registers):

  print "GIFtag:"

  nreg = int(data[0], 16)
  data = long(data, 16)

  flg = (data >> 58) & 0x3
  prim = (data >> 47) & 0x7ff
  pre = (data >> 46) & 0x1
  eop = (data >> 15) & 0x1
  nloop = data & 0x7fff

  if flg == 0:
    nloop_count = str(nreg * nloop) + " qwords"
  elif flg == 1:
    nloop_count = str(nreg * nloop) + " dwords"
  elif flg == 2:
    nloop_count = str(nloop) + " qwords"
  elif flg == 3:
    nloop_count = str(nloop) + " qwords"

  #print "%x" % data

  reglist = []
  for a in registers:
    reglist.append(reg_desc[int(a,16)])

  reglist.reverse()
  reglist = ",".join(reglist[0:nreg])

  if prim < len(prim_desc):
    prim = prim_desc[prim] + " (" + str(prim) + ")"
  else:
    prim = str(prim)

  print "  nloop: " + str(nloop) + " (" + nloop_count + ")"
  print "    eop: " + eop_desc[eop] + " (" + str(eop) + ")"
  print "    pre: " + pre_desc[pre] + " (" + str(pre) + ")"
  print "   prim: " + prim
  print "    flg: " + flg_desc[flg] + " (" + str(flg) + ")"
  print "   nreg: " + str(nreg)
  print "reglist: " + reglist

  return nloop

def show_colclamp(data):
  print "COLCLAMP: Color Clamp Control"
  print "CLAMP: " + str(data & 0x1)

def show_dthe(data):
  print "DTHE: Dither Control"
  print "Dithering Control (DTHE): " + str(data & 0x1)

def show_frame(data, address):
  psm = (data >> 24) & 0x3f

  print address + ": Frame Buffer Setting"
  print "Frame Buffer Base Pointer (FBP): 0x%x" % ((data & 0x1ff) * 2048)
  print "       Frame Buffer Width (FBW): " + str(((data >> 16) & 0x3f) * 64)
  print "     Pixel Storage Format (PSM): " + psm_desc[psm]
  print "      Frame Buffer Mask (FBMSK): " + str(data >> 32)

def show_zbuf(data, address):
  psm = (data >> 24) & 0xf

  print address + ": Z Buffer Setting"
  print " Z Buffer Base Pointer (ZBP): 0x%x"  % ((data & 0x1ff) * 2048)
  print "Z Value Storage Format (PSM): " + zpsm_desc[psm]
  print " Z Value Drawing Mask (ZMSK): " + str((data >> 32) & 1)

def show_prim(data):
  print "PRIM: Drawing Primitive"

  if (data >> 3) & 1 == 0: iip = "Flat Shading (0)"
  else: iip = "Gouraud Shading (1)"

  if (data >> 4) & 1 == 0: tme = "Texture Mapping Off (0)"
  else: tme = "Texture Mapping On (1)"

  if (data >> 5) & 1 == 0: fge = "Fogging Off (0)"
  else: fge = "Fogging On (1)"

  if (data >> 6) & 1 == 0: abe = "Alpha Blending Off (0)"
  else: abe = "Alpha Blending On (1)"

  if (data >> 7) & 1 == 0: aa1 = "Pass Antialiasing Off (0)"
  else: aa1 = "Pass Antialiasing On (1)"

  if (data >> 8) & 1 == 0: fst = "STQ For Texture Coordinates (0)"
  else: fst = "UV For Texture Coordinates (1)"

  if (data >> 9) & 1 == 0: ctxt = "Environmental Context 1 Used (0)"
  else: ctxt = "Environmental Context 2 Used (1)"

  if (data >> 10) & 1 == 0: fix = "Fragment Value Control Unfixed (0)"
  else: fix = "Fragment Value Control Fixed (1)"

  print "PRIM: " + prim_desc[data & 0x7] + " (" + str(data & 0x7) + ")"
  print " IIP: " + iip
  print " TME: " + tme
  print " FGE: " + fge
  print " ABE: " + abe
  print " AA1: " + aa1
  print " FST: " + fst
  print "CTXT: " + ctxt
  print " FIX: " + fix

def show_prmodecont(data):
  print "PRMODECONT: Specification of Primitive Attribute Setting Method"
  print "AC: " + str(data & 0x1)

def show_rgbaq(data):
  print "RGBAQ: Vertex Color Setting"

  q = (data >> 32)

  print "R: " + str(data & 0xff)
  print "G: " + str((data >> 8) & 0xff)
  print "B: " + str((data >> 16) & 0xff)
  print "A: " + str((data >> 24) & 0xff)
  print "Q: " + str(struct.unpack("f", struct.pack("I", q))[0])

def show_xyoffset(data, address):
  print address + ": Offset Value Setting"

  x_hex = "%02x" % (data & 0xffff)
  offset_x = float(int(x_hex[:-1], 16)) + (float(int(x_hex[-1])) / 16)

  y_hex = "%02x" % ((data >> 32) & 0xffff)
  offset_y = float(int(y_hex[:-1], 16)) + (float(int(y_hex[-1])) / 16)

  print "Offset X (OFX): " + str(offset_x) + " (0x" + x_hex + ")"
  print "Offset Y (OFY): " + str(offset_y) + " (0x" + y_hex + ")"

def show_xyz2(data):
  print "XYZ2: Setting for Vertex Coordinate Values"

  x_hex = "%02x" % (data & 0xffff)
  x = float(int(x_hex[:-1], 16)) + (float(int(x_hex[-1])) / 16)

  y_hex = "%02x" % ((data >> 16) & 0xffff)
  y = float(int(y_hex[:-1], 16)) + (float(int(y_hex[-1])) / 16)

  print "X: " + str(x) + " (0x" + x_hex + ")"
  print "Y: " + str(y) + " (0x" + y_hex + ")"
  print "Z: " + str(data >> 32)

def show_xyz3(data):
  print "XYZ3: Setting for Vertex Coordinate Values (without Drawing Kick)"

  x_hex = "%02x" % (data & 0xffff)
  x = float(int(x_hex[:-1], 16)) + (float(int(x_hex[-1])) / 16)

  y_hex = "%02x" % ((data >> 16) & 0xffff)
  y = float(int(y_hex[:-1], 16)) + (float(int(y_hex[-1])) / 16)

  print "X: " + str(x) + " (0x" + x_hex + ")"
  print "Y: " + str(y) + " (0x" + y_hex + ")"
  print "Z: " + str(data >> 32)

def show_scissor(data, address):
  print address + ": Setting for Scissoring Area"

  x0 = str(data & 0x7ff)
  x1 = str((data >> 16) & 0x7ff)
  y0 = str((data >> 32) & 0x7ff)
  y1 = str((data >> 48) & 0x7ff)

  print " Upper Left Drawing Area (SCAX0): " + str(data & 0x7ff)
  print "Lower Right Drawing Area (SCAX1): " + str((data >> 16) & 0x7ff)
  print " Upper Left Drawing Area (SCAY0): " + str((data >> 32) & 0x7ff)
  print "Lower Right Drawing Area (SCAY1): " + str((data >> 48) & 0x7ff)

  print
  print "(" + x0 + "," + y0 + ")-(" + x1 + "," + y1 + ")"

def show_test(data, address):
  print address + ": Pixel Test Control"
  print "                     Alpha Test (ATE): " + str(data & 1)
  print "             Alpha Test Method (ATST): " + str((data >> 1) & 0x7)
  print "          Alpha Value Compared (AREF): " + str((data >> 4) & 0xff)
  print "Processing Method When Failed (AFAIL): " + str((data >> 12) & 0x3)
  print "        Destination Alpha Test (DATE): " + str((data >> 14) & 0x1)
  print "   Destination Alpha Test Mode (DATM): " + str((data >> 15) & 0x1)
  print "                     Depth Test (ZTE): " + str((data >> 16) & 0x1)
  print "             Depth Test Method (ZTST): " + str((data >> 17) & 0x3)

def show_tex1(data, address):
  print address + ": Texture Information Setting"
  print "       LOD Calculation Method (LCM): " + str(data & 1)
  print "            Maximum MIP Level (MXL): " + str((data >> 2) & 0x7)
  print "Filter When Texture Expanded (MMAG): " + str((data >> 5) & 0x1)
  print " Filter When Texture Reduced (MMIN): " + str((data >> 6) & 0x7)
  print "      Base Address of MIPMAP (MTBA): " + str((data >> 9) & 0x1)
  print "                  LOD Parameter (L): " + str((data >> 19) & 0x3)
  print "                  LOD Parameter (K): " + str((data >> 32) & 0x7ff)

def show_data(data, address):
  address = reg_desc[int(address, 16)]

  data = int(data, 16)

  if address == "COLCLAMP":
    show_colclamp(data)
  elif address == "DTHE":
    show_dthe(data)
  elif address == "FRAME_1":
    show_frame(data, address)
  elif address == "FRAME_2":
    show_frame(data, address)
  elif address == "PRIM":
    show_prim(data)
  elif address == "PRMODECONT":
    show_prmodecont(data)
  elif address == "RGBAQ":
    show_rgbaq(data)
  elif address == "XYOFFSET_1":
    show_xyoffset(data, address)
  elif address == "XYOFFSET_2":
    show_xyoffset(data, address)
  elif address == "XYZ2":
    show_xyz2(data)
  elif address == "XYZ3":
    show_xyz3(data)
  elif address == "SCISSOR_1":
    show_scissor(data, address)
  elif address == "SCISSOR_2":
    show_scissor(data, address)
  elif address == "TEST_1":
    show_test(data, address)
  elif address == "TEST_2":
    show_test(data, address)
  elif address == "TEX1_1":
    show_tex1(data, address)
  elif address == "TEX1_2":
    show_tex1(data, address)
  elif address == "ZBUF_1":
    show_zbuf(data, address)
  elif address == "ZBUF_2":
    show_zbuf(data, address)
  else:
    print address + ":"

# ----------------------------- fold here -----------------------------

if len(sys.argv) != 3:
  print "Usage: " + sys.argv[0] + " <gifdump_file> <giftag_address>"
  sys.exit(0)

fp = open(sys.argv[1], "rb")
start = sys.argv[2] + ":"

count = 0
total = -1

for line in fp:
  tokens = line.strip().split()
  if len(tokens) == 0: continue

  if count == total: break

  if total == -1:
    if not tokens[0].startswith(start): continue

  data = bytes_to_64bit(tokens[1:9])
  address = bytes_to_64bit(tokens[9:17])

  print "----------------------------"
  print tokens[0] + " " + data + " " + address

  if count == 0:
    total = show_giftag(data, address)
    total += 1
  else:
    show_data(data, address)

  count += 1

fp.close()



