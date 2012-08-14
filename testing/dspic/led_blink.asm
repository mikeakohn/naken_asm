
.dspic

.org 0

main:
  clr w0
  mov w0, 0x163
  mov #2, w0
  mov w0, 0x165
repeat:
  bra repeat

  goto main

