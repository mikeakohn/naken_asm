; C8051F520.h - Register Declarations for the SiLabs C8051F52x-F53x
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
P0 equ 0x80                   ; Port 0
SP equ 0x81                   ; Stack Pointer
DPL equ 0x82                  ; Data Pointer Low Byte
DPH equ 0x83                  ; Data Pointer High Byte
PCON equ 0x87                 ; Power Mode Control
TCON equ 0x88                 ; Timer Control
IT0 equ 0x88+0                ; Ext. Interrupt 0 Type Select
IE0 equ 0x88+1                ; Ext. Interrupt 0 Flag
IT1 equ 0x88+2                ; Ext. Interrupt 1 Type Select
IE1 equ 0x88+3                ; Ext. Interrupt 1 Flag
TR0 equ 0x88+4                ; Timer 0 Run Control
TF0 equ 0x88+5                ; Timer 0 Overflow Flag
TR1 equ 0x88+6                ; Timer 1 Run Control
TF1 equ 0x88+7                ; Timer 1 Overflow Flag
TMOD equ 0x89                 ; Timer Mode
TMR0 equ 0x8C8A               ; Timer/Counter 0 Word
TL0 equ 0x8A                  ; Timer/Counter 0 Low Byte
TH0 equ 0x8C                  ; Timer/Counter 0 High Byte
TMR1 equ 0x8D8B               ; Timer/Counter 1 Word
TL1 equ 0x8B                  ; Timer/Counter 1 Low Byte
TH1 equ 0x8D                  ; Timer/Counter 1 High Byte
CKCON equ 0x8E                ; Clock Control
PSCTL equ 0x8F                ; Program Store R/W Control
P1 equ 0x90                   ; Port 1
LINADDR equ 0x92              ; LIN Indirect Address Pointer
LINDATA equ 0x93              ; LIN Indirect Data Buffer
LINCF equ 0x95                ; LIN Control Mode
SCON0 equ 0x98                ; Serial Port 0 Control
RI0 equ 0x98+0                ; Receive Interrupt Flag
TI0 equ 0x98+1                ; Transmit Interrupt Flag
RB80 equ 0x98+2               ; Ninth Receive Bit
TB80 equ 0x98+3               ; Ninth Transmission Bit
REN0 equ 0x98+4               ; Receive Enable
MCE0 equ 0x98+5               ; Multiprocessor Communication Enable
S0MODE equ 0x98+7             ; Serial Port 0 Operation Mode
SBUF0 equ 0x99                ; Serial Port 0 Data Buffer
CPT0CN equ 0x9B               ; Comparator 0 Control
CPT0MD equ 0x9D               ; Comparator 0 Mode Selection
CPT0MX equ 0x9F               ; Comparator 0 MUX Selection
SPI0CFG equ 0xA1              ; SPI Configuration
SPI0CKR equ 0xA2              ; SPI Clock Rate Control
SPI0DAT equ 0xA3              ; SPI Data
P0MDOUT equ 0xA4              ; Port 0 Output Mode Configuration
P1MDOUT equ 0xA5              ; Port 1 Output Mode Configuration
IE equ 0xA8                   ; Interrupt Enable
EX0 equ 0xA8+0                ; Enable External Interrupt 0
ET0 equ 0xA8+1                ; Enable Timer 0 Interrupt
EX1 equ 0xA8+2                ; Enable External Interrupt 1
ET1 equ 0xA8+3                ; Enable Timer 1 Interrupt
ES0 equ 0xA8+4                ; Enable Serial Port Interrupt
ET2 equ 0xA8+5                ; Enable Timer 2 Interrupt
ESPI0 equ 0xA8+6              ; Enable SPI0 Interrupt
EA equ 0xA8+7                 ; Global Interrupt Enable
CLKSEL equ 0xA9               ; Clock Select
OSCIFIN equ 0xB0              ; Internal Oscillator Fine Calibration
OSCXCN equ 0xB1               ; External Oscillator Control
OSCICN equ 0xB2               ; Internal Oscillator Control
OSCICL equ 0xB3               ; Internal Oscillator Calibration
FLKEY equ 0xB7                ; Flash Lock and Key
IP equ 0xB8                   ; Interrupt Priority
PX0 equ 0xB8+0                ; External Interrupt 0 Priority
PT0 equ 0xB8+1                ; Timer 0 Interrupt Priority
PX1 equ 0xB8+2                ; External Interrupt 1 Priority
PT1 equ 0xB8+3                ; Timer 1 Interrupt Priority
PS0 equ 0xB8+4                ; Serial Port Interrupt Priority
PT2 equ 0xB8+5                ; Timer 2 Interrupt Priority
PSPI0 equ 0xB8+6              ; SPI0 Interrupt Priority
ADC0TK equ 0xBA               ; ADC0 Tracking Mode Select
ADC0MX equ 0xBB               ; ADC0 Channel Select
ADC0CF equ 0xBC               ; ADC0 Configuration
ADC0 equ 0xBD                 ; ADC0 Word
ADC0L equ 0xBD                ; ADC0 Low Byte
ADC0H equ 0xBE                ; ADC0 High Byte
P1MASK equ 0xBF               ; Port 1 Mask
ADC0GT equ 0xC3               ; ADC0 Greater-Than Data Word
ADC0GTL equ 0xC3              ; ADC0 Greater-Than Data Low Byte
ADC0GTH equ 0xC4              ; ADC0 Greater-Than Data High Byte
ADC0LT equ 0xC5               ; ADC0 Less-Than Data Word
ADC0LTL equ 0xC5              ; ADC0 Less-Than Data Low Byte
ADC0LTH equ 0xC6              ; ADC0 Less-Than Data High Byte
P0MASK equ 0xC7               ; Port 0 Mask
TMR2CN equ 0xC8               ; Timer/Counter 2 Control
REG0CN equ 0xC9               ; Voltage Regulator Control
TMR2RL equ 0xCA               ; Timer/Counter 2 Reload Word
TMR2RLL equ 0xCA              ; Timer/Counter 2 Reload Low Byte
TMR2RLH equ 0xCB              ; Timer/Counter 2 Reload High Byte
TMR2 equ 0xCC                 ; Timer/Counter 2 Word
TMR2L equ 0xCC                ; Timer/Counter 2 Low Byte
TMR2H equ 0xCD                ; Timer/Counter 2 High Byte
P1MAT equ 0xCF                ; Port 1 Match
PSW equ 0xD0                  ; Program Status Word
P equ 0xD0+0                  ; Parity Flag
F1 equ 0xD0+1                 ; User-Defined Flag
OV equ 0xD0+2                 ; Overflow Flag
RS0 equ 0xD0+3                ; Register Bank Select 0
RS1 equ 0xD0+4                ; Register Bank Select 1
F0 equ 0xD0+5                 ; User-Defined Flag
AC equ 0xD0+6                 ; Auxiliary Carry Flag
CY equ 0xD0+7                 ; Carry Flag
REF0CN equ 0xD1               ; Voltage Reference Control
P0SKIP equ 0xD4               ; Port 0 Skip
P1SKIP equ 0xD5               ; Port 1 Skip
P0MAT equ 0xD7                ; Port 0 Match
PCA0CN equ 0xD8               ; PCA Control
CCF0 equ 0xD8+0               ; PCA Module 0 Capture/Compare Flag
CCF1 equ 0xD8+1               ; PCA Module 1 Capture/Compare Flag
CCF2 equ 0xD8+2               ; PCA Module 2 Capture/Compare Flag
CR equ 0xD8+6                 ; PCA Counter/Timer Run Control
CF equ 0xD8+7                 ; PCA Counter/Timer Overflow Flag
PCA0MD equ 0xD9               ; PCA Mode
PCA0CPM0 equ 0xDA             ; PCA Module 0 Mode
PCA0CPM1 equ 0xDB             ; PCA Module 1 Mode
PCA0CPM2 equ 0xDC             ; PCA Module 2 Mode
ACC equ 0xE0                  ; Accumulator
XBR0 equ 0xE1                 ; Port I/O Crossbar Control 0
XBR1 equ 0xE2                 ; Port I/O Crossbar Control 1
IT01CF equ 0xE4               ; INT0/INT1 Configuration
EIE1 equ 0xE6                 ; Extended Interrupt Enable 1
ADC0CN equ 0xE8               ; ADC0 Control
AD0CM0 equ 0xE8+0             ; ADC0 Conversion Start Mode Select Bit 0
AD0CM1 equ 0xE8+1             ; ADC0 Conversion Start Mode Select Bit 1
AD0LJST equ 0xE8+2            ; ADC0 Left Justify Select
AD0WINT equ 0xE8+3            ; ADC0 Window Compare Interrupt Flag
AD0BUSY equ 0xE8+4            ; ADC0 Busy Bit
AD0INT equ 0xE8+5             ; ADC0 Conversion Complete Interrupt Flag
BURSTEN equ 0xE8+6            ; ADC0 Burst Mode Enable Bit
AD0EN equ 0xE8+7              ; ADC0 Enable Bit
PCA0CP1 equ 0xE9              ; PCA Capture 1 Word
PCA0CPL1 equ 0xE9             ; PCA Capture 1 Low Byte
PCA0CPH1 equ 0xEA             ; PCA Capture 1 High Byte
PCA0CP2 equ 0xEB              ; PCA Capture 2 Word
PCA0CPL2 equ 0xEB             ; PCA Capture 2 Low Byte
PCA0CPH2 equ 0xEC             ; PCA Capture 2 High Byte
RSTSRC equ 0xEF               ; Reset Source Configuration/Status
B equ 0xF0                    ; B Register
P0MDIN equ 0xF1               ; Port 0 Input Mode Configuration
P1MDIN equ 0xF2               ; Port 1 Input Mode Configuration
EIP1 equ 0xF6                 ; Extended Interrupt Priority 1
SPI0CN equ 0xF8               ; SPI0 Control
SPIEN equ 0xF8+0              ; SPI0 Enable
TXBMT equ 0xF8+1              ; SPI0 Transmit Buffer Empty
NSSMD0 equ 0xF8+2             ; SPI0 Slave Select Mode Bit 0
NSSMD1 equ 0xF8+3             ; SPI0 Slave Select Mode Bit 1
RXOVRN equ 0xF8+4             ; SPI0 Receive Overrun Flag
MODF equ 0xF8+5               ; SPI0 Mode Fault Flag
WCOL equ 0xF8+6               ; SPI0 Write Collision Flag
SPIF equ 0xF8+7               ; SPI0 Interrupt Flag
PCA0 equ 0xF9                 ; PCA Counter Word
PCA0L equ 0xF9                ; PCA Counter Low Byte
PCA0H equ 0xFA                ; PCA Counter High Byte
PCA0CP0 equ 0xFB              ; PCA Capture 0 Word
PCA0CPL0 equ 0xFB             ; PCA Capture 0 Low Byte
PCA0CPH0 equ 0xFC             ; PCA Capture 0 High Byte
VDDMON equ 0xFF               ; VDD Control

