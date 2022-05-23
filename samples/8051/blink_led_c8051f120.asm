;; c8051f120 sample
;;
;; This is a small example for Silicon Labs' c8051f120 development board.
;; The demo will blink a led wire to P3.3.

.8051

.include "c8051f120.inc"

LED equ P3.3

org 0
start:
  ; disable watchdog
  clr EA
  mov WDTCN, #0xde
  mov WDTCN, #0xad
  setb EA

  ; setup SFRPAGE to 0x0f
  mov SFRPAGE, #CONFIG_PAGE

  ; enable Crossbar 
  mov XBR2, #0x40

  ; set P3.3 to Push-Pull 
  mov P3MDOUT, #0x08
 
  setb LED 
  loop:
    cpl LED
    lcall delay
    sjmp loop

delay:
    mov r3, #255
    outer:
      mov r2, #255
      inner:
        nop
        djnz r2, inner
      djnz r3, outer
  ret

end
