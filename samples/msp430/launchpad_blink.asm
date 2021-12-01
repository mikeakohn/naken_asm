
.include "msp430x2xx.inc"

.org 0xf800
start:
  ;; Turn off watchdog timer.
  mov.w #WDTPW|WDTHOLD, &WDTCTL

  ;; Set P0 and P6 as outputs.
  mov.b #0x41, &P1DIR

main:
  ;; Initial value for LEDs turns on the P0 LED and
  ;; keeps P6 turned off.
  mov.w #0x01, r8
while_1:
  ;; Set LEDs to current value of r8 and toggle P0 and P6.
  mov.b r8, &P1OUT
  xor.b #0x41, r8

  ;; Delay by decrementing r9 60000 times.
  mov.w #60000, r9
delay_loop:
  dec r9
  jnz delay_loop

  ;; Repeat loop.
  jmp while_1

.org 0xfffe
  dw start             ; set reset vector to 'init' label

