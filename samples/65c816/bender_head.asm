;; Bender Head
;; Copyright 2017 - Michael Kohn (http://www.mikekohn.net/)
;;
;; Sound and motion for a 3D printed Bender head
;;
;; Parts:
;; Sound board: https://www.sparkfun.com/products/13720
;; W65C256SXB: http://wdc65xx.com/boards/w65c265sxb-engineering-development-system/
;;
;; Video: https://youtu.be/CZB954vQRqU
;;
;; A friend of mine 3D printed a Bender (from Futurama) head so
;; I could put some electronics around it.  Since the Bender on
;; the TV show runs on a 6502 chip, I figured it might be fun to
;; put some hardware / software around it using a chip from the
;; the 6502 family.
;;
;; Under the head is a servo motor and uses one of the 4 UARTs
;; to communicate with the sound board, so hopefully this code
;; can serve as a good example for:
;;
;; 1) Controlling a servo.
;; 2) Setting up timer interrupts.
;; 3) Setting up a UART on the SXB board.
;;
;; In this example the servo is connected to PD5 pin 0.

.65816
.include "w65c265.inc"

.define TOP_SERVO_COUNTER 400

  ; Global variables used in this program.
.org 0xf0
servo_position:
  resw 1
delay_count:
  resw 1
count_20ms:
  dw TOP_SERVO_COUNTER

.org 0x1000
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

  ; Timer control
  ; Bit 7: UART3 timer (0 = Timer 3, 1 = Timer4)
  ; Bit 6: UART2 timer (0 = Timer 3, 1 = Timer4)
  ; Bit 5: UART1 timer (0 = Timer 3, 1 = Timer4)
  ; Bit 4: UART1 timer (0 = Timer 3, 1 = Timer4)
  ; Bit 3-2: PWM Edge Interrupt Select on P62
  ;          (0=disable, 1=positive edge, 2=negative edge, 3=both edges)
  ; Bit 1: Timer 4 Output Enable (1=enable, 0=disable)
  ; Bit 0: Timer 4 Input clock (0=FCLK, 1=P60)
  lda.b #0x18
  sta TCR

  ; Enable timer 2, disable timer 0
  lda TER
  ora.b #0x04
  and.b #0xfe
  sta TER

  ; Disable UART, Edge, and Parallel Interface Interrupt
  lda.b #0x00
  sta UIER
  sta PIBER
  sta EIER

  ; UART3 control: receiver enable, 8 bit, transmit enable
  lda.b #0x25
  sta ACSR0

  ; UART control: receiver enable, 8 bit, transmit enable
  ;lda.b #0x25
  ;sta ACSR1

  ; Set port PD50-53 as output (for possible future project.. only P50
  ; is used here.
  lda.b #0x0f
  sta PDD5

  ; set port 2 value to 0
  lda.b #0x0f
  sta PD5

 ; Set timer top value
  ; (3686400 / 16 / 12) = 19200
  ; 1 / 19200 = 0.000052 seconds - approx 50us per interrupt
  ; 16 * 12 cpu cycles per interrupt
  ldx #12
  stx T2LL

  ; Set timer interrupt vector.  These RAM addresses are pointed to
  ; from the ROM address of IRQT2 (0xff84, 0xff85).  When an interrupt
  ; occurs, the CPU will jump to this area (which has a jump instruction
  ; for the address in 0x0121, 0x0122.
  lda.b #timer2_interrupt & 0xff
  sta 0x0121
  lda.b #timer2_interrupt >> 8
  sta 0x0122
  lda.b #0
  sta 0x0123

  ; Enable interrupt for Timer 2
  lda.b #0x04
  sta TIER

  ; Enable interrupts
  cli

main:
  jsr set_center
  jsr delay
  jsr set_right
  jsr play_sound
  jsr delay
  jsr set_center
  jsr delay
  jsr set_left
  jsr play_sound
  jsr delay
  jmp main

play_sound:
  ; Write '*' to UART for testing
  lda.b #'F'
  sta ARTD0
  rts

delay:
  ldy #40000
  sty delay_count
delay_while:
  ldy delay_count
  bne delay_while
  rts

set_center:
  ldx #370
  stx servo_position
  rts

set_left:
  ldx #360
  stx servo_position
  rts

set_right:
  ldx #380
  stx servo_position
  rts

.align 16
timer2_interrupt:
  ; Save A and status register (from main context)
  pha
  php

  ; Clear interrupt for Timer 2
  lda.b #0x04
  sta TIFR

  ; Set A to 16-bit
  ;rep #0x20
  ;dec delay_count
  ;sep #0x20
  ldy delay_count
  dey
  sty delay_count

  ; Update count_20ms count
  ldy count_20ms
  dey
  sty count_20ms
  bne timer2_interrupt_not_0

  ; Reset count_20ms count
  ldy #TOP_SERVO_COUNTER
  sty count_20ms

  ; Set P50 on
  lda.b #0x01
  sta PD5

  ; Restore A and status register
  plp
  pla
  rti

timer2_interrupt_not_0:
  cpy servo_position
  bne timer2_do_not_update_servo
  lda PD5
  and.b #0xfe
  sta PD5
timer2_do_not_update_servo:

timer2_interrupt_exit:
  ; Restore A and status register
  plp
  pla
  rti

