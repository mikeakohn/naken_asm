.pdp11

;; Testing that instructions can be assembled.

; rts
; 0 000 000 010 000 xxx
; 0000 0000 1000 0xxx

; swab
; 0 000 000 011 xxx xxx
; 0000 0000 11xx xxxx

main:
  mov r1, r2
  mov #5, r2
  mov @#5, r2
  ble label_1

  jsr r4, label_1

  mark 20

  mov (r1), r2
  mul #5, r2
  xor r1, 100
  rts r5
  bgt main

label_1:
  nop
  sob r1, label_1

  trap 80

  sez
  cln

  cmp r2, #4
  cmpb r2, #4

  c 4
  s 6

