; C8051F120.h - Register Declarations for the Cygnal/SiLabs C8051F12x-F13x
; Processor Range
; 
; Copyright (C) 2003, Maarten Brock, sourceforge.brock@dse.nl
; 
; This library is free software; you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published by the
; Free Software Foundation; either version 2, or (at your option) any
; later version.
; 
; This library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this library; see the file COPYING. If not, write to the
; Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
; MA 02110-1301, USA.
; 
; As a special exception, if you link this library with other files,
; some of which are compiled with SDCC, to produce an executable,
; this library does not by itself cause the resulting executable to
; be covered by the GNU General Public License. This exception does
; not however invalidate any other reasons why the executable file
; might be covered by the GNU General Public License.

;  BYTE Registers  

;  All Pages 
P0 equ 0x80                   ; PORT 0
SP equ 0x81                   ; STACK POINTER
DPL equ 0x82                  ; DATA POINTER - LOW BYTE
DPH equ 0x83                  ; DATA POINTER - HIGH BYTE
SFRPAGE equ 0x84              ; SFR PAGE SELECT
SFRNEXT equ 0x85              ; SFR STACK NEXT PAGE
SFRLAST equ 0x86              ; SFR STACK LAST PAGE
PCON equ 0x87                 ; POWER CONTROL
P1 equ 0x90                   ; PORT 1
P2 equ 0xA0                   ; PORT 2
IE equ 0xA8                   ; INTERRUPT ENABLE
P3 equ 0xB0                   ; PORT 3
PSBANK equ 0xB1               ; FLASH BANK SELECT
IP equ 0xB8                   ; INTERRUPT PRIORITY
PSW equ 0xD0                  ; PROGRAM STATUS WORD
ACC equ 0xE0                  ; ACCUMULATOR
EIE1 equ 0xE6                 ; EXTERNAL INTERRUPT ENABLE 1
EIE2 equ 0xE7                 ; EXTERNAL INTERRUPT ENABLE 2
B equ 0xF0                    ; B REGISTER
EIP1 equ 0xF6                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 1
EIP2 equ 0xF7                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 2
WDTCN equ 0xFF                ; WATCHDOG TIMER CONTROL

