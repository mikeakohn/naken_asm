;; Ethernet test
;;
;; Copyright 2019 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Example using an ENC28J60 chip to send five IP/UDP packets over Ethernet
;; with an MSP430G2553.

.msp430
.include "msp430x2xx.inc"
.include "ethernet/enc28j60.inc"

.define PACKET_RAM 0x200

;; Port 2
.define ETH_RST 0x10
.define SPI_CS 0x08

;; Port 1
.define SPI_CLK 0x20
.define SPI_SOMI 0x40
.define SPI_SIMO 0x80

.define CS_SELECT bic.b #SPI_CS, &P2OUT
.define CS_DESELECT bis.b #SPI_CS, &P2OUT

; 000_aaaaa
.macro READ_EREG(a)
  mov.b #a, r15
  call #read_control_reg
.endm

; 010_aaaaa_dddddddd
.macro WRITE_REG(a,d)
  mov.b #0x40|a, r15
  mov.b #d, r14
  call #write_control_reg
.endm

; 001_11010
.macro READ_BUFFER
  mov.b #0x3a, r15
  call #read_control_reg
.endm

; 011_11010_dddddddd
.macro WRITE_BUFFER(d)
  mov.b #0x7a, r15
  mov.b #d, r14
  call #write_control_reg
.endm

; 1 0 0 a a a a a  d d d d d d d d
.macro BIT_SET(a,d)
  mov.b #0x80|a, r15
  mov.b #d, r14
  call #write_control_reg
.endm

; 1 0 1 a a a a a  d d d d d d d d
.macro BIT_CLEAR(a,d)
  mov.b #0xa0|a, r15
  mov.b #d, r14
  call #write_control_reg
.endm

.macro READ_MREG(a)
  mov.b #a, r15
  call #read_control_reg_m
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

  ;; r13 points to which interrupt routine should be called
  mov.w #null_interrupt, r13

  ;; Set up stack pointer
  mov.w #0x0400, SP

  ;; Set MCLK to 8 MHz with DCO
  mov.b #DCO_5, &DCOCTL
  mov.b #RSEL_13, &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Set up output pins
  ;; P1.5 = UCB0CLK
  ;; P1.6 = UCB0SOMI
  ;; P1.7 = UCB0SIMO
  ;; P2.4 = /CS for ETH
  ;; P2.5 = /RESET for ETH
  mov.b #0x00, &P1DIR
  mov.b #0x00, &P1OUT
  mov.b #SPI_CLK|SPI_SOMI|SPI_SIMO, &P1SEL
  mov.b #SPI_CLK|SPI_SOMI|SPI_SIMO, &P1SEL2
  mov.b #SPI_CS|ETH_RST, &P2DIR
  mov.b #SPI_CS, &P2OUT

  ;; Set up SPI
  mov.b #UCSWRST, &UCB0CTL1
  bis.b #UCSSEL_2, &UCB0CTL1
  mov.b #UCCKPH|UCMSB|UCMST|UCSYNC, &UCB0CTL0
  mov.b #1, &UCB0BR0
  mov.b #0, &UCB0BR1
  bic.b #UCSWRST, &UCB0CTL1

  ;; Set up Timer
  ;mov.w #SOUND_RATE, &TACCR0
  ;mov.w #(TASSEL_2|MC_1), &TACTL ; SMCLK, DIV1, COUNT to TACCR0
  ;mov.w #CCIE, &TACCTL0
  ;mov.w #0, &TACCTL1

  ;; Enable interrupts
  eint

  call #reset_ethernet
  call #wait_for_ost
  call #init_buffers
  call #init_mac
  call #init_phy

  call #copy_packet_to_ram

main:
  ;; Send 5 UDP packets.
  mov.b #5, r6
main_loop:
  call #delay
  call #calc_checksum_ip
  call #load_buffers
  call #send_packet
  dec.b r6
  jnz main_loop
  ;; while(1) { }
while_1:
  jmp while_1

reset_ethernet:
  ;; Probably not needed, but wait before clearing /RESET.
  call #delay

  ;; Clear reset on ethernet
  bis.b #ETH_RST, &P2OUT

  call #delay
  call #soft_reset
  call #delay
  ret

wait_for_ost:
  READ_EREG(ESTAT)
  bit.b #CLKRDY, r15
  jz wait_for_ost
  ret

