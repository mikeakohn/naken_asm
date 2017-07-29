;; uart.asm
;;
;; Copyright 2011-2017 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Serial port (UART) example

.include "msp430x2xx.inc"

RAM equ 0x0200

;  r4 =
;  r5 =
;  r6 =
;  r7 =
;  r8 =
;  r9 =
; r10 =
; r11 =
; r12 =
; r13 =
; r14 =
; r15 =

.org 0xc000
start:
  ;; Turn off watchdog
  mov.w #(WDTPW|WDTHOLD), &WDTCTL

  ;; Interrupts off
  dint

  ;; Setup stack pointer
  mov.w #0x0400, SP

  ;; Set MCLK to 16 MHz with DCO 
  mov.b #(DCO_4), &DCOCTL
  mov.b #RSEL_15, &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Setup output pins
  ;; P1.2 = Debug LED
  mov.b #0x01, &P1DIR        ; P1.0
  mov.b #1, &P1OUT
  mov.b #6, &P1SEL
  mov.b #6, &P1SEL2

  ;; Setup UART
  mov.b #UCSSEL_2|UCSWRST, &UCA0CTL1
  mov.b #0, &UCA0CTL0
  ;mov.b #0x82, &UCA0BR0
  ;mov.b #0x06, &UCA0BR1
  mov.b #0xf8, &UCA0BR0  ; 9600 baud
  mov.b #0x05, &UCA0BR1
  bic.b #UCSWRST, &UCA0CTL1

  ;; Interrupts on
  eint

  ;; send a bunch of A's

  mov #0, r7
  mov.b #'A', &UCA0TXBUF

main:
  bit.b #UCA0TXIFG, &IFG2
  jz main
  mov.b #'A', &UCA0TXBUF
  inc r7
  jmp main

.org 0xfffe
vectors:
  dw start                 ; Reset