; Predefined SFR Bit Masks 
PCON_IDLE equ 0x01            ; PCON
PCON_STOP equ 0x02            ; PCON
T1M equ 0x08                  ; CKCON
PSWE equ 0x01                 ; PSCTL
PSEE equ 0x02                 ; PSCTL
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

; Interrupts 
INT_EXT0 equ 0                ; External Interrupt 0
INT_TIMER0 equ 1              ; Timer0 Overflow
INT_EXT1 equ 2                ; External Interrupt 1
INT_TIMER1 equ 3              ; Timer1 Overflow
INT_UART0 equ 4               ; Serial Port 0
INT_TIMER2 equ 5              ; Timer2 Overflow
INT_SPI0 equ 6                ; Serial Peripheral Interface 0
INT_ADC0_WINDOW equ 7         ; ADC0 Window Comparison
INT_ADC0_EOC equ 8            ; ADC0 End Of Conversion
INT_PCA0 equ 9                ; PCA0 Peripheral
INT_COMP_FALLING equ 10       ; Comparator0 Falling
INT_COMP_RISING equ 11        ; Comparator0 Rising
INT_LIN equ 12                ; LIN
INT_VREG_DROPOUT equ 13       ; VREG dropout
INT_PORT_MATCH equ 14         ; Port Match
