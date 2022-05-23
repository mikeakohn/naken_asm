; C8051F340.h - Register Declarations for the Cygnal/SiLabs C8051F34x
; Processor Range
; 
; Copyright (C) 2006, Maarten Brock, sourceforge.brock@dse.nl
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
P0 equ 0x80                   ; PORT 0
SP equ 0x81                   ; STACK POINTER
DPL equ 0x82                  ; DATA POINTER - LOW BYTE
DPH equ 0x83                  ; DATA POINTER - HIGH BYTE
EMI0TC equ 0x84               ; EXTERNAL MEMORY INTERFACE TIMING
EMI0CF equ 0x85               ; EXTERNAL MEMORY INTERFACE CONFIGURATION
OSCLCN equ 0x86               ; LOW-FREQUENCY OSCILLATOR CONTROL
PCON equ 0x87                 ; POWER CONTROL
TCON equ 0x88                 ; TIMER CONTROL
TMOD equ 0x89                 ; TIMER MODE
TL0 equ 0x8A                  ; TIMER 0 - LOW BYTE
TL1 equ 0x8B                  ; TIMER 1 - LOW BYTE
TH0 equ 0x8C                  ; TIMER 0 - HIGH BYTE
TH1 equ 0x8D                  ; TIMER 1 - HIGH BYTE
CKCON equ 0x8E                ; CLOCK CONTROL
PSCTL equ 0x8F                ; PROGRAM STORE R/W CONTROL
P1 equ 0x90                   ; PORT 1
TMR3CN equ 0x91               ; TIMER 3 CONTROL
TMR3RLL equ 0x92              ; TIMER 3 CAPTURE REGISTER - LOW BYTE
TMR3RLH equ 0x93              ; TIMER 3 CAPTURE REGISTER - HIGH BYTE
TMR3L equ 0x94                ; TIMER 3 - LOW BYTE
TMR3H equ 0x95                ; TIMER 3 - HIGH BYTE
USB0ADR equ 0x96              ; USB0 INDIRECT ADDRESS REGISTER
USB0DAT equ 0x97              ; USB0 DATA REGISTER
SCON equ 0x98                 ; SERIAL PORT CONTROL
SCON0 equ 0x98                ; SERIAL PORT CONTROL
SBUF equ 0x99                 ; SERIAL PORT BUFFER
SBUF0 equ 0x99                ; SERIAL PORT BUFFER
CPT1CN equ 0x9A               ; COMPARATOR 1 CONTROL
CPT0CN equ 0x9B               ; COMPARATOR 0 CONTROL
CPT1MD equ 0x9C               ; COMPARATOR 1 MODE SELECTION
CPT0MD equ 0x9D               ; COMPARATOR 0 MODE SELECTION
CPT1MX equ 0x9E               ; COMPARATOR 1 MUX SELECTION
CPT0MX equ 0x9F               ; COMPARATOR 0 MUX SELECTION
P2 equ 0xA0                   ; PORT 2
SPI0CFG equ 0xA1              ; SPI0 CONFIGURATION
SPI0CKR equ 0xA2              ; SPI0 CLOCK RATE CONTROL
SPI0DAT equ 0xA3              ; SPI0 DATA
P0MDOUT equ 0xA4              ; PORT 0 OUTPUT MODE CONFIGURATION
P1MDOUT equ 0xA5              ; PORT 1 OUTPUT MODE CONFIGURATION
P2MDOUT equ 0xA6              ; PORT 2 OUTPUT MODE CONFIGURATION
P3MDOUT equ 0xA7              ; PORT 3 OUTPUT MODE CONFIGURATION
IE equ 0xA8                   ; INTERRUPT ENABLE
CLKSEL equ 0xA9               ; SYSTEM CLOCK SELECT
EMI0CN equ 0xAA               ; EXTERNAL MEMORY INTERFACE CONTROL
_XPAGE equ 0xAA               ; XDATA/PDATA PAGE
SBCON1 equ 0xAC               ; UART 1 BAUDRATE GENERATOR CONTROL
P4MDOUT equ 0xAE              ; PORT 4 OUTPUT MODE CONFIGURATION
PFE0CN equ 0xAF               ; PREFETCH ENGINE CONTROL
P3 equ 0xB0                   ; PORT 3
OSCXCN equ 0xB1               ; EXTERNAL OSCILLATOR CONTROL
OSCICN equ 0xB2               ; INTERNAL OSCILLATOR CONTROL
OSCICL equ 0xB3               ; INTERNAL OSCILLATOR CALIBRATION
SBRLL1 equ 0xB4               ; UART 1 BAUDRATE GENERATOR - LOW BYTE
SBRLH1 equ 0xB5               ; UART 1 BAUDRATE GENERATOR - HIGH BYTE
FLSCL equ 0xB6                ; FLASH MEMORY TIMING PRESCALER
FLKEY equ 0xB7                ; FLASH ACESS LIMIT
IP equ 0xB8                   ; INTERRUPT PRIORITY
CLKMUL equ 0xB9               ; CLOCK MULTIPLIER CONTROL REGISTER
AMX0N equ 0xBA                ; ADC 0 MUX NEGATIVE CHANNEL SELECTION
AMX0P equ 0xBB                ; ADC 0 MUX POSITIVE CHANNEL SELECTION
ADC0CF equ 0xBC               ; ADC 0 CONFIGURATION
ADC0L equ 0xBD                ; ADC 0 DATA WORD LSB
ADC0H equ 0xBE                ; ADC 0 DATA WORD MSB
SMB0CN equ 0xC0               ; SMBUS CONTROL
SMB0CF equ 0xC1               ; SMBUS CONFIGURATION
SMB0DAT equ 0xC2              ; SMBUS DATA
ADC0GTL equ 0xC3              ; ADC 0 GREATER-THAN LOW BYTE
ADC0GTH equ 0xC4              ; ADC 0 GREATER-THAN HIGH BYTE
ADC0LTL equ 0xC5              ; ADC 0 LESS-THAN LOW BYTE
ADC0LTH equ 0xC6              ; ADC 0 LESS-THAN HIGH BYTE
P4 equ 0xC7                   ; PORT 4
T2CON equ 0xC8                ; TIMER 2 CONTROL
TMR2CN equ 0xC8               ; TIMER 2 CONTROL
REG0CN equ 0xC9               ; VOLTAGE REGULATOR CONTROL
RCAP2L equ 0xCA               ; TIMER 2 CAPTURE REGISTER - LOW BYTE
TMR2RLL equ 0xCA              ; TIMER 2 CAPTURE REGISTER - LOW BYTE
RCAP2H equ 0xCB               ; TIMER 2 CAPTURE REGISTER - HIGH BYTE
TMR2RLH equ 0xCB              ; TIMER 2 CAPTURE REGISTER - HIGH BYTE
TL2 equ 0xCC                  ; TIMER 2 - LOW BYTE
TMR2L equ 0xCC                ; TIMER 2 - LOW BYTE
TH2 equ 0xCD                  ; TIMER 2 - HIGH BYTE
TMR2H equ 0xCD                ; TIMER 2 - HIGH BYTE
PSW equ 0xD0                  ; PROGRAM STATUS WORD
REF0CN equ 0xD1               ; VOLTAGE REFERENCE 0 CONTROL
SCON1 equ 0xD2                ; UART 1 CONTROL
SBUF1 equ 0xD3                ; UART 1 DATA BUFFER
P0SKIP equ 0xD4               ; PORT 0 SKIP
P1SKIP equ 0xD5               ; PORT 1 SKIP
P2SKIP equ 0xD6               ; PORT 2 SKIP
USB0XCN equ 0xD7              ; USB0 TRANSCEIVER CONTROL
PCA0CN equ 0xD8               ; PCA CONTROL
PCA0MD equ 0xD9               ; PCA MODE
PCA0CPM0 equ 0xDA             ; PCA MODULE 0 MODE REGISTER
PCA0CPM1 equ 0xDB             ; PCA MODULE 1 MODE REGISTER
PCA0CPM2 equ 0xDC             ; PCA MODULE 2 MODE REGISTER
PCA0CPM3 equ 0xDD             ; PCA MODULE 3 MODE REGISTER
PCA0CPM4 equ 0xDE             ; PCA MODULE 4 MODE REGISTER
P3SKIP equ 0xDF               ; PORT 3 SKIP
ACC equ 0xE0                  ; ACCUMULATOR
XBR0 equ 0xE1                 ; PORT MUX CONFIGURATION REGISTER 0
XBR1 equ 0xE2                 ; PORT MUX CONFIGURATION REGISTER 1
XBR2 equ 0xE3                 ; PORT MUX CONFIGURATION REGISTER 2
IT01CF equ 0xE4               ; INT0/INT1 CONFIGURATION REGISTER
INT01CF equ 0xE4              ; INT0/INT1 CONFIGURATION REGISTER
SMOD1 equ 0xE5                ; UART 1 MODE
EIE1 equ 0xE6                 ; EXTERNAL INTERRUPT ENABLE 1
EIE2 equ 0xE7                 ; EXTERNAL INTERRUPT ENABLE 2
ADC0CN equ 0xE8               ; ADC 0 CONTROL
PCA0CPL1 equ 0xE9             ; PCA CAPTURE 1 LOW
PCA0CPH1 equ 0xEA             ; PCA CAPTURE 1 HIGH
PCA0CPL2 equ 0xEB             ; PCA CAPTURE 2 LOW
PCA0CPH2 equ 0xEC             ; PCA CAPTURE 2 HIGH
PCA0CPL3 equ 0xED             ; PCA CAPTURE 3 LOW
PCA0CPH3 equ 0xEE             ; PCA CAPTURE 3 HIGH
RSTSRC equ 0xEF               ; RESET SOURCE
B equ 0xF0                    ; B REGISTER
P0MODE equ 0xF1               ; PORT 0 INPUT MODE CONFIGURATION
P0MDIN equ 0xF1               ; PORT 0 INPUT MODE CONFIGURATION
P1MODE equ 0xF2               ; PORT 1 INPUT MODE CONFIGURATION
P1MDIN equ 0xF2               ; PORT 1 INPUT MODE CONFIGURATION
P2MODE equ 0xF3               ; PORT 2 INPUT MODE CONFIGURATION
P2MDIN equ 0xF3               ; PORT 2 INPUT MODE CONFIGURATION
P3MODE equ 0xF4               ; PORT 3 INPUT MODE CONFIGURATION
P3MDIN equ 0xF4               ; PORT 3 INPUT MODE CONFIGURATION
P4MDIN equ 0xF5               ; PORT 4 INPUT MODE CONFIGURATION
EIP1 equ 0xF6                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 1
EIP2 equ 0xF7                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 2
SPI0CN equ 0xF8               ; SPI0 CONTROL
PCA0L equ 0xF9                ; PCA COUNTER LOW
PCA0H equ 0xFA                ; PCA COUNTER HIGH
PCA0CPL0 equ 0xFB             ; PCA CAPTURE 0 LOW
PCA0CPH0 equ 0xFC             ; PCA CAPTURE 0 HIGH
PCA0CPL4 equ 0xFD             ; PCA CAPTURE 4 LOW
PCA0CPH4 equ 0xFE             ; PCA CAPTURE 4 HIGH
VDM0CN equ 0xFF               ; VDD MONITOR CONTROL

