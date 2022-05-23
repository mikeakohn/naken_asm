; C8051F200.h - Register Declarations for the Cygnal/SiLabs C8051F2xx
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
PCON equ 0x87                 ; POWER Control
TCON equ 0x88                 ; TIMER Control
TMOD equ 0x89                 ; TIMER MODE
TL0 equ 0x8A                  ; TIMER 0 - LOW BYTE
TL1 equ 0x8B                  ; TIMER 1 - LOW BYTE
TH0 equ 0x8C                  ; TIMER 0 - HIGH BYTE
TH1 equ 0x8D                  ; TIMER 1 - HIGH BYTE
CKCON equ 0x8E                ; CLOCK Control
PSCTL equ 0x8F                ; PROGRAM STORE R/W Control
P1 equ 0x90                   ; PORT 1
SCON equ 0x98                 ; SERIAL PORT Control
SBUF equ 0x99                 ; SERIAL PORT BUFFER
SPI0CFG equ 0x9A              ; SERIAL PERIPHERAL INTERFACE 0 Configuration
SPI0DAT equ 0x9B              ; SERIAL PERIPHERAL INTERFACE 0 DATA
SPI0CKR equ 0x9D              ; SERIAL PERIPHERAL INTERFACE 0 CLOCK RATE Control
CPT0CN equ 0x9E               ; COMPARATOR 0 Control
CPT1CN equ 0x9F               ; COMPARATOR 1 Control
P2 equ 0xA0                   ; PORT 2
PRT0CF equ 0xA4               ; PORT 0 OUTPUT MODE Configuration
PRT1CF equ 0xA5               ; PORT 1 OUTPUT MODE Configuration
PRT2CF equ 0xA6               ; PORT 2 OUTPUT MODE Configuration
PRT3CF equ 0xA7               ; PORT 3 OUTPUT MODE Configuration
IE equ 0xA8                   ; Interrupt Enable
SWCINT equ 0xAD               ; SOFTWARE-Controlled Interrupt FLAGS
PRT1IF equ 0xAD               ; SOFTWARE-Controlled Interrupt FLAGS (LEGACY NAME equ
EMI0CN equ 0xAF               ; EXTERNAL MEMORY INTERFACE Control (F206/F226/F236 equ
_XPAGE equ 0xAF               ; XDATA/PDATA PAGE
P3 equ 0xB0                   ; PORT 3
OSCXCN equ 0xB1               ; EXTERNAL OSCILLATOR Control
OSCICN equ 0xB2               ; INTERNAL OSCILLATOR Control
FLSCL equ 0xB6                ; FLASH MEMORY TIMING PRESCALER
FLACL equ 0xB7                ; FLASH ACESS LIMIT
IP equ 0xB8                   ; Interrupt Priority
AMX0SL equ 0xBB               ; ADC 0 MUX CHANNEL SELECTION (Not on F230/1/6 equ
ADC0CF equ 0xBC               ; ADC 0 Configuration (Not on F230/1/6 equ
ADC0L equ 0xBE                ; ADC 0 Data LOW ( F206 only equ
ADC0H equ 0xBF                ; ADC 0 Data High
ADC0GTL equ 0xC4              ; ADC 0 GREATER-THAN Register LOW( F206 only equ
ADC0GTH equ 0xC5              ; ADC 0 GREATER-THAN Register (Not on F230/1/6 equ
ADC0LTL equ 0xC6              ; ADC 0 LESS-THAN Register LOW ( F206 only equ
ADC0LTH equ 0xC7              ; ADC 0 LESS-THAN Register (Not on F230/1/6 equ
T2CON equ 0xC8                ; TIMER 2 Control
RCAP2L equ 0xCA               ; TIMER 2 CAPTURE Register - LOW BYTE
RCAP2H equ 0xCB               ; TIMER 2 CAPTURE Register - HIGH BYTE
TL2 equ 0xCC                  ; TIMER 2 - LOW BYTE
TH2 equ 0xCD                  ; TIMER 2 - HIGH BYTE
PSW equ 0xD0                  ; PROGRAM STATUS WORD
REF0CN equ 0xD1               ; VOLTAGE REFERENCE 0 Control
ACC equ 0xE0                  ; ACCUMULATOR
PRT0MX equ 0xE1               ; PORT MUX Configuration Register 0
PRT1MX equ 0xE2               ; PORT MUX Configuration Register 1
PRT2MX equ 0xE3               ; PORT MUX Configuration Register 2
EIE1 equ 0xE6                 ; EXTERNAL Interrupt Enable 1
EIE2 equ 0xE7                 ; EXTERNAL Interrupt Enable 2
ADC0CN equ 0xE8               ; ADC 0 Control (Not on F230/1/6 equ
RSTSRC equ 0xEF               ; RESET SOURCE
B equ 0xF0                    ; B Register
P0MODE equ 0xF1               ; PORT 0 INPUT MODE Configuration
P1MODE equ 0xF2               ; PORT 1 INPUT MODE Configuration
P2MODE equ 0xF3               ; PORT 2 INPUT MODE Configuration
P3MODE equ 0xF4               ; PORT 3 INPUT MODE Configuration (Not on F221/F231 equ
EIP1 equ 0xF6                 ; EXTERNAL Interrupt Priority Register 1
EIP2 equ 0xF7                 ; EXTERNAL Interrupt Priority Register 2
SPI0CN equ 0xF8               ; SERIAL PERIPHERAL INTERFACE 0 Control
WDTCN equ 0xFF                ; WATCHDOG TIMER Control

;  WORD/DWORD Registers  
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
TMR2 equ 0xCDCC               ; TIMER 2 COUNTER
RCAP2 equ 0xCBCA              ; TIMER 2 CAPTURE REGISTER WORD
ADC0 equ 0xBFBE               ; ADC 0 DATA WORD
ADC0GT equ 0xC5C4             ; ADC 0 GREATER-THAN REGISTER WORD
ADC0LT equ 0xC7C6             ; ADC 0 LESS-THAN REGISTER WORD

;  BIT Registers  

;  TCON  0x88 
IT0 equ 0x88                  ; EXT. Interrupt 0 TYPE
IE0 equ 0x89                  ; EXT. Interrupt 0 EDGE FLAG
IT1 equ 0x8A                  ; EXT. Interrupt 1 TYPE
IE1 equ 0x8B                  ; EXT. Interrupt 1 EDGE FLAG
TR0 equ 0x8C                  ; TIMER 0 ON/OFF Control
TF0 equ 0x8D                  ; TIMER 0 Overflow FLAG
TR1 equ 0x8E                  ; TIMER 1 ON/OFF Control
TF1 equ 0x8F                  ; TIMER 1 Overflow FLAG

;  SCON  0x98 
RI equ 0x98                   ; RECEIVE Interrupt FLAG
TI equ 0x99                   ; TRANSMIT Interrupt FLAG
RB8 equ 0x9A                  ; RECEIVE BIT 8
TB8 equ 0x9B                  ; TRANSMIT BIT 8
REN equ 0x9C                  ; RECEIVE Enable
SM2 equ 0x9D                  ; MULTIPROCESSOR COMMUNICATION Enable
SM1 equ 0x9E                  ; SERIAL MODE Control BIT 1
SM0 equ 0x9F                  ; SERIAL MODE Control BIT 0

;  IE  0xA8 
EX0 equ 0xA8                  ; EXTERNAL Interrupt 0 Enable
ET0 equ 0xA9                  ; TIMER 0 Interrupt Enable
EX1 equ 0xAA                  ; EXTERNAL Interrupt 1 Enable
ET1 equ 0xAB                  ; TIMER 1 Interrupt Enable
ES equ 0xAC                   ; SERIAL PORT Interrupt Enable
ET2 equ 0xAD                  ; TIMER 2 Interrupt Enable
; Bit 6 not used
EA equ 0xAF                   ; GLOBAL Interrupt Enable

;  IP  0xB8 
PX0 equ 0xB8                  ; EXTERNAL Interrupt 0 Priority
PT0 equ 0xB9                  ; TIMER 0 Priority
PX1 equ 0xBA                  ; EXTERNAL Interrupt 1 Priority
PT1 equ 0xBB                  ; TIMER 1 Priority
PS equ 0xBC                   ; SERIAL PORT Priority
PT2 equ 0xBD                  ; TIMER 2 Priority
; Bit 6 not used
; Bit 7 not used

;  T2CON  0xC8 
CPRL2 equ 0xC8                ; CAPTURE OR RELOAD SELECT
CT2 equ 0xC9                  ; TIMER OR COUNTER SELECT
TR2 equ 0xCA                  ; TIMER 2 ON/OFF Control
EXEN2 equ 0xCB                ; TIMER 2 EXTERNAL Enable FLAG
TCLK equ 0xCC                 ; TRANSMIT CLOCK FLAG
RCLK equ 0xCD                 ; RECEIVE CLOCK FLAG
EXF2 equ 0xCE                 ; EXTERNAL FLAG
TF2 equ 0xCF                  ; TIMER 2 Overflow FLAG

;  PSW  0xD0 
P equ 0xD0                    ; ACCUMULATOR PARITY FLAG
F1 equ 0xD1                   ; USER FLAG 1
OV equ 0xD2                   ; Overflow FLAG
RS0 equ 0xD3                  ; Register BANK SELECT 0
RS1 equ 0xD4                  ; Register BANK SELECT 1
F0 equ 0xD5                   ; USER FLAG 0
AC equ 0xD6                   ; AUXILIARY CARRY FLAG
CY equ 0xD7                   ; CARRY FLAG

; ADC0CN 0xE8 
ADLJST equ 0xE8               ; Left Justify Data (F206 only equ
ADWINT equ 0xE9               ; WINDOW COMPARE Interrupt FLAG
ADSTM0 equ 0xEA               ; START OF CONVERSION MODE BIT 0
ADSTM1 equ 0xEB               ; START OF CONVERSION MODE BIT 1
ADBUSY equ 0xEC               ; BUSY FLAG
ADCINT equ 0xED               ; CONVERISION COMPLETE Interrupt FLAG
ADCTM equ 0xEE                ; TRACK MODE
ADCEN equ 0xEF                ; Enable

; SPI0CN 0xF8 
SPIEN equ 0xF8                ; SPI Enable
MSTEN equ 0xF9                ; MASTER Enable
SLVSEL equ 0xFA               ; SLAVE SELECT
TXBSY equ 0xFB                ; TX BUSY FLAG
RXOVRN equ 0xFC               ; RX OVERRUN FLAG
MODF equ 0xFD                 ; MODE FAULT FLAG
WCOL equ 0xFE                 ; WRITE COLLISION FLAG
SPIF equ 0xFF                 ; Interrupt FLAG
