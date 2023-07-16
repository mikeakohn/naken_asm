;; P-Star Blink
;;
;; Copyright 2023 - By Michael Kohn
;; https://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Sample of blinking LEDs with a Pololu P-Star 45K50 Mini SV.
;; The board uses a PIC18F45K50 and the LEDs are are connected
;; to pins:
;;
;; RB6 - yellow  (sourced)
;; RB7 - green   (sourced)
;; RC6 - red     (sink)

.pic18
.include "p18f45k50.inc"

led_state equ 0xf52

.org 0x00_2000
start:
  goto main

.org 0x00_2008
interrupt:
  clrf TMR0IF

  movlb 0x0
  ;movlw 0xc0
  movf led_state, w
  xorlw 0xc0
  movwf led_state, 1
  movlb 0xf
  movwf LATB

  bcf INTCON, TMR0IF
  retfie

main:
  movlb 0xf

  ;; Set pins to 0.
  clrf LATB
  clrf LATC

  movlw 0xff
  movwf ANSELB
  movwf ANSELC

  ;; Set RB7 and RB6 as outputs.
  movlw 0b0011_1111
  movwf TRISB
  ;movlw 0b1100_0000
  movlw 0b0000_0000
  movwf LATB

  movlw 0b0011_1111
  movwf TRISC
  movlw 0b0000_0000
  movwf LATC

  movlb 0x0
  movlw 0b1000_0000
  movwf led_state
  movlb 0xf

  ;; Setup Timer0
  movlw 0x87
  movwf T0CON
  bsf INTCON, TMR0IE
  bsf INTCON, GIE

while_1:
  bra while_1

