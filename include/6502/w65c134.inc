;;
;; This is an include file for the Western Design Center W65C134 chips.
;;
;; For more info:
;; https://www.westerndesigncenter.com/
;;

;---- equ 0x002f ; Reserved
;---- equ 0x002e ; Reserved

;; I/O Data pins
PD0 equ 0x0000 ; Port 0 Data Register
PD1 equ 0x0001 ; Port 1 Data Register
PD2 equ 0x0002 ; Port 2 Data Register
PD3 equ 0x0003 ; Port 3 Data Register
PD4 equ 0x001c ; Port 4 Data Register
PD5 equ 0x001d ; Port 5 Data Register
PD6 equ 0x0020 ; Port 6 Data Register

;; I/O Data Direction
PDD0 equ 0x0004 ; Port 0 Data Direction Register
PDD1 equ 0x0005 ; Port 1 Data Direction Register
PDD2 equ 0x0006 ; Port 2 Data Direction Register
PDD4 equ 0x001e ; Port 4 Data Direction Register
PDD5 equ 0x001f ; Port 5 Data Direction Register
PDD6 equ 0x0021 ; Port 6 Data Direction Register

;; Port 3 Chip Select
PCS3 equ 0x0007 ; Port 3 Chip Select Register

;; Interrupt registers
IER1 equ 0x002d ; Interrupt Enable Register One
IFR1 equ 0x002c ; Interrupt Flag Register One
IER2 equ 0x0009 ; Interrupt Enable Register Two
IFR2 equ 0x0008 ; Interrupt Flag Register Two

;; Timers
TMCH equ 0x002b ; Timer M Counter High (read only)
TMCL equ 0x002a ; Timer M Counter Low (read only)
TMLH equ 0x0029 ; Timer M Latch High
TMLL equ 0x0028 ; Timer M Latch Low
TACH equ 0x0027 ; Timer A Counter High
TACL equ 0x0026 ; Timer A Counter Low
TALH equ 0x0025 ; Timer A Latch High
TALL equ 0x0024 ; Timer A Latch Low
T2CH equ 0x0013 ; Timer 2 Counter High
T2CL equ 0x0012 ; Timer 2 Counter Low
T1CH equ 0x0011 ; Timer 1 Counter High
T1CL equ 0x0010 ; Timer 1 Counter Low
T2LH equ 0x000f ; Timer 2 Latch High
T2LL equ 0x000e ; Timer 2 Latch Low
T1LH equ 0x000d ; Timer 1 Latch High
T1LL equ 0x000c ; Timer 1 Latch Low
TCR2 equ 0x000b ; Timer Control Register Two
TCR1 equ 0x000a ; Timer Control Register One

;; UARTs
ARTD equ 0x0023 ; Asynch. RXD/TXD Data Register
ACSR equ 0x0022 ; Asynch. RXD/TXD Control/StatusRegister

;; Bus and speed control
BCR equ 0x001b ; Bus Control Register
BAR equ 0x001a ; SIB Address Register
SCSR equ 0x0019 ; SIB Control and Status Register
SR3 equ 0x0018 ; SIB Shift Register 3
SR2 equ 0x0017 ; SIB Shift Register 2
SR1 equ 0x0016 ; SIB Shift Register 1
SR0 equ 0x0015 ; SIB Shift Register 0
STATE equ 0x0014 ; SIB State Register (read only)

