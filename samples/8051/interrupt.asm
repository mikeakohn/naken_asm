;; interrupt sample
;;
;; Demo how to use timer and interrupt with naken_asm.
;; It works on stc89cxx dev board with clock 11.0592Mhz, and should also work on AT89C51.

.8051

.include "stc89xx.inc"

LED equ P0.7

org 0
  ljmp start

; 0x03 is entry of External Interrupt 0
; 0x0b is entry of Timer 0 Interrupt
; 0x13 is entry of External Interrupt 1
; 0x1b is entry of Timer 1 interrupt
; 0x23 is entry of UART0 Interrupt
org 0x0b
  ljmp interrupt_1

; start from 0x30, left enough space for interrupts.
org 0x30
start:
  mov R7, 0x00

  clr LED

  ; setup timer
  clr TR0
  mov TMOD, 0x00
  ; 20ms
  mov TH0, 0xb1
  mov TL0, 0xe0
  ; enable timer0
  setb TR0

  ; enable timer0 interrupt
  setb ET0
  ; enable all interrupts
  setb EA

  loop:
    sjmp loop

; counting to 50 means 1 second, then blink LED 
interrupt_1:
  cjne R7, #50, count
  mov R7, 0x00
  cpl P0.7
  count:
    inc R7
  reti

end
