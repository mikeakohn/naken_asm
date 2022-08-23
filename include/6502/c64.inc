;;
;; This is an include file for the Commodore 64.
;; This is a part of the naken_asm assembler
;;
;; For more info:
;; https://www.mikekohn.net/micro/naken_asm.php
;;

;; SID (sound).
SID_V1_FREQ_LO            equ 0xd400
SID_V1_FREQ_HI            equ 0xd401
SID_V1_PULSE_WIDTH_HI     equ 0xd402
SID_V1_PULSE_WIDTH_LO     equ 0xd403
SID_V1_CONTROL            equ 0xd404
SID_V1_ATTACK_DECAY       equ 0xd405
SID_V1_SUSTAIN_RELEASE    equ 0xd406

SID_V2_FREQ_LO            equ 0xd407
SID_V2_FREQ_HI            equ 0xd408
SID_V2_PULSE_WIDTH_HI     equ 0xd409
SID_V2_PULSE_WIDTH_LO     equ 0xd40a
SID_V2_CONTROL            equ 0xd40b
SID_V2_ATTACK_DECAY       equ 0xd40c
SID_V2_SUSTAIN_RELEASE    equ 0xd40d

SID_V3_FREQ_LO            equ 0xd40e
SID_V3_FREQ_HI            equ 0xd40f
SID_V3_PULSE_WIDTH_HI     equ 0xd410
SID_V3_PULSE_WIDTH_LO     equ 0xd411
SID_V3_CONTROL            equ 0xd412
SID_V3_ATTACK_DECAY       equ 0xd413
SID_V3_SUSTAIN_RELEASE    equ 0xd414

SID_FILTER_CUTOFF_FREQ_LO equ 0xd415
SID_FILTER_CUTOFF_FREQ_HI equ 0xd416
SID_FILTER_CONTROL        equ 0xd417
SID_FILTER_VOLUME         equ 0xd418

SID_PADDLE_X              equ 0xd419
SID_PADDLE_Y              equ 0xd41a

SID_V3_WAVFORM_OUT        equ 0xd41b
SID_V3_ADSR_OUT           equ 0xd41c

;; Sprites.
VIC_SPRITE0_X             equ 0xd000
VIC_SPRITE0_Y             equ 0xd001
VIC_SPRITE1_X             equ 0xd002
VIC_SPRITE1_Y             equ 0xd003
VIC_SPRITE2_X             equ 0xd004
VIC_SPRITE2_Y             equ 0xd005
VIC_SPRITE3_X             equ 0xd006
VIC_SPRITE3_Y             equ 0xd007
VIC_SPRITE4_X             equ 0xd008
VIC_SPRITE4_Y             equ 0xd009
VIC_SPRITE5_X             equ 0xd00a
VIC_SPRITE5_Y             equ 0xd00b
VIC_SPRITE6_X             equ 0xd00c
VIC_SPRITE6_Y             equ 0xd00d
VIC_SPRITE7_X             equ 0xd00e
VIC_SPRITE7_Y             equ 0xd00f
VIC_SPRITE07_X_BIT_8      equ 0xd010
VIC_SCREEN_CONTROL_1      equ 0xd011
VIC_CURRENT_RASTER_LINE   equ 0xd012
VIC_LIGHTPEN_X            equ 0xd013
VIC_LIGHTPEN_Y            equ 0xd014
VIC_SPRITE_ENABLE         equ 0xd015
VIC_SCREEN_CONTROL_2      equ 0xd016
VIC_SPRITE_DOUBLE_HEIGHT  equ 0xd017
VIC_MEMORY_SETUP          equ 0xd018
VIC_INTERRUPT_STATUS      equ 0xd019
VIC_INTERRUPT_CONTROL     equ 0xd01a
VIC_SPRITE_PRIORITY       equ 0xd01b
VIC_SPRITE_MULTICOLOR     equ 0xd01c
VIC_SPRITE_DOUBLE_WIDTH   equ 0xd01d
VIC_SPRITE_COLLISION      equ 0xd01e
VIC_SPRITE_BG_COLLISION   equ 0xd01f
VIC_BORDER_COLOR          equ 0xd020
VIC_BG_COLOR              equ 0xd021
VIC_BG_COLOR_EXTRA_1      equ 0xd022
VIC_BG_COLOR_EXTRA_2      equ 0xd023
VIC_BG_COLOR_EXTRA_3      equ 0xd024
VIC_SPRITE_COLOR_EXTRA_1  equ 0xd025
VIC_SPRITE_COLOR_EXTRA_2  equ 0xd026
VIC_SPRITE0_COLOR         equ 0xd027
VIC_SPRITE1_COLOR         equ 0xd028
VIC_SPRITE2_COLOR         equ 0xd029
VIC_SPRITE3_COLOR         equ 0xd02a
VIC_SPRITE4_COLOR         equ 0xd02b
VIC_SPRITE5_COLOR         equ 0xd02c
VIC_SPRITE6_COLOR         equ 0xd02d
VIC_SPRITE7_COLOR         equ 0xd02e

;; CIA1: keyboard, joystick, mouse, I/O, etc.
CIA1_JOYSTICK_2            equ 0xdc00
CIA1_JOYSTICK_1            equ 0xdc01
CIA1_PORTA_DATA_DIR        equ 0xdc02
CIA1_PORTB_DATA_DIR        equ 0xdc03
CIA1_TIMERA_LO             equ 0xdc04
CIA1_TIMERA_HI             equ 0xdc05
CIA1_TIMERB_LO             equ 0xdc06
CIA1_TIMERB_HI             equ 0xdc07
CIA1_TOD_10SECONDS         equ 0xdc08
CIA1_TOD_SECONDS           equ 0xdc09
CIA1_TOD_MINUTES           equ 0xdc0a
CIA1_TOD_HOURS             equ 0xdc0b
CIA1_SERIAL_SHIFT          equ 0xdc0c
CIA1_IRQ_CONTROL           equ 0xdc0d
CIA1_TIMERA_CONTROL        equ 0xdc0e
CIA1_TIMERB_CONTROL        equ 0xdc0f

;; CIA2: serial bus, RS232, NMI.
CIA2_PORTA_SERIAL_BUS      equ 0xdd00
CIA2_PORTB_RS232           equ 0xdd01
CIA2_PORTA_DATA_DIR        equ 0xdd02
CIA2_PORTB_DATA_DIR        equ 0xdd03
CIA2_TIMERA_LO             equ 0xdd04
CIA2_TIMERA_HI             equ 0xdd05
CIA2_TIMERB_LO             equ 0xdd06
CIA2_TIMERB_HI             equ 0xdd07
CIA2_TOD_10SECONDS         equ 0xdd08
CIA2_TOD_SECONDS           equ 0xdd09
CIA2_TOD_MINUTES           equ 0xdd0a
CIA2_TOD_HOURS             equ 0xdd0b
CIA2_SERIAL_SHIFT          equ 0xdd0c
CIA2_IRQ_CONTROL           equ 0xdd0d
CIA2_TIMERA_CONTROL        equ 0xdd0e
CIA2_TIMERB_CONTROL        equ 0xdd0f

;; Hardware vectors
VECTOR_NMI_LO              equ 0xfffa
VECTOR_NMI_HI              equ 0xfffb
VECTOR_RESET_LO            equ 0xfffc
VECTOR_RESET_HI            equ 0xfffd
VECTOR_INT_LO              equ 0xfffe
VECTOR_INT_HI              equ 0xffff