;  WORD/DWORD Registers  
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
TMR2 equ 0xCDCC               ; TIMER 2 COUNTER
RCAP2 equ 0xCBCA              ; TIMER 2 CAPTURE REGISTER WORD
TMR2RL equ 0xCBCA             ; TIMER 2 CAPTURE REGISTER WORD
TMR3 equ 0x9594               ; TIMER 3 COUNTER
TMR3RL equ 0x9392             ; TIMER 3 CAPTURE REGISTER WORD
SBRL1 equ 0xB5B4              ; UART 1 BAUDRATE GENERATOR WORD
ADC0 equ 0xBEBD               ; ADC 0 DATA WORD
ADC0GT equ 0xC4C3             ; ADC 0 GREATER-THAN REGISTER WORD
ADC0LT equ 0xC6C5             ; ADC 0 LESS-THAN REGISTER WORD
PCA0 equ 0xFAF9               ; PCA COUNTER
PCA0CP0 equ 0xFCFB            ; PCA CAPTURE 0 WORD
PCA0CP1 equ 0xEAE9            ; PCA CAPTURE 1 WORD
PCA0CP2 equ 0xECEB            ; PCA CAPTURE 2 WORD
PCA0CP3 equ 0xEEED            ; PCA CAPTURE 3 WORD
PCA0CP4 equ 0xFEFD            ; PCA CAPTURE 4 WORD

