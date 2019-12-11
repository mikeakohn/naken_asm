;; SPI test for MSP430G2553.
;;
;; Copyright 2011-2019 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike to mikekohn.net
;;
;; Sample of writing data out to SPI for a 16 bit MCP4921 DAC.

.include "msp430x2xx.inc"

SPI_CS equ 0x10
SPI_CLK equ 0x20
SPI_SOMI equ 0x40
SPI_SIMO equ 0x80

.org 0xc000
start:
  ;; Turn off watchdog
  mov.w #WDTPW|WDTHOLD, &WDTCTL

  ;; Turn interrupts off 
  dint

  ;; Set up stack pointer
  mov.w #0x0400, SP

  ;; Set MCLK to 1.4 MHz with DCO
  mov.b #DCO_3, &DCOCTL
  mov.b #RSEL_8, &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Set up output pins
  ;; P1.4 = SPI /SEN (chip select)
  ;; P1.5 = SPI CLK
  ;; P1.6 = SPI SOMI
  ;; P1.7 = SPI SIMO
  mov.b #SPI_CS, &P1DIR
  mov.b #SPI_CS, &P1OUT
  mov.b #SPI_SIMO|SPI_SOMI|SPI_CLK, &P1SEL
  mov.b #SPI_SIMO|SPI_SOMI|SPI_CLK, &P1SEL2

  ;; Setup SPI
  ;; MSB first, Master, 3 pin SPI, Sync mode
  mov.b #UCSWRST, &UCB0CTL1      ; Set reset
  bis.b #UCSSEL_2, &UCB0CTL1     ; SMCLK
  mov.b #UCCKPH|UCMSB|UCMST|UCSYNC, &UCB0CTL0
  mov.b #1, &UCB0BR0
  mov.b #0, &UCB0BR1
  bic.b #UCSWRST, &UCB0CTL1      ; Clear reset

  call #send_spi

main:
  jmp main

send_spi:
  ;; Testing SPI
  bic.b #SPI_CS, &P1OUT          ; /CS = 0 (enable)

  mov.b #0x3f, &UCB0TXBUF

wait_spi_msb:
  bit.b #UCB0RXIFG, &IFG2
  jz wait_spi_msb

  mov.b &UCB0RXBUF, r15
  mov.b #0xff, &UCB0TXBUF

wait_spi_lsb:
  bit.b #UCB0RXIFG, &IFG2
  jz wait_spi_lsb

  mov.b &UCB0RXBUF, r15

  bis.b #SPI_CS, &P1OUT          ; /CS = 1 (disable)
  ret

.org 0xfffe
  dw start                 ; Reset

