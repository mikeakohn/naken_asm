#!/usr/bin/env python3

def convert_frac(whole, frac):
  if (whole & 0x8000) != 0:
    whole ^= 0xffff
    whole += 1
    whole *= -1

  return "%d.%04x" % (whole, frac) 

# ------------------------------ fold here -----------------------------

#data = [
#  ".dc64 0x080002fb02aa01e0",
#  ".dc64 0x0052462afffd097b",
#  ".dc64 0x009615c1ffff6ef5",
#  ".dc64 0x0095f0bf000065b0",
#]

data = [ ]

fp = open("sample.asm", "r")
for line in fp:
  line = line.strip()
  if line.startswith(".dc64 0x"):
    data.append(line)
fp.close()

print(data)

t = [ ]

for d in data:
  d = d.split()[1].replace("0x", "")

  for b in range(0, 16, 4):
    h = d[b : b + 4]
    i = int(h, 16)
    t.append(i)

print(t)

c = t[0] >> 8
command = "Command:"

if (c & 8) == 0: command += " error"
if (c & 4) == 4: command += " shaded"
if (c & 2) == 2: command += " texture"
if (c & 1) == 1: command += " zbuffer"
if c == 8: command += " non-shaded"

YL = "%.2f" % ((t[1] >> 2) + (float(t[1] & 0x3) / 4))
YM = "%.2f" % ((t[2] >> 2) + (float(t[2] & 0x3) / 4))
YH = "%.2f" % ((t[3] >> 2) + (float(t[3] & 0x3) / 4))

XL = convert_frac(t[4], t[5])
DxLDy = convert_frac(t[6], t[7])
XH = convert_frac(t[8], t[9])
DxHDy = convert_frac(t[10], t[11])
XM = convert_frac(t[12], t[13])
DxMDy = convert_frac(t[14], t[15])

print(command + " triangle")

print("left_major_flag: " + str((t[0] >> 7) & 1))
print(" mips-map level: " + str(((t[0] >> 3) & 0x7) + 1))
print("           tile: " + str(t[0] & 7))
print("           YL: " + YL)
print("           YM: " + YM)
print("           YH: " + YH)
print("           XL: " + XL)
print("        DxLDy: " + DxLDy)
print("           XH: " + XH)
print("        DxHDy: " + DxHDy)
print("           XM: " + XM)
print("        DxMDy: " + DxMDy)

