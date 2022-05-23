; C8051F920.h -Register Declarations for the SiLabs C8051F92x-93x
; Processor Range
; 
; Copyright (C) 2009, Steven Borley, steven.borley@partnerelectronics.com
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

; Page 0x00 (and all pages) 
P0 equ 0x80                   ; PORT 0
SP equ 0x81                   ; STACK POINTER
DPL equ 0x82                  ; DATA POINTER - LOW BYTE
DPH equ 0x83                  ; DATA POINTER - HIGH BYTE
SPI1CFG equ 0x84              ; SPI1 Configuration
SPI1CKR equ 0x85              ; SPI1 Clock Rate Control
SPI1DAT equ 0x86              ; SPI1 Data
PCON equ 0x87                 ; POWER CONTROL
TCON equ 0x88                 ; TIMER CONTROL
TMOD equ 0x89                 ; TIMER MODE
TL0 equ 0x8A                  ; TIMER 0 - LOW BYTE
TL1 equ 0x8B                  ; TIMER 1 - LOW BYTE
TH0 equ 0x8C                  ; TIMER 0 - HIGH BYTE
TH1 equ 0x8D                  ; TIMER 1 - HIGH BYTE
CKCON equ 0x8E                ; CLOCK CONTROL
PSCTL equ 0x8F                ; Program Store R/W Control
P1 equ 0x90                   ; PORT 1
TMR3CN equ 0x91               ; TIMER 3 CONTROL
TMR3RLL equ 0x92              ; TIMER 3 CAPTURE REGISTER - LOW BYTE
TMR3RLH equ 0x93              ; TIMER 3 CAPTURE REGISTER - HIGH BYTE
TMR3L equ 0x94                ; TIMER 3 - LOW BYTE
TMR3H equ 0x95                ; TIMER 3 - HIGH BYTE
DC0CF equ 0x96                ; DC0 (DC-DC Converter Configuration
DC0CN equ 0x97                ; DC0 (DC-DC Converter Control
SCON0 equ 0x98                ; Serial Port Control
SBUF0 equ 0x99                ; Serial Port Buffer
CPT1CN equ 0x9A               ; Comparator 1 Control
CPT0CN equ 0x9B               ; Comparator 0 Control
CPT1MD equ 0x9C               ; Comparator 1 Mode Selection
CPT0MD equ 0x9D               ; Comparator 0 Mode Selection
CPT1MX equ 0x9E               ; Comparator 1 mux selection
CPT0MX equ 0x9F               ; Comparator 0 mux selection
P2 equ 0xA0                   ; PORT 2
SPI0CFG equ 0xA1              ; SPI0 CONFIGURATION
SPI0CKR equ 0xA2              ; SPI0 Clock Rate Control
SPI0DAT equ 0xA3              ; SPI0 Data
P0MDOUT equ 0xA4              ; PORT 0 OUTPUT MODE CONFIGURATION
P1MDOUT equ 0xA5              ; PORT 1 OUTPUT MODE CONFIGURATION
P2MDOUT equ 0xA6              ; PORT 2 OUTPUT MODE CONFIGURATION
SFRPAGE equ 0xA7              ; SFR Page
IE equ 0xA8                   ; INTERRUPT ENABLE
CLKSEL equ 0xA9               ; SYSTEM CLOCK SELECT
EMI0CN equ 0xAA               ; EMIF Control
_XPAGE equ 0xAA               ; XDATA/PDATA page alias for SDCC
EMI0CF equ 0xAB               ; EMIF Configuration
RTC0ADR equ 0xAC              ; RTC0 Address
RTC0DAT equ 0xAD              ; RTC0 Data
RTC0KEY equ 0xAE              ; RTC0 Key
EMI0TC equ 0xAF               ; EMIF Timing Control
SPI1CN equ 0xB0               ; SPI1 Control
OSCXCN equ 0xB1               ; EXTERNAL OSCILLATOR CONTROL
OSCICN equ 0xB2               ; INTERNAL OSCILLATOR CONTROL
OSCICL equ 0xB3               ; INTERNAL OSCILLATOR CALIBRATION
PMU0CF equ 0xB5               ; PMU0 Configuration
FLSCL equ 0xB6                ; IFlash Scale
FLKEY equ 0xB7                ; Flash Lock And Key
IP equ 0xB8                   ; INTERRUPT PRIORITY
IREF0CN equ 0xB9              ; Current Reference IREF Control
ADC0AC equ 0xBA               ; ADC0 Accumulator Configuration
ADC0MX equ 0xBB               ; AMUX0 Channel Select
ADC0CF equ 0xBC               ; ADC 0 CONFIGURATION
ADC0L equ 0xBD                ; ADC 0 DATA - LOW BYTE
ADC0H equ 0xBE                ; ADC 0 DATA - HIGH BYTE
P1MASK equ 0xBF               ; Port 1 Mask
SMB0CN equ 0xC0               ; SMBUS CONTROL
SMB0CF equ 0xC1               ; SMBUS CONFIGURATION
SMB0DAT equ 0xC2              ; SMBUS DATA
ADC0GTL equ 0xC3              ; ADC 0 GREATER-THAN REGISTER - LOW BYTE
ADC0GTH equ 0xC4              ; ADC 0 GREATER-THAN REGISTER - HIGH BYTE
ADC0LTL equ 0xC5              ; ADC 0 LESS-THAN REGISTER - LOW BYTE
ADC0LTH equ 0xC6              ; ADC 0 LESS-THAN REGISTER - HIGH BYTE
P0MASK equ 0xC7               ; Port 0 Mask
TMR2CN equ 0xC8               ; Timer 2 control
REG0CN equ 0xC9               ; Voltage Regulator (VREG0 Control
TMR2RLL equ 0xCA              ; Timer 2 capture register - low byte
TMR2RLH equ 0xCB              ; Timer 2 capture register - high byte
TMR2L equ 0xCC                ; Timer 2 - low byte
TMR2H equ 0xCD                ; Timer 2 - high byte
PCA0CPM5 equ 0xCE             ; PCA0 Module 5 Mode Register
P1MAT equ 0xCF                ; Port 1 Match
PSW equ 0xD0                  ; PROGRAM STATUS WORD
REF0CN equ 0xD1               ; VOLTAGE REFERENCE 0 CONTROL
PCA0CPL5 equ 0xD2             ; PCA0 Capture 5 Low
PCA0CPH5 equ 0xD3             ; PCA0 Capture 5 High
P0SKIP equ 0xD4               ; PORT 0 SKIP
P1SKIP equ 0xD5               ; PORT 1 SKIP
P2SKIP equ 0xD6               ; PORT 2 SKIP
P0MAT equ 0xD7                ; Port 0 Match
PCA0CN equ 0xD8               ; PCA CONTROL
PCA0MD equ 0xD9               ; PCA MODE
PCA0CPM0 equ 0xDA             ; PCA0 Module 0 Mode Register
PCA0CPM1 equ 0xDB             ; PCA0 Module 1 Mode Register
PCA0CPM2 equ 0xDC             ; PCA0 Module 2 Mode Register
PCA0CPM3 equ 0xDD             ; PCA0 Module 3 Mode Register
PCA0CPM4 equ 0xDE             ; PCA0 Module 4 Mode Register
PCA0PWM equ 0xDF              ; PCA0 PWM Configuration
ACC equ 0xE0                  ; ACCUMULATOR
XBR0 equ 0xE1                 ; Port Mux Configuration Register 0
XBR1 equ 0xE2                 ; Port Mux Configuration Register 1
XBR2 equ 0xE3                 ; Port Mux Configuration Register 2
INT01CF equ 0xE4              ; INT0/INT1 Configuration Register
EIE1 equ 0xE6                 ; EXTERNAL INTERRUPT ENABLE 1
EIE2 equ 0xE7                 ; EXTERNAL INTERRUPT ENABLE 2
ADC0CN equ 0xE8               ; ADC 0 CONTROL
PCA0CPL1 equ 0xE9             ; PCA CAPTURE 1 LOW
PCA0CPH1 equ 0xEA             ; PCA CAPTURE 1 HIGH
PCA0CPL2 equ 0xEB             ; PCA CAPTURE 2 LOW
PCA0CPH2 equ 0xEC             ; PCA CAPTURE 2 HIGH
PCA0CPL3 equ 0xED             ; PCA0 Capture 3 Low
PCA0CPH3 equ 0xEE             ; PCA0 Capture 3 High
RSTSRC equ 0xEF               ; RESET SOURCE
B equ 0xF0                    ; B REGISTER
P0MDIN equ 0xF1               ; Port 0 Input Mode Configuration
P1MDIN equ 0xF2               ; Port 1 Input Mode Configuration
P2MDIN equ 0xF3               ; Port 2 Input Mode Configuration
SMB0ADR equ 0xF4              ; SMBus Slave Address
SMB0ADM equ 0xF5              ; SMBus Slave Address Mask
EIP1 equ 0xF6                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 1
EIP2 equ 0xF7                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 2
SPI0CN equ 0xF8               ; SPI0 Control
PCA0L equ 0xF9                ; PCA COUNTER LOW
PCA0H equ 0xFA                ; PCA COUNTER HIGH
PCA0CPL0 equ 0xFB             ; PCA CAPTURE 0 LOW
PCA0CPH0 equ 0xFC             ; PCA CAPTURE 0 HIGH
PCA0CPL4 equ 0xFD             ; PCA0 Capture 4 Low
PCA0CPH4 equ 0xFE             ; PCA0 Capture 4 High
VDM0CN equ 0xFF               ; VDD Monitor Control

; Page 0x0F only 
TOFFL equ 0x85                ; Temperature Offset Low
TOFFH equ 0x86                ; Temperature Offset High
CRC0DAT equ 0x91              ; CRC0 Data
CRC0CN equ 0x92               ; CRC0 Control
CRC0IN equ 0x93               ; CRC0 Input
CRC0FLIP equ 0x95             ; CRC0 Flip
CRC0AUTO equ 0x96             ; CRC0 Automatic Control
CRC0CNT equ 0x97              ; CRC0 Automatic Flash Sector Count
P0DRV equ 0xA4                ; Port 0 Drive Strength
P1DRV equ 0xA5                ; Port 1 Drive Strength
P2DRV equ 0xA6                ; Port 2 Drive Strength
ADC0PWR equ 0xBA              ; ADC0 Burst Mode Power-Up Time
ADC0TK equ 0xBD               ; ADC0 Tracking Control

;  WORD/DWORD Registers  

; page 0x00 
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
TMR3RL equ 0x9392             ; Timer 3 reload word
TMR3 equ 0x9594               ; Timer 3 counter word
ADC0 equ 0xBEBD               ; ADC0 word
ADC0GT equ 0xC4C3             ; ADC 0 GREATER-THAN REGISTER WORD
ADC0LT equ 0xC6C5             ; ADC 0 LESS-THAN REGISTER WORD
TMR2RL equ 0xCBCA             ; Timer 2 reload word
TMR2 equ 0xCDCC               ; Timer 2 counter word
PCA0 equ 0xFAF9               ; PCA0 counter word
PCA0CP0 equ 0xFCFB            ; PCA0 Capture 0 word
PCA0CP1 equ 0xEAE9            ; PCA0 Capture 1 word
PCA0CP2 equ 0xECEB            ; PCA0 Capture 2 word
PCA0CP3 equ 0xEEED            ; PCA0 Capture 3 word
PCA0CP4 equ 0xFEFD            ; PCA0 Capture 4 word
PCA0CP5 equ 0xD3D4            ; PCA0 Capture 5 word

; Page 0x0F 
TOFF equ 0x8685               ; TEMPERATURE SENSOR OFFSET WORD

;  BIT Registers  

;  TCON  0x88 
IT0 equ 0x88+0                ; TCON.0 - EXT. INTERRUPT 0 TYPE
IE0 equ 0x88+1                ; TCON.1 - EXT. INTERRUPT 0 EDGE FLAG
IT1 equ 0x88+2                ; TCON.2 - EXT. INTERRUPT 1 TYPE
IE1 equ 0x88+3                ; TCON.3 - EXT. INTERRUPT 1 EDGE FLAG
TR0 equ 0x88+4                ; TCON.4 - TIMER 0 ON/OFF CONTROL
TF0 equ 0x88+5                ; TCON.5 - TIMER 0 OVERFLOW FLAG
TR1 equ 0x88+6                ; TCON.6 - TIMER 1 ON/OFF CONTROL
TF1 equ 0x88+7                ; TCON.7 - TIMER 1 OVERFLOW FLAG

;  SCON0  0x98 
RI equ 0x98+0                 ; SCON.0 - RECEIVE INTERRUPT FLAG
RI0 equ 0x98+0                ; SCON.0 - RECEIVE INTERRUPT FLAG
TI equ 0x98+1                 ; SCON.1 - TRANSMIT INTERRUPT FLAG
TI0 equ 0x98+1                ; SCON.1 - TRANSMIT INTERRUPT FLAG
RB8 equ 0x98+2                ; SCON.2 - RECEIVE BIT 8
RB80 equ 0x98+2               ; SCON.2 - RECEIVE BIT 8
TB8 equ 0x98+3                ; SCON.3 - TRANSMIT BIT 8
TB80 equ 0x98+3               ; SCON.3 - TRANSMIT BIT 8
REN equ 0x98+4                ; SCON.4 - RECEIVE ENABLE
REN0 equ 0x98+4               ; SCON.4 - RECEIVE ENABLE
SM2 equ 0x98+5                ; SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE
MCE0 equ 0x98+5               ; SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE
SM0 equ 0x98+7                ; SCON.7 - SERIAL MODE CONTROL BIT 0
S0MODE equ 0x98+7             ; SCON.7 - SERIAL MODE CONTROL BIT 0

;  IE  0xA8 
EX0 equ 0xA8+0                ; IE.0 - EXTERNAL INTERRUPT 0 ENABLE
ET0 equ 0xA8+1                ; IE.1 - TIMER 0 INTERRUPT ENABLE
EX1 equ 0xA8+2                ; IE.2 - EXTERNAL INTERRUPT 1 ENABLE
ET1 equ 0xA8+3                ; IE.3 - TIMER 1 INTERRUPT ENABLE
ES equ 0xA8+4                 ; IE.4 - SERIAL PORT INTERRUPT ENABLE
ES0 equ 0xA8+4                ; IE.4 - SERIAL PORT INTERRUPT ENABLE
ET2 equ 0xA8+5                ; IE.5 - TIMER 2 INTERRUPT ENABLE
IEGF0 equ 0xA8+6              ; IE.6 - GENERAL PURPOSE FLAG 0
EA equ 0xA8+7                 ; IE.7 - GLOBAL INTERRUPT ENABLE

; SPI1CN 0xB0 
SPI1EN equ 0xB0+0             ; SPI1 Enable
TXBMT1 equ 0xB0+1             ; SPI1 Transmit Buffer Empty
NSS1MD0 equ 0xB0+2            ; SPI1 Slave Select Mode bit-0
NSS1MD1 equ 0xB0+3            ; SPI1 Slave Select Mode bit-1
RXOVRN1 equ 0xB0+4            ; SPI1 Receive Overrun Flag
MODF1 equ 0xB0+5              ; SPI1 Mode Fault Flag
WCOL1 equ 0xB0+6              ; SPI1 Write Collision Flag
SPIF1 equ 0xB0+7              ; SPI1 Interrupt Flag

;  IP  0xB8 
PX0 equ 0xB8+0                ; IP.0 - EXTERNAL INTERRUPT 0 PRIORITY
PT0 equ 0xB8+1                ; IP.1 - TIMER 0 PRIORITY
PX1 equ 0xB8+2                ; IP.2 - EXTERNAL INTERRUPT 1 PRIORITY
PT1 equ 0xB8+3                ; IP.3 - TIMER 1 PRIORITY
PS equ 0xB8+4                 ; IP.4 - SERIAL PORT PRIORITY
PS0 equ 0xB8+4                ; IP.4 - SERIAL PORT PRIORITY
PT2 equ 0xB8+5                ; IP.5 - TIMER 2 PRIORITY

;  SMB0CN  0xC0 
SI equ 0xC0+0                 ; SMB0CN.0 - SMBUS 0 INTERRUPT PENDING FLAG
ACK equ 0xC0+1                ; SMB0CN.1 - SMBUS 0 ACKNOWLEDGE FLAG
ARBLOST equ 0xC0+2            ; SMB0CN.2 - SMBUS 0 ARBITRATION LOST INDICATOR
ACKRQ equ 0xC0+3              ; SMB0CN.3 - SMBUS 0 ACKNOWLEDGE REQUEST
STO equ 0xC0+4                ; SMB0CN.4 - SMBUS 0 STOP FLAG
STA equ 0xC0+5                ; SMB0CN.5 - SMBUS 0 START FLAG
TXMODE equ 0xC0+6             ; SMB0CN.6 - SMBUS 0 TRANSMIT MODE INDICATOR
MASTER equ 0xC0+7             ; SMB0CN.7 - SMBUS 0 MASTER/SLAVE INDICATOR

;  TMR2CN  0xC8 
T2XCLK equ 0xC8+0             ; TMR2CN.0 - TIMER 2 EXTERNAL CLOCK SELECT
TR2 equ 0xC8+2                ; TMR2CN.2 - TIMER 2 ON/OFF CONTROL
T2SPLIT equ 0xC8+3            ; TMR2CN.3 - TIMER 2 SPLIT MODE ENABLE
TF2LEN equ 0xC8+5             ; TMR2CN.5 - TIMER 2 LOW BYTE INTERRUPT ENABLE
TF2L equ 0xC8+6               ; TMR2CN.6 - TIMER 2 LOW BYTE OVERFLOW FLAG
TF2 equ 0xC8+7                ; TMR2CN.7 - TIMER 2 OVERFLOW FLAG
TF2H equ 0xC8+7               ; TMR2CN.7 - TIMER 2 HIGH BYTE OVERFLOW FLAG

;  PSW  0xD0 
PARITY equ 0xD0+0             ; PSW.0 - ACCUMULATOR PARITY FLAG
F1 equ 0xD0+1                 ; PSW.1 - FLAG 1
OV equ 0xD0+2                 ; PSW.2 - OVERFLOW FLAG
RS0 equ 0xD0+3                ; PSW.3 - REGISTER BANK SELECT 0
RS1 equ 0xD0+4                ; PSW.4 - REGISTER BANK SELECT 1
F0 equ 0xD0+5                 ; PSW.5 - FLAG 0
AC equ 0xD0+6                 ; PSW.6 - AUXILIARY CARRY FLAG
CY equ 0xD0+7                 ; PSW.7 - CARRY FLAG

;  PCA0CN  0xD8 
CCF0 equ 0xD8+0               ; PCA0CN.0 - PCA MODULE 0 CAPTURE/COMPARE FLAG
CCF1 equ 0xD8+1               ; PCA0CN.1 - PCA MODULE 1 CAPTURE/COMPARE FLAG
CCF2 equ 0xD8+2               ; PCA0CN.2 - PCA MODULE 2 CAPTURE/COMPARE FLAG
CR equ 0xD8+6                 ; PCA0CN.6 - PCA COUNTER/TIMER RUN CONTROL
CF equ 0xD8+7                 ; PCA0CN.7 - PCA COUNTER/TIMER OVERFLOW FLAG

;  ADC0CN  0xE8 
AD0CM0 equ 0xE8+0             ; ADC0CN.0 - ADC 0 START OF CONV. MODE BIT 0
AD0CM1 equ 0xE8+1             ; ADC0CN.1 - ADC 0 START OF CONV. MODE BIT 1
AD0CM2 equ 0xE8+2             ; ADC0CN.2 - ADC 0 START OF CONV. MODE BIT 2
AD0WINT equ 0xE8+3            ; ADC0CN.3 - ADC 0 WINDOW COMPARE INT. FLAG
AD0BUSY equ 0xE8+4            ; ADC0CN.4 - ADC 0 BUSY FLAG
AD0INT equ 0xE8+5             ; ADC0CN.5 - ADC 0 CONV. COMPLETE INT. FLAG
AD0TM equ 0xE8+6              ; ADC0CN.6 - ADC 0 TRACK MODE
AD0EN equ 0xE8+7              ; ADC0CN.7 - ADC 0 ENABLE

;  SPI0CN  0xF8 
SPI0EN equ 0xF8+0             ; SPI0 Enable
TXBMT0 equ 0xF8+1             ; SPI0 Transmit Buffer Empty
NSS0MD0 equ 0xF8+2            ; SPI0 Slave Select Mode bit-0
NSS0MD1 equ 0xF8+3            ; SPI0 Slave Select Mode bit-1
RXOVRN0 equ 0xF8+4            ; SPI0 Receive Overrun Flag
MODF0 equ 0xF8+5              ; SPI0 Mode Fault Flag
WCOL0 equ 0xF8+6              ; SPI0 Write Collision Flag
SPIF0 equ 0xF8+7              ; SPI0 Interrupt Flag

; Indirectly accessed registers 

; smaRTClock Internal Registers 
CAPTURE0 equ 0x00             ; smaRTClock Capture register 0
CAPTURE1 equ 0x01             ; smaRTClock Capture register 1
CAPTURE2 equ 0x02             ; smaRTClock Capture register 2
CAPTURE3 equ 0x03             ; smaRTClock Capture register 3
RTC0CN equ 0x04               ; smaRTClock Control
RTC0XCN equ 0x05              ; smaRTClock Oscillator Control
RTC0XCF equ 0x06              ; smaRTClock Oscillator Configuration
RTC0PIN equ 0x07              ; smaRTClock Pin Configuration
ALARM0 equ 0x08               ; smaRTClock Alarm Register 0
ALARM1 equ 0x09               ; smaRTClock Alarm Register 1
ALARM2 equ 0x0A               ; smaRTClock Alarm Register 2
ALARM3 equ 0x0B               ; smaRTClock Alarm Register 3

; Predefined SFR Bit Masks 

; PCON 0x87 
PCON_IDLE equ (1<<0)          ; PCON
PCON_STOP equ (1<<1)          ; PCON

; CKON 0x8E  
T0M equ (1<<2)                ; CKCON Timer 0 Clock Select
T1M equ (1<<3)                ; CKCON Timer 1 Clock Select

; PSCTL 0x8F 
PSWE equ (1<<0)               ; Program Store Write Enable
PSEE equ (1<<1)               ; Program Store Erase Enable
SFLE equ (1<<2)               ; Scratchpad Flash Access Enable

; EIE1 0xE6 
ESMB0 equ (1<<0)              ; Enable SMBus (SMB0) Interrupt
ERTC0A equ (1<<1)             ; Enable smaRTClock Alarm Interrupts
EWADC0 equ (1<<2)             ; Enable Window Comparison ADC0 Int.
EADC0 equ (1<<3)              ; Enable ADC0 Convert Complete Int.
EPCA0 equ (1<<4)              ; Enable PCA0 Interrupt
ECP0 equ (1<<5)               ; Enable Comparator0 (CP0) Interrupt
ECP1 equ (1<<6)               ; Enable Comparator1 (CP1) Interrupt
ET3 equ (1<<7)                ; Enable Timer 3 Interrupt

; RSTSRC 
PINRSF equ (1<<0)             ; HW Pin Reset Flag
PORSF equ (1<<1)              ; Power-on/fail Reset Rlag
MCDRSF equ (1<<2)             ; Missing Clock Detector Reset Rlag
WDTRSF equ (1<<3)             ; Watchdog Timer Reset Rlag
SWRSF equ (1<<4)              ; Software Force/Reset Rlag
C0RSEF equ (1<<5)             ; Comparator0 Reset Rlag
FERROR equ (1<<6)             ; Flash Error Reset Rlag
RTC0RE equ (1<<7)             ; smaRTClock Reset Rlag

; PCA0CPMn 
ECCF equ (1<<0)               ; Capture/Compare Flag Interrupt En.
PWM equ (1<<1)                ; Pulse Width Modulation Mode Enable
TOG equ (1<<2)                ; Toggle Function Enable
MAT equ (1<<3)                ; Match Function Enable
CAPN equ (1<<4)               ; Capture Negative Function Enable
CAPP equ (1<<5)               ; Capture Positive Function Enable.
ECOM equ (1<<6)               ; Comparator Function Enable.
PWM16 equ (1<<7)              ; 16-bit Pulse Width Modulation Enable

; XBR0 0xE1 
URT0E equ (1<<0)              ; UART0 I/O enable
SPI0E equ (1<<1)              ; SPI0 I/O Enable
SMB0E equ (1<<2)              ; SMBus I/O Enable
SYSCKE equ (1<<3)             ; SYSCLK Output Enable.
CP0E equ (1<<4)               ; Comparator0 Output Enable
CP0AE equ (1<<5)              ; Comparator0 Asynchronous Output En.
CP1E equ (1<<6)               ; Comparator1 Output Enable
CP1AE equ (1<<7)              ; Comparator1 Asynchronous Output En.

; XBR1 0xE2 
PCA0ME0 equ (1<<0)            ; PCA0 Module I/O Enable bit-0
PCA0ME1 equ (1<<1)            ; PCA0 Module I/O Enable bit-1
PCA0ME2 equ (1<<2)            ; PCA0 Module I/O Enable bit-2
ECIE equ (1<<3)               ; PCA0 Ext. Counter Input Enable
T0E equ (1<<4)                ; Timer0 Input Enable
T1E equ (1<<5)                ; Timer1 Input Enable
SPI1E equ (1<<6)              ; SPI1 I/O Enable

; XBR2 0xE3 
XBARE equ (1<<6)              ; Crossbar Enable
WEAKPUD equ (1<<7)            ; Port I/O Weak Pullup Disable

; Interrupts 
INT_EXT0 equ 0                ; External Interrupt 0
INT_TIMER0 equ 1              ; Timer0 Overflow
INT_EXT1 equ 2                ; External Interrupt 1
INT_TIMER1 equ 3              ; Timer1 Overflow
INT_UART0 equ 4               ; Serial Port 0
INT_TIMER2 equ 5              ; Timer2 Overflow
INT_SPI0 equ 6                ; SPI0
INT_SMBUS0 equ 7              ; SMBus0 Interface
INT_ALARM equ 8               ; smaRTClock Alarm
INT_ADC0_WINDOW equ 9         ; ADC0 Window Comparison
INT_ADC0_EOC equ 10           ; ADC0 End Of Conversion
INT_PCA0 equ 11               ; PCA0 Peripheral
INT_CP0 equ 12                ; Comparator 0
INT_CP1 equ 13                ; Comparator 1
INT_TIMER3 equ 14             ; Timer3 Overflow
INT_VWARN equ 15              ; VDD/DC+ Supply Monitor early warning
INT_MATCH equ 16              ; Port Match
INT_OSCFAIL equ 17            ; smaRTClock Oscillator Fail
INT_SPI1 equ 18               ; SPI1

; aliases - these map alternative names to names use in the datasheet 
SCON equ SCON0                ; Serial Port Control
SBUF equ SBUF0                ; Serial Port Buffer
T2CON equ TMR2CN              ; Timer 2 control
RCAP2 equ TMR2RL              ; Timer 2 capture register word
RCAP2L equ TMR2RLL            ; Timer 2 capture register - low byte
RCAP2H equ TMR2RLH            ; Timer 2 capture register - high byte
T2 equ TMR2                   ; Timer 2 - word
TL2 equ TMR2L                 ; Timer 2 - low byte
TH2 equ TMR2H                 ; Timer 2 - high byte
PRT0MX equ XBR0               ; Port Mux Configuration Register 0
PRT1MX equ XBR1               ; Port Mux Configuration Register 1
PRT2MX equ XBR2               ; Port Mux Configuration Register 2
IT01CF equ INT01CF            ; INT0/INT1 Configuration Register
P0MODE equ P0MDIN             ; Port 0 Input Mode Configuration
P1MODE equ P1MDIN             ; Port 1 Input Mode Configuration
P2MODE equ P2MDIN             ; Port 2 Input Mode Configuration
CP0OEN equ CP0E               ; Comparator 0 Output Enable bit
CP0AOEN equ CP0AE             ; Comparator 0 Asynchronous Output En. bit
