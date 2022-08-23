;;
;; This is an include file for the Nintendo SNES game console.
;; This is a part of the naken_asm assembler
;;
;; For more info:
;; https://www.mikekohn.net/micro/naken_asm.php
;;

SNES_INIDISP     equ 0x2100 ; Screen display register.
SNES_OBSEL       equ 0x2101 ; Object size and character size.
SNES_OAMADDL     equ 0x2102 ; OAM address register low.
SNES_OAMADDH     equ 0x2103 ; OAM address register high.
SNES_OAMDATA     equ 0x2104 ; OAM data write register.
SNES_BGMODE      equ 0x2105 ; BG Mode and character size.
SNES_MOSAIC      equ 0x2106 ; Mosaic register.
SNES_BG1SC       equ 0x2107 ; BG Tilemap address (BG1).
SNES_BG2SC       equ 0x2108 ; BG Tilemap address (BG2).
SNES_BG3SC       equ 0x2109 ; BG Tilemap address (BG3).
SNES_BG4SC       equ 0x210a ; BG Tilemap address (BG4).
SNES_BG12NBA     equ 0x210b ; BG character address (BG1&2).
SNES_BG34NBA     equ 0x210c ; BG character address (BG3&4).
SNES_BG1HOFS     equ 0x210d ; BG horizontal scroll (BG1)
SNES_BG1VOFS     equ 0x210e ; BG vertical scroll (BG1)
SNES_BG2HOFS     equ 0x210f ; BG horizontal scroll (BG2)
SNES_BG2VOFS     equ 0x2110 ; BG vertical scroll (BG2)
SNES_BG3HOFS     equ 0x2111 ; BG horizontal scroll (BG3)
SNES_BG3VOFS     equ 0x2112 ; BG vertical scroll (BG3)
SNES_BG4HOFS     equ 0x2113 ; BG horizontal scroll (BG4)
SNES_BG4VOFS     equ 0x2114 ; BG vertical scroll (BG4)
SNES_VMAIN       equ 0x2115 ; Video port control.
SNES_VMADDL      equ 0x2116 ; VRAM address register low.
SNES_VMADDH      equ 0x2117 ; VRAM address register high.
SNES_VMDATAL     equ 0x2118 ; VRAM data write register low.
SNES_VMDATAH     equ 0x2119 ; VRAM data write register high.
SNES_M7SEL       equ 0x211a ; Mode 7 settings.
SNES_M7A         equ 0x211b ; Mode 7 matrix registers.
SNES_M7B         equ 0x211c ; Mode 7 matrix registers.
SNES_M7C         equ 0x211d ; Mode 7 matrix registers.
SNES_M7D         equ 0x211e ; Mode 7 matrix registers.
SNES_M7X         equ 0x211f ; Mode 7 matrix registers.
SNES_M7Y         equ 0x2120 ; Mode 7 matrix registers.
SNES_CGADD       equ 0x2121 ; CGRAM address.
SNES_CGDATA      equ 0x2122 ; CGRAM data write.
SNES_W12SEL      equ 0x2123 ; Window mask settings.
SNES_W34SEL      equ 0x2124 ; Window mask settings.
SNES_WOBJSEL     equ 0x2125 ; Window mask settings.
SNES_WH0         equ 0x2126 ; Window position.
SNES_WH1         equ 0x2127 ; Window position.
SNES_WH2         equ 0x2128 ; Window position.
SNES_WH3         equ 0x2129 ; Window position.
SNES_WBGLOG      equ 0x212a ; Window mask logic (BG).
SNES_WOBJLOG     equ 0x212b ; Window mask logic (OBJ).
SNES_TM          equ 0x212c ; Screen destination.
SNES_TS          equ 0x212d ; Screen destination.
SNES_TMW         equ 0x212e ; Window mask destination.
SNES_TSW         equ 0x212f ; Window mask destination.
SNES_CGWSEL      equ 0x2130 ; Color math registers.
SNES_CGADSUB     equ 0x2131 ; Color math registers.
SNES_COLDATA     equ 0x2132 ; Color math registers.
SNES_SETINI      equ 0x2133 ; Screen mode select.
SNES_MPYL        equ 0x2134 ; Multiplication result.
SNES_MPYM        equ 0x2135 ; Multiplication result.
SNES_MPYH        equ 0x2136 ; Multiplication result.
SNES_SLHV        equ 0x2137 ; Software latch register.
SNES_OAMDATAREAD equ 0x2138 ; OAM data read register.
SNES_VMDATALREAD equ 0x2139 ; VRAM data read register low.
SNES_VMDATAHREAD equ 0x213a ; VRAM data read register high.
SNES_CGDATAREAD  equ 0x213b ; CGRAM data read register.
SNES_OPHCT       equ 0x213c ; Scanline horizontal location.
SNES_OPVCT       equ 0x213d ; Scanline vertical location.
SNES_STAT77      equ 0x213e ; PPU status.
SNES_STAT78      equ 0x213f ; PPU status.
SNES_APUIO0      equ 0x2140 ; APU I/O.
SNES_APUIO1      equ 0x2141 ; APU I/O.
SNES_APUIO2      equ 0x2142 ; APU I/O.
SNES_APUIO3      equ 0x2143 ; APU I/O.
SNES_WMDATA      equ 0x2180 ; WRAM data register.
SNES_WMADDL      equ 0x2181 ; WRAM address register.
SNES_WMADDM      equ 0x2182 ; WRAM address register.
SNES_WMADDH      equ 0x2183 ; WRAM address register.

