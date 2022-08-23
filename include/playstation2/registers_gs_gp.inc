;;
;; Playstation 2 Graphics Synthesizer General Purpose Registers
;;
;; A part of the naken_asm assembler 
;;
;; For more information:
;; https://www.mikekohn.net/
;;

;; GS General Purpose Registers
;; These registers are accessed through DMA transfers

REG_PRIM equ 0x00
REG_RGBAQ equ 0x01
REG_ST equ 0x02
REG_UV equ 0x03
REG_XYZF2 equ 0x04
REG_XYZ2 equ 0x05
REG_TEX0_1 equ 0x06
REG_TEX0_2 equ 0x07
REG_CLAMP_1 equ 0x08
REG_CLAMP_2 equ 0x09
REG_FOG equ 0x0a
REG_XYZF3 equ 0x0c
REG_XYZ3 equ 0x0d
REG_A_D equ 0x0e      ; for GIFtag only
REG_NOP equ 0x0f      ; for GIFtag only
REG_TEX1_1 equ 0x14
REG_TEX1_2 equ 0x15
REG_TEX2_1 equ 0x16
REG_TEX2_2 equ 0x17
REG_XYOFFSET_1 equ 0x18
REG_XYOFFSET_2 equ 0x19
REG_PRMODECONT equ 0x1a
REG_PRMODE equ 0x1b
REG_TEXCLUT equ 0x1c
REG_SCANMSK equ 0x22
REG_MIPTBP1_1 equ 0x34
REG_MIPTBP1_2 equ 0x35
REG_MIPTBP2_1 equ 0x36
REG_MIPTBP2_2 equ 0x37
REG_TEXA equ 0x3b
REG_FOGCOL equ 0x3d
REG_TEXFLUSH equ 0x3f
REG_SCISSOR_1 equ 0x40
REG_SCISSOR_2 equ 0x41
REG_ALPHA_1 equ 0x42
REG_ALPHA_2 equ 0x43
REG_DIMX equ 0x44
REG_DTHE equ 0x45
REG_COLCLAMP equ 0x46
REG_TEST_1 equ 0x47
REG_TEST_2 equ 0x48
REG_PABE equ 0x49
REG_FBA_1 equ 0x4a
REG_FBA_2 equ 0x4b
REG_FRAME_1 equ 0x4c
REG_FRAME_2 equ 0x4d
REG_ZBUF_1 equ 0x4e
REG_ZBUF_2 equ 0x4f
REG_BITBLTBUF equ 0x50
REG_TRXPOS equ 0x51
REG_TRXREG equ 0x52
REG_TRXDIR equ 0x53

