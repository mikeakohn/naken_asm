;;
;; Playstation 2 include
;;
;; A part of the naken_asm assembler 
;;
;; For more information:
;; http://www.mikekohn.net/
;;

;; TIMER
T0_COUNT equ 0x1000_0000
T0_MODE equ 0x1000_0010
T0_COMP equ 0x1000_0020
T0_HOLD equ 0x1000_0030

T1_COUNT equ 0x1000_0800
T1_MODE equ 0x1000_0810
T1_COMP equ 0x1000_0820
T1_HOLD equ 0x1000_0830

T2_COUNT equ 0x1000_1000
T2_MODE equ 0x1000_1010
T2_COMP equ 0x1000_1020

T3_COUNT equ 0x1000_1800
T3_MODE equ 0x1000_1810
T3_COMP equ 0x1000_1820

;; IPU
IPU_CMD equ 0x1000_2000
IPU_CTRL equ 0x1000_2010
IPU_BP equ 0x1000_2020
IPU_TOP equ 0x1000_2030

;; GIF registers
GIF_CTRL equ 0x1000_3000
GIF_MODE equ 0x1000_3010
GIF_STAT equ 0x1000_3020
GIF_TAG0 equ 0x1000_3040
GIF_TAG1 equ 0x1000_3050
GIF_TAG2 equ 0x1000_3060
GIF_TAG3 equ 0x1000_3070
GIF_CNT equ 0x1000_3080
GIF_P3CNT equ 0x1000_3090
GIF_P3TAG equ 0x1000_30a0

;; VIF0
VIF0_STAT equ 0x1000_3800
VIF0_FBRST equ 0x1000_3810
VIF0_ERR equ 0x1000_3820
VIF0_MARK equ 0x1000_3830
VIF0_CYCLE equ 0x1000_3840
VIF0_MODE equ 0x1000_3850
VIF0_NUM equ 0x1000_3860
VIF0_MASK equ 0x1000_3870
VIF0_CODE equ 0x1000_3880
VIF0_ITOPS equ 0x1000_3890
VIF0_ITOP equ 0x1000_38d0
VIF0_R0 equ 0x1000_3900
VIF0_R1 equ 0x1000_3910
VIF0_R2 equ 0x1000_3920
VIF0_R3 equ 0x1000_3930
VIF0_C0 equ 0x1000_3940
VIF0_C1 equ 0x1000_3950
VIF0_C2 equ 0x1000_3960
VIF0_C3 equ 0x1000_3970

;; VIF1
VIF1_STAT equ 0x1000_3c00
VIF1_FBRST equ 0x1000_3c10
VIF1_ERR equ 0x1000_3c20
VIF1_MARK equ 0x1000_3c30
VIF1_CYCLE equ 0x1000_3c40
VIF1_MODE equ 0x1000_3c50
VIF1_NUM equ 0x1000_3c60
VIF1_MASK equ 0x1000_3c70
VIF1_CODE equ 0x1000_3c80
VIF1_ITOPS equ 0x1000_3c90
VIF1_BASE equ 0x1000_3ca0
VIF1_OFST equ 0x1000_3cb0
VIF1_TOPS equ 0x1000_3cc0
VIF1_ITOP equ 0x1000_3cd0
VIF1_TOP equ 0x1000_3ce0
VIF1_R0 equ 0x1000_3d00
VIF1_R1 equ 0x1000_3d10
VIF1_R2 equ 0x1000_3d20
VIF1_R3 equ 0x1000_3d30
VIF1_C0 equ 0x1000_3d40
VIF1_C1 equ 0x1000_3d50
VIF1_C2 equ 0x1000_3d60
VIF1_C3 equ 0x1000_3d70

;; FIFO
VIF0_FIFO equ 0x1000_4000
VIF1_FIFO equ 0x1000_5000
GIF_FIFO equ 0x1000_6000
IPU_OUT_FIFO equ 0x1000_7000
IPU_IN_FIFO equ 0x1000_7010

;; DMA To VIF0
D0_CHCR equ 0x1000_8000
D0_MADR equ 0x1000_8010
D0_QWC  equ 0x1000_8020
D0_TADR equ 0x1000_8030
D0_ASR0 equ 0x1000_8040
D0_ASR1 equ 0x1000_8050

;; DMA To VIF1
D1_CHCR equ 0x1000_9000
D1_MADR equ 0x1000_9010
D1_QWC  equ 0x1000_9020
D1_TADR equ 0x1000_9030
D1_ASR0 equ 0x1000_9040
D1_ASR1 equ 0x1000_9050

;; DMA To GIF
D2_CHCR equ 0x1000_a000
D2_MADR equ 0x1000_a010
D2_QWC  equ 0x1000_a020
D2_TADR equ 0x1000_a030
D2_ASR0 equ 0x1000_a040
D2_ASR1 equ 0x1000_a050

;; DMA from IPU
D3_CHCR equ 0x1000_b000
D3_MADR equ 0x1000_b010
D3_QWC  equ 0x1000_b020

;; DMA to IPU
D4_CHCR equ 0x1000_b400
D4_MADR equ 0x1000_b410
D4_QWC  equ 0x1000_b420
D4_TADR equ 0x1000_b430

;; DMA from SIF0
D5_CHCR equ 0x1000_c000
D5_MADR equ 0x1000_c010
D5_QWC  equ 0x1000_c020

