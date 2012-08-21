
.dspic

.org 0
  goto main
  ;goto 256

;.org 0x200
.org 0x100
main:
  clr w0
  mov w0, 0x163
  mov #2, w0
  mov w0, 0x165
repeat:
  bra repeat


