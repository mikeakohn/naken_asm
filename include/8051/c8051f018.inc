; C8051F018.h - Register Declarations for the Cygnal/SiLabs C8051F018-F019
; Processor Range
; 
; Copyright (C) 2004, Maarten Brock, sourceforge.brock@dse.nl
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
TMR3RLL equ 0x92              ; TIMER 3 RELOAD REGISTER - LOW BYTE
TMR3RLH equ 0x93              ; TIMER 3 RELOAD REGISTER - HIGH BYTE
TMR3L equ 0x94                ; TIMER 3 - LOW BYTE
TMR3H equ 0x95                ; TIMER 3 - HIGH BYTE
SCON equ 0x98                 ; SERIAL PORT CONTROL
SBUF equ 0x99                 ; SERIAL PORT BUFFER
SPI0CFG equ 0x9A              ; SERIAL PERIPHERAL INTERFACE 0 CONFIGURATION
SPI0DAT equ 0x9B              ; SERIAL PERIPHERAL INTERFACE 0 DATA
SPI0CKR equ 0x9D              ; SERIAL PERIPHERAL INTERFACE 0 CLOCK RATE CONTROL
CPT0CN equ 0x9E               ; COMPARATOR 0 CONTROL
CPT1CN equ 0x9F               ; COMPARATOR 1 CONTROL
P2 equ 0xA0                   ; PORT 2
PRT0CF equ 0xA4               ; PORT 0 CONFIGURATION
PRT1CF equ 0xA5               ; PORT 1 CONFIGURATION
PRT2CF equ 0xA6               ; PORT 2 CONFIGURATION
PRT3CF equ 0xA7               ; PORT 3 CONFIGURATION
IE equ 0xA8                   ; INTERRUPT ENABLE
PRT1IF equ 0xAD               ; PORT 1 EXTERNAL INTERRUPT FLAGS
EMI0CN equ 0xAF               ; EXTERNAL MEMORY INTERFACE CONTROL
_XPAGE equ 0xAF               ; XDATA/PDATA PAGE
P3 equ 0xB0                   ; PORT 3
OSCXCN equ 0xB1               ; EXTERNAL OSCILLATOR CONTROL
OSCICN equ 0xB2               ; INTERNAL OSCILLATOR CONTROL
FLSCL equ 0xB6                ; FLASH MEMORY TIMING PRESCALER
FLACL equ 0xB7                ; FLASH ACESS LIMIT
IP equ 0xB8                   ; INTERRUPT PRIORITY
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
T2CON equ 0xC8                ; TIMER 2 CONTROL
RCAP2L equ 0xCA               ; TIMER 2 CAPTURE REGISTER - LOW BYTE
RCAP2H equ 0xCB               ; TIMER 2 CAPTURE REGISTER - HIGH BYTE
TL2 equ 0xCC                  ; TIMER 2 - LOW BYTE
TH2 equ 0xCD                  ; TIMER 2 - HIGH BYTE
SMB0CR equ 0xCF               ; SMBUS 0 CLOCK RATE
PSW equ 0xD0                  ; PROGRAM STATUS WORD
REF0CN equ 0xD1               ; VOLTAGE REFERENCE 0 CONTROL
PCA0CN equ 0xD8               ; PCA 0 COUNTER CONTROL
PCA0MD equ 0xD9               ; PCA 0 COUNTER MODE
PCA0CPM0 equ 0xDA             ; CONTROL REGISTER FOR PCA 0 MODULE 0
PCA0CPM1 equ 0xDB             ; CONTROL REGISTER FOR PCA 0 MODULE 1
PCA0CPM2 equ 0xDC             ; CONTROL REGISTER FOR PCA 0 MODULE 2
PCA0CPM3 equ 0xDD             ; CONTROL REGISTER FOR PCA 0 MODULE 3
PCA0CPM4 equ 0xDE             ; CONTROL REGISTER FOR PCA 0 MODULE 4
ACC equ 0xE0                  ; ACCUMULATOR
XBR0 equ 0xE1                 ; DIGITAL CROSSBAR CONFIGURATION REGISTER 0
XBR1 equ 0xE2                 ; DIGITAL CROSSBAR CONFIGURATION REGISTER 1
XBR2 equ 0xE3                 ; DIGITAL CROSSBAR CONFIGURATION REGISTER 2
EIE1 equ 0xE6                 ; EXTERNAL INTERRUPT ENABLE 1
EIE2 equ 0xE7                 ; EXTERNAL INTERRUPT ENABLE 2
ADC0CN equ 0xE8               ; ADC 0 CONTROL
PCA0L equ 0xE9                ; PCA 0 TIMER - LOW BYTE
PCA0CPL0 equ 0xEA             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 0 - LOW BYTE
PCA0CPL1 equ 0xEB             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 1 - LOW BYTE
PCA0CPL2 equ 0xEC             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 2 - LOW BYTE
PCA0CPL3 equ 0xED             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 3 - LOW BYTE
PCA0CPL4 equ 0xEE             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 4 - LOW BYTE
RSTSRC equ 0xEF               ; RESET SOURCE
B equ 0xF0                    ; B REGISTER
EIP1 equ 0xF6                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 1
EIP2 equ 0xF7                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 2
SPI0CN equ 0xF8               ; SERIAL PERIPHERAL INTERFACE 0 CONTROL
PCA0H equ 0xF9                ; PCA 0 TIMER - HIGH BYTE
PCA0CPH0 equ 0xFA             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 0 - HIGH BYTE
PCA0CPH1 equ 0xFB             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 1 - HIGH BYTE
PCA0CPH2 equ 0xFC             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 2 - HIGH BYTE
PCA0CPH3 equ 0xFD             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 3 - HIGH BYTE
PCA0CPH4 equ 0xFE             ; CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 4 - HIGH BYTE
WDTCN equ 0xFF                ; WATCHDOG TIMER CONTROL