;; DMA to SIF1
D6_CHCR equ 0x1000_c400
D6_MADR equ 0x1000_c410
D6_QWC  equ 0x1000_c420
D6_TADR equ 0x1000_c430

;; DMA to/from SIF2
D7_CHCR equ 0x1000_c800
D7_MADR equ 0x1000_c810
D7_QWC  equ 0x1000_c820

;; DMA from SPR
D8_CHCR equ 0x1000_d000
D8_MADR equ 0x1000_d010
D8_QWC  equ 0x1000_d020
D8_SADR equ 0x1000_d080

;; DMA to SPR
D9_CHCR equ 0x1000_d400
D9_MADR equ 0x1000_d410
D9_QWC  equ 0x1000_d420
D9_TADR equ 0x1000_d430
D9_SADR equ 0x1000_d480

D_CTRL equ 0x1000_e000
D_STAT equ 0x1000_e010
D_PCR equ 0x1000_e020
D_SQWC equ 0x1000_e030
D_RBSR equ 0x1000_e040
D_RBOR equ 0x1000_e050
D_STADR equ 0x1000_e060

;; Interrupt Control
I_STAT equ 0x1000_f000
I_MASK equ 0x1000_f010

;; SIF
SB_SMFLG equ 0x1000_f230

;; DMAC
D_ENABLER equ 0x1000_f520
D_ENABLEW equ 0x1000_f590

;; VU Memory
;VU0_MICRO equ 0x1100_0000
;VU0_VU equ 0x1100_4000
;VU1_MICRO equ 0x1100_8000
;VU1_VU equ 0x1101_4000

;; GS registers
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

;; Interrupt causes
INTC_GS equ 0
INTC_SBUS equ 1
INTC_VBLANK_S equ 2
INTC_VBLANK_E equ 3
INTC_VIF0 equ 4
INTC_VIF1 equ 5
INTC_VU0 equ 6
INTC_VU1 equ 7
INTC_IPU equ 8
INTC_TIM0 equ 9
INTC_TIM1 equ 10

;; GS General Purpose Registers
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

;; System calls
SetGsCrt equ 2
AddIntcHandler equ 16
RemoveIntcHandler equ 17
_EnableIntc equ 20
_DisableIntc equ 21
GsGetIMR equ 112
GsPutIMR equ 113

;; Primitive types
PRIM_POINT equ 0
PRIM_LINE equ 1
PRIM_LINE_STRIP equ 2
PRIM_TRIANGLE equ 3
PRIM_TRIANGLE_STRIP equ 4
PRIM_TRIANGLE_FAN equ 5
PRIM_SPRITE equ 6

;; Macros to help with GIF data formatting
.define SETREG_RGBAQ(r,g,b,a,q) (q<<32)|(a<<24)|(b<<16)|(g<<8)|r

.define SETREG_XYZ2(x,y,z) (z<<32)|(y<<16)|x

.define PRIM_SETTING(prim,shading,texture_mapping,fogging,alpha_blend,anti_alias,texture_coords,context,fix) ((fix)<<10)|((context)<<9)|((texture_coords)<<8)|((anti_alias)<<7)|((alpha_blend)<<6)|((fogging)<<5)|((texture_mapping)<<4)|((shading)<<3)|(prim)

.define GIF_TAG(nloop,eop,pre,prim,flg,nreg,regs) ((regs)<<64)|((nreg)<<60)|((flg)<<58)|((prim)<<47)|((pre)<<46)|((eop)<<15)|(nloop)

FLG_PACKED equ 0
FLG_REGIST equ 1
FLG_IMAGE equ 2

.define SETREG_TRXPOS(src_x,src_y,dst_x,dst_y,dir) ((dir)<<59)|((dst_y)<<48)|((dst_x)<<32)|((src_y)<<16)|(src_x)

DIR_UL_LR equ 0
DIR_LL_UR equ 1
DIR_UR_LL equ 2
DIR_LR_UL equ 3

.define SETREG_TRXREG(width,height) (height<<32)|width

.define SETREG_TRXDIR(dir) dir

XDIR_HOST_TO_LOCAL equ 0
XDIR_LOCAL_TO_HOST equ 1
XDIR_LOCAL_TO_LOCAL equ 2
XDIR_DEACTIVATED equ 3

.define SETREG_BITBLTBUF(src_base,src_width,src_format,dst_base,dst_width,dst_format) ((dst_format)<<56)|((dst_width)<<48)|((dst_base)<<32)|((src_format)<<24)|((src_width)<<16)|(src_base)

FMT_PSMCT32 equ 0x00
FMT_PSMCT24 equ 0x01
FMT_PSMCT16 equ 0x02
FMT_PCMCT16S equ 0x0a
FMT_PCGPU24 equ 0x12     ; for DISPFB
FMT_PSMT8 equ 0x13
FMT_PSMT4 equ 0x14
FMT_PSMT8H equ 0x1b
FMT_PSMT4HL equ 0x24
FMT_PSMT4HH equ 0x2c
FMT_PSMZ32 equ 0x30
FMT_PSMZ24 equ 0x31
FMT_PSMZ16 equ 0x32
FMT_PSMZ16S equ 0x3a

.define SET_DISPLAY(dx,dy,magh,magv,dw,dh) ((dh)<<44)|((dw)<<32)|((magv)<<27)|((magh)<<23)|((dy)<<12)|(dx)

.define SET_DISPFB(fpb,fbw,psm,dbx,dby) ((dby)<<43)|((dbx)<<32)|((psm)<<15)|((fbw)<<9)|(fpb)

