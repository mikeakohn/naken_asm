;; DS1305 real time clock test
;;
;; Copyright 2011-2017 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike to mikekohn.net
;;

.include "msp430x2xx.inc"

RAM equ 0x0200

.org 0xf800
start:
  ;; Turn off watchdog
  mov.w #(WDTPW|WDTHOLD), &WDTCTL

  ;; Turn interrupts off
  dint

  ;; Set up stack pointer
  mov.w #0x0280, SP

  ;; Set MCLK to 16 MHz with DCO 
  mov.b #(DCO_4), &DCOCTL
  mov.b #RSEL_15, &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Set up output pins
  ;; P1.0 = Data Out for DS18B20
  ;; P1.1 =  CE for RTC
  ;; P1.2 = /CS for DAC
  ;; P1.3 = /CS for SD
  mov.b #0x0f, &P1DIR        ; P1.0, P1.1, P1.2, P1.3
  mov.b #(8|4|1), &P1OUT

  ;; Set up SPI
  mov.b #(USIPE7|USIPE6|USIPE5|USIMST|USIOE|USISWRST), &USICTL0
  mov.b #USICKPH, &USICTL1
  mov.b #(USIDIV_7|USISSEL_2), &USICKCTL ; div 128, SMCLK
  bic.b #USISWRST, &USICTL0      ; clear reset

  ;; Interrupts back on
  eint

  ;; Turn write protect off, turn oscillator on
  mov.b #0x8f, r9
  mov.b #0x00, r10
  call #write_rtc
  call #write_rtc   ;; Someone in a forum said this should be done twice
                    ;; because of the state of WP.  Better safe than sorry.

main:
  call #read_time
  jmp main

read_time:
  mov.w #0x200, r4           ; r4 points to the start of RAM

  bis.b #2, &P1OUT            ; RTC CE = 1 (enabled)
  mov.b #0x00, r15           ; read starting at location 0
  call #spi_send_char
again:
  call #spi_get_char
  mov.b r15, 0(r4)           ; get char back from ds1305 and put in RAM

  inc.w r4                   ; r4++
  cmp.w #0x210, r4           ; if (r4 != 0x210) repeat;
  jne again

  bic.b #2, &P1OUT            ; RTC CE = 0 (disabled)
  ret

;; read_rtc(r9)   r9 = address  , r15 = return value
read_rtc:
  bis.b #2, &P1OUT            ; RTC CE = 1 (enabled)
  mov.b r9, r15
  call #spi_send_char
  call #spi_get_char
  bic.b #2, &P1OUT            ; RTC CE = 0 (disabled)
  ret

;; write_rtc(r9, r10)  r9 = address, r10 = value
write_rtc:
  bis.b #2, &P1OUT            ; RTC CE = 1 (enabled)
  mov.b r9, r15
  call #spi_send_char
  mov.b r10, r15
  call #spi_send_char
  bic.b #2, &P1OUT            ; RTC CE = 0 (disabled)
  ret

; spi_send_char(r15)
spi_send_char:
  mov.b r15, &USISRL
  mov.b #8, &USICNT
wait_spi_read_write:
  bit.b #USIIFG, &USICTL1
  jz wait_spi_read_write
  mov.b &USISRL, r15
  ret

; spi_get_char()
spi_get_char:
  mov.b #0xff, &USISRL
  mov.b #8, &USICNT
wait_spi_get_char:
  bit.b #USIIFG, &USICTL1
  jz wait_spi_get_char
  mov.b &USISRL, r15
  ret

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
  dw 0
  dw 0
  dw 0
  dw 0
  dw 0
  dw 0
  dw start                 ; Reset


