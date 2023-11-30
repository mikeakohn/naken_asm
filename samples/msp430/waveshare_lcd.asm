;; Waveshare LCD.
;;
;; Copyright 2023 - By Michael Kohn
;; https://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Example using a Waveshare 96x64 OLED sold by Parallax interfaced
;; with an SPI SSD1331 chip.

.msp430
.include "msp430x2xx.inc"

;; Port 1.
SPI_CLK  equ 0x20
SPI_SOMI equ 0x40
SPI_SIMO equ 0x80

;; Port 2.
LCD_RES  equ 0x01
LCD_DC   equ 0x02
LCD_CS   equ 0x04

COMMAND_DISPLAY_OFF     equ 0xae
COMMAND_SET_REMAP       equ 0xa0
COMMAND_START_LINE      equ 0xa1
COMMAND_DISPLAY_OFFSET  equ 0xa2
COMMAND_NORMAL_DISPLAY  equ 0xa4
COMMAND_SET_MULTIPLEX   equ 0xa8
COMMAND_SET_MASTER      equ 0xad
COMMAND_POWER_MODE      equ 0xb0
COMMAND_PRECHARGE       equ 0xb1
COMMAND_CLOCKDIV        equ 0xb3
COMMAND_PRECHARGE_A     equ 0x8a
COMMAND_PRECHARGE_B     equ 0x8b
COMMAND_PRECHARGE_C     equ 0x8c
COMMAND_PRECHARGE_LEVEL equ 0xbb
COMMAND_VCOMH           equ 0xbe
COMMAND_MASTER_CURRENT  equ 0x87
COMMAND_CONTRASTA       equ 0x81
COMMAND_CONTRASTB       equ 0x82
COMMAND_CONTRASTC       equ 0x83
COMMAND_DISPLAY_ON      equ 0xaf

.macro send_command(a)
  mov.b #a, r15
  call #lcd_send_cmd
.endm

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
; r13 = pointer to interrupt
; r14 =
; r15 =

.org 0xc000
start:
  ;; Turn off watchdog
  mov.w #(WDTPW|WDTHOLD), &WDTCTL

  ;; Disable interrupts
  dint

  ;; Set MCLK to 4 MHz with DCO 
  ;mov.b #DCO_4, &DCOCTL
  ;mov.b #RSEL_11, &BCSCTL1
  mov.b #DCO_5, &DCOCTL
  mov.b #RSEL_13, &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Set up output pins
  ;; P1.5 = UCB0CLK
  ;; P1.6 = UCB0SOMI
  ;; P1.7 = UCB0SIMO
  ;; P2.0 = /RESET for LCD
  ;; P2.1 = D/C for LCD
  ;; P2.2 = /CS for LCD
  mov.b #0x00, &P1DIR
  mov.b #0x00, &P1OUT
  mov.b #SPI_CLK|SPI_SOMI|SPI_SIMO, &P1SEL
  mov.b #SPI_CLK|SPI_SOMI|SPI_SIMO, &P1SEL2
  mov.b #LCD_CS|LCD_DC|LCD_RES, &P2DIR
  mov.b #LCD_CS, &P2OUT

  ;; Set up SPI
  mov.b #UCSWRST, &UCB0CTL1
  bis.b #UCSSEL_2, &UCB0CTL1
  mov.b #UCCKPH|UCMSB|UCMST|UCSYNC, &UCB0CTL0
  mov.b #1, &UCB0BR0
  mov.b #0, &UCB0BR1
  bic.b #UCSWRST, &UCB0CTL1

main:
  call #lcd_init
  call #lcd_clear
while_1:
  jmp while_1

lcd_init:
  mov.b #LCD_CS, &P2OUT
  call #delay
  bis.b #LCD_RES, &P2OUT

  send_command(COMMAND_DISPLAY_OFF)
  send_command(COMMAND_SET_REMAP)
  send_command(0x72)
  send_command(COMMAND_START_LINE)
  send_command(0x00)
  send_command(COMMAND_DISPLAY_OFFSET)
  send_command(0x00)
  send_command(COMMAND_NORMAL_DISPLAY)
  send_command(COMMAND_SET_MULTIPLEX)
  send_command(0x3f)
  send_command(COMMAND_SET_MASTER)
  send_command(0x8e)
  send_command(COMMAND_POWER_MODE)
  send_command(COMMAND_PRECHARGE)
  send_command(0x31)
  send_command(COMMAND_CLOCKDIV)
  send_command(0xf0)
  send_command(COMMAND_PRECHARGE_A)
  send_command(0x64)
  send_command(COMMAND_PRECHARGE_B)
  send_command(0x78)
  send_command(COMMAND_PRECHARGE_C)
  send_command(0x64)
  send_command(COMMAND_PRECHARGE_LEVEL)
  send_command(0x3a)
  send_command(COMMAND_VCOMH)
  send_command(0x3e)
  send_command(COMMAND_MASTER_CURRENT)
  send_command(0x06)
  send_command(COMMAND_CONTRASTA)
  send_command(0x91)
  send_command(COMMAND_CONTRASTB)
  send_command(0x50)
  send_command(COMMAND_CONTRASTC)
  send_command(0x7d)
  send_command(COMMAND_DISPLAY_ON)
  ret

lcd_clear:
  mov #96 * 32, r10
lcd_clear_loop_0:
  mov.b #0xff, r15
  call #lcd_send_data
  mov.b #0x00, r15
  call #lcd_send_data
  dec.w r10
  jnz lcd_clear_loop_0

  mov #96 * 32, r10
lcd_clear_loop_1:
  mov.b #0x00, r15
  call #lcd_send_data
  mov.b #0xff, r15
  call #lcd_send_data
  dec.w r10
  jnz lcd_clear_loop_1
  ret

;; lcd_send_cmd(r15)
lcd_send_cmd:
  bic.b #LCD_DC, &P2OUT 
  bic.b #LCD_CS, &P2OUT
  call #spi_send_char
  bis.b #LCD_CS, &P2OUT
  ret

;; lcd_send_data(r15)
lcd_send_data:
  bis.b #LCD_DC, &P2OUT 
  bic.b #LCD_CS, &P2OUT
  call #spi_send_char
  bis.b #LCD_CS, &P2OUT
  ret

; spi_send_char(r15)
spi_send_char:
  mov.b r15, &UCB0TXBUF
spi_send_char_wait:
  bit.b #UCB0RXIFG, &IFG2
  jz spi_send_char_wait
  mov.b &UCB0RXBUF, r15
  ret 

delay:
  mov.w #0, r15
delay_loop:
  dec.w r15
  jnz delay_loop
  ret

.org 0xfffe
  dw start

