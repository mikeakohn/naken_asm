.dspic
.include "p30f3012.inc"

.org 0
  goto main

.org 0x14+(3*2)
  dc32 timer1_interrupt

.org 0x100
main:
  clr w0
  mov wreg, TRISB
  mov #2, w0
  mov wreg, PORTB
  mov #1, w0
  mov wreg, TRISD

  call delay

wait_for_pin:
  btss.b PORTD, #0
  bra wait_for_pin

  mov #1, w0
  mov wreg, PORTB

  clr T1CON
  mov #10000, w0
  mov wreg, PR1
  bset IEC0, #T1IE
  bset T1CON, #TON

.if 0
  mov #OSCCONL, w3
  mov #0x46, w0
  mov #0x57, w1
  mov #0x82, w2
  mov.b w0, [w3]
  mov.b w1, [w3]
  mov.b w2, [w3]
.endif

  mov #2, w0
  mov #999, w2

repeat:
  bra repeat

timer1_interrupt:
  dec w2,w2
  bra nz, leave_interrupt
  xor.b #3, w0
  mov wreg, PORTB
  mov #999, w2
leave_interrupt:
  bclr IFS0, #T1IF
  retfie

delay:
  mov w4, 1000
repeat_loop:
  dec w4, w4
  bra nz, repeat_loop
  return

  ; FRC=1, FPR=?
.org __FOSC
  ;dc32 0x0000f9e0
  dc32 0x0000fff1     ; 20 MHz resonator
.org __FWDT
  ;dc32 0x00007fff
  dc32 0x00007fff
.org __FBORPOR
  dc32 0x0000ff7f
.org __FGS
  dc32 0x0000ffff