;  BIT Registers  

;  TCON  0x88 
IT0 equ 0x88                  ; TCON.0 - EXT. INTERRUPT 0 TYPE
IE0 equ 0x89                  ; TCON.1 - EXT. INTERRUPT 0 EDGE FLAG
IT1 equ 0x8A                  ; TCON.2 - EXT. INTERRUPT 1 TYPE
IE1 equ 0x8B                  ; TCON.3 - EXT. INTERRUPT 1 EDGE FLAG
TR0 equ 0x8C                  ; TCON.4 - TIMER 0 ON/OFF CONTROL
TF0 equ 0x8D                  ; TCON.5 - TIMER 0 OVERFLOW FLAG
TR1 equ 0x8E                  ; TCON.6 - TIMER 1 ON/OFF CONTROL
TF1 equ 0x8F                  ; TCON.7 - TIMER 1 OVERFLOW FLAG

;  SCON  0x98 
RI equ 0x98                   ; SCON.0 - RECEIVE INTERRUPT FLAG
RI0 equ 0x98                  ; SCON.0 - RECEIVE INTERRUPT FLAG
TI equ 0x99                   ; SCON.1 - TRANSMIT INTERRUPT FLAG
TI0 equ 0x99                  ; SCON.1 - TRANSMIT INTERRUPT FLAG
RB8 equ 0x9A                  ; SCON.2 - RECEIVE BIT 8
RB80 equ 0x9A                 ; SCON.2 - RECEIVE BIT 8
TB8 equ 0x9B                  ; SCON.3 - TRANSMIT BIT 8
TB80 equ 0x9B                 ; SCON.3 - TRANSMIT BIT 8
REN equ 0x9C                  ; SCON.4 - RECEIVE ENABLE
REN0 equ 0x9C                 ; SCON.4 - RECEIVE ENABLE
SM2 equ 0x9D                  ; SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE
MCE0 equ 0x9D                 ; SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE
SM0 equ 0x9F                  ; SCON.7 - SERIAL MODE CONTROL BIT 0
S0MODE equ 0x9F               ; SCON.7 - SERIAL MODE CONTROL BIT 0

