;;
;; This is an include file for the SunPlus SPG2xx game SoC series.
;; This is a part of the naken_asm assembler
;;
;; For more info:
;; http://www.mikekohn.net/micro/naken_asm.php
;;

PPU_BG1_SCROLL_X                        equ 0x2810
PPU_BG1_SCROLL_Y                        equ 0x2811
PPU_BG1_ATTR                            equ 0x2812
PPU_BG1_CTRL                            equ 0x2813
PPU_BG1_TILE_ADDR                       equ 0x2814
PPU_BG1_ATTR_ADDR                       equ 0x2815
PPU_BG2_SCROLL_X                        equ 0x2816
PPU_BG2_SCROLL_Y                        equ 0x2817
PPU_BG2_ATTR                            equ 0x2818
PPU_BG2_CTRL                            equ 0x2819
PPU_BG2_TILE_ADDR                       equ 0x281a
PPU_BG2_ATTR_ADDR                       equ 0x281b
PPU_VERT_COMPRESS_AMOUNT                equ 0x281c
PPU_VERT_COMPRESS_OFFSET                equ 0x281d
PPU_BG1_SEGMENT_ADDR                    equ 0x2820
PPU_BG2_SEGMENT_ADDR                    equ 0x2821
PPU_SPRITE_SEGMENT_ADDR                 equ 0x2822
PPU_BLEND_LEVEL                         equ 0x282a
PPU_FADE_CTRL                           equ 0x2830
PPU_IRQ_POS_Y                           equ 0x2836
PPU_IRQ_POS_X                           equ 0x2837
PPU_CURRENT_LINE                        equ 0x2838
PPU_LIGHTPEN_LATCH_1ST_LINE             equ 0x2839
PPU_TV_CTRL1                            equ 0x283c
PPU_TV_CTRL2                            equ 0x283d
PPU_LIGHTPEN_Y                          equ 0x283e
PPU_LIGHTPEN_X                          equ 0x283f
PPU_SPRITE_CTRL                         equ 0x2842
PPU_STN_LCD_CTRL                        equ 0x2854
PPU_IRQ_ENABLE                          equ 0x2862
PPU_IRQ_STATUS                          equ 0x2863
PPU_SPRITE_DMA_SRC                      equ 0x2870
PPU_SPRITE_DMA_DEST                     equ 0x2871
PPU_SPRITE_DMA_LEN                      equ 0x2872

PPU_LINE_SCROLL_MEM                     equ 0x2900
PPU_LINE_COMPRESS_MEM                   equ 0x2a00
PPU_COLOR_MEM                           equ 0x2b00
PPU_SPRITE_MEM                          equ 0x2c00

.define PPU_LINE_SCROLL(n)              (0x2900 + (n))
.define PPU_LINE_COMPRESS(n)            (0x2a00 + (n))
.define PPU_COLOR(n)                    (0x2b00 + (n))
.define PPU_SPRITE_TILE(n)              (0x2c00 + (n)*4)
.define PPU_SPRITE_X(n)                 (0x2c01 + (n)*4)
.define PPU_SPRITE_Y(n)                 (0x2c02 + (n)*4)
.define PPU_SPRITE_ATTR(n)              (0x2c03 + (n)*4)

.define SPU_CH_WAVE_ADDR(n)             (0x3000 + (n)*16)
.define SPU_CH_MODE(n)                  (0x3001 + (n)*16)
.define SPU_CH_LOOP_ADDR(n)             (0x3002 + (n)*16)
.define SPU_CH_PAN_VOL(n)               (0x3003 + (n)*16)
.define SPU_CH_ENVELOPE0(n)             (0x3004 + (n)*16)
.define SPU_CH_ENVELOPE_DATA(n)         (0x3005 + (n)*16)
.define SPU_CH_ENVELOPE1(n)             (0x3006 + (n)*16)
.define SPU_CH_ENVELOPE_ADDR_HI(n)      (0x3007 + (n)*16)
.define SPU_CH_ENVELOPE_ADDR_LO(n)      (0x3008 + (n)*16)
.define SPU_CH_WAVE_DATA_PREV(n)        (0x3009 + (n)*16)
.define SPU_CH_ENVELOPE_LOOP_CTRL(n)    (0x300a + (n)*16)
.define SPU_CH_WAVE_DATA(n)             (0x300b + (n)*16)
.define SPU_CH_ADPCM_SEL(n)             (0x300d + (n)*16)

.define SPU_CH_PHASE_HI(n)              (0x3200 + (n)*16)
.define SPU_CH_PHASE_ACCUM_HI(n)        (0x3201 + (n)*16)
.define SPU_CH_TARGET_PHASE_HI(n)       (0x3202 + (n)*16)
.define SPU_CH_RAMP_DOWN_CLOCK(n)       (0x3203 + (n)*16)
.define SPU_CH_PHASE_LO(n)              (0x3204 + (n)*16)
.define SPU_CH_PHASE_ACCUM_LO(n)        (0x3205 + (n)*16)
.define SPU_CH_TARGET_PHASE_LO(n)       (0x3206 + (n)*16)
.define SPU_CH_PHASE_CTRL(n)            (0x3207 + (n)*16)

