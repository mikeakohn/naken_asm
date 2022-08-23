;;
;; This is an include file for the Nintendo NES game console.
;; This is a part of the naken_asm assembler
;;
;; For more info:
;; https://www.mikekohn.net/micro/naken_asm.php
;;

NES_CONTROLLER_2             equ 0x4017
NES_CONTROLLER_1             equ 0x4016

;; Sound registers.
NES_APU_CLOCK_SIGNAL         equ 0x4015
NES_APU_MOD_DATA_LENGTH      equ 0x4013
NES_APU_MOD_ADDRESS          equ 0x4012
NES_APU_MOD_DA               equ 0x4011
NES_APU_MOD_CONTROL          equ 0x4010
NES_APU_NOISE_FREQ_2         equ 0x400F
NES_APU_NOISE_FREQ_1         equ 0x400E
NES_APU_NOISE_CONTROL_1      equ 0x400C
NES_APU_TRIANGLE_FREQ_2      equ 0x400B
NES_APU_TRIANGLE_FREQ_1      equ 0x400A
NES_APU_TRIANGLE_CONTROL_2   equ 0x4009
NES_APU_TRIANGLE_CONTROL_1   equ 0x4008
NES_APU_PULSE_2_COARSE_TUNE  equ 0x4007
NES_APU_PULSE_2_FINE_TUNE    equ 0x4006
NES_APU_PULSE_2_RAMP_CONTROL equ 0x4005
NES_APU_PULSE_2_CONTROL      equ 0x4004
NES_APU_PULSE_1_COARSE_TUNE  equ 0x4003
NES_APU_PULSE_1_FINE_TUNE    equ 0x4002
NES_APU_PULSE_1_RAMP_CONTROL equ 0x4001
NES_APU_PULSE_1_CONTROL      equ 0x4000

;; Sprites.
NES_OAM_DMA                  equ 0x4014
NES_OAM_DATA                 equ 0x2004
NES_OAM_ADRRESS              equ 0x2003

;; Display registers.
NES_PPU_DATA                 equ 0x2007
NES_PPU_ADDRESS              equ 0x2006
NES_PPU_SCROLL               equ 0x2005
NES_PPU_STATUS               equ 0x2002
NES_PPU_MASK                 equ 0x2001
NES_PPU_CONTROL              equ 0x2000

