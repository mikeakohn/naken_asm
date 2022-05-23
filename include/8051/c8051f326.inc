; C8051F326.h - Register Declarations for the Cygnal/SiLabs C8051F326/7
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
PCON equ 0x87                 ; POWER CONTROL
TCON equ 0x88                 ; TIMER CONTROL
TMOD equ 0x89                 ; TIMER MODE
TL0 equ 0x8A                  ; TIMER 0 - LOW BYTE
TL1 equ 0x8B                  ; TIMER 1 - LOW BYTE
TH0 equ 0x8C                  ; TIMER 0 - HIGH BYTE
TH1 equ 0x8D                  ; TIMER 1 - HIGH BYTE
CKCON equ 0x8E                ; CLOCK CONTROL
PSCTL equ 0x8F                ; PROGRAM STORE R/W CONTROL
SBCON0 equ 0x91               ; BAUDRATE GENERATOR 0 CONTROL
SBRLL0 equ 0x93               ; BAUDRATE GENERATOR 0 RELOAD VALUE - LOW BYTE
SBRLH0 equ 0x94               ; BAUDRATE GENERATOR 0 RELOAD VALUE - HIGH BYTE
USB0ADR equ 0x96              ; USB0 INDIRECT ADDRESS REGISTER
USB0DAT equ 0x97              ; USB0 DATA REGISTER
SCON equ 0x98                 ; UART0 CONTROL
SCON0 equ 0x98                ; UART0 CONTROL
SBUF equ 0x99                 ; UART0 BUFFER
SBUF0 equ 0x99                ; UART0 BUFFER
SMOD0 equ 0x9A                ; UART0 MODE
P2 equ 0xA0                   ; PORT 2
P0MDOUT equ 0xA4              ; PORT 0 OUTPUT MODE CONFIGURATION
P2MDOUT equ 0xA6              ; PORT 2 OUTPUT MODE CONFIGURATION
P3MDOUT equ 0xA7              ; PORT 3 OUTPUT MODE CONFIGURATION
IE equ 0xA8                   ; INTERRUPT ENABLE
CLKSEL equ 0xA9               ; SYSTEM CLOCK SELECT
EMI0CN equ 0xAA               ; EXTERNAL MEMORY INTERFACE CONTROL
_XPAGE equ 0xAA               ; XDATA/PDATA PAGE
P3 equ 0xB0                   ; PORT 3
OSCICN equ 0xB2               ; INTERNAL OSCILLATOR CONTROL
OSCICL equ 0xB3               ; INTERNAL OSCILLATOR CALIBRATION
FLSCL equ 0xB6                ; FLASH MEMORY TIMING PRESCALER
FLKEY equ 0xB7                ; FLASH ACESS LIMIT
IP equ 0xB8                   ; INTERRUPT PRIORITY
CLKMUL equ 0xB9               ; CLOCK MULTIPLIER CONTROL REGISTER
REG0CN equ 0xC9               ; VOLTAGE REGULATOR CONTROL
PSW equ 0xD0                  ; PROGRAM STATUS WORD
USB0XCN equ 0xD7              ; USB0 TRANSCEIVER CONTROL
ACC equ 0xE0                  ; ACCUMULATOR
GPIOCN equ 0xE2               ; GLOBAL PORT I/O CONTROL
OSCLCN equ 0xE3               ; LOW-FREQUENCY OSCILLATOR CONTROL
EIE1 equ 0xE6                 ; EXTERNAL INTERRUPT ENABLE 1
EIE2 equ 0xE7                 ; EXTERNAL INTERRUPT ENABLE 2
RSTSRC equ 0xEF               ; RESET SOURCE
B equ 0xF0                    ; B REGISTER
EIP1 equ 0xF6                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 1
EIP2 equ 0xF7                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 2
VDM0CN equ 0xFF               ; VDD MONITOR CONTROL

;  WORD/DWORD Registers  
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
SBRL0 equ 0x9493              ; BAUDRATE GENERATOR 0 RELOAD VALUE WORD

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
RBX0 equ 0x9A                 ; SCON.2 - EXTRA RECEIVE BIT
TB8 equ 0x9B                  ; SCON.3 - TRANSMIT BIT 8
TBX0 equ 0x9B                 ; SCON.3 - EXTRA TRANSMIT BIT
REN equ 0x9C                  ; SCON.4 - RECEIVE ENABLE
REN0 equ 0x9C                 ; SCON.4 - RECEIVE ENABLE
PERR0 equ 0x9E                ; SCON.6 - PARITY ERROR FLAG
OVR0 equ 0x9F                 ; SCON.7 - RECEIVE FIFO OVERRUN FLAG

;  IE  0xA8 
EX0 equ 0xA8                  ; IE.0 - EXTERNAL INTERRUPT 0 ENABLE
ET0 equ 0xA9                  ; IE.1 - TIMER 0 INTERRUPT ENABLE
EX1 equ 0xAA                  ; IE.2 - EXTERNAL INTERRUPT 1 ENABLE
ET1 equ 0xAB                  ; IE.3 - TIMER 1 INTERRUPT ENABLE
ES equ 0xAC                   ; IE.4 - SERIAL PORT INTERRUPT ENABLE
ES0 equ 0xAC                  ; IE.4 - SERIAL PORT INTERRUPT ENABLE
EA equ 0xAF                   ; IE.7 - GLOBAL INTERRUPT ENABLE

;  IP  0xB8 
PX0 equ 0xB8                  ; IP.0 - EXTERNAL INTERRUPT 0 PRIORITY
PT0 equ 0xB9                  ; IP.1 - TIMER 0 PRIORITY
PX1 equ 0xBA                  ; IP.2 - EXTERNAL INTERRUPT 1 PRIORITY
PT1 equ 0xBB                  ; IP.3 - TIMER 1 PRIORITY
PS equ 0xBC                   ; IP.4 - SERIAL PORT PRIORITY
PS0 equ 0xBC                  ; IP.4 - SERIAL PORT PRIORITY

;  PSW  0xD0 
PARITY equ 0xD0               ; PSW.0 - ACCUMULATOR PARITY FLAG
F1 equ 0xD1                   ; PSW.1 - FLAG 1
OV equ 0xD2                   ; PSW.2 - OVERFLOW FLAG
RS0 equ 0xD3                  ; PSW.3 - REGISTER BANK SELECT 0
RS1 equ 0xD4                  ; PSW.4 - REGISTER BANK SELECT 1
F0 equ 0xD5                   ; PSW.5 - FLAG 0
AC equ 0xD6                   ; PSW.6 - AUXILIARY CARRY FLAG
CY equ 0xD7                   ; PSW.7 - CARRY FLAG

; Predefined SFR Bit Masks 
PCON_IDLE equ 0x01            ; PCON
PCON_STOP equ 0x02            ; PCON
T0M equ 0x04                  ; CKCON
T1M equ 0x08                  ; CKCON
PSWE equ 0x01                 ; PSCTL
PSEE equ 0x02                 ; PSCTL
EUSB0 equ 0x02                ; EIE1
EVBUS equ 0x01                ; EIE2
PORSF equ 0x02                ; RSTSRC
SWRSF equ 0x10                ; RSTSRC
