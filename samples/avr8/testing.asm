.avr8
;.device ATmega168

main:
  clc
  brne main
  clr r31
  tst r31
  add r31, r30
  andi r20, 5
  cbr r20, 1
  adc r10, r2

zomg:
  push r6
  pop r7
  inc r31
  sbrc r16, 3

  rcall zomg
  rjmp zomg
  call zomg
  jmp zomg


  adiw r24, 63
  cbi 0x5, 7
  bset 3
  ser r20
  in r7, 0x3f
  out 0x3f, r9

  movw r16, r30
  ;spm Z+
  spm

  ld r13, Z
  ld r12, Y+
  ld r19, -Y 
  ld r19, Y 
  ld r0, Z+

  cpi r16, 'z'

  st Z, r9
  st Y+, r10
  st -Y, r31
  st Y, r10
  st Z+, r3

  ret


  fmul r16, r18
  mul r16, r30

  ;des 5
  lds r14, 0x1000
  sts 0x2000, r1

  std Z+63, r4
  std Y+21, r31

  ldd r5, Y+7
  ldd r8, Z+56

  ldi r30, blah&0xff
  ldi r31, blah>>8

blah:
  dw 0x1234, 0x4321