;; Joypad registers.
SNES_JOYSER0     equ 0x4016
SNES_JOYSER1     equ 0x4017

;; Internal CPU registers.
SNES_NMITMEN     equ 0x4200 ; Interrupt enable.
SNES_WRIO        equ 0x4201 ; I/O port write enable.
SNES_WRMPYA      equ 0x4202 ; Multiplicand.
SNES_WRMPYB      equ 0x4203 ; Multiplicand.
SNES_WRDIVL      equ 0x4204 ; Divisor and dividend.
SNES_WRDIVH      equ 0x4205 ; Divisor and dividend.
SNES_WRDIVB      equ 0x4206 ; Divisor and dividend.
SNES_HTIMEL      equ 0x4207 ; IRQ timer (horizontal - low).
SNES_HTIMEH      equ 0x4208 ; IRQ timer (horizontal - high).
SNES_VTIMEL      equ 0x4209 ; IRQ timer (vertical - low).
SNES_VTIMEH      equ 0x420a ; IRQ timer (vertical - high).
SNES_MDMAEN      equ 0x420b ; DMA Enable.
SNES_HDMAEN      equ 0x420c ; HDMA Enable.
SNES_MEMSEL      equ 0x420d ; ROM speed.
SNES_RDNMI       equ 0x4210 ; Interrupt flag.
SNES_TIMEUP      equ 0x4211 ; Interrupt flag.
SNES_HVBJOY      equ 0x4212 ; PPU status?
SNES_RDIO        equ 0x4213 ; I/O port read.
SNES_RDDIVL      equ 0x4214 ; Divide result low.
SNES_RDDIVH      equ 0x4215 ; Divide result high.
SNES_RDMPYL      equ 0x4216 ; Multiplication result low.
SNES_RDMPYH      equ 0x4217 ; Multiplication result high.
SNES_JOY1L       equ 0x4218 ; Controller 1 port data low.
SNES_JOY1H       equ 0x4219 ; Controller 1 port data low.
SNES_JOY2L       equ 0x421a ; Controller 2 port data low.
SNES_JOY2H       equ 0x421b ; Controller 2 port data low.
SNES_JOY3L       equ 0x421c ; Controller 3 port data low.
SNES_JOY3H       equ 0x421d ; Controller 3 port data low.
SNES_JOY4L       equ 0x421e ; Controller 4 port data low.
SNES_JOY4H       equ 0x421f ; Controller 4 port data low.

;; DMA registers.
SNES_DMAP0       equ 0x4300 ; DMA control.
SNES_BBAD0       equ 0x4301 ; DMA destination.
SNES_A1T0L       equ 0x4302 ; DMA source address low.
SNES_A1T0H       equ 0x4303 ; DMA source address high.
SNES_A1B0        equ 0x4304 ; DMA source address.
SNES_DAS0L       equ 0x4305 ; DMA size low.
SNES_DAS0H       equ 0x4306 ; DMA size high.
SNES_DAS0B       equ 0x4307 ; HDMA indirect.
SNES_A2A0L       equ 0x4308 ; HDMA mid frame table address low.
SNES_A2A0H       equ 0x4309 ; HDMA mid frame table address high.
SNES_NTLR0       equ 0x430a ; HDMA mid frame table address high.

SNES_DMAP1       equ 0x4310 ; DMA control.
SNES_BBAD1       equ 0x4311 ; DMA destination.
SNES_A1T1L       equ 0x4312 ; DMA source address low.
SNES_A1T1H       equ 0x4313 ; DMA source address high.
SNES_A1B1        equ 0x4314 ; DMA source address.
SNES_DAS1L       equ 0x4315 ; DMA size low.
SNES_DAS1H       equ 0x4316 ; DMA size high.
SNES_DAS1B       equ 0x4317 ; HDMA indirect.
SNES_A2A1L       equ 0x4318 ; HDMA mid frame table address low.
SNES_A2A1H       equ 0x4319 ; HDMA mid frame table address high.
SNES_NTLR1       equ 0x431a ; HDMA mid frame table address high.

