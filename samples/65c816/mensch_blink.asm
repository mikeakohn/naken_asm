;; Mensch Blink
;; Copyright 2017 - Michael Kohn
;;
;; Blink LED's on the WDC MENSCH Microcomputer board.
;;
;;   Web: http://www.mikekohn.net/micro/mensch_led_blink.php
;; Video: https://youtu.be/VX9gKZkqpps
;;        https://youtu.be/Ri98V-ArU1I
;;
;; P50 is an input/output loading and listening to the IR receiver
;; P51 is output to continuous rotation servo
;; P52 is output to regular servero
;;

.65816

.include "w65c265.inc"

.macro PAUSE
.scope
  ldy #0
repeat:
  dey
  bne repeat
.ends
.endm

.org 0xc0
start:

  ; Disable interrupts
  sei

  ; Set native mode
  clc
  xce

  ; Set A to 8-bit
  sep #0x20

  ; Set X/Y to 16-bit
  rep #0x10

  ; Set port 7 in data mode.  Port 7 seems to be always an output
  ; port.  The PCS7 register needs to be set to 0 to let this port
  ; work.
  lda.b #0x00
  sta PCS7

  ; Set port 7 value to 0x80.  Port 7 is hooked up to 8 LEDs.  Every
  ; bit in the PD7 register is conneted to one LED.  So by setting this
  ; register to 0x80 (binary 10000000), LED number 7 will be turned on.
  lda.b #0x80
  sta PD7

  ; Enable interrupts
  cli

main:
repeat_right:
  clc
  ror
  sta PD7
  PAUSE
  cmp.b #0x01
  bne repeat_right

repeat_left:
  clc
  rol
  sta PD7
  PAUSE
  cmp.b #0x80
  bne repeat_left

  jmp main

