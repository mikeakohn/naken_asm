;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; SSD1331 (96x64 dot matrix OLED display controller)
;; Used in Waveshare display sold by Parallax.
;; part of the naken_asm assembler
;;
;; Created by: Michael Kohn (mike@mikekohn.net)
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SSD1331_SET_REMAP       equ 0xa0
SSD1331_START_LINE      equ 0xa1
SSD1331_DISPLAY_OFFSET  equ 0xa2
SSD1331_SET_MULTIPLEX   equ 0xa8
SSD1331_SET_MASTER      equ 0xad
SSD1331_POWER_MODE      equ 0xb0
SSD1331_PRECHARGE       equ 0xb1
SSD1331_CLOCKDIV        equ 0xb3
SSD1331_VCOMH           equ 0xbe
SSD1331_MASTER_CURRENT  equ 0x87
SSD1331_MCU_PROT_STATUS equ 0xfd

SSD1331_CONTRAST_A      equ 0x81
SSD1331_CONTRAST_B      equ 0x82
SSD1331_CONTRAST_C      equ 0x83

SSD1331_PRECHARGE_A     equ 0x8a
SSD1331_PRECHARGE_B     equ 0x8b
SSD1331_PRECHARGE_C     equ 0x8c
SSD1331_PRECHARGE_LEVEL equ 0xbb

SSD1331_DISPLAY_NORMAL  equ 0xa4
SSD1331_DISPLAY_ALL_ON  equ 0xa5
SSD1331_DISPLAY_ALL_OFF equ 0xa6
SSD1331_DISPLAY_INVERSE equ 0xa7

SSD1331_DISPLAY_DIM     equ 0xac
SSD1331_DISPLAY_OFF     equ 0xae
SSD1331_DISPLAY_ON      equ 0xaf

SSD1331_SET_COLUMN      equ 0x15
SSD1331_SET_ROW         equ 0x75

SSD1331_DIM_MODE        equ 0xab

SSD1331_GS_PULSE_WIDTH  equ 0xb8
SSD1331_GS_TABLE_RESET  equ 0xb9

SSD1331_NOP_0           equ 0xbc
SSD1331_NOP_1           equ 0xbd
SSD1331_NOP_2           equ 0xe3

SSD1331_DRAW_LINE       equ 0x21
SSD1331_DRAW_RECT       equ 0x22
SSD1331_DRAW_COPY       equ 0x23
SSD1331_DIM_WINDOW      equ 0x24
SSD1331_CLEAR_WINDOW    equ 0x25
SSD1331_FILL_ENABLE     equ 0x26
SSD1331_SCROLL_SETUP    equ 0x27
SSD1331_SCROLL_STOP     equ 0x2e
SSD1331_SCROLL_START    equ 0x2f

