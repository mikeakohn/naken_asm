; C8051F360.h - Register Declarations for the SiLabs C8051F36x
; Processor Range
; 
; Copyright (C) 2007, Maarten Brock, sourceforge.brock@dse.nl
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

;  All Pages 
P0 equ 0x80                   ; Port 0
SP equ 0x81                   ; Stack Pointer
DPL equ 0x82                  ; Data Pointer Low Byte
DPH equ 0x83                  ; Data Pointer High Byte
SFRNEXT equ 0x85              ; SFR Stack Next Page
SFRLAST equ 0x86              ; SFR Stack Last Page
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
P1 equ 0x90                   ; Port 1
TMR3CN equ 0x91               ; Timer 3 Control
TMR3RL equ 0x92               ; Timer 3 Reload Register Word
TMR3RLL equ 0x92              ; Timer 3 Reload Register Low Byte
TMR3RLH equ 0x93              ; Timer 3 Reload Register High Byte
TMR3 equ 0x94                 ; Timer 3 Word
TMR3L equ 0x94                ; Timer 3 Low Byte
TMR3H equ 0x95                ; Timer 3 High Byte
IDA0 equ 0x96                 ; IDAC 0 Word
IDA0L equ 0x96                ; IDAC 0 Low Byte
IDA0H equ 0x97                ; IDAC 0 High Byte
SCON0 equ 0x98                ; Serial Port 0 Control
RI0 equ 0x98+0                ; Receive Interrupt Flag
TI0 equ 0x98+1                ; Transmit Interrupt Flag
RB80 equ 0x98+2               ; Ninth Receive Bit
TB80 equ 0x98+3               ; Ninth Transmission Bit
REN0 equ 0x98+4               ; Receive Enable
MCE0 equ 0x98+5               ; Multiprocessor Communication Enable
S0MODE equ 0x98+7             ; Serial Port 0 Operation Mode
SBUF0 equ 0x99                ; Serial Port 0 Data Buffer
CPT1CN equ 0x9A               ; Comparator 1 Control
CPT0CN equ 0x9B               ; Comparator 0 Control
CPT1MD equ 0x9C               ; Comparator 1 Mode Selection
CPT0MD equ 0x9D               ; Comparator 0 Mode Selection
CPT1MX equ 0x9E               ; Comparator 1 MUX Selection
CPT0MX equ 0x9F               ; Comparator 0 MUX Selection
P2 equ 0xA0                   ; Port 2
SPI0CFG equ 0xA1              ; SPI Configuration
SPI0CKR equ 0xA2              ; SPI Clock Rate Control
SPI0DAT equ 0xA3              ; SPI Data
SFRPAGE equ 0xA7              ; SFR Page Select
IE equ 0xA8                   ; Interrupt Enable
EX0 equ 0xA8+0                ; Enable External Interrupt 0
ET0 equ 0xA8+1                ; Enable Timer 0 Interrupt
EX1 equ 0xA8+2                ; Enable External Interrupt 1
ET1 equ 0xA8+3                ; Enable Timer 1 Interrupt
ES0 equ 0xA8+4                ; Enable Serial Port Interrupt
ET2 equ 0xA8+5                ; Enable Timer 2 Interrupt
ESPI0 equ 0xA8+6              ; Enable SPI0 Interrupt
EA equ 0xA8+7                 ; Global Interrupt Enable
EMI0CN equ 0xAA               ; EMIF Control
_XPAGE equ 0xAA               ; SDCC: XDATA/PDATA Page
P3 equ 0xB0                   ; Port 3
P4 equ 0xB5                   ; Port 4
IP equ 0xB8                   ; Interrupt Priority
PX0 equ 0xB8+0                ; External Interrupt 0 Priority
PT0 equ 0xB8+1                ; Timer 0 Interrupt Priority
PX1 equ 0xB8+2                ; External Interrupt 1 Priority
PT1 equ 0xB8+3                ; Timer 1 Interrupt Priority
PS0 equ 0xB8+4                ; Serial Port Interrupt Priority
PT2 equ 0xB8+5                ; Timer 2 Interrupt Priority
PSPI0 equ 0xB8+6              ; SPI0 Interrupt Priority
IDA0CN equ 0xB9               ; IDAC 0 Control
AMX0N equ 0xBA                ; AMUX 0 Negative Channel Select
AMX0P equ 0xBB                ; AMUX 0 Positive Channel Select
ADC0CF equ 0xBC               ; ADC0 Configuration
ADC0 equ 0xBD                 ; ADC0 Word
ADC0L equ 0xBD                ; ADC0 Low Byte
ADC0H equ 0xBE                ; ADC0 High Byte
SMB0CN equ 0xC0               ; SMBus Control
SI equ 0xC0+0                 ; SMBus Interrupt Flag
ACK equ 0xC0+1                ; SMBus Acknowledge Flag
ARBLOST equ 0xC0+2            ; SMBus Arbitration Lost Indicator
ACKRQ equ 0xC0+3              ; SMBus Acknowledge Request
STO equ 0xC0+4                ; SMBus Stop Flag
STA equ 0xC0+5                ; SMBus Start Flag
TXMODE equ 0xC0+6             ; SMBus Transmit Mode Indicator
MASTER equ 0xC0+7             ; SMBus Master/Slave Indicator
SMB0CF equ 0xC1               ; SMBus Configuration
SMB0DAT equ 0xC2              ; SMBus Data
ADC0GT equ 0xC3               ; ADC0 Greater-Than Data Word
ADC0GTL equ 0xC3              ; ADC0 Greater-Than Data Low Byte
ADC0GTH equ 0xC4              ; ADC0 Greater-Than Data High Byte
ADC0LT equ 0xC5               ; ADC0 Less-Than Data Word
ADC0LTL equ 0xC5              ; ADC0 Less-Than Data Low Byte
ADC0LTH equ 0xC6              ; ADC0 Less-Than Data High Byte
TMR2CN equ 0xC8               ; Timer/Counter 2 Control
T2XCLK equ 0xC8+0             ; Timer 2 External Clock Select
TR2 equ 0xC8+2                ; Timer 2 Run Control
T2SPLIT equ 0xC8+3            ; Timer 2 Split Mode Enable
TF2CEN equ 0xC8+4             ; Timer 2 Low-Frequency Oscillator Capture Enable
TF2LEN equ 0xC8+5             ; Timer 2 Low Byte Interrupt Enable
TF2L equ 0xC8+6               ; Timer 2 Low Byte Overflow Flag
TF2H equ 0xC8+7               ; Timer 2 High Byte Overflow Flag
TMR2RL equ 0xCA               ; Timer/Counter 2 Reload Word
TMR2RLL equ 0xCA              ; Timer/Counter 2 Reload Low Byte
TMR2RLH equ 0xCB              ; Timer/Counter 2 Reload High Byte
TMR2 equ 0xCC                 ; Timer/Counter 2 Word
TMR2L equ 0xCC                ; Timer/Counter 2 Low Byte
TMR2H equ 0xCD                ; Timer/Counter 2 High Byte
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
PCA0CN equ 0xD8               ; PCA Control
CCF0 equ 0xD8+0               ; PCA Module 0 Capture/Compare Flag
CCF1 equ 0xD8+1               ; PCA Module 1 Capture/Compare Flag
CCF2 equ 0xD8+2               ; PCA Module 2 Capture/Compare Flag
CCF3 equ 0xD8+3               ; PCA Module 3 Capture/Compare Flag
CCF4 equ 0xD8+4               ; PCA Module 4 Capture/Compare Flag
CCF5 equ 0xD8+5               ; PCA Module 5 Capture/Compare Flag
CR equ 0xD8+6                 ; PCA Counter/Timer Run Control
CF equ 0xD8+7                 ; PCA Counter/Timer Overflow Flag
PCA0MD equ 0xD9               ; PCA Mode
PCA0CPM0 equ 0xDA             ; PCA Module 0 Mode
PCA0CPM1 equ 0xDB             ; PCA Module 1 Mode
PCA0CPM2 equ 0xDC             ; PCA Module 2 Mode
PCA0CPM3 equ 0xDD             ; PCA Module 3 Mode
PCA0CPM4 equ 0xDE             ; PCA Module 4 Mode
PCA0CPM5 equ 0xDF             ; PCA Module 5 Mode
ACC equ 0xE0                  ; Accumulator
IT01CF equ 0xE4               ; INT0/INT1 Configuration
EIE1 equ 0xE6                 ; Extended Interrupt Enable 1
EIE2 equ 0xE7                 ; Extended Interrupt Enable 2
ADC0CN equ 0xE8               ; ADC0 Control
AD0CM0 equ 0xE8+0             ; ADC0 Conversion Start Mode Select Bit 0
AD0CM1 equ 0xE8+1             ; ADC0 Conversion Start Mode Select Bit 1
AD0CM2 equ 0xE8+2             ; ADC0 Conversion Start Mode Select Bit 2
AD0WINT equ 0xE8+3            ; ADC0 Window Compare Interrupt Flag
AD0BUSY equ 0xE8+4            ; ADC0 Busy Bit
AD0INT equ 0xE8+5             ; ADC0 Conversion Complete Interrupt Flag
AD0TM equ 0xE8+6              ; ADC0 Track Mode Bit
AD0EN equ 0xE8+7              ; ADC0 Enable Bit
PCA0CP1 equ 0xE9              ; PCA Capture 1 Word
PCA0CPL1 equ 0xE9             ; PCA Capture 1 Low Byte
PCA0CPH1 equ 0xEA             ; PCA Capture 1 High Byte
PCA0CP2 equ 0xEB              ; PCA Capture 2 Word
PCA0CPL2 equ 0xEB             ; PCA Capture 2 Low Byte
PCA0CPH2 equ 0xEC             ; PCA Capture 2 High Byte
PCA0CP3 equ 0xED              ; PCA Capture 3 Word
PCA0CPL3 equ 0xED             ; PCA Capture 3 Low Byte
PCA0CPH3 equ 0xEE             ; PCA Capture 3 High Byte
RSTSRC equ 0xEF               ; Reset Source Configuration/Status
B equ 0xF0                    ; B Register
PCA0CP5 equ 0xF5              ; PCA Capture 5 Word
PCA0CPL5 equ 0xF5             ; PCA Capture 5 Low Byte
PCA0CPH5 equ 0xF6             ; PCA Capture 5 High Byte
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
PCA0CP4 equ 0xFD              ; PCA Capture 4 Word
PCA0CPL4 equ 0xFD             ; PCA Capture 4 Low Byte
PCA0CPH4 equ 0xFE             ; PCA Capture 4 High Byte
VDM0CN equ 0xFF               ; VDD Monitor Control