init_buffers:
  ;; Set up receive buffers
  BIT_CLEAR(ECON1, BANK3)

  ;; http://ww1.microchip.com/downloads/en/DeviceDoc/80349c.pdf
  ;; RX buffer must be 0
  WRITE_REG(ERXSTL, 0x0000 & 0xff)
  WRITE_REG(ERXSTH, 0x0000 >> 8)
  WRITE_REG(ERXNDL, 0x0fff & 0xff)
  WRITE_REG(ERXNDH, 0x0fff >> 8)
  ;; REVIEW: These two lines aren't needed since the docs say that updating
  ;; the ERXST pointer will update this too. (Section 6.1)
  WRITE_REG(ERDPTL, 0x0000 & 0xff)
  WRITE_REG(ERDPTH, 0x0000 >> 8)

  ;; Set up transmit buffers
  WRITE_REG(ETXSTL, 0x1000 & 0xff)
  WRITE_REG(ETXSTH, 0x1000 >> 8)
  WRITE_REG(ETXNDL, 0x1fff & 0xff)
  WRITE_REG(ETXNDH, 0x1fff >> 8)
  WRITE_REG(EWRPTL, 0x1000 & 0xff)
  WRITE_REG(EWRPTH, 0x1000 >> 8)

  BIT_SET(ECON2, AUTOINC)
  ret

load_buffers:
  BIT_CLEAR(ECON1, BANK3)

  WRITE_REG(ETXSTL, 0x1000 & 0xff)
  WRITE_REG(ETXSTH, 0x1000 >> 8)

  mov.w #PACKET_RAM+1, r4

  CS_SELECT

  mov.b #0x60|0x1a, r15
  call #spi_send_char

load_buffers_next:
  mov.b @r4+, r15
  call #spi_send_char

  cmp.w #PACKET_RAM+(packet_end-packet_pad), r4
  jnz load_buffers_next

  CS_DESELECT

  ;WRITE_REG(ETXSTL, 0x1000 & 0xff)
  ;WRITE_REG(ETXSTH, 0x1000 >> 8)
  WRITE_REG(ETXNDL, ((0x1000 + (packet_end - packet) - 1) & 0xff))
  WRITE_REG(ETXNDH, ((0x1000 + (packet_end - packet) - 1) >> 8))
  ret

send_packet:
  BIT_CLEAR(EIR, TXIF)
  BIT_CLEAR(EIE, TXIE|INTIE)
  BIT_SET(ECON1, TXRTS)
send_packet_wait:
  READ_EREG(ECON1)
  mov.b r15, r7
  bit.b #TXRTS, r7
  jnz send_packet_wait

  BIT_CLEAR(ECON1, BANK3)
  READ_EREG(EWRPTL)
  ret

init_mac:
  ;; Init mac
  BIT_CLEAR(ECON1, BANK3)
  BIT_SET(ECON1, BANK2)

  WRITE_REG(MACON1, MARXEN|TXPAUS|RXPAUS)
  WRITE_REG(MACON2, 0)
  WRITE_REG(MACON3, PADCFG0|TXCRCEN|FRMLNEN|FULDPX)
  WRITE_REG(MACON4, 0)

  ;; Set mac address to de:ad:ba:dd:00:d4
  BIT_SET(ECON1, BANK3)
  WRITE_REG(MAADR0, 0xde)
  WRITE_REG(MAADR1, 0xad)
  WRITE_REG(MAADR2, 0xba)
  WRITE_REG(MAADR3, 0xdd)
  WRITE_REG(MAADR4, 0x00)
  WRITE_REG(MAADR5, 0xd4)
  ret

init_phy:
  ;; I think the phy defaults to okay settings.
  ;; But this function still must wait until the PHY is ready after
  ;; a reset before trying to transmit data.
  BIT_CLEAR(ECON1, BANK3)
  BIT_SET(ECON1, BANK2)
  WRITE_REG(MIREGADR, PHCON1)
init_phy_wait_busy:
  BIT_SET(MICMD, MIIRD)
init_phy_wait_read:
  call #delay
  READ_MREG(MISTAT)
  bit.b #MBUSY, r15
  jnz init_phy_wait_read
  BIT_CLEAR(MICMD, MIIRD)
  READ_MREG(MIRDH)
  bit.b #0x80, r15
  jnz init_phy_wait_busy
  ret

delay:
  mov.w #0, r15
delay_loop:
  dec.w r15
  jnz delay_loop
  ret

; spi_send_char(r15)
spi_send_char:
  mov.b r15, &UCB0TXBUF