;  IE  0xA8 
EX0 equ 0xA8                  ; IE.0 - EXTERNAL INTERRUPT 0 ENABLE
ET0 equ 0xA9                  ; IE.1 - TIMER 0 INTERRUPT ENABLE
EX1 equ 0xAA                  ; IE.2 - EXTERNAL INTERRUPT 1 ENABLE
ET1 equ 0xAB                  ; IE.3 - TIMER 1 INTERRUPT ENABLE
ES equ 0xAC                   ; IE.4 - SERIAL PORT INTERRUPT ENABLE
ES0 equ 0xAC                  ; IE.4 - SERIAL PORT INTERRUPT ENABLE
ET2 equ 0xAD                  ; IE.5 - TIMER 2 INTERRUPT ENABLE
ESPI0 equ 0xAE                ; IE.6 - SPI0 INTERRUPT ENABLE
EA equ 0xAF                   ; IE.7 - GLOBAL INTERRUPT ENABLE

;  IP  0xB8 
PX0 equ 0xB8                  ; IP.0 - EXTERNAL INTERRUPT 0 PRIORITY
PT0 equ 0xB9                  ; IP.1 - TIMER 0 PRIORITY
PX1 equ 0xBA                  ; IP.2 - EXTERNAL INTERRUPT 1 PRIORITY
PT1 equ 0xBB                  ; IP.3 - TIMER 1 PRIORITY
PS equ 0xBC                   ; IP.4 - SERIAL PORT PRIORITY
PS0 equ 0xBC                  ; IP.4 - SERIAL PORT PRIORITY
PT2 equ 0xBD                  ; IP.5 - TIMER 2 PRIORITY
PSPI0 equ 0xBE                ; IP.6 - SPI0 PRIORITY