;  Page 0x00 
TCON equ 0x88                 ; TIMER CONTROL
TMOD equ 0x89                 ; TIMER MODE
TL0 equ 0x8A                  ; TIMER 0 - LOW BYTE
TL1 equ 0x8B                  ; TIMER 1 - LOW BYTE
TH0 equ 0x8C                  ; TIMER 0 - HIGH BYTE
TH1 equ 0x8D                  ; TIMER 1 - HIGH BYTE
CKCON equ 0x8E                ; TIMER 0/1 CLOCK CONTROL
PSCTL equ 0x8F                ; FLASH WRITE/ERASE CONTROL
SSTA0 equ 0x91                ; UART 0 STATUS
SCON0 equ 0x98                ; UART 0 CONTROL
SCON equ 0x98                 ; UART 0 CONTROL
SBUF0 equ 0x99                ; UART 0 BUFFER
SBUF equ 0x99                 ; UART 0 BUFFER
SPI0CFG equ 0x9A              ; SPI 0 CONFIGURATION
SPI0DAT equ 0x9B              ; SPI 0 DATA
SPI0CKR equ 0x9D              ; SPI 0 CLOCK RATE CONTROL
EMI0TC equ 0xA1               ; EMIF TIMING CONTROL
EMI0CN equ 0xA2               ; EMIF CONTROL
_XPAGE equ 0xA2               ; XDATA/PDATA PAGE
EMI0CF equ 0xA3               ; EMIF CONFIGURATION
SADDR0 equ 0xA9               ; UART 0 SLAVE ADDRESS
FLSCL equ 0xB7                ; FLASH SCALE
SADEN0 equ 0xB9               ; UART 0 SLAVE ADDRESS MASK
AMX0CF equ 0xBA               ; ADC 0 MUX CONFIGURATION
AMX0SL equ 0xBB               ; ADC 0 MUX CHANNEL SELECTION
ADC0CF equ 0xBC               ; ADC 0 CONFIGURATION
ADC0L equ 0xBE                ; ADC 0 DATA - LOW BYTE
ADC0H equ 0xBF                ; ADC 0 DATA - HIGH BYTE
SMB0CN equ 0xC0               ; SMBUS 0 CONTROL
SMB0STA equ 0xC1              ; SMBUS 0 STATUS
SMB0DAT equ 0xC2              ; SMBUS 0 DATA
SMB0ADR equ 0xC3              ; SMBUS 0 SLAVE ADDRESS
ADC0GTL equ 0xC4              ; ADC 0 GREATER-THAN REGISTER - LOW BYTE
ADC0GTH equ 0xC5              ; ADC 0 GREATER-THAN REGISTER - HIGH BYTE
ADC0LTL equ 0xC6              ; ADC 0 LESS-THAN REGISTER - LOW BYTE
ADC0LTH equ 0xC7              ; ADC 0 LESS-THAN REGISTER - HIGH BYTE
TMR2CN equ 0xC8               ; TIMER 2 CONTROL
TMR2CF equ 0xC9               ; TIMER 2 CONFIGURATION
RCAP2L equ 0xCA               ; TIMER 2 CAPTURE REGISTER - LOW BYTE
RCAP2H equ 0xCB               ; TIMER 2 CAPTURE REGISTER - HIGH BYTE
TMR2L equ 0xCC                ; TIMER 2 - LOW BYTE
TL2 equ 0xCC                  ; TIMER 2 - LOW BYTE
TMR2H equ 0xCD                ; TIMER 2 - HIGH BYTE
TH2 equ 0xCD                  ; TIMER 2 - HIGH BYTE
SMB0CR equ 0xCF               ; SMBUS 0 CLOCK RATE
REF0CN equ 0xD1               ; VOLTAGE REFERENCE 0 CONTROL
DAC0L equ 0xD2                ; DAC 0 REGISTER - LOW BYTE
DAC0H equ 0xD3                ; DAC 0 REGISTER - HIGH BYTE
DAC0CN equ 0xD4               ; DAC 0 CONTROL
PCA0CN equ 0xD8               ; PCA 0 COUNTER CONTROL
PCA0MD equ 0xD9               ; PCA 0 COUNTER MODE
PCA0CPM0 equ 0xDA             ; PCA 0 MODULE 0 CONTROL
PCA0CPM1 equ 0xDB             ; PCA 0 MODULE 1 CONTROL
PCA0CPM2 equ 0xDC             ; PCA 0 MODULE 2 CONTROL
PCA0CPM3 equ 0xDD             ; PCA 0 MODULE 3 CONTROL
PCA0CPM4 equ 0xDE             ; PCA 0 MODULE 4 CONTROL
PCA0CPM5 equ 0xDF             ; PCA 0 MODULE 5 CONTROL
PCA0CPL5 equ 0xE1             ; PCA 0 MODULE 5 CAPTURE/COMPARE - LOW BYTE
PCA0CPH5 equ 0xE2             ; PCA 0 MODULE 5 CAPTURE/COMPARE - HIGH BYTE
ADC0CN equ 0xE8               ; ADC 0 CONTROL
PCA0CPL2 equ 0xE9             ; PCA 0 MODULE 2 CAPTURE/COMPARE - LOW BYTE
PCA0CPH2 equ 0xEA             ; PCA 0 MODULE 2 CAPTURE/COMPARE - HIGH BYTE
PCA0CPL3 equ 0xEB             ; PCA 0 MODULE 3 CAPTURE/COMPARE - LOW BYTE
PCA0CPH3 equ 0xEC             ; PCA 0 MODULE 3 CAPTURE/COMPARE - HIGH BYTE
PCA0CPL4 equ 0xED             ; PCA 0 MODULE 4 CAPTURE/COMPARE - LOW BYTE
PCA0CPH4 equ 0xEE             ; PCA 0 MODULE 4 CAPTURE/COMPARE - HIGH BYTE
RSTSRC equ 0xEF               ; RESET SOURCE
SPI0CN equ 0xF8               ; SPI 0 CONTROL
PCA0L equ 0xF9                ; PCA 0 TIMER - LOW BYTE
PCA0H equ 0xFA                ; PCA 0 TIMER - HIGH BYTE
PCA0CPL0 equ 0xFB             ; PCA 0 MODULE 0 CAPTURE/COMPARE - LOW BYTE
PCA0CPH0 equ 0xFC             ; PCA 0 MODULE 0 CAPTURE/COMPARE - HIGH BYTE
PCA0CPL1 equ 0xFD             ; PCA 0 MODULE 1 CAPTURE/COMPARE - LOW BYTE
PCA0CPH1 equ 0xFE             ; PCA 0 MODULE 1 CAPTURE/COMPARE - HIGH BYTE