SPU_CHANNEL_ENABLE                      equ 0x3400
SPU_MAIN_VOLUME                         equ 0x3401
SPU_CHANNEL_FIQ_ENABLE                  equ 0x3402
SPU_CHANNEL_FIQ_STATUS                  equ 0x3403
SPU_BEAT_BASE_COUNT                     equ 0x3404
SPU_BEAT_COUNT                          equ 0x3405
SPU_ENVCLK0_LO                          equ 0x3406
SPU_ENVCLK0_HI                          equ 0x3407
SPU_ENVCLK1_LO                          equ 0x3408
SPU_ENVCLK1_HI                          equ 0x3409
SPU_ENV_RAMP_DOWN                       equ 0x340a
SPU_CHANNEL_STOP                        equ 0x340b
SPU_CHANNEL_ZERO_CROSS                  equ 0x340c
SPU_CTRL                                equ 0x340d
SPU_COMPRESS_CTRL                       equ 0x340e
SPU_CHANNEL_STATUS                      equ 0x340f
SPU_WAVE_IN_L                           equ 0x3410
SPU_FIFO_WRITE_DATA                     equ 0x3410
SPU_WAVE_IN_R                           equ 0x3411
SPU_FIFO_IRQ_CTRL                       equ 0x3411
SPU_WAVE_OUT_L                          equ 0x3412
SPU_WAVE_OUT_R                          equ 0x3413
SPU_CHANNEL_REPEAT                      equ 0x3414
SPU_CHANNEL_ENV_MODE                    equ 0x3415
SPU_CHANNEL_TONE_RELEASE                equ 0x3416
SPU_ENV_IRQ                             equ 0x3417
SPU_CHANNEL_PITCH_BEND                  equ 0x3418
SPU_SOFT_PHASE                          equ 0x3419
SPU_ATTACK_RELEASE                      equ 0x341a
SPU_EQ_CUTOFF10                         equ 0x341b
SPU_EQ_CUTOFF32                         equ 0x341c
SPU_EQ_GAIN10                           equ 0x341d
SPU_EQ_GAIN32                           equ 0x341e

GPIO_MODE                               equ 0x3d00
GPIO_A_DATA                             equ 0x3d01
GPIO_A_BUFFER                           equ 0x3d02
GPIO_A_DIR                              equ 0x3d03
GPIO_A_ATTRIB                           equ 0x3d04
GPIO_A_MASK                             equ 0x3d05
GPIO_B_DATA                             equ 0x3d06
GPIO_B_BUFFER                           equ 0x3d07
GPIO_B_DIR                              equ 0x3d08
GPIO_B_ATTRIB                           equ 0x3d09
GPIO_B_MASK                             equ 0x3d0a
GPIO_C_DATA                             equ 0x3d0b
GPIO_C_BUFFER                           equ 0x3d0c
GPIO_C_DIR                              equ 0x3d0d
GPIO_C_ATTRIB                           equ 0x3d0e
GPIO_C_MASK                             equ 0x3d0f

TIMEBASE_SETUP                          equ 0x3d10
TIMEBASE_CLEAR                          equ 0x3d11
TIMER_A_DATA                            equ 0x3d12
TIMER_A_CTRL                            equ 0x3d13
TIMER_A_ON                              equ 0x3d14
TIMER_A_IRQCLR                          equ 0x3d15
TIMER_B_DATA                            equ 0x3d16
TIMER_B_CTRL                            equ 0x3d17
TIMER_B_ON                              equ 0x3d18
TIMER_B_IRQCLR                          equ 0x3d19

CURRENT_LINE                            equ 0x3d1c

SYSTEM_CTRL                             equ 0x3d20
IO_IRQ_ENABLE                           equ 0x3d21
IO_IRQ_STATUS                           equ 0x3d22
EXT_MEMORY_CTRL                         equ 0x3d23
WATCHDOG_CLEAR                          equ 0x3d24

ADC_CTRL                                equ 0x3d25
ADC_PAD                                 equ 0x3d26
ADC_DATA                                equ 0x3d27

SLEEP_MODE                              equ 0x3d28
WAKEUP_SOURCE                           equ 0x3d29
WAKEUP_TIME                             equ 0x3d2a

TV_SYSTEM                               equ 0x3d2b

PRNG_GEN1                               equ 0x3d2c
PRNG_GEN2                               equ 0x3d2d

FIQ_SELECT                              equ 0x3d2e

DS_ACCESS                               equ 0x3d2f

UART_CTRL                               equ 0x3d30
UART_STATUS                             equ 0x3d31
UART_RESET                              equ 0x3d32
UART_BAUD                               equ 0x3d33
UART_BAUD_LO                            equ 0x3d33
UART_BAUD_HI                            equ 0x3d34
UART_TXBUF                              equ 0x3d35
UART_RXBUF                              equ 0x3d36
UART_RXFIFO                             equ 0x3d37

SPI_CTRL                                equ 0x3d40
SPI_TXSTATUS                            equ 0x3d41
SPI_TXDATA                              equ 0x3d42
SPI_RXSTATUS                            equ 0x3d43
SPI_RXDATA                              equ 0x3d44
SPI_MISC                                equ 0x3d45

SIO_SETUP                               equ 0x3d50
SIO_STATUS                              equ 0x3d51
SIO_ADDR_LO                             equ 0x3d52
SIO_ADDR_HI                             equ 0x3d53
SIO_DATA                                equ 0x3d54
SIO_AUTO_TX_NUM                         equ 0x3d55

I2C_CMD                                 equ 0x3d58
I2C_STATUS                              equ 0x3d59
I2C_ACCESS                              equ 0x3d5a
I2C_ADDR                                equ 0x3d5b
I2C_SUBADDR                             equ 0x3d5c
I2C_DATA_OUT                            equ 0x3d5d
I2C_DATA_IN                             equ 0x3d5e
I2C_MODE                                equ 0x3d5f

REGULATOR_CTRL                          equ 0x3d60
CLOCK_CTRL                              equ 0x3d61
IODRIVE_CTRL                            equ 0x3d62

SYSDMA_SRC_LO                           equ 0x3e00
SYSDMA_SRC_HI                           equ 0x3e01
SYSDMA_LEN                              equ 0x3e02
SYSDMA_DEST                             equ 0x3e03
