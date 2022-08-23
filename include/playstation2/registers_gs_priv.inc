;;
;; Playstation 2 Graphics Synthesizer Priviliged Registers
;;
;; A part of the naken_asm assembler 
;;
;; For more information:
;; https://www.mikekohn.net/
;;

;; GS privileged registers
;; These registers are accessed directly through the EE

GS_PMODE equ 0x1200_0000
GS_SMODE1 equ 0x1200_0010
GS_SMODE2 equ 0x1200_0020
GS_SRFSH equ 0x1200_0030
GS_SYNCH1 equ 0x1200_0040
GS_SYNCH2 equ 0x1200_0050
GS_SYNCV equ 0x1200_0060
GS_DISPFB1 equ 0x1200_0070
GS_DISPLAY1 equ 0x1200_0080
GS_DISPFB2 equ 0x1200_0090
GS_DISPLAY2 equ 0x1200_00a0
GS_EXTBUF equ 0x1200_00b0
GS_EXTDATA equ 0x1200_00c0
GS_EXTWRITE equ 0x1200_00d0
GS_BGCOLOR equ 0x1200_00e0
GS_CSR equ 0x1200_1000
GS_IMR equ 0x1200_1010
GS_BUSDIR equ 0x1200_1040
GS_SIGLBLID equ 0x1200_1080