;  WORD/DWORD Registers  
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
TMR2 equ 0xCDCC               ; TIMER 2 COUNTER
RCAP2 equ 0xCBCA              ; TIMER 2 CAPTURE REGISTER WORD
TMR3 equ 0x9594               ; TIMER 3 COUNTER
TMR3RL equ 0x9392             ; TIMER 3 CAPTURE REGISTER WORD
ADC0 equ 0xBFBE               ; ADC 0 DATA WORD
ADC0GT equ 0xC5C4             ; ADC 0 GREATER-THAN REGISTER WORD
ADC0LT equ 0xC7C6             ; ADC 0 LESS-THAN REGISTER WORD
PCA0 equ 0xF9E9               ; PCA COUNTER
PCA0CP0 equ 0xFAEA            ; PCA CAPTURE 0 WORD
PCA0CP1 equ 0xFBEB            ; PCA CAPTURE 1 WORD
PCA0CP2 equ 0xFCEC            ; PCA CAPTURE 2 WORD
PCA0CP3 equ 0xFDED            ; PCA CAPTURE 3 WORD
PCA0CP4 equ 0xFEEE            ; PCA CAPTURE 4 WORD

;  BIT Registers  

;  TCON  0x88 
TF1 equ 0x8F                  ; TIMER 1 OVERFLOW FLAG
TR1 equ 0x8E                  ; TIMER 1 ON/OFF CONTROL
TF0 equ 0x8D                  ; TIMER 0 OVERFLOW FLAG
TR0 equ 0x8C                  ; TIMER 0 ON/OFF CONTROL
IE1 equ 0x8B                  ; EXT. INTERRUPT 1 EDGE FLAG
IT1 equ 0x8A                  ; EXT. INTERRUPT 1 TYPE
IE0 equ 0x89                  ; EXT. INTERRUPT 0 EDGE FLAG
IT0 equ 0x88                  ; EXT. INTERRUPT 0 TYPE

;  SCON  0x98 
SM0 equ 0x9F                  ; SERIAL MODE CONTROL BIT 0
SM1 equ 0x9E                  ; SERIAL MODE CONTROL BIT 1
SM2 equ 0x9D                  ; MULTIPROCESSOR COMMUNICATION ENABLE
REN equ 0x9C                  ; RECEIVE ENABLE
TB8 equ 0x9B                  ; TRANSMIT BIT 8
RB8 equ 0x9A                  ; RECEIVE BIT 8
TI equ 0x99                   ; TRANSMIT INTERRUPT FLAG
RI equ 0x98                   ; RECEIVE INTERRUPT FLAG

;  IE  0xA8 
EA equ 0xAF                   ; GLOBAL INTERRUPT ENABLE
ET2 equ 0xAD                  ; TIMER 2 INTERRUPT ENABLE
ES equ 0xAC                   ; SERIAL PORT INTERRUPT ENABLE
ET1 equ 0xAB                  ; TIMER 1 INTERRUPT ENABLE
EX1 equ 0xAA                  ; EXTERNAL INTERRUPT 1 ENABLE
ET0 equ 0xA9                  ; TIMER 0 INTERRUPT ENABLE
EX0 equ 0xA8                  ; EXTERNAL INTERRUPT 0 ENABLE

;  IP  0xB8 
PT2 equ 0xBD                  ; TIMER 2 PRIORITY
PS equ 0xBC                   ; SERIAL PORT PRIORITY
PT1 equ 0xBB                  ; TIMER 1 PRIORITY
PX1 equ 0xBA                  ; EXTERNAL INTERRUPT 1 PRIORITY
PT0 equ 0xB9                  ; TIMER 0 PRIORITY
PX0 equ 0xB8                  ; EXTERNAL INTERRUPT 0 PRIORITY

