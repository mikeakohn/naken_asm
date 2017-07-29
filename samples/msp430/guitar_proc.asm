;; MSP430G2231 Guitar Processor
;;
;; Copyright 2011 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Short sample of using naken_asm and MSP430 to make a guitar
;; processor.

.msp430

.include "msp430x2xx.inc"

.org 0xf800
start:
  ;; Turn off watchdog
  mov.w #0x5a80, &WDTCTL

  ;; Interrupts off
  dint

  ;; Set up stack pointer
  mov.w #0x0280, SP

  ;; Set up crystal
  mov.b #(DCO_3), &DCOCTL
  mov.b #(RSEL_15), &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Set up output pins
  mov.b #0x03, &P1DIR        ; P1.0 P1.1
  ;mov.b #0x04, &P1SEL        ; for A/D on pin A2
  mov.b #0x03, &P1OUT        ; P1.0=/CS

  ;; Set up SPI
  mov.b #(USIPE6|USIPE5|USIMST|USIOE|USISWRST), &USICTL0
  mov.b #USICKPH, &USICTL1
  mov.b #(USIDIV_0|USISSEL_2), &USICKCTL ; div 1, SMCLK
  bic.b #USISWRST, &USICTL0      ; clear reset

  ;; Set up A/D
  mov.w #(ADC10ON), &ADC10CTL0
  mov.w #(INCH_2|ADC10SSEL_2), &ADC10CTL1

  ;; Set up Timer
  mov.w #533, &TACCR0
  mov.w #(TASSEL_2|MC_1), &TACTL ; SMCLK, DIV1, COUNT to TACCR0
  mov.w #CCIE, &TACCTL0
  mov.w #0, &TACCTL1

  ;; Interrupts on
  eint

main:
  jmp main

timer_interrupt:

  ;; Send word over SPI
  bic.b #0x01, &P1OUT            ; /CS = 0 (enable)

  mov.w &ADC10MEM, r9

  rla.w r9
  rla.w r9                       ; 10 bit DAC data becomes 12 bit

  cmp.w #(2048+20), r9
  jge raise_output
  cmp.w #(2048-20), r9
  jge setup_spi

lower_output:
  mov.w #0x0000, r9
  jmp setup_spi

raise_output:
  mov.w #0x0fff, r9

setup_spi:
  bis.w #0x3000, r9              ; add SPI command nibble

  ;; send a byte out of SPI
  mov.w r9, &USISR
  mov.b #(USI16B|16), &USICNT    ; Flush out 16 bits

wait_spi:
  bit.b #USIIFG, &USICTL1
  jz wait_spi

  bis.b #0x01, &P1OUT            ; /CS = 1 (disable)

  bis.w #(ENC|ADC10SC), &ADC10CTL0

  reti

spi_interrupt:
  ;; shouldn't happen
  reti

.org 0xffe8
vectors:
  dw spi_interrupt
  dw 0
  dw 0
  dw 0
  dw 0
  dw timer_interrupt       ; Timer_A2 TACCR0, CCIFG
  dw 0
  dw 0
  dw 0
  dw 0
  dw 0
  dw start                 ; Reset