spi_send_char_wait:
  bit.b #UCB0RXIFG, &IFG2
  jz spi_send_char_wait
  mov.b &UCB0RXBUF, r15
  ret

; read_control_reg(r15)  000_aaaaa
read_control_reg:
  CS_SELECT
  call #spi_send_char
  mov.b #0, r15
  call #spi_send_char
  CS_DESELECT
  ret

; read_control_reg(r15)
read_control_reg_m:
  CS_SELECT
  call #spi_send_char
  mov.b #0, r15
  call #spi_send_char
  mov.b #0, r15
  call #spi_send_char
  CS_DESELECT
  ret

;; read_buffer_memory()
read_buffer_memory:
  CS_SELECT
  mov.b #0x3a, r15   ; 001_11010
  call #spi_send_char
  CS_DESELECT
  ret

;; write_buffer_memory(r14)
write_buffer_memory:
  CS_SELECT
  mov.b #0x7a, r15   ; 011_11010
  call #spi_send_char
  mov.b r14, r15
  call #spi_send_char
  CS_DESELECT
  ret

;; write_control_reg(r14)
write_control_reg:
  CS_SELECT
  call #spi_send_char
  mov.b r14, r15
  call #spi_send_char
  CS_DESELECT
  ret

soft_reset:
  CS_SELECT
  mov.b #0xff, r15   ; 111_11111
  call #spi_send_char
  CS_DESELECT
  ret

copy_packet_to_ram:
  mov.w #PACKET_RAM, r13
  mov.w #packet_pad, r14
copy_packet_to_ram_loop:
  mov.b @r14+, r15
  mov.b r15, 0(r13)
  add.w #1, r13
  cmp.w #packet_end, r14
  jnz copy_packet_to_ram_loop
  ret

calc_checksum_ip:
  mov.w #0, &PACKET_RAM+16+10
  mov.w #0, r10
  mov.w #0, r11
  mov.w #PACKET_RAM+16, r14
calc_checksum_ip_loop:
  mov.w @r14+, r15
  swpb r15
  add.w r15, r10
  addc.w #0, r11
  cmp.w #PACKET_RAM+16+20, r14
  jnz calc_checksum_ip_loop
  add.w r11, r10
  addc.w #0, r10
  xor.w #0xffff, r10
  mov.w r10, r11
  swpb r10
  mov.w r10, &PACKET_RAM+16+10
  ret

timer_interrupt:
  br r13

null_interrupt:
  reti

; IP packet is:
; Version = 4, IHL = 5, Total Length = 0x24 (9 * 4 = 36 bytes)
; Identification = 0, Flags = 0, Fragment Offset = 0
; Time To Live = 15, Protocol = 0x11 (17 UDP), Header Checksum = 0x2805
; Source IP Address = 192.168.2.15 (0xc0 0xa8, 0x02, 0x0f)
; Destination IP Address = 192.168.2.16 (0xc0 0xa8, 0x02, 0x10)
; UDP part is:
; Source Port = 0x22b8 (8888), Destination Port = 0x1e61 (7777)
; Data = "HELLO!!\n" (0x48 0x45 0x4c 04c 0x4f 0x21 0x21 0x0a)

packet_pad:
  db 0x00 ; Needed to align the ethernet frame on 16 bit boundary
packet:
  db 0x0e ; Control byte
ethernet_frame:
  db 0x30, 0x85, 0xa9, 0x00, 0x00, 0x00  ; Destination MAC address
  db 0xde, 0xad, 0xba, 0xdd, 0x00, 0xd4  ; Source MAC address
  db 0x08, 0x00 ; EtherType = IPv4
ip_packet:
  db 0x45, 0x00, 0x00, 0x24,
  db 0x00, 0x00, 0x00, 0x00,
  db 0x0f, 0x11, 0x00, 0x00,
  db 0xc0, 0xa8, 0x02, 0x0f, ; Source IP Address (192.168.2.15)
  db 0xc0, 0xa8, 0x02, 0x10, ; Destination IP Address (192.168.2.16)
udp_packet:
  db 0x22, 0xb8, 0x1e, 0x61,
  db 0x00, 0x10, 0x00, 0x00,
  db 0x48, 0x45, 0x4c, 0x4c, ; H E L L
  db 0x4f, 0x21, 0x21, '\n', ; O ! ! <CR>
packet_end:

.org 0xfff2
  dw timer_interrupt       ; Timer_A2 TACCR0, CCIFG
.org 0xfffe
  dw start                 ; Reset

