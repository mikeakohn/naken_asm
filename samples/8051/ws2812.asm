;; stc8h8k64u sample
;; 
;; Copyright 2022 - By Cjacker Huang
;; cjacker@gmail.com
;;
;; This is a small example for STC stc8h8k64u development board.
;; The demo will turn on the 8bit ws2812 rgb led

.8051

.include "stc8hxx.inc"

; ws2812 DI pin to P4.1
DI equ P4.1

org 0
start:
  ; set P4.1 to push pull mode.
  mov P4M0, #0x02
  mov P4M1, #0x00

main_loop:
  ; send rgb data 8 times.
  mov r2, #0x08

  ; rgb data for one led
  led:
    ; green
    mov r5, #0x81
    ; red 
    mov r6, #0x81
    ; blue
    mov r7, #0x81
    lcall ws2812_sendrgb

  djnz r2, led

  ; data sent, reset
  lcall ws2812_ret

  sjmp main_loop

;use carry flag to determine bit is 0 or 1
ws2812_bit:
  jc one
  zero:
    setb DI
    nop
    nop
    nop
    nop
    clr DI
    nop
    nop
    nop
    nop
    nop
    ; sjmp is 3 cycle, nop is 1. 1 sjmp + 5 nop = 8 cycle.
    sjmp bit_done
  one:
    setb DI
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    clr DI
    nop
    nop
    nop
    nop
bit_done:
  ret

; use r5, r6, r7 to store r, g, b
; 'rlc' will set carry flag
ws2812_sendrgb:
; send green byte
  mov r4, #8;
  mov a, r6;
while_green: 
  rlc a
  lcall ws2812_bit
  djnz r4, while_green

; send red byte
  mov r4, #8
  mov a, r5
while_red:
  rlc a
  lcall ws2812_bit
  djnz r4, while_red

; send blue byte
  mov r4, #8
  mov a, r7
while_blue:
  rlc a
  lcall ws2812_bit
  djnz r4, while_blue

done:
  ret

ws2812_ret:
; send 8bit zero
  mov r4, #8
  send_zero:
    setb DI
    nop
    nop
    nop
    nop
    clr DI
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
  djnz r4, send_zero

; and delay some us
  delay:
    mov r5, #10
    mov r4, #200
    inner:
      nop
      djnz r4, inner
    djnz r5, delay
  ret

end 