;  Page 0x01 
CPT0CN equ 0x88               ; COMPARATOR 0 CONTROL
CPT0MD equ 0x89               ; COMPARATOR 0 CONFIGURATION
SCON1 equ 0x98                ; UART 1 CONTROL
SBUF1 equ 0x99                ; UART 1 BUFFER
TMR3CN equ 0xC8               ; TIMER 3 CONTROL
TMR3CF equ 0xC9               ; TIMER 3 CONFIGURATION
RCAP3L equ 0xCA               ; TIMER 3 CAPTURE REGISTER - LOW BYTE
RCAP3H equ 0xCB               ; TIMER 3 CAPTURE REGISTER - HIGH BYTE
TMR3L equ 0xCC                ; TIMER 3 - LOW BYTE
TMR3H equ 0xCD                ; TIMER 3 - HIGH BYTE
DAC1L equ 0xD2                ; DAC 1 REGISTER - LOW BYTE
DAC1H equ 0xD3                ; DAC 1 REGISTER - HIGH BYTE
DAC1CN equ 0xD4               ; DAC 1 CONTROL

;  Page 0x02 
CPT1CN equ 0x88               ; COMPARATOR 1 CONTROL
CPT1MD equ 0x89               ; COMPARATOR 1 CONFIGURATION
AMX2CF equ 0xBA               ; ADC 2 MUX CONFIGURATION
AMX2SL equ 0xBB               ; ADC 2 MUX CHANNEL SELECTION
ADC2CF equ 0xBC               ; ADC 2 CONFIGURATION
ADC2 equ 0xBE                 ; ADC 2 DATA
ADC2GT equ 0xC4               ; ADC 2 GREATER-THAN REGISTER
ADC2LT equ 0xC6               ; ADC 2 LESS-THAN REGISTER
TMR4CN equ 0xC8               ; TIMER 4 CONTROL
TMR4CF equ 0xC9               ; TIMER 4 CONFIGURATION
RCAP4L equ 0xCA               ; TIMER 4 CAPTURE REGISTER - LOW BYTE
RCAP4H equ 0xCB               ; TIMER 4 CAPTURE REGISTER - HIGH BYTE
TMR4L equ 0xCC                ; TIMER 4 - LOW BYTE
TMR4H equ 0xCD                ; TIMER 4 - HIGH BYTE
ADC2CN equ 0xE8               ; ADC 2 CONTROL

