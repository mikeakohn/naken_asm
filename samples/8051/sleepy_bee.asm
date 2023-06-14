;; Sleepy Bee (EFM8SB2) sample.
;;
;; Copyright 2022 - By Michael Kohn
;; https://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; This is a small example for SiLabs' SLSTK2011A development board.
;; The demo will toggle the tri-color LED and draw some bars on the
;; 128x128 LCD display using the SPI bus.

.8051

.include "EFM8SB2.inc"

.org 0
start:
  ;; Watchdog.
  anl PCA0MD, #0xbf
  mov PCA0MD, A

  ;; Set stack to location 8.
  mov SP, #0x08

  ;; Setup P2 as output.
  mov P2MDOUT, #0x47
  mov XBR2, #0x40

  ;; Set EFM_DISP_ENABLE and EFM_DISP_CS for LCD display.
  setb P2.6
  clr P1.5

  ;; Setup SPI for LCD display.
  ;; P1.0: SPI1 SCLK
  ;; P1.2: SPI1 MOSI
  ;; P1.5: SPI1 CS
  mov P1MDOUT, #0x25
  mov XBR1, #0x40
  mov SPI1CFG, #0x40
  mov SPI1CN0, #0x01
  mov SPI1CKR, #0x0b

  ;; Setup ADC for Joystick.
  ;; P1.7: ADC in.
  mov A, P1MDIN
  anl A, #0x7f
  mov P1MDIN, A
  setb P1.7
  mov A, P1SKIP
  orl A, #0x80
  mov P1SKIP, A
  mov ADC0MX, #ADC0P15
  mov ADC0CN0, #0x80
  mov ADC0CF, #0xfc
  ;mov ADC0AC, #0x18
  mov REF0CN, #0x08

  lcall clear_display

  ;; Send a few lines of data to 128x128 LCD.
  ;; Command: 0x80
  ;;    Line: <line number>
  ;;    Data: 128 / 8 = 16 bytes of data
  ;; Trailer: 0x00, 0x00
  mov r0, #0
lcd_repeat:
  setb P1.5
  mov A, #0x80
  lcall write_spi
  mov A, r0
  lcall write_spi_reverse
  mov r1, #16
lcd_line_data:
  mov A, #0xf0
  lcall write_spi
  djnz r1, lcd_line_data
  mov A, #0x00
  lcall write_spi
  mov A, #0x00
  lcall write_spi
  clr P1.5
  inc r0
  cjne r0, #128, lcd_repeat

  ;; Turn off green.
  setb P2.1

repeat:
  ;; Sink LEDs to turn them on.
  ;; P2.0 green = off, P2.1 blue = off, P2.2 red = on.
  setb P2.0
  clr P2.2
  lcall delay

  ;; P2.0 green = on, P2.1 blue = off, P2.2 red = off.
  clr P2.0
  setb P2.2
  lcall delay

  ;; Get ADC value.
  mov ADC0CN0, #0x90
wait_adc:
  mov A, ADC0CN0
  anl A, #0x20
  jnz wait_adc

  mov A, ADC0L
  anl A, #0x80
  jnz set_green
  setb P2.1
  sjmp repeat

set_green:
  clr P2.1
  sjmp repeat

delay:
  mov r2, #0x10
delay_loop_far:
  mov r1, #0x00
delay_loop_outer:
  mov r0, #0x00
delay_loop_inner:
  djnz r0, delay_loop_inner
  djnz r1, delay_loop_outer
  djnz r2, delay_loop_far
  ret

write_spi:
  mov SPI1CN0, #0x01
  mov SPI1DAT, A
write_spi_wait:
  mov A, SPI1CN0
  anl A, #0x80
  jz write_spi_wait
  ret

write_spi_reverse:
  mov r3, #8
  mov r4, A
write_spi_reverse_loop:
  mov A, r4
  rlc A
  mov r4, A
  mov A, r5
  rrc A
  mov r5, A
  djnz r3, write_spi_reverse_loop
  lcall write_spi
  ret

write_spi_software:
  mov r3, #8
  mov r4, A
write_spi_software_loop:
  mov A, r4
  clr P1.2
  anl A, #0x80
  jz  write_spi_software_not_1
  setb P1.2
write_spi_software_not_1:
  setb P1.0
  mov A, r4
  rl A
  mov r4, A
  clr P1.0
  djnz r3, write_spi_software_loop
  clr P1.2
  ret

clear_display:
  setb P1.5
  mov A, #0x20
  lcall write_spi
  mov A, #0x00
  lcall write_spi
  clr P1.5
  ret

