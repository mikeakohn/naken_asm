; C8051F300.h - Register Declarations for the Cygnal/SiLabs C8051F30x
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
SCON equ 0x98                 ; SERIAL PORT CONTROL
SCON0 equ 0x98                ; SERIAL PORT CONTROL
SBUF equ 0x99                 ; SERIAL PORT BUFFER
SBUF0 equ 0x99                ; SERIAL PORT BUFFER
CPT0MD equ 0x9D               ; COMPARATOR 0 MODE SELECTION
CPT0MX equ 0x9F               ; COMPARATOR 0 MUX SELECTION
P0MDOUT equ 0xA4              ; PORT 0 OUTPUT MODE CONFIGURATION
IE equ 0xA8                   ; INTERRUPT ENABLE
OSCXCN equ 0xB1               ; EXTERNAL OSCILLATOR CONTROL
OSCICN equ 0xB2               ; INTERNAL OSCILLATOR CONTROL
OSCICL equ 0xB3               ; INTERNAL OSCILLATOR CALIBRATION
FLSCL equ 0xB6                ; FLASH MEMORY TIMING PRESCALER
FLKEY equ 0xB7                ; FLASH ACESS LIMIT
IP equ 0xB8                   ; INTERRUPT PRIORITY
AMX0SL equ 0xBB               ; ADC 0 MUX CHANNEL SELECTION
ADC0CF equ 0xBC               ; ADC 0 CONFIGURATION
ADC0 equ 0xBE                 ; ADC 0 DATA
SMB0CN equ 0xC0               ; SMBUS CONTROL
SMB0CF equ 0xC1               ; SMBUS CONFIGURATION
SMB0DAT equ 0xC2              ; SMBUS DATA
ADC0GT equ 0xC4               ; ADC 0 GREATER-THAN REGISTER
ADC0LT equ 0xC6               ; ADC 0 LESS-THAN REGISTER
T2CON equ 0xC8                ; TIMER 2 CONTROL
TMR2CN equ 0xC8               ; TIMER 2 CONTROL
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
PCA0CN equ 0xD8               ; PCA CONTROL
PCA0MD equ 0xD9               ; PCA MODE
PCA0CPM0 equ 0xDA             ; PCA MODULE 0 MODE REGISTER
PCA0CPM1 equ 0xDB             ; PCA MODULE 1 MODE REGISTER
PCA0CPM2 equ 0xDC             ; PCA MODULE 2 MODE REGISTER
ACC equ 0xE0                  ; ACCUMULATOR
PRT0MX equ 0xE1               ; PORT MUX CONFIGURATION REGISTER 0
XBR0 equ 0xE1                 ; PORT MUX CONFIGURATION REGISTER 0
PRT1MX equ 0xE2               ; PORT MUX CONFIGURATION REGISTER 1
XBR1 equ 0xE2                 ; PORT MUX CONFIGURATION REGISTER 1
PRT2MX equ 0xE3               ; PORT MUX CONFIGURATION REGISTER 2
XBR2 equ 0xE3                 ; PORT MUX CONFIGURATION REGISTER 2
IT01CF equ 0xE4               ; INT0/INT1 CONFIGURATION REGISTER
INT01CF equ 0xE4              ; INT0/INT1 CONFIGURATION REGISTER
EIE1 equ 0xE6                 ; EXTERNAL INTERRUPT ENABLE 1
ADC0CN equ 0xE8               ; ADC 0 CONTROL
PCA0CPL1 equ 0xE9             ; PCA CAPTURE 1 LOW
PCA0CPH1 equ 0xEA             ; PCA CAPTURE 1 HIGH
PCA0CPL2 equ 0xEB             ; PCA CAPTURE 2 LOW
PCA0CPH2 equ 0xEC             ; PCA CAPTURE 2 HIGH
RSTSRC equ 0xEF               ; RESET SOURCE
B equ 0xF0                    ; B REGISTER
P0MODE equ 0xF1               ; PORT 0 INPUT MODE CONFIGURATION
P0MDIN equ 0xF1               ; PORT 0 INPUT MODE CONFIGURATION
EIP1 equ 0xF6                 ; EXTERNAL INTERRUPT PRIORITY REGISTER 1
CPT0CN equ 0xF8               ; COMPARATOR 0 CONTROL
PCA0L equ 0xF9                ; PCA COUNTER LOW
PCA0H equ 0xFA                ; PCA COUNTER HIGH
PCA0CPL0 equ 0xFB             ; PCA CAPTURE 0 LOW
PCA0CPH0 equ 0xFC             ; PCA CAPTURE 0 HIGH

