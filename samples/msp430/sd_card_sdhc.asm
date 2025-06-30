;; SD card.
;;
;; Copyright 2025 - By Michael Kohn
;; https://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Simple example reading 16 bytes from an SD card.

.msp430
.include "msp430x2xx.inc"

;; Port 1
.define SPI_CS   0x10
.define SPI_CLK  0x20
.define SPI_SOMI 0x40
.define SPI_SIMO 0x80

.define CS_SELECT   bic.b #SPI_CS, &P1OUT
.define CS_DESELECT bis.b #SPI_CS, &P1OUT

RAM equ 0x0200
SD_BUFFER equ RAM

.macro SEND_BYTE(value)
  mov.b #value, r15
  call #spi_send_char
.endm

;  r4 =
;  r5 =
;  r6 =
;  r7 =
;  r8 =
;  r9 =
; r10 =
; r11 =
; r12 = Function paramter.
; r13 = Function paramter.
; r14 = Function paramter.
; r15 = Function paramter.

.org 0xc000
start:
  ;; Turn off watchdog
  mov.w #WDTPW|WDTHOLD, &WDTCTL

  ;; Disable interrupts
  dint

  ;; Set up stack pointer
  mov.w #0x0400, SP

  ;; Set MCLK to 8 MHz with DCO 
  mov.b #DCO_5, &DCOCTL
  mov.b #RSEL_13, &BCSCTL1
  mov.b #0, &BCSCTL2

  ;; Set up output pins
  ;; P1.4 = /CS
  ;; P1.5 = UCB0CLK
  ;; P1.6 = UCB0SOMI
  ;; P1.7 = UCB0SIMO
  mov.b #SPI_CS, &P1DIR
  mov.b #SPI_CS, &P1OUT
  mov.b #SPI_CLK|SPI_SOMI|SPI_SIMO, &P1SEL
  mov.b #SPI_CLK|SPI_SOMI|SPI_SIMO, &P1SEL2

  ;; Set up SPI. At 8MHz this should run SPI at 100kHz.
  mov.b #UCSWRST,  &UCB0CTL1
  bis.b #UCSSEL_2, &UCB0CTL1
  mov.b #UCCKPH|UCMSB|UCMST|UCSYNC, &UCB0CTL0
  mov.b #80, &UCB0BR0
  mov.b #0,  &UCB0BR1
  bic.b #UCSWRST, &UCB0CTL1

  ;; Enable interrupts
  eint

  ;; Clear 64 bytes of MSP430 RAM.
  mov.w #0x200, r15
memset:
  mov.w #0, 0(r15)
  add.w #2, r15
  cmp.w #0x240, r15
  jnz memset

  ;; This delay shouldn't be needed.
  call #delay

  ;; Clock out 80 1's while chip isn't selected.
  call #sd_send_ff

  ;; Reset card. Repeat if card doesn't go into idle state.
reset_loop:
  mov.w #sd_command_reset_cmd1, r14
  call #sd_send_command
  add.b #1, &0x231         ; Count loop for debugging.
  cmp.b #1, r15
  jnz reset_loop

  ;; Read CSD (CMD8).
  mov.w #sd_command_csd_cmd8, r14
  call #sd_send_csd

  ;; Init card.
init_loop:
  mov.w #sd_command_app_spec_cmd55, r14
  call #sd_send_command
  mov.w #sd_command_cond_acmd41, r14
  call #sd_send_command
  bit.b #1, r15
  jnz init_loop

  ;mov.w #sd_command_set_block_size_512, r14
  ;call #sd_send_command

  ;; Read sector 0.
  mov.w #sd_command_read_0, r14
  call #sd_read_sector

  ;; Read sector 1.
  mov.w #sd_command_read_1, r14
  call #sd_read_sector

main:
  jmp main

;; sd_send_ff()
sd_send_ff:
  CS_DESELECT
  mov.w #10, r13
  mov.w #0x200, r14
sd_send_ff_repeat:
  mov.b #0xff, r15
  call #spi_send_char
  inc.w r14
  dec.w r13
  jnz sd_send_ff_repeat
  ret

;; sd_send_command(r14) - Returns R1 byte.
sd_send_command:
  CS_SELECT
  mov.w #6, r13