SNES_DMAP2       equ 0x4320 ; DMA control.
SNES_BBAD2       equ 0x4321 ; DMA destination.
SNES_A1T2L       equ 0x4322 ; DMA source address low.
SNES_A1T2H       equ 0x4323 ; DMA source address high.
SNES_A1B2        equ 0x4324 ; DMA source address.
SNES_DAS2L       equ 0x4325 ; DMA size low.
SNES_DAS2H       equ 0x4326 ; DMA size high.
SNES_DAS2B       equ 0x4327 ; HDMA indirect.
SNES_A2A2L       equ 0x4328 ; HDMA mid frame table address low.
SNES_A2A2H       equ 0x4329 ; HDMA mid frame table address high.
SNES_NTLR2       equ 0x432a ; HDMA mid frame table address high.

SNES_DMAP3       equ 0x4330 ; DMA control.
SNES_BBAD3       equ 0x4331 ; DMA destination.
SNES_A1T3L       equ 0x4332 ; DMA source address low.
SNES_A1T3H       equ 0x4333 ; DMA source address high.
SNES_A1B3        equ 0x4334 ; DMA source address.
SNES_DAS3L       equ 0x4335 ; DMA size low.
SNES_DAS3H       equ 0x4336 ; DMA size high.
SNES_DAS3B       equ 0x4337 ; HDMA indirect.
SNES_A2A3L       equ 0x4338 ; HDMA mid frame table address low.
SNES_A2A3H       equ 0x4339 ; HDMA mid frame table address high.
SNES_NTLR3       equ 0x433a ; HDMA mid frame table address high.

SNES_DMAP4       equ 0x4340 ; DMA control.
SNES_BBAD4       equ 0x4341 ; DMA destination.
SNES_A1T4L       equ 0x4342 ; DMA source address low.
SNES_A1T4H       equ 0x4343 ; DMA source address high.
SNES_A1B4        equ 0x4344 ; DMA source address.
SNES_DAS4L       equ 0x4345 ; DMA size low.
SNES_DAS4H       equ 0x4346 ; DMA size high.
SNES_DAS4B       equ 0x4347 ; HDMA indirect.
SNES_A2A4L       equ 0x4348 ; HDMA mid frame table address low.
SNES_A2A4H       equ 0x4349 ; HDMA mid frame table address high.
SNES_NTLR4       equ 0x434a ; HDMA mid frame table address high.

SNES_DMAP5       equ 0x4350 ; DMA control.
SNES_BBAD5       equ 0x4351 ; DMA destination.
SNES_A1T5L       equ 0x4352 ; DMA source address low.
SNES_A1T5H       equ 0x4353 ; DMA source address high.
SNES_A1B5        equ 0x4354 ; DMA source address.
SNES_DAS5L       equ 0x4355 ; DMA size low.
SNES_DAS5H       equ 0x4356 ; DMA size high.
SNES_DAS5B       equ 0x4357 ; HDMA indirect.
SNES_A2A5L       equ 0x4358 ; HDMA mid frame table address low.
SNES_A2A5H       equ 0x4359 ; HDMA mid frame table address high.
SNES_NTLR5       equ 0x435a ; HDMA mid frame table address high.

SNES_DMAP6       equ 0x4360 ; DMA control.
SNES_BBAD6       equ 0x4361 ; DMA destination.
SNES_A1T6L       equ 0x4362 ; DMA source address low.
SNES_A1T6H       equ 0x4363 ; DMA source address high.
SNES_A1B6        equ 0x4364 ; DMA source address.
SNES_DAS6L       equ 0x4365 ; DMA size low.
SNES_DAS6H       equ 0x4366 ; DMA size high.
SNES_DAS6B       equ 0x4367 ; HDMA indirect.
SNES_A2A6L       equ 0x4368 ; HDMA mid frame table address low.
SNES_A2A6H       equ 0x4369 ; HDMA mid frame table address high.
SNES_NTLR6       equ 0x436a ; HDMA mid frame table address high.

SNES_DMAP7       equ 0x4370 ; DMA control.
SNES_BBAD7       equ 0x4371 ; DMA destination.
SNES_A1T7L       equ 0x4372 ; DMA source address low.
SNES_A1T7H       equ 0x4373 ; DMA source address high.
SNES_A1B7        equ 0x4374 ; DMA source address.
SNES_DAS7L       equ 0x4375 ; DMA size low.
SNES_DAS7H       equ 0x4376 ; DMA size high.
SNES_DAS7B       equ 0x4377 ; HDMA indirect.
SNES_A2A7L       equ 0x4378 ; HDMA mid frame table address low.
SNES_A2A7H       equ 0x4379 ; HDMA mid frame table address high.
SNES_NTLR7       equ 0x437a ; HDMA mid frame table address high.