;  WORD/DWORD Registers  
TMR0 equ 0x8C8A               ; TIMER 0 COUNTER
TMR1 equ 0x8D8B               ; TIMER 1 COUNTER
TMR2 equ 0xCDCC               ; TIMER 2 COUNTER
RCAP2 equ 0xCBCA              ; TIMER 2 CAPTURE REGISTER WORD
TMR2RL equ 0xCBCA             ; TIMER 2 CAPTURE REGISTER WORD
PCA0 equ 0xFAF9               ; PCA COUNTER
PCA0CP0 equ 0xFCFB            ; PCA CAPTURE 0 WORD
PCA0CP1 equ 0xEAE9            ; PCA CAPTURE 1 WORD
PCA0CP2 equ 0xECEB            ; PCA CAPTURE 2 WORD

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
IEGF0 equ 0xAE                ; IE.6 - GENERAL PURPOSE FLAG 0
EA equ 0xAF                   ; IE.7 - GLOBAL INTERRUPT ENABLE

;  IP  0xB8 
PX0 equ 0xB8                  ; IP.0 - EXTERNAL INTERRUPT 0 PRIORITY
PT0 equ 0xB9                  ; IP.1 - TIMER 0 PRIORITY
PX1 equ 0xBA                  ; IP.2 - EXTERNAL INTERRUPT 1 PRIORITY
PT1 equ 0xBB                  ; IP.3 - TIMER 1 PRIORITY
PS equ 0xBC                   ; IP.4 - SERIAL PORT PRIORITY
PS0 equ 0xBC                  ; IP.4 - SERIAL PORT PRIORITY
PT2 equ 0xBD                  ; IP.5 - TIMER 2 PRIORITY

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
TR2 equ 0xCA                  ; TMR2CN.2 - TIMER 2 ON/OFF CONTROL
T2SPLIT equ 0xCB              ; TMR2CN.3 - TIMER 2 SPLIT MODE ENABLE
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

;  CPT0CN  0xF8 
CP0HYN0 equ 0xF8              ; CPT0CN.0 - Comp.0 Neg. Hysteresis Control Bit0
CP0HYN1 equ 0xF9              ; CPT0CN.1 - Comp.0 Neg. Hysteresis Control Bit1
CP0HYP0 equ 0xFA              ; CPT0CN.2 - Comp.0 Pos. Hysteresis Control Bit0
CP0HYP1 equ 0xFB              ; CPT0CN.3 - Comp.0 Pos. Hysteresis Control Bit1
CP0FIF equ 0xFC               ; CPT0CN.4 - Comparator0 Falling-Edge Int. Flag
CP0RIF equ 0xFD               ; CPT0CN.5 - Comparator0 Rising-Edge Int. Flag
CP0OUT equ 0xFE               ; CPT0CN.6 - Comparator0 Output State Flag
CP0EN equ 0xFF                ; CPT0CN.7 - Comparator0 Enable Bit

; Predefined SFR Bit Masks 
PCON_IDLE equ 0x01            ; PCON
PCON_STOP equ 0x02            ; PCON
T1M equ 0x10                  ; CKCON
PSWE equ 0x01                 ; PSCTL
PSEE equ 0x02                 ; PSCTL
ECP0F equ 0x10                ; EIE1
ECP0R equ 0x20                ; EIE1
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
CP0E equ 0x10                 ; XBR1
CP0OEN equ 0x10               ; XBR1
CP0AE equ 0x20                ; XBR1
CP0AOEN equ 0x20              ; XBR1