sd_send_command_loop:
  mov.b @r14+, r15
  call #spi_send_char
  dec.w r13
  jnz sd_send_command_loop
  mov.b #0xff, r15
  call #spi_send_char
  mov.b #0xff, r15
  call #spi_send_char
  CS_DESELECT
  ret

;; sd_send_command_wait_unidle(r14) - Returns R1 byte.
sd_send_command_r3:
  CS_SELECT
  mov.w #6, r13
sd_send_command_r3_loop:
  mov.b @r14+, r15
  call #spi_send_char
  dec.w r13
  jnz sd_send_command_r3_loop
  mov.b #0xff, r15
  call #spi_send_char
wait:
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, &0x238
  bit.b #1, r15
  jnz wait
  CS_DESELECT
  ret

;; sd_send_csd(r14) - Returns R1 byte.
sd_send_csd:
  CS_SELECT
  mov.w #6, r13
sd_send_csd_loop:
  mov.b @r14+, r15
  call #spi_send_char
  dec.w r13
  jnz sd_send_csd_loop
  mov.b #0xff, r15
  call #spi_send_char
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, &0x210
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, &0x211
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, &0x212
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, &0x213
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, &0x214
  CS_DESELECT
  ret

;; sd_read_sector(r14)
sd_read_sector:
  CS_SELECT
  ;; Send command to read from sector: 0x40|17, S3, S2, S1, S0, CRC.
  ;; S3 to S0 is the address in big endian. The { S3, S2, S1, S0 }
  ;; should be an address at the start of a sector. So 0x00000000
  ;; is good (sector 0) and 0x00000200 is good (sector 1), but
  ;; 0x00000001 (1 byte into sector 0) seems to have problems.
  ;; This assumes each sector is 512 bytes. CMD16 will set the block
  ;; (sector) length.
  mov.w #6, r13
sd_read_sector_loop:
  mov.b @r14+, r15
  call #spi_send_char
  dec.w r13
  jnz sd_read_sector_loop
  mov.b #0xff, r15
  call #spi_send_char
  mov.b #0xff, r15
  call #spi_send_char
  ;; DEBUG
  mov.b r15, &0x220
  bit.b #0x04, r15
  jnz sd_read_sector_exit
  ;; Read from SPI until card responds with 0xfe.
sd_read_sector_wait_fe:
  mov.b #0xff, r15
  call #spi_send_char
  cmp.b #0xfe, r15
  jnz sd_read_sector_wait_fe
  ;; Read 16 bytes out of the card. There should be more bytes
  ;; available based on the configured sector size, but the smallest
  ;; sector of 512 bytes won't fit in the MSP430G2553's memory.
  mov.w #16, r13
  mov.w #0x200, r14
sd_read_sector_data_loop:
  mov.b #0xff, r15
  call #spi_send_char
  mov.b r15, @r14
  inc.w r14
  dec.w r13
  jnz sd_read_sector_data_loop
  ;; Read the remaining bytes and 2 byte checksum.
  mov.w #512 - 16 + 2, r13
sd_read_sector_data_loop_2:
  mov.b #0xff, r15
  call #spi_send_char
  dec.w r13
  jnz sd_read_sector_data_loop_2
sd_read_sector_exit:
  CS_DESELECT
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

sd_command_reset_cmd1:
  .db 0x40, 0x00, 0x00, 0x00, 0x00, 0x95

sd_command_init_cmd1:
  .db 0x41, 0x40, 0x00, 0x00, 0x00, 0x00

sd_command_csd_cmd8:
  .db 0x48, 0x00, 0x00, 0x01, 0xaa, 0x87

sd_command_app_spec_cmd55:
  .db 0x77, 0x00, 0x00, 0x00, 0x00, 0x00

sd_command_cond_acmd41:
  .db 0x69, 0x40, 0x00, 0x00, 0x00, 0x00

sd_command_set_block_size_512:
  .db 0x50, 0x00, 0x00, 0x01, 0x00, 0x00

sd_command_read_0:
  .db 0x51, 0x00, 0x00, 0x00, 0x00, 0x00

sd_command_read_1:
  .db 0x51, 0x00, 0x00, 0x00, 0x01, 0x00

sd_command_stop_read:
  .db 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00

;; Vectors.
.org 0xfffe
  dw start

