.68hc08

main:
  psha
  rts
blah:
  adc #8
  cphx #8
roar:
  cphx #$ff00
  bne roar

  cmp ,x

  mov roar, blah
  mov #8, blah
  cbeqa #8, blah
  cbeqx #8, main
  cbeq 8, X+, blah
  cbeq ,X+, blah
  dbnz ,X, blah
  dbnz 8, X, blah
  cbeq 9, blah
  cbeq main, SP, main

  sub #8
  sub roar
  sub $ff00
  sub roar, X
  sub $ff00, X
  sub X
  sub roar, SP
  sub $ff00, SP

  mov roar, X+
  mov ,X+, roar

addr:
  bset 3, addr
  bset 7, addr
  brset 7, addr, addr
  brset 0, addr, addr