;  Page 0x00 
PSCTL equ 0x8F                ; Program Store R/W Control
MAC0A equ 0xA4                ; MAC0 A Register Word
MAC0AL equ 0xA4               ; MAC0 A Register Low Byte
MAC0AH equ 0xA5               ; MAC0 A Register High Byte
MAC0RND equ 0xAE              ; MAC0 Rounding Register Word
MAC0RNDL equ 0xAE             ; MAC0 Rounding Register Low Byte
MAC0RNDH equ 0xAF             ; MAC0 Rounding Register High Byte
P2MAT equ 0xB1                ; Port 2 Match
P2MASK equ 0xB2               ; Port 2 Mask
FLSCL equ 0xB6                ; Flash Scale
FLKEY equ 0xB7                ; Flash Lock and Key
MAC0STA equ 0xCF              ; MAC0 Status Register
MAC0ACC equ 0xD2              ; MAC0 Accumulator Long Word
MAC0ACC0 equ 0xD2             ; MAC0 Accumulator Byte 0 (LSB
MAC0ACC1 equ 0xD3             ; MAC0 Accumulator Byte 1
MAC0ACC2 equ 0xD4             ; MAC0 Accumulator Byte 2
MAC0ACC3 equ 0xD5             ; MAC0 Accumulator Byte 3 (MSB
MAC0OVR equ 0xD6              ; MAC0 Accumulator Overflow
MAC0CF equ 0xD7               ; MAC0 Configuration
P1MAT equ 0xE1                ; Port 1 Match
P1MASK equ 0xE2               ; Port 1 Mask

; No sfr16 definition for MAC0B because MAC0BL must be written last
MAC0BL equ 0xF1               ; MAC0 B Register Low Byte
MAC0BH equ 0xF2               ; MAC0 B Register High Byte
P0MAT equ 0xF3                ; Port 0 Match
P0MASK equ 0xF4               ; Port 0 Mask

;  Page 0x0F 
CCH0CN equ 0x84               ; Cache Control
CLKSEL equ 0x8F               ; Clock Select
P0MDOUT equ 0xA4              ; Port 0 Output Mode Configuration
P1MDOUT equ 0xA5              ; Port 1 Output Mode Configuration
P2MDOUT equ 0xA6              ; Port 2 Output Mode Configuration
PLL0DIV equ 0xA9              ; PLL Divider
FLSTAT equ 0xAC               ; Flash Status
OSCLCN equ 0xAD               ; Internal Low-Frequency Oscillator Control
P4MDOUT equ 0xAE              ; Port 4 Output Mode Configuration
P3MDOUT equ 0xAF              ; Port 3 Output Mode Configuration
PLL0MUL equ 0xB1              ; PLL Multiplier
PLL0FLT equ 0xB2              ; PLL Filter
PLL0CN equ 0xB3               ; PLL Control
OSCXCN equ 0xB6               ; External Oscillator Control
OSCICN equ 0xB7               ; Internal Oscillator Control
OSCICL equ 0xBF               ; Internal Oscillator Calibration
EMI0CF equ 0xC7               ; EMIF Configuration
CCH0TN equ 0xC9               ; Cache Tuning
EIP1 equ 0xCE                 ; Extended Interrupt Priority 1
EIP2 equ 0xCF                 ; Extended Interrupt Priority 2
CCH0LC equ 0xD2               ; Cache Lock
CCH0MA equ 0xD3               ; Cache Miss Accumulator
P0SKIP equ 0xD4               ; Port 0 Skip
P1SKIP equ 0xD5               ; Port 1 Skip
P2SKIP equ 0xD6               ; Port 2 Skip
P3SKIP equ 0xD7               ; Port 3 Skip
XBR0 equ 0xE1                 ; Port I/O Crossbar Control 0
XBR1 equ 0xE2                 ; Port I/O Crossbar Control 1
SFR0CN equ 0xE5               ; SFR Page Control
P0MDIN equ 0xF1               ; Port 0 Input Mode Configuration
P1MDIN equ 0xF2               ; Port 1 Input Mode Configuration
P2MDIN equ 0xF3               ; Port 2 Input Mode Configuration
P3MDIN equ 0xF4               ; Port 3 Input Mode Configuration
EMI0TC equ 0xF7               ; EMIF Timing Control

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
INT_SMBUS0 equ 7              ; SMBus0 Interface

;                        8          Reserved
INT_ADC0_WINDOW equ 9         ; ADC0 Window Comparison
INT_ADC0_EOC equ 10           ; ADC0 End Of Conversion
INT_PCA0 equ 11               ; PCA0 Peripheral
INT_COMPARATOR0 equ 12        ; Comparator0
INT_COMPARATOR1 equ 13        ; Comparator1
INT_TIMER3 equ 14             ; Timer3 Overflow

;                        15         Reserved
INT_PORT_MATCH equ 16         ; Port Match
