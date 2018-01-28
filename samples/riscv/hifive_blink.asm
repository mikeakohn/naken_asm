;; HiFive LED Blink
;;
;; Copyright 2018 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Test program for the SiFive HiFive1 RISC-V devkit.  Just blink the 3
;; LED's on the board.

.riscv
.entry_point start

.org 0x2040_0000
start:
  ;; x10 = 0x1001_2000 (value)
  ;; x11 = 0x1001_2004 (input_en)
  ;; x12 = 0x1001_2008 (output_en)
  ;; x13 = 0x1001_200c (port)
  lui x10, 0x1001_2
  ori x11, x10, 0x4
  ori x12, x10, 0x8
  ori x13, x10, 0xc

  ;; 0000 0000 0110 1000 | 0000 0000 0000 0000
  ;; Pin 19: Green Led
  ;; Pin 21: Blue Led
  ;; Pin 22: Red Led
  lui x14, 0x0068_0
  sw x14, (x12)

main:
  lui x15, 0x0048_0
  sw x15, (x13)
  jal ra, delay

  lui x15, 0x0028_0
  sw x15, (x13)
  jal ra, delay

  lui x15, 0x0060_0
  sw x15, (x13)
  jal ra, delay

  jal x0, main

delay:
  lui x20, 0x0008_0
delay_loop:
  addi x20, x20, -1
  bne x20, x0, delay_loop
  jalr x0, ra, 0