;  SMB0CN  0xC0 
SI equ 0xC0                   ; SMB0CN.0 - SMBUS 0 INTERRUPT PENDING FLAG
ACK equ 0xC1                  ; SMB0CN.1 - SMBUS 0 ACKNOWLEDGE FLAG
ARBLOST equ 0xC2              ; SMB0CN.2 - SMBUS 0 ARBITRATION LOST INDICATOR
ACKRQ equ 0xC3                ; SMB0CN.3 - SMBUS 0 ACKNOWLEDGE REQUEST
STO equ 0xC4                  ; SMB0CN.4 - SMBUS 0 STOP FLAG
STA equ 0xC5                  ; SMB0CN.5 - SMBUS 0 START FLAG
TXMODE equ 0xC6               ; SMB0CN.6 - SMBUS 0 TRANSMIT MODE INDICATOR
MASTER equ 0xC7               ; SMB0CN.7 - SMBUS 0 MASTER/SLAVE INDICATOR

;  TMR2CN  0xC8 
T2XCLK equ 0xC8               ; TMR2CN.0 - TIMER 2 EXTERNAL CLOCK SELECT
T2CSS equ 0xC9                ; TMR2CN.1 - TIMER 2 CAPTURE SOURCE SELECT
TR2 equ 0xCA                  ; TMR2CN.2 - TIMER 2 ON/OFF CONTROL
T2SPLIT equ 0xCB              ; TMR2CN.3 - TIMER 2 SPLIT MODE ENABLE
T2CE equ 0xCC                 ; TMR2CN.4 - TIMER 2 CAPTURE ENABLE
TF2LEN equ 0xCD               ; TMR2CN.5 - TIMER 2 LOW BYTE INTERRUPT ENABLE
TF2L equ 0xCE                 ; TMR2CN.6 - TIMER 2 LOW BYTE OVERFLOW FLAG
TF2 equ 0xCF                  ; TMR2CN.7 - TIMER 2 OVERFLOW FLAG
TF2H equ 0xCF                 ; TMR2CN.7 - TIMER 2 HIGH BYTE OVERFLOW FLAG

;  PSW  0xD0 
PARITY equ 0xD0               ; PSW.0 - ACCUMULATOR PARITY FLAG
F1 equ 0xD1                   ; PSW.1 - FLAG 1
OV equ 0xD2                   ; PSW.2 - OVERFLOW FLAG
RS0 equ 0xD3                  ; PSW.3 - REGISTER BANK SELECT 0
RS1 equ 0xD4                  ; PSW.4 - REGISTER BANK SELECT 1
F0 equ 0xD5                   ; PSW.5 - FLAG 0
AC equ 0xD6                   ; PSW.6 - AUXILIARY CARRY FLAG
CY equ 0xD7                   ; PSW.7 - CARRY FLAG

