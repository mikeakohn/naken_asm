.stm8

;.big_endian

PD_ODR equ 0x500F
PD_IDR equ 0x5010
PD_DDR equ 0x5011
PD_CR1 equ 0x5012
PD_CR2 equ 0x5013

.org 0x8000
;.org 0x8080
main:
  mov PD_ODR, #$00
  mov PD_DDR, #$01
repeat:
  jp repeat