;  Page 0x03 
MAC0BL equ 0x91               ; MAC0 B Register Low Byte
MAC0BH equ 0x92               ; MAC0 B Register High Byte
MAC0ACC0 equ 0x93             ; MAC0 Accumulator Byte 0 (LSB equ
MAC0ACC1 equ 0x94             ; MAC0 Accumulator Byte 1
MAC0ACC2 equ 0x95             ; MAC0 Accumulator Byte 2
MAC0ACC3 equ 0x96             ; MAC0 Accumulator Byte 3 (MSB equ
MAC0OVR equ 0x97              ; MAC0 Accumulator Overflow
MAC0STA equ 0xC0              ; MAC0 Status Register
MAC0AL equ 0xC1               ; MAC0 A Register Low Byte
MAC0AH equ 0xC2               ; MAC0 A Register High Byte
MAC0CF equ 0xC3               ; MAC0 Configuration
MAC0RNDL equ 0xCE             ; MAC0 Rounding Register Low Byte
MAC0RNDH equ 0xCF             ; MAC0 Rounding Register High Byte

;  Page 0x0F 
FLSTAT equ 0x88               ; FLASH STATUS
PLL0CN equ 0x89               ; PLL 0 CONTROL
OSCICN equ 0x8A               ; INTERNAL OSCILLATOR CONTROL
OSCICL equ 0x8B               ; INTERNAL OSCILLATOR CALIBRATION
OSCXCN equ 0x8C               ; EXTERNAL OSCILLATOR CONTROL
PLL0DIV equ 0x8D              ; PLL 0 DIVIDER
PLL0MUL equ 0x8E              ; PLL 0 MULTIPLIER
PLL0FLT equ 0x8F              ; PLL 0 FILTER
SFRPGCN equ 0x96              ; SFR PAGE CONTROL
CLKSEL equ 0x97               ; SYSTEM CLOCK SELECT
CCH0MA equ 0x9A               ; CACHE MISS ACCUMULATOR
P4MDOUT equ 0x9C              ; PORT 4 OUTPUT MODE
P5MDOUT equ 0x9D              ; PORT 5 OUTPUT MODE
P6MDOUT equ 0x9E              ; PORT 6 OUTPUT MODE
P7MDOUT equ 0x9F              ; PORT 7 OUTPUT MODE
CCH0CN equ 0xA1               ; CACHE CONTROL
CCH0TN equ 0xA2               ; CACHE TUNING REGISTER
CCH0LC equ 0xA3               ; CACHE LOCK
P0MDOUT equ 0xA4              ; PORT 0 OUTPUT MODE
P1MDOUT equ 0xA5              ; PORT 1 OUTPUT MODE
P2MDOUT equ 0xA6              ; PORT 2 OUTPUT MODE CONFIGURATION
P3MDOUT equ 0xA7              ; PORT 3 OUTPUT MODE CONFIGURATION
P1MDIN equ 0xAD               ; PORT 1 INPUT MODE
FLACL equ 0xB7                ; FLASH ACCESS LIMIT
P4 equ 0xC8                   ; PORT 4
P5 equ 0xD8                   ; PORT 5
XBR0 equ 0xE1                 ; CROSSBAR CONFIGURATION REGISTER 0
XBR1 equ 0xE2                 ; CROSSBAR CONFIGURATION REGISTER 1
XBR2 equ 0xE3                 ; CROSSBAR CONFIGURATION REGISTER 2
P6 equ 0xE8                   ; PORT 6
P7 equ 0xF8                   ; PORT 7

;  WORD/DWORD Registers  

;  Page 0x00 
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
TMR2 equ 0xCDCC               ; TIMER 2 COUNTER
RCAP2 equ 0xCBCA              ; TIMER 2 CAPTURE REGISTER WORD
ADC0 equ 0xBFBE               ; ADC 0 DATA WORD
ADC0GT equ 0xC5C4             ; ADC 0 GREATER-THAN REGISTER WORD
ADC0LT equ 0xC7C6             ; ADC 0 LESS-THAN REGISTER WORD
DAC0 equ 0xD3D2               ; DAC 0 REGISTER WORD
PCA0 equ 0xFAF9               ; PCA 0 TIMER COUNTER
PCA0CP0 equ 0xFCFB            ; PCA 0 MODULE 0 CAPTURE/COMPARE WORD
PCA0CP1 equ 0xFEFD            ; PCA 0 MODULE 1 CAPTURE/COMPARE WORD
PCA0CP2 equ 0xEAE9            ; PCA 0 MODULE 2 CAPTURE/COMPARE WORD
PCA0CP3 equ 0xECEB            ; PCA 0 MODULE 3 CAPTURE/COMPARE WORD
PCA0CP4 equ 0xEEED            ; PCA 0 MODULE 4 CAPTURE/COMPARE WORD
PCA0CP5 equ 0xE2E1            ; PCA 0 MODULE 5 CAPTURE/COMPARE WORD

;  Page 0x01 
TMR3 equ 0xCDCC               ; TIMER 3 COUNTER
RCAP3 equ 0xCBCA              ; TIMER 3 CAPTURE REGISTER WORD
DAC1 equ 0xD3D2               ; DAC 1 REGISTER WORD

;  Page 0x02 
TMR4 equ 0xCDCC               ; TIMER 4 COUNTER
RCAP4 equ 0xCBCA              ; TIMER 4 CAPTURE REGISTER WORD

;  Page 0x03 
MAC0A equ 0xC2C1              ; MAC0 A Register

; No sfr16 definition for MAC0B because MAC0BL must be written last 
; __sfr32 __at (0x96959493) MAC0ACC  ;  /* MAC0 Accumulator                              */
MAC0RND equ 0xCFCE            ; MAC0 Rounding Register

;  BIT Registers  

;  TCON  0x88 
IT0 equ 0x88                  ; EXT. INTERRUPT 0 TYPE
IE0 equ 0x89                  ; EXT. INTERRUPT 0 EDGE FLAG
IT1 equ 0x8A                  ; EXT. INTERRUPT 1 TYPE
IE1 equ 0x8B                  ; EXT. INTERRUPT 1 EDGE FLAG
TR0 equ 0x8C                  ; TIMER 0 ON/OFF CONTROL
TF0 equ 0x8D                  ; TIMER 0 OVERFLOW FLAG
TR1 equ 0x8E                  ; TIMER 1 ON/OFF CONTROL
TF1 equ 0x8F                  ; TIMER 1 OVERFLOW FLAG

;  CPT0CN  0x88 
CP0HYN0 equ 0x88              ; COMPARATOR 0 NEGATIVE HYSTERESIS 0
CP0HYN1 equ 0x89              ; COMPARATOR 0 NEGATIVE HYSTERESIS 1
CP0HYP0 equ 0x8A              ; COMPARATOR 0 POSITIVE HYSTERESIS 0
CP0HYP1 equ 0x8B              ; COMPARATOR 0 POSITIVE HYSTERESIS 1
CP0FIF equ 0x8C               ; COMPARATOR 0 FALLING EDGE INTERRUPT
CP0RIF equ 0x8D               ; COMPARATOR 0 RISING EDGE INTERRUPT
CP0OUT equ 0x8E               ; COMPARATOR 0 OUTPUT
CP0EN equ 0x8F                ; COMPARATOR 0 ENABLE

;  CPT1CN  0x88 
CP1HYN0 equ 0x88              ; COMPARATOR 1 NEGATIVE HYSTERESIS 0
CP1HYN1 equ 0x89              ; COMPARATOR 1 NEGATIVE HYSTERESIS 1
CP1HYP0 equ 0x8A              ; COMPARATOR 1 POSITIVE HYSTERESIS 0
CP1HYP1 equ 0x8B              ; COMPARATOR 1 POSITIVE HYSTERESIS 1
CP1FIF equ 0x8C               ; COMPARATOR 1 FALLING EDGE INTERRUPT
CP1RIF equ 0x8D               ; COMPARATOR 1 RISING EDGE INTERRUPT
CP1OUT equ 0x8E               ; COMPARATOR 1 OUTPUT
CP1EN equ 0x8F                ; COMPARATOR 1 ENABLE

;  FLSTAT  0x88 
FLHBUSY equ 0x88              ; FLASH BUSY

;  SCON0  0x98 
RI0 equ 0x98                  ; UART 0 RX INTERRUPT FLAG
RI equ 0x98                   ; UART 0 RX INTERRUPT FLAG
TI0 equ 0x99                  ; UART 0 TX INTERRUPT FLAG
TI equ 0x99                   ; UART 0 TX INTERRUPT FLAG
RB80 equ 0x9A                 ; UART 0 RX BIT 8
TB80 equ 0x9B                 ; UART 0 TX BIT 8
REN0 equ 0x9C                 ; UART 0 RX ENABLE
REN equ 0x9C                  ; UART 0 RX ENABLE
SM20 equ 0x9D                 ; UART 0 MULTIPROCESSOR EN
SM10 equ 0x9E                 ; UART 0 MODE 1
SM00 equ 0x9F                 ; UART 0 MODE 0

;  SCON1  0x98 
RI1 equ 0x98                  ; UART 1 RX INTERRUPT FLAG
TI1 equ 0x99                  ; UART 1 TX INTERRUPT FLAG
RB81 equ 0x9A                 ; UART 1 RX BIT 8
TB81 equ 0x9B                 ; UART 1 TX BIT 8
REN1 equ 0x9C                 ; UART 1 RX ENABLE
MCE1 equ 0x9D                 ; UART 1 MCE
S1MODE equ 0x9F               ; UART 1 MODE

;  IE  0xA8 
EX0 equ 0xA8                  ; EXTERNAL INTERRUPT 0 ENABLE
ET0 equ 0xA9                  ; TIMER 0 INTERRUPT ENABLE
EX1 equ 0xAA                  ; EXTERNAL INTERRUPT 1 ENABLE
ET1 equ 0xAB                  ; TIMER 1 INTERRUPT ENABLE
ES0 equ 0xAC                  ; UART0 INTERRUPT ENABLE
ES equ 0xAC                   ; UART0 INTERRUPT ENABLE
ET2 equ 0xAD                  ; TIMER 2 INTERRUPT ENABLE
EA equ 0xAF                   ; GLOBAL INTERRUPT ENABLE

;  IP  0xB8 
PX0 equ 0xB8                  ; EXTERNAL INTERRUPT 0 PRIORITY
PT0 equ 0xB9                  ; TIMER 0 PRIORITY
PX1 equ 0xBA                  ; EXTERNAL INTERRUPT 1 PRIORITY
PT1 equ 0xBB                  ; TIMER 1 PRIORITY
PS0 equ 0xBC                  ; SERIAL PORT PRIORITY
PS equ 0xBC                   ; SERIAL PORT PRIORITY
PT2 equ 0xBD                  ; TIMER 2 PRIORITY

; SMB0CN 0xC0 
SMBTOE equ 0xC0               ; SMBUS 0 TIMEOUT ENABLE
SMBFTE equ 0xC1               ; SMBUS 0 FREE TIMER ENABLE
AA equ 0xC2                   ; SMBUS 0 ASSERT/ACKNOWLEDGE FLAG
SI equ 0xC3                   ; SMBUS 0 INTERRUPT PENDING FLAG
STO equ 0xC4                  ; SMBUS 0 STOP FLAG
STA equ 0xC5                  ; SMBUS 0 START FLAG
ENSMB equ 0xC6                ; SMBUS 0 ENABLE
BUSY equ 0xC7                 ; SMBUS 0 BUSY

; MAC0STA 0xC0 
MAC0N equ 0xC0                ; MAC 0 NEGATIVE FLAG
MAC0SO equ 0xC1               ; MAC 0 SOFT OVERFLOW FLAG
MAC0Z equ 0xC2                ; MAC 0 ZERO FLAG
MAC0HO equ 0xC3               ; MAC 0 HARD OVERFLOW FLAG

;  TMR2CN  0xC8 
CPRL2 equ 0xC8                ; TIMER 2 CAPTURE SELECT
CT2 equ 0xC9                  ; TIMER 2 COUNTER SELECT
TR2 equ 0xCA                  ; TIMER 2 ON/OFF CONTROL
EXEN2 equ 0xCB                ; TIMER 2 EXTERNAL ENABLE FLAG
EXF2 equ 0xCE                 ; TIMER 2 EXTERNAL FLAG
TF2 equ 0xCF                  ; TIMER 2 OVERFLOW FLAG

;  TMR3CN  0xC8 
CPRL3 equ 0xC8                ; TIMER 3 CAPTURE SELECT
CT3 equ 0xC9                  ; TIMER 3 COUNTER SELECT
TR3 equ 0xCA                  ; TIMER 3 ON/OFF CONTROL
EXEN3 equ 0xCB                ; TIMER 3 EXTERNAL ENABLE FLAG
EXF3 equ 0xCE                 ; TIMER 3 EXTERNAL FLAG
TF3 equ 0xCF                  ; TIMER 3 OVERFLOW FLAG

;  TMR4CN  0xC8 
CPRL4 equ 0xC8                ; TIMER 4 CAPTURE SELECT
CT4 equ 0xC9                  ; TIMER 4 COUNTER SELECT
TR4 equ 0xCA                  ; TIMER 4 ON/OFF CONTROL
EXEN4 equ 0xCB                ; TIMER 4 EXTERNAL ENABLE FLAG
EXF4 equ 0xCE                 ; TIMER 4 EXTERNAL FLAG
TF4 equ 0xCF                  ; TIMER 4 OVERFLOW FLAG

;  PSW  0xD0 
P equ 0xD0                    ; ACCUMULATOR PARITY FLAG
F1 equ 0xD1                   ; USER FLAG 1
OV equ 0xD2                   ; OVERFLOW FLAG
RS0 equ 0xD3                  ; REGISTER BANK SELECT 0
RS1 equ 0xD4                  ; REGISTER BANK SELECT 1
F0 equ 0xD5                   ; USER FLAG 0
AC equ 0xD6                   ; AUXILIARY CARRY FLAG
CY equ 0xD7                   ; CARRY FLAG

; PCA0CN D8H 
CCF0 equ 0xD8                 ; PCA 0 MODULE 0 INTERRUPT FLAG
CCF1 equ 0xD9                 ; PCA 0 MODULE 1 INTERRUPT FLAG
CCF2 equ 0xDA                 ; PCA 0 MODULE 2 INTERRUPT FLAG
CCF3 equ 0xDB                 ; PCA 0 MODULE 3 INTERRUPT FLAG
CCF4 equ 0xDC                 ; PCA 0 MODULE 4 INTERRUPT FLAG
CCF5 equ 0xDD                 ; PCA 0 MODULE 5 INTERRUPT FLAG
CR equ 0xDE                   ; PCA 0 COUNTER RUN CONTROL BIT
CF equ 0xDF                   ; PCA 0 COUNTER OVERFLOW FLAG

; ADC0CN E8H 
AD0LJST equ 0xE8              ; ADC 0 RIGHT JUSTIFY DATA BIT
AD0WINT equ 0xE9              ; ADC 0 WINDOW INTERRUPT FLAG
AD0CM0 equ 0xEA               ; ADC 0 CONVERT START MODE BIT 0
AD0CM1 equ 0xEB               ; ADC 0 CONVERT START MODE BIT 1
AD0BUSY equ 0xEC              ; ADC 0 BUSY FLAG
AD0INT equ 0xED               ; ADC 0 EOC INTERRUPT FLAG
AD0TM equ 0xEE                ; ADC 0 TRACK MODE
AD0EN equ 0xEF                ; ADC 0 ENABLE

; ADC2CN E8H 
AD2WINT equ 0xE8              ; ADC 2 WINDOW INTERRUPT FLAG
AD2CM0 equ 0xE9               ; ADC 2 CONVERT START MODE BIT 0
AD2CM1 equ 0xEA               ; ADC 2 CONVERT START MODE BIT 1
AD2CM2 equ 0xEB               ; ADC 2 CONVERT START MODE BIT 2
AD2BUSY equ 0xEC              ; ADC 2 BUSY FLAG
AD2INT equ 0xED               ; ADC 2 EOC INTERRUPT FLAG
AD2TM equ 0xEE                ; ADC 2 TRACK MODE
AD2EN equ 0xEF                ; ADC 2 ENABLE

; SPI0CN F8H 
SPIEN equ 0xF8                ; SPI 0 SPI ENABLE
TXBMT equ 0xF9                ; SPI 0 TX BUFFER EMPTY FLAG
NSSMD0 equ 0xFA               ; SPI 0 SLAVE SELECT MODE 0
NSSMD1 equ 0xFB               ; SPI 0 SLAVE SELECT MODE 1
RXOVRN equ 0xFC               ; SPI 0 RX OVERRUN FLAG
MODF equ 0xFD                 ; SPI 0 MODE FAULT FLAG
WCOL equ 0xFE                 ; SPI 0 WRITE COLLISION FLAG
SPIF equ 0xFF                 ; SPI 0 INTERRUPT FLAG

; Predefined SFR Bit Masks 
PCON_IDLE equ 0x01            ; PCON
PCON_STOP equ 0x02            ; PCON
ECCF equ 0x01                 ; PCA0CPMn
PWM equ 0x02                  ; PCA0CPMn
TOG equ 0x04                  ; PCA0CPMn
MAT equ 0x08                  ; PCA0CPMn
CAPN equ 0x10                 ; PCA0CPMn
CAPP equ 0x20                 ; PCA0CPMn
ECOM equ 0x40                 ; PCA0CPMn
PWM16 equ 0x80                ; PCA0CPMn
PINRSF equ 0x01               ; RSTSRC
PORSF equ 0x02                ; RSTSRC
MCDRSF equ 0x04               ; RSTSRC
WDTRSF equ 0x08               ; RSTSRC
SWRSF equ 0x10                ; RSTSRC
C0RSEF equ 0x20               ; RSTSRC
CNVRSEF equ 0x40              ; RSTSRC

; SFR PAGE DEFINITIONS 
CONFIG_PAGE equ 0x0F          ; SYSTEM AND PORT CONFIGURATION PAGE
LEGACY_PAGE equ 0x00          ; LEGACY SFR PAGE
TIMER01_PAGE equ 0x00         ; TIMER 0 AND TIMER 1
CPT0_PAGE equ 0x01            ; COMPARATOR 0
CPT1_PAGE equ 0x02            ; COMPARATOR 1
UART0_PAGE equ 0x00           ; UART 0
UART1_PAGE equ 0x01           ; UART 1
SPI0_PAGE equ 0x00            ; SPI 0
EMI0_PAGE equ 0x00            ; EXTERNAL MEMORY INTERFACE
ADC0_PAGE equ 0x00            ; ADC 0
ADC2_PAGE equ 0x02            ; ADC 2
SMB0_PAGE equ 0x00            ; SMBUS 0
TMR2_PAGE equ 0x00            ; TIMER 2
TMR3_PAGE equ 0x01            ; TIMER 3
TMR4_PAGE equ 0x02            ; TIMER 4
DAC0_PAGE equ 0x00            ; DAC 0
DAC1_PAGE equ 0x01            ; DAC 1
PCA0_PAGE equ 0x00            ; PCA 0
PLL0_PAGE equ 0x0F            ; PLL 0
MAC0_PAGE equ 0x03            ; MULTIPLY / ACCUMULATE 0