;  PCA0CN  0xD8 
CCF0 equ 0xD8                 ; PCA0CN.0 - PCA MODULE 0 CAPTURE/COMPARE FLAG
CCF1 equ 0xD9                 ; PCA0CN.1 - PCA MODULE 1 CAPTURE/COMPARE FLAG
CCF2 equ 0xDA                 ; PCA0CN.2 - PCA MODULE 2 CAPTURE/COMPARE FLAG
CCF3 equ 0xDB                 ; PCA0CN.3 - PCA MODULE 3 CAPTURE/COMPARE FLAG
CCF4 equ 0xDC                 ; PCA0CN.4 - PCA MODULE 4 CAPTURE/COMPARE FLAG
CR equ 0xDE                   ; PCA0CN.6 - PCA COUNTER/TIMER RUN CONTROL
CF equ 0xDF                   ; PCA0CN.7 - PCA COUNTER/TIMER OVERFLOW FLAG

;  ADC0CN  0xE8 
AD0CM0 equ 0xE8               ; ADC0CN.0 - ADC 0 START OF CONV. MODE BIT 0
AD0CM1 equ 0xE9               ; ADC0CN.1 - ADC 0 START OF CONV. MODE BIT 1
AD0CM2 equ 0xEA               ; ADC0CN.2 - ADC 0 START OF CONV. MODE BIT 2
AD0WINT equ 0xEB              ; ADC0CN.3 - ADC 0 WINDOW COMPARE INT. FLAG
AD0BUSY equ 0xEC              ; ADC0CN.4 - ADC 0 BUSY FLAG
AD0INT equ 0xED               ; ADC0CN.5 - ADC 0 CONV. COMPLETE INT. FLAG
AD0TM equ 0xEE                ; ADC0CN.6 - ADC 0 TRACK MODE
AD0EN equ 0xEF                ; ADC0CN.7 - ADC 0 ENABLE

;  SPI0CN  0xF8 
SPIEN equ 0xF8                ; SPI0CN.0 - SPI0 ENABLE
TXBMT equ 0xF9                ; SPI0CN.1 - TRANSMIT BUFFER EMPTY
NSSMD0 equ 0xFA               ; SPI0CN.2 - SLAVE SELECT MODE BIT 0
NSSMD1 equ 0xFB               ; SPI0CN.3 - SLAVE SELECT MODE BIT 1
RXOVRN equ 0xFC               ; SPI0CN.4 - RECEIVE OVERRUN FLAG
MODF equ 0xFD                 ; SPI0CN.5 - MODE FAULT FLAG
WCOL equ 0xFE                 ; SPI0CN.6 - WRITE COLLISION FLAG
SPIF equ 0xFF                 ; SPI0CN.7 - SPI0 INTERRUPT FLAG

; Predefined SFR Bit Masks 
PCON_IDLE equ 0x01            ; PCON
PCON_STOP equ 0x02            ; PCON
T1M equ 0x08                  ; CKCON
PSWE equ 0x01                 ; PSCTL
PSEE equ 0x02                 ; PSCTL
ECP0 equ 0x20                 ; EIE1
ECP1 equ 0x40                 ; EIE1
PORSF equ 0x02                ; RSTSRC
SWRSF equ 0x10                ; RSTSRC
ECCF equ 0x01                 ; PCA0CPMn
PWM equ 0x02                  ; PCA0CPMn
TOG equ 0x04                  ; PCA0CPMn
MAT equ 0x08                  ; PCA0CPMn
CAPN equ 0x10                 ; PCA0CPMn
CAPP equ 0x20                 ; PCA0CPMn
ECOM equ 0x40                 ; PCA0CPMn
PWM16 equ 0x80                ; PCA0CPMn
CP0E equ 0x10                 ; XBR0
CP0AE equ 0x20                ; XBR0
CP1E equ 0x40                 ; XBR0
CP1AE equ 0x80                ; XBR0
