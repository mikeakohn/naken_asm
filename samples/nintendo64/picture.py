#!/usr/bin/evn python3

# rrrr_rggg_ggbb_bbba
colors = {
  ' ': 0x0000,
  'R': 0xf800,
  'G': 0x07c0,
  'B': 0x003e,
  'Y': 0xffc0,
  'P': 0xf83e,
}

fp = open("picture.txt", "r")
out = open("picture.raw", "wb")

for line in fp:
  line = line.strip()
  for i in range(0, len(line), 2):
    data = (colors[line[i]] << 16) | colors[line[i + 1]]
    out.write(data.to_bytes(4, 'big'))

fp.close()