;  SMB0CN  0xC0 
BUSY equ 0xC7                 ; SMBUS 0 BUSY
ENSMB equ 0xC6                ; SMBUS 0 ENABLE
STA equ 0xC5                  ; SMBUS 0 START FLAG
STO equ 0xC4                  ; SMBUS 0 STOP FLAG
SI equ 0xC3                   ; SMBUS 0 INTERRUPT PENDING FLAG
AA equ 0xC2                   ; SMBUS 0 ASSERT/ACKNOWLEDGE FLAG
SMBFTE equ 0xC1               ; SMBUS 0 FREE TIMER ENABLE
SMBTOE equ 0xC0               ; SMBUS 0 TIMEOUT ENABLE

;  T2CON  0xC8 
TF2 equ 0xCF                  ; TIMER 2 OVERFLOW FLAG
EXF2 equ 0xCE                 ; EXTERNAL FLAG
RCLK equ 0xCD                 ; RECEIVE CLOCK FLAG
TCLK equ 0xCC                 ; TRANSMIT CLOCK FLAG
EXEN2 equ 0xCB                ; TIMER 2 EXTERNAL ENABLE FLAG
TR2 equ 0xCA                  ; TIMER 2 ON/OFF CONTROL
CT2 equ 0xC9                  ; TIMER OR COUNTER SELECT
CPRL2 equ 0xC8                ; CAPTURE OR RELOAD SELECT

;  PSW  0xD0 
CY equ 0xD7                   ; CARRY FLAG
AC equ 0xD6                   ; AUXILIARY CARRY FLAG
F0 equ 0xD5                   ; USER FLAG 0
RS1 equ 0xD4                  ; REGISTER BANK SELECT 1
RS0 equ 0xD3                  ; REGISTER BANK SELECT 0
OV equ 0xD2                   ; OVERFLOW FLAG
F1 equ 0xD1                   ; USER FLAG 1
P equ 0xD0                    ; ACCUMULATOR PARITY FLAG

;  PCA0CN  0xD8H 
CF equ 0xDF                   ; PCA 0 COUNTER OVERFLOW FLAG
CR equ 0xDE                   ; PCA 0 COUNTER RUN CONTROL BIT
CCF4 equ 0xDC                 ; PCA 0 MODULE 4 INTERRUPT FLAG
CCF3 equ 0xDB                 ; PCA 0 MODULE 3 INTERRUPT FLAG
CCF2 equ 0xDA                 ; PCA 0 MODULE 2 INTERRUPT FLAG
CCF1 equ 0xD9                 ; PCA 0 MODULE 1 INTERRUPT FLAG
CCF0 equ 0xD8                 ; PCA 0 MODULE 0 INTERRUPT FLAG

;  ADC0CN  0xE8H 
AD0EN equ 0xEF                ; ADC 0 ENABLE
AD0TM equ 0xEE                ; ADC 0 TRACK MODE
AD0INT equ 0xED               ; ADC 0 CONVERISION COMPLETE INTERRUPT FLAG
AD0BUSY equ 0xEC              ; ADC 0 BUSY FLAG
ADSTM1 equ 0xEB               ; ADC 0 START OF CONVERSION MODE BIT 1
ADSTM0 equ 0xEA               ; ADC 0 START OF CONVERSION MODE BIT 0
AD0WINT equ 0xE9              ; ADC 0 WINDOW COMPARE INTERRUPT FLAG
ADLJST equ 0xE8               ; ADC 0 RIGHT JUSTIFY DATA BIT

;  SPI0CN  0xF8H 
SPIF equ 0xFF                 ; SPI 0 INTERRUPT FLAG
WCOL equ 0xFE                 ; SPI 0 WRITE COLLISION FLAG
MODF equ 0xFD                 ; SPI 0 MODE FAULT FLAG
RXOVRN equ 0xFC               ; SPI 0 RX OVERRUN FLAG
TXBSY equ 0xFB                ; SPI 0 TX BUSY FLAG
SLVSEL equ 0xFA               ; SPI 0 SLAVE SELECT
MSTEN equ 0xF9                ; SPI 0 MASTER ENABLE
SPIEN equ 0xF8                ; SPI 0 SPI ENABLE

; Predefined SFR Bit Masks 
PCON_IDLE equ 0x01            ; PCON
PCON_STOP equ 0x02            ; PCON
TF3 equ 0x80                  ; TMR3CN
CPFIF equ 0x10                ; CPTnCN
CPRIF equ 0x20                ; CPTnCN
CPOUT equ 0x40                ; CPTnCN
ECCF equ 0x01                 ; PCA0CPMn
PWM equ 0x02                  ; PCA0CPMn
TOG equ 0x04                  ; PCA0CPMn
MAT equ 0x08                  ; PCA0CPMn
CAPN equ 0x10                 ; PCA0CPMn
CAPP equ 0x20                 ; PCA0CPMn
ECOM equ 0x40                 ; PCA0CPMn
