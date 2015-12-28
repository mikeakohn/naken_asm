.mips32

; LED4 is RG6 (pin 4)
; LED5 is RF0 (pin 58)

TRISF equ 0xbf886140
PORTF equ 0xbf886150
LATF equ 0xbf886160
ODCF equ 0xbf886170
TRISG equ 0xbf886180
PORTG equ 0xbf886190
LATG equ 0xbf8861a0
ODCG equ 0xbf8861b0

;.org 0x1d000000
.org 0x1d001000

start:
  ;; Set as output
  li $t1, TRISG
  sw $0, ($t1)
  li $t1, TRISF
  sw $0, ($t1)

  ;; Set value to 1
  li $t0, 0x40
  li $t1, LATG
  sw $t0, ($t1)

  li $t0, 0x01
  li $t1, LATF
  sw $t0, ($t1)

while_1:
  b while_1
  nop

