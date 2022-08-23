;;
;; Playstation 2 VIF registers (vector unit)
;;
;; A part of the naken_asm assembler
;;
;; For more information:
;; https://www.mikekohn.net/
;;

VIF_NOP        equ 0x00
VIF_NOP_I      equ 0x80
VIF_STCYCL     equ 0x01
VIF_STCYCL_I   equ 0x81
VIF_OFFSET     equ 0x02
VIF_OFFSET_I   equ 0x82
VIF_BASE       equ 0x03
VIF_BASE_I     equ 0x83
VIF_ITOP       equ 0x04
VIF_ITOP_I     equ 0x84
VIF_STMOD      equ 0x05
VIF_STMOD_I    equ 0x85
VIF_MSKPATH3   equ 0x06
VIF_MSKPATH3_I equ 0x86
VIF_MARK       equ 0x07
VIF_MARK_I     equ 0x87
VIF_FLUSHE     equ 0x10
VIF_FLUSHE_I   equ 0x90
VIF_FLUSH      equ 0x11
VIF_FLUSH_I    equ 0x91
VIF_FLUSHA     equ 0x13
VIF_FLUSHA_I   equ 0x93
VIF_MSCAL      equ 0x14
VIF_MSCAL_I    equ 0x94
VIF_MSCNT      equ 0x17
VIF_MSCNT_I    equ 0x97
VIF_MSCALF     equ 0x15
VIF_MSCALF_I   equ 0x95
VIF_STMASK     equ 0x20
VIF_STMASK_I   equ 0xa0
VIF_STROW      equ 0x30
VIF_STROW_I    equ 0xb0
VIF_STCOL      equ 0x31
VIF_STCOL_I    equ 0xb1
VIF_MPG        equ 0x4a
VIF_MPG_I      equ 0xca
VIF_DIRECT     equ 0x50
VIF_DIRECT_I   equ 0xd0
VIF_DIRECTHL   equ 0x51
VIF_DIRECTHL_I equ 0xd1
VIF_UNPACK     equ 0x60
VIF_UNPACK_I   equ 0xe0

;; Specific versions of UNPACK
VIF_UNPACK_S32       equ 0x60
VIF_UNPACK_S32_I     equ 0xe0
VIF_UNPACK_S16       equ 0x61
VIF_UNPACK_S16_I     equ 0xe1
VIF_UNPACK_S8        equ 0x62
VIF_UNPACK_S8_I      equ 0xe2

VIF_UNPACK_S32_M     equ 0x70
VIF_UNPACK_S32_IM    equ 0xf0
VIF_UNPACK_S16_M     equ 0x71
VIF_UNPACK_S16_IM    equ 0xf1
VIF_UNPACK_S8_M      equ 0x72
VIF_UNPACK_S8_IM     equ 0xf2

VIF_UNPACK_V2_32     equ 0x64
VIF_UNPACK_V2_32_I   equ 0xe4
VIF_UNPACK_V2_16     equ 0x65
VIF_UNPACK_V2_16_I   equ 0xe5
VIF_UNPACK_V2_8      equ 0x66
VIF_UNPACK_V2_8_I    equ 0xe6

VIF_UNPACK_V2_32_M   equ 0x74
VIF_UNPACK_V2_32_IM  equ 0xf4
VIF_UNPACK_V2_16_M   equ 0x75
VIF_UNPACK_V2_16_IM  equ 0xf5
VIF_UNPACK_V2_8_M    equ 0x76
VIF_UNPACK_V2_8_IM   equ 0xf6

VIF_UNPACK_V3_32     equ 0x68
VIF_UNPACK_V3_32_I   equ 0xe8
VIF_UNPACK_V3_16     equ 0x69
VIF_UNPACK_V3_16_I   equ 0xe9
VIF_UNPACK_V3_8      equ 0x6a
VIF_UNPACK_V3_8_I    equ 0xea

VIF_UNPACK_V3_32_M   equ 0x78
VIF_UNPACK_V3_32_IM  equ 0xf8
VIF_UNPACK_V3_16_M   equ 0x79
VIF_UNPACK_V3_16_IM  equ 0xf9
VIF_UNPACK_V3_8_M    equ 0x7a
VIF_UNPACK_V3_8_IM   equ 0xfa

VIF_UNPACK_V4_32     equ 0x6c
VIF_UNPACK_V4_32_I   equ 0xec
VIF_UNPACK_V4_16     equ 0x6d
VIF_UNPACK_V4_16_I   equ 0xed
VIF_UNPACK_V4_8      equ 0x6e
VIF_UNPACK_V4_8_I    equ 0xee
VIF_UNPACK_V4_5      equ 0x6f
VIF_UNPACK_V4_5_I    equ 0xef

VIF_UNPACK_V4_32_M   equ 0x7c
VIF_UNPACK_V4_32_IM  equ 0xfc
VIF_UNPACK_V4_16_M   equ 0x7d
VIF_UNPACK_V4_16_IM  equ 0xfd
VIF_UNPACK_V4_8_M    equ 0x7e
VIF_UNPACK_V4_8_IM   equ 0xfe
VIF_UNPACK_V4_5_M    equ 0x7f
VIF_UNPACK_V4_5_IM   equ 0xff

