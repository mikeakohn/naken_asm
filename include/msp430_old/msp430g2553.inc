;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  msp430g2553 include file
;;;
;;;  hacked from the msp430-gcc header file with a series of regex replaces
;;;  and a little bit of hand tweaking.
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;
;;; STANDARD BITS
;;;

BIT0		equ	0x0001	
BIT1		equ	0x0002	
BIT2		equ	0x0004	
BIT3		equ	0x0008	
BIT4		equ	0x0010	
BIT5		equ	0x0020	
BIT6		equ	0x0040	
BIT7		equ	0x0080	
BIT8		equ	0x0100	
BIT9		equ	0x0200	
BITA		equ	0x0400	
BITB		equ	0x0800	
BITC		equ	0x1000	
BITD		equ	0x2000	
BITE		equ	0x4000	
BITF		equ	0x8000	

;;;
;;; STATUS REGISTER BITS
;;;

C		equ	0x0001	
Z		equ	0x0002	
N		equ	0x0004	
V		equ	0x0100	
GIE		equ	0x0008	
CPUOFF		equ	0x0010	
OSCOFF		equ	0x0020	
SCG0		equ	0x0040	
SCG1		equ	0x0080	

;;;
;;; Low Power Modes coded with Bits 4-7 in SR 
;;;

LPM0		equ	CPUOFF	
LPM1		equ	SCG0+CPUOFF	
LPM2		equ	SCG1+CPUOFF	
LPM3		equ	SCG1+SCG0+CPUOFF	
LPM4		equ	SCG1+SCG0+OSCOFF+CPUOFF	


;;;
;;; SPECIAL FUNCTION REGISTER ADDRESSES + CONTROL BITS
;;;

IE1		equ	0x0000		; Interrupt Enable 1 

WDTIE		equ	0x01		; Watchdog Interrupt Enable 
OFIE		equ	0x02		; Osc. Fault  Interrupt Enable 
NMIIE		equ	0x10		; NMI Interrupt Enable 
ACCVIE		equ	0x20		; Flash Access Violation Interrupt Enable 

IFG1		equ	0x0002		; Interrupt Flag 1 

WDTIFG		equ	0x01		; Watchdog Interrupt Flag 
OFIFG		equ	0x02		; Osc. Fault Interrupt Flag 
PORIFG		equ	0x04		; Power On Interrupt Flag 
RSTIFG		equ	0x08		; Reset Interrupt Flag 
NMIIFG		equ	0x10		; NMI Interrupt Flag 

IE2		equ	0x0001		; Interrupt Enable 2 
UC0IE		equ	IE2	
UCA0RXIE	equ	0x01	
UCA0TXIE	equ	0x02	
UCB0RXIE	equ	0x04	
UCB0TXIE	equ	0x08	

IFG2		equ	0x0003		; Interrupt Flag 2 
UC0IFG		equ	IFG2	
UCA0RXIFG	equ	0x01	
UCA0TXIFG	equ	0x02	
UCB0RXIFG	equ	0x04	
UCB0TXIFG	equ	0x08	


;;;
;;; ADC10
;;;

#define __MSP430_HAS_ADC10__		; Definition to show that Module is available 

ADC10DTC0	equ	0x0048		; ADC10 Data Transfer Control 0 
ADC10DTC1	equ	0x0049		; ADC10 Data Transfer Control 1 
ADC10AE0	equ	0x004A		; ADC10 Analog Enable 0 
ADC10CTL0	equ	0x01B0		; ADC10 Control 0 
ADC10CTL1	equ	0x01B2		; ADC10 Control 1 
ADC10MEM	equ	0x01B4		; ADC10 Memory 
ADC10SA		equ	0x01BC		; ADC10 Data Transfer Start Address 

; ADC10CTL0 
ADC10SC		equ	0x001		; ADC10 Start Conversion 
ENC		equ	0x002		; ADC10 Enable Conversion 
ADC10IFG	equ	0x004		; ADC10 Interrupt Flag 
ADC10IE		equ	0x008		; ADC10 Interrupt Enalbe 
ADC10ON		equ	0x010		; ADC10 On/Enable 
REFON		equ	0x020		; ADC10 Reference on 
REF2_5V		equ	0x040		; ADC10 Ref 0:1.5V / 1:2.5V 
MSC		equ	0x080		; ADC10 Multiple SampleConversion 
REFBURST	equ	0x100		; ADC10 Reference Burst Mode 
REFOUT		equ	0x200		; ADC10 Enalbe output of Ref. 
ADC10SR		equ	0x400		; ADC10 Sampling Rate 0:200ksps / 1:50ksps 
ADC10SHT0	equ	0x800		; ADC10 Sample Hold Select Bit: 0 
ADC10SHT1	equ	0x1000		; ADC10 Sample Hold Select Bit: 1 
SREF0		equ	0x2000		; ADC10 Reference Select Bit: 0 
SREF1		equ	0x4000		; ADC10 Reference Select Bit: 1 
SREF2		equ	0x8000		; ADC10 Reference Select Bit: 2 
ADC10SHT_0	equ	0x0000		; 4 x ADC10CLKs 
ADC10SHT_1	equ	0x0800		; 8 x ADC10CLKs 
ADC10SHT_2	equ	0x1000		; 16 x ADC10CLKs 
ADC10SHT_3	equ	0x1800		; 64 x ADC10CLKs 

SREF_0		equ	0x0000		; VR+ = AVCC and VR- = AVSS 
SREF_1		equ	0x2000		; VR+ = VREF+ and VR- = AVSS 
SREF_2		equ	0x4000		; VR+ = VEREF+ and VR- = AVSS 
SREF_3		equ	0x6000		; VR+ = VEREF+ and VR- = AVSS 
SREF_4		equ	0x8000		; VR+ = AVCC and VR- = VREF-/VEREF- 
SREF_5		equ	0xA000		; VR+ = VREF+ and VR- = VREF-/VEREF- 
SREF_6		equ	0xC000		; VR+ = VEREF+ and VR- = VREF-/VEREF- 
SREF_7		equ	0xE000		; VR+ = VEREF+ and VR- = VREF-/VEREF- 

; ADC10CTL1 
ADC10BUSY	equ	0x0001		; ADC10 BUSY 
CONSEQ0		equ	0x0002		; ADC10 Conversion Sequence Select 0 
CONSEQ1		equ	0x0004		; ADC10 Conversion Sequence Select 1 
ADC10SSEL0	equ	0x0008		; ADC10 Clock Source Select Bit: 0 
ADC10SSEL1	equ	0x0010		; ADC10 Clock Source Select Bit: 1 
ADC10DIV0	equ	0x0020		; ADC10 Clock Divider Select Bit: 0 
ADC10DIV1	equ	0x0040		; ADC10 Clock Divider Select Bit: 1 
ADC10DIV2	equ	0x0080		; ADC10 Clock Divider Select Bit: 2 
ISSH		equ	0x0100		; ADC10 Invert Sample Hold Signal 
ADC10DF		equ	0x0200		; ADC10 Data Format 0:binary 1:2's complement 
SHS0		equ	0x0400		; ADC10 Sample/Hold Source Bit: 0 
SHS1		equ	0x0800		; ADC10 Sample/Hold Source Bit: 1 
INCH0		equ	0x1000		; ADC10 Input Channel Select Bit: 0 
INCH1		equ	0x2000		; ADC10 Input Channel Select Bit: 1 
INCH2		equ	0x4000		; ADC10 Input Channel Select Bit: 2 
INCH3		equ	0x8000		; ADC10 Input Channel Select Bit: 3 

CONSEQ_0	equ	0x0000		; Single channel single conversion 
CONSEQ_1	equ	0x0002		; Sequence of channels 
CONSEQ_2	equ	0x0004		; Repeat single channel 
CONSEQ_3	equ	0x0006		; Repeat sequence of channels 

ADC10SSEL_0	equ	0x0000		; ADC10OSC 
ADC10SSEL_1	equ	0x0008		; ACLK 
ADC10SSEL_2	equ	0x0010		; MCLK 
ADC10SSEL_3	equ	0x0018		; SMCLK 

ADC10DIV_0	equ	0x0000		; ADC10 Clock Divider Select 0 
ADC10DIV_1	equ	0x0020		; ADC10 Clock Divider Select 1 
ADC10DIV_2	equ	0x0040		; ADC10 Clock Divider Select 2 
ADC10DIV_3	equ	0x0060		; ADC10 Clock Divider Select 3 
ADC10DIV_4	equ	0x0080		; ADC10 Clock Divider Select 4 
ADC10DIV_5	equ	0x00A0		; ADC10 Clock Divider Select 5 
ADC10DIV_6	equ	0x00C0		; ADC10 Clock Divider Select 6 
ADC10DIV_7	equ	0x00E0		; ADC10 Clock Divider Select 7 

SHS_0		equ	0x0000		; ADC10SC 
SHS_1		equ	0x0400		; TA3 OUT1 
SHS_2		equ	0x0800		; TA3 OUT0 
SHS_3		equ	0x0C00		; TA3 OUT2 

INCH_0		equ	0x0000		; Selects Channel 0 
INCH_1		equ	0x1000		; Selects Channel 1 
INCH_2		equ	0x2000		; Selects Channel 2 
INCH_3		equ	0x3000		; Selects Channel 3 
INCH_4		equ	0x4000		; Selects Channel 4 
INCH_5		equ	0x5000		; Selects Channel 5 
INCH_6		equ	0x6000		; Selects Channel 6 
INCH_7		equ	0x7000		; Selects Channel 7 
INCH_8		equ	0x8000		; Selects Channel 8 
INCH_9		equ	0x9000		; Selects Channel 9 
INCH_10		equ	0xA000		; Selects Channel 10 
INCH_11		equ	0xB000		; Selects Channel 11 
INCH_12		equ	0xC000		; Selects Channel 12 
INCH_13		equ	0xD000		; Selects Channel 13 
INCH_14		equ	0xE000		; Selects Channel 14 
INCH_15		equ	0xF000		; Selects Channel 15 

; ADC10DTC0 
ADC10FETCH	equ	0x001		; This bit should normally be reset 
ADC10B1		equ	0x002		; ADC10 block one 
ADC10CT		equ	0x004		; ADC10 continuous transfer 
ADC10TB		equ	0x008		; ADC10 two-block mode 
ADC10DISABLE	equ	0x000		; ADC10DTC1 

;;;
;;; Basic Clock Module
;;;

#define __MSP430_HAS_BC2__		; Definition to show that Module is available 

DCOCTL		equ	0x0056		; DCO Clock Frequency Control 
BCSCTL1		equ	0x0057		; Basic Clock System Control 1 
BCSCTL2		equ	0x0058		; Basic Clock System Control 2 
BCSCTL3		equ	0x0053		; Basic Clock System Control 3 

MOD0		equ	0x01		; Modulation Bit 0 
MOD1		equ	0x02		; Modulation Bit 1 
MOD2		equ	0x04		; Modulation Bit 2 
MOD3		equ	0x08		; Modulation Bit 3 
MOD4		equ	0x10		; Modulation Bit 4 
DCO0		equ	0x20		; DCO Select Bit 0 
DCO1		equ	0x40		; DCO Select Bit 1 
DCO2		equ	0x80		; DCO Select Bit 2 

RSEL0		equ	0x01		; Range Select Bit 0 
RSEL1		equ	0x02		; Range Select Bit 1 
RSEL2		equ	0x04		; Range Select Bit 2 
RSEL3		equ	0x08		; Range Select Bit 3 
DIVA0		equ	0x10		; ACLK Divider 0 
DIVA1		equ	0x20		; ACLK Divider 1 
XTS		equ	0x40		; LFXTCLK 0:Low Freq. / 1: High Freq. 
XT2OFF		equ	0x80		; Enable XT2CLK 

DIVA_0		equ	0x00		; ACLK Divider 0: /1 
DIVA_1		equ	0x10		; ACLK Divider 1: /2 
DIVA_2		equ	0x20		; ACLK Divider 2: /4 
DIVA_3		equ	0x30		; ACLK Divider 3: /8 

DIVS0		equ	0x02		; SMCLK Divider 0 
DIVS1		equ	0x04		; SMCLK Divider 1 
SELS		equ	0x08		; SMCLK Source Select 0:DCOCLK / 1:XT2CLK/LFXTCLK 
DIVM0		equ	0x10		; MCLK Divider 0 
DIVM1		equ	0x20		; MCLK Divider 1 
SELM0		equ	0x40		; MCLK Source Select 0 
SELM1		equ	0x80		; MCLK Source Select 1 

DIVS_0		equ	0x00		; SMCLK Divider 0: /1 
DIVS_1		equ	0x02		; SMCLK Divider 1: /2 
DIVS_2		equ	0x04		; SMCLK Divider 2: /4 
DIVS_3		equ	0x06		; SMCLK Divider 3: /8 

DIVM_0		equ	0x00		; MCLK Divider 0: /1 
DIVM_1		equ	0x10		; MCLK Divider 1: /2 
DIVM_2		equ	0x20		; MCLK Divider 2: /4 
DIVM_3		equ	0x30		; MCLK Divider 3: /8 

SELM_0		equ	0x00		; MCLK Source Select 0: DCOCLK 
SELM_1		equ	0x40		; MCLK Source Select 1: DCOCLK 
SELM_2		equ	0x80		; MCLK Source Select 2: XT2CLK/LFXTCLK 
SELM_3		equ	0xC0		; MCLK Source Select 3: LFXTCLK 
										   
LFXT1OF		equ	0x01		; Low/high Frequency Oscillator Fault Flag 
XT2OF		equ	0x02		; High frequency oscillator 2 fault flag 
XCAP0		equ	0x04		; XIN/XOUT Cap 0 
XCAP1		equ	0x08		; XIN/XOUT Cap 1 
LFXT1S0		equ	0x10		; Mode 0 for LFXT1 XTS = 0 
LFXT1S1		equ	0x20		; Mode 1 for LFXT1 XTS = 0 
XT2S0		equ	0x40		; Mode 0 for XT2 
XT2S1		equ	0x80		; Mode 1 for XT2 

XCAP_0		equ	0x00		; XIN/XOUT Cap : 0 pF 
XCAP_1		equ	0x04		; XIN/XOUT Cap : 6 pF 
XCAP_2		equ	0x08		; XIN/XOUT Cap : 10 pF 
XCAP_3		equ	0x0C		; XIN/XOUT Cap : 12.5 pF 

LFXT1S_0	equ	0x00		; Mode 0 for LFXT1 : Normal operation 
LFXT1S_1	equ	0x10		; Mode 1 for LFXT1 : Reserved 
LFXT1S_2	equ	0x20		; Mode 2 for LFXT1 : VLO 
LFXT1S_3	equ	0x30		; Mode 3 for LFXT1 : Digital input signal 

XT2S_0		equ	0x00		; Mode 0 for XT2 : 0.4 - 1 MHz 
XT2S_1		equ	0x40		; Mode 1 for XT2 : 1 - 4 MHz 
XT2S_2		equ	0x80		; Mode 2 for XT2 : 2 - 16 MHz 
XT2S_3		equ	0xC0		; Mode 3 for XT2 : Digital input signal 

;;;
;;; Comparator A
;;;

#define __MSP430_HAS_CAPLUS__		; Definition to show that Module is available 

CACTL1		equ	0x0059		; Comparator A Control 1 
CACTL2		equ	0x005A		; Comparator A Control 2 
CAPD		equ	0x005B		; Comparator A Port Disable 

CAIFG		equ	0x01		; Comp. A Interrupt Flag 
CAIE		equ	0x02		; Comp. A Interrupt Enable 
CAIES		equ	0x04		; Comp. A Int. Edge Select: 0:rising / 1:falling 
CAON		equ	0x08		; Comp. A enable 
CAREF0		equ	0x10		; Comp. A Internal Reference Select 0 
CAREF1		equ	0x20		; Comp. A Internal Reference Select 1 
CARSEL		equ	0x40		; Comp. A Internal Reference Enable 
CAEX		equ	0x80		; Comp. A Exchange Inputs 

CAREF_0		equ	0x00		; Comp. A Int. Ref. Select 0 : Off 
CAREF_1		equ	0x10		; Comp. A Int. Ref. Select 1 : 0.25*Vcc 
CAREF_2		equ	0x20		; Comp. A Int. Ref. Select 2 : 0.5*Vcc 
CAREF_3		equ	0x30		; Comp. A Int. Ref. Select 3 : Vt

CAOUT		equ	0x01		; Comp. A Output 
CAF		equ	0x02		; Comp. A Enable Output Filter 
P2CA0		equ	0x04		; Comp. A +Terminal Multiplexer 
P2CA1		equ	0x08		; Comp. A -Terminal Multiplexer 
P2CA2		equ	0x10		; Comp. A -Terminal Multiplexer 
P2CA3		equ	0x20		; Comp. A -Terminal Multiplexer 
P2CA4		equ	0x40		; Comp. A +Terminal Multiplexer 
CASHORT		equ	0x80		; Comp. A Short + and - Terminals 

CAPD0		equ	0x01		; Comp. A Disable Input Buffer of Port Register .0 
CAPD1		equ	0x02		; Comp. A Disable Input Buffer of Port Register .1 
CAPD2		equ	0x04		; Comp. A Disable Input Buffer of Port Register .2 
CAPD3		equ	0x08		; Comp. A Disable Input Buffer of Port Register .3 
CAPD4		equ	0x10		; Comp. A Disable Input Buffer of Port Register .4 
CAPD5		equ	0x20		; Comp. A Disable Input Buffer of Port Register .5 
CAPD6		equ	0x40		; Comp. A Disable Input Buffer of Port Register .6 
CAPD7		equ	0x80		; Comp. A Disable Input Buffer of Port Register .7 

;;;
;;; Flash Memory
;;;

#define __MSP430_HAS_FLASH2__		; Definition to show that Module is available 

FCTL1		equ	0x0128		; FLASH Control 1 
FCTL2		equ	0x012A		; FLASH Control 2 
FCTL3		equ	0x012C		; FLASH Control 3 

FRKEY		equ	0x9600		; Flash key returned by read 
FWKEY		equ	0xA500		; Flash key for write 
FXKEY		equ	0x3300		; for use with XOR instruction 

ERASE		equ	0x0002		; Enable bit for Flash segment erase 
MERAS		equ	0x0004		; Enable bit for Flash mass erase 
WRT		equ	0x0040		; Enable bit for Flash write 
BLKWRT		equ	0x0080		; Enable bit for Flash segment write 
SEGWRT		equ	0x0080		; old definition -- Enable bit for Flash segment write 

FN0		equ	0x0001		; Divide Flash clock by 1 to 64 using FN0 to FN5 according to: 
FN1		equ	0x0002		;  32*FN5 + 16*FN4 + 8*FN3 + 4*FN2 + 2*FN1 + FN0 + 1 
FN2		equ	0x0004	
FN3		equ	0x0008	
FN4		equ	0x0010	
FN5		equ	0x0020	
FSSEL0		equ	0x0040		; Flash clock select 0 -- to distinguish from USART SSELx 
FSSEL1		equ	0x0080		; Flash clock select 1 

FSSEL_0		equ	0x0000		; Flash clock select: 0 - ACLK 
FSSEL_1		equ	0x0040		; Flash clock select: 1 - MCLK 
FSSEL_2		equ	0x0080		; Flash clock select: 2 - SMCLK 
FSSEL_3		equ	0x00C0		; Flash clock select: 3 - SMCLK 

BUSY		equ	0x0001		; Flash busy: 1 
KEYV		equ	0x0002		; Flash Key violation flag 
ACCVIFG		equ	0x0004		; Flash Access violation flag 
WAIT		equ	0x0008		; Wait flag for segment write 
LOCK		equ	0x0010		; Lock bit: 1 - Flash is locked read only 
EMEX		equ	0x0020		; Flash Emergency Exit 
LOCKA		equ	0x0040		; Segment A Lock bit: read = 1 - Segment is locked read only 
FAIL		equ	0x0080		; Last Program or Erase failed 

;;;
;;; DIGITAL I/O Port1/2 Pull up / Pull down Resistors
;;;

#define __MSP430_HAS_PORT1_R__		; Definition to show that Module is available 
#define __MSP430_HAS_PORT2_R__		; Definition to show that Module is available 

P1IN		equ	0x0020		; Port 1 Input 
P1OUT		equ	0x0021		; Port 1 Output 
P1DIR		equ	0x0022		; Port 1 Direction 
P1IFG		equ	0x0023		; Port 1 Interrupt Flag 
P1IES		equ	0x0024		; Port 1 Interrupt Edge Select 
P1IE		equ	0x0025		; Port 1 Interrupt Enable 
P1SEL		equ	0x0026		; Port 1 Selection 
P1SEL2		equ	0x0041		; Port 1 Selection 2 
P1REN		equ	0x0027		; Port 1 Resistor Enable 

P2IN		equ	0x0028		; Port 2 Input 
P2OUT		equ	0x0029		; Port 2 Output 
P2DIR		equ	0x002A		; Port 2 Direction 
P2IFG		equ	0x002B		; Port 2 Interrupt Flag 
P2IES		equ	0x002C		; Port 2 Interrupt Edge Select 
P2IE		equ	0x002D		; Port 2 Interrupt Enable 
P2SEL		equ	0x002E		; Port 2 Selection 
P2SEL2		equ	0x0042		; Port 2 Selection 2 
P2REN		equ	0x002F		; Port 2 Resistor Enable 

;;;
;;; DIGITAL I/O Port3 Pull up / Pull down Resistors
;;;

#define __MSP430_HAS_PORT3_R__		; Definition to show that Module is available 

P3IN		equ	0x0018		; Port 3 Input 
P3OUT		equ	0x0019		; Port 3 Output 
P3DIR		equ	0x001A		; Port 3 Direction 
P3SEL		equ	0x001B		; Port 3 Selection 
P3SEL2		equ	0x0043		; Port 3 Selection 2 
P3REN		equ	0x0010		; Port 3 Resistor Enable 

;;;
;;; Timer0_A3
;;;

#define __MSP430_HAS_TA3__		; Definition to show that Module is available 

TA0IV		equ	0x012E		; Timer0_A3 Interrupt Vector Word 
TA0CTL		equ	0x0160		; Timer0_A3 Control 
TA0CCTL0	equ	0x0162		; Timer0_A3 Capture/Compare Control 0 
TA0CCTL1	equ	0x0164		; Timer0_A3 Capture/Compare Control 1 
TA0CCTL2	equ	0x0166		; Timer0_A3 Capture/Compare Control 2 
TA0R		equ	0x0170		; Timer0_A3 
TA0CCR0		equ	0x0172		; Timer0_A3 Capture/Compare 0 
TA0CCR1		equ	0x0174		; Timer0_A3 Capture/Compare 1 
TA0CCR2		equ	0x0176		; Timer0_A3 Capture/Compare 2 

; Alternate register names 
TAIV		equ	TA0IV		; Timer A Interrupt Vector Word 
TACTL		equ	TA0CTL		; Timer A Control 
TACCTL0		equ	TA0CCTL0	; Timer A Capture/Compare Control 0 
TACCTL1		equ	TA0CCTL1	; Timer A Capture/Compare Control 1 
TACCTL2		equ	TA0CCTL2	; Timer A Capture/Compare Control 2 
TAR		equ	TA0R		; Timer A 
TACCR0		equ	TA0CCR0		; Timer A Capture/Compare 0 
TACCR1		equ	TA0CCR1		; Timer A Capture/Compare 1 
TACCR2		equ	TA0CCR2		; Timer A Capture/Compare 2 

; Alternate register names 2 
CCTL0		equ	TACCTL0		; Timer A Capture/Compare Control 0 
CCTL1		equ	TACCTL1		; Timer A Capture/Compare Control 1 
CCTL2		equ	TACCTL2		; Timer A Capture/Compare Control 2 
CCR0		equ	TACCR0		; Timer A Capture/Compare 0 
CCR1		equ	TACCR1		; Timer A Capture/Compare 1 
CCR2		equ	TACCR2		; Timer A Capture/Compare 2 

TASSEL1		equ	0x0200		; Timer A clock source select 0 
TASSEL0		equ	0x0100		; Timer A clock source select 1 
ID1		equ	0x0080		; Timer A clock input divider 1 
ID0		equ	0x0040		; Timer A clock input divider 0 
MC1		equ	0x0020		; Timer A mode control 1 
MC0		equ	0x0010		; Timer A mode control 0 
TACLR		equ	0x0004		; Timer A counter clear 
TAIE		equ	0x0002		; Timer A counter interrupt enable 
TAIFG		equ	0x0001		; Timer A counter interrupt flag 

MC_0		equ	0x0000		; Timer A mode control: 0 - Stop 
MC_1		equ	0x0010		; Timer A mode control: 1 - Up to CCR0 
MC_2		equ	0x0020		; Timer A mode control: 2 - Continous up 
MC_3		equ	0x0030		; Timer A mode control: 3 - Up/Down 
ID_0		equ	0x0000		; Timer A input divider: 0 - /1 
ID_1		equ	0x0040		; Timer A input divider: 1 - /2 
ID_2		equ	0x0080		; Timer A input divider: 2 - /4 
ID_3		equ	0x00C0		; Timer A input divider: 3 - /8 
TASSEL_0	equ	0x0000		; Timer A clock source select: 0 - TACLK 
TASSEL_1	equ	0x0100		; Timer A clock source select: 1 - ACLK	 
TASSEL_2	equ	0x0200		; Timer A clock source select: 2 - SMCLK 
TASSEL_3	equ	0x0300		; Timer A clock source select: 3 - INCLK 

CM1		equ	0x8000		; Capture mode 1 
CM0		equ	0x4000		; Capture mode 0 
CCIS1		equ	0x2000		; Capture input select 1 
CCIS0		equ	0x1000		; Capture input select 0 
SCS		equ	0x0800		; Capture sychronize 
SCCI		equ	0x0400		; Latched capture signal read 
CAP		equ	0x0100		; Capture mode: 1 /Compare mode : 0 
OUTMOD2		equ	0x0080		; Output mode 2 
OUTMOD1		equ	0x0040		; Output mode 1 
OUTMOD0		equ	0x0020		; Output mode 0 
CCIE		equ	0x0010		; Capture/compare interrupt enable 
CCI		equ	0x0008		; Capture input signal read 
OUT		equ	0x0004		; PWM Output signal if output mode 0 
COV		equ	0x0002		; Capture/compare overflow flag 
CCIFG		equ	0x0001		; Capture/compare interrupt flag 

OUTMOD_0	equ	0x0000		; PWM output mode: 0 - output only 
OUTMOD_1	equ	0x0020		; PWM output mode: 1 - set 
OUTMOD_2	equ	0x0040		; PWM output mode: 2 - PWM toggle/reset 
OUTMOD_3	equ	0x0060		; PWM output mode: 3 - PWM set/reset 
OUTMOD_4	equ	0x0080		; PWM output mode: 4 - toggle 
OUTMOD_5	equ	0x00A0		; PWM output mode: 5 - Reset 
OUTMOD_6	equ	0x00C0		; PWM output mode: 6 - PWM toggle/set 
OUTMOD_7	equ	0x00E0		; PWM output mode: 7 - PWM reset/set 
CCIS_0		equ	0x0000		; Capture input select: 0 - CCIxA 
CCIS_1		equ	0x1000		; Capture input select: 1 - CCIxB 
CCIS_2		equ	0x2000		; Capture input select: 2 - GND 
CCIS_3		equ	0x3000		; Capture input select: 3 - Vcc 
CM_0		equ	0x0000		; Capture mode: 0 - disabled 
CM_1		equ	0x4000		; Capture mode: 1 - pos. edge 
CM_2		equ	0x8000		; Capture mode: 1 - neg. edge 
CM_3		equ	0xC000		; Capture mode: 1 - both edges 

; T0_A3IV Definitions 
TA0IV_NONE	equ	0x0000		; No Interrupt pending 
TA0IV_TACCR1	equ	0x0002		; TA0CCR1_CCIFG 
TA0IV_TACCR2	equ	0x0004		; TA0CCR2_CCIFG 
TA0IV_6		equ	0x0006		; Reserved 
TA0IV_8		equ	0x0008		; Reserved 
TA0IV_TAIFG	equ	0x000A		; TA0IFG 

;;;
;;; Timer1_A3
;;;

#define __MSP430_HAS_T1A3__		; Definition to show that Module is available 

TA1IV		equ	0x011E		; Timer1_A3 Interrupt Vector Word 
TA1CTL		equ	0x0180		; Timer1_A3 Control 
TA1CCTL0	equ	0x0182		; Timer1_A3 Capture/Compare Control 0 
TA1CCTL1	equ	0x0184		; Timer1_A3 Capture/Compare Control 1 
TA1CCTL2	equ	0x0186		; Timer1_A3 Capture/Compare Control 2 
TA1R		equ	0x0190		; Timer1_A3 
TA1CCR0		equ	0x0192		; Timer1_A3 Capture/Compare 0 
TA1CCR1		equ	0x0194		; Timer1_A3 Capture/Compare 1 
TA1CCR2		equ	0x0196		; Timer1_A3 Capture/Compare 2 

; Bits are already defined within the Timer0_Ax 

; T1_A3IV Definitions 
TA1IV_NONE	equ	0x0000		; No Interrupt pending 
TA1IV_TACCR1	equ	0x0002		; TA1CCR1_CCIFG 
TA1IV_TACCR2	equ	0x0004		; TA1CCR2_CCIFG 
TA1IV_TAIFG	equ	0x000A		; TA1IFG 

;;;
;;; USCI
;;;

#define __MSP430_HAS_USCI__		; Definition to show that Module is available 

UCA0CTL0	equ	0x0060		; USCI A0 Control Register 0 
UCA0CTL1	equ	0x0061		; USCI A0 Control Register 1 
UCA0BR0		equ	0x0062		; USCI A0 Baud Rate 0 
UCA0BR1		equ	0x0063		; USCI A0 Baud Rate 1 
UCA0MCTL	equ	0x0064		; USCI A0 Modulation Control 
UCA0STAT	equ	0x0065		; USCI A0 Status Register 
UCA0RXBUF	equ	0x0066		; USCI A0 Receive Buffer 
UCA0TXBUF	equ	0x0067		; USCI A0 Transmit Buffer 
UCA0ABCTL	equ	0x005D		; USCI A0 LIN Control 
UCA0IRTCTL	equ	0x005E		; USCI A0 IrDA Transmit Control 
UCA0IRRCTL	equ	0x005F		; USCI A0 IrDA Receive Control 


UCB0CTL0	equ	0x0068		; USCI B0 Control Register 0 
UCB0CTL1	equ	0x0069		; USCI B0 Control Register 1 
UCB0BR0		equ	0x006A		; USCI B0 Baud Rate 0 
UCB0BR1		equ	0x006B		; USCI B0 Baud Rate 1 
UCB0I2CIE	equ	0x006C		; USCI B0 I2C Interrupt Enable Register 
UCB0STAT	equ	0x006D		; USCI B0 Status Register 
UCB0RXBUF	equ	0x006E		; USCI B0 Receive Buffer 
UCB0TXBUF	equ	0x006F		; USCI B0 Transmit Buffer 
UCB0I2COA	equ	0x0118		; USCI B0 I2C Own Address 
UCB0I2CSA	equ	0x011A		; USCI B0 I2C Slave Address 


; UART-Mode Bits
UCPEN		equ	0x80		; Async. Mode: Parity enable 
UCPAR		equ	0x40		; Async. Mode: Parity	  0:odd / 1:even 
UCMSB		equ	0x20		; Async. Mode: MSB first  0:LSB / 1:MSB 
UC7BIT		equ	0x10		; Async. Mode: Data Bits  0:8-bits / 1:7-bits 
UCSPB		equ	0x08		; Async. Mode: Stop Bits  0:one / 1: two 
UCMODE1		equ	0x04		; Async. Mode: USCI Mode 1 
UCMODE0		equ	0x02		; Async. Mode: USCI Mode 0 
UCSYNC		equ	0x01		; Sync-Mode  0:UART-Mode / 1:SPI-Mode 

; SPI-Mode Bits
UCCKPH		equ	0x80		; Sync. Mode: Clock Phase 
UCCKPL		equ	0x40		; Sync. Mode: Clock Polarity 
UCMST		equ	0x08		; Sync. Mode: Master Select 

; I2C-Mode Bits
UCA10		equ	0x80		; 10-bit Address Mode 
UCSLA10		equ	0x40		; 10-bit Slave Address Mode 
UCMM		equ	0x20		; Multi-Master Environment 
;			0x10		; reserved 
UCMODE_0	equ	0x00		; Sync. Mode: USCI Mode: 0 
UCMODE_1	equ	0x02		; Sync. Mode: USCI Mode: 1 
UCMODE_2	equ	0x04		; Sync. Mode: USCI Mode: 2 
UCMODE_3	equ	0x06		; Sync. Mode: USCI Mode: 3 

; UART-Mode Bits
UCSSEL1		equ	0x80		; USCI 0 Clock Source Select 1 
UCSSEL0		equ	0x40		; USCI 0 Clock Source Select 0 
UCRXEIE		equ	0x20		; RX Error interrupt enable 
UCBRKIE		equ	0x10		; Break interrupt enable 
UCDORM		equ	0x08		; Dormant Sleep Mode 
UCTXADDR	equ	0x04		; Send next Data as Address 
UCTXBRK		equ	0x02		; Send next Data as Break 
UCSWRST		equ	0x01		; USCI Software Reset 

; SPI-Mode Bits
;			0x20		; reserved 
;			0x10		; reserved 
;			0x08		; reserved 
;			0x04		; reserved 
;			0x02		; reserved 

; I2C-Mode Bits
;			0x20		; reserved 
UCTR		equ	0x10		; Transmit/Receive Select/Flag 
UCTXNACK	equ	0x08		; Transmit NACK 
UCTXSTP		equ	0x04		; Transmit STOP 
UCTXSTT		equ	0x02		; Transmit START 
UCSSEL_0	equ	0x00		; USCI 0 Clock Source: 0 
UCSSEL_1	equ	0x40		; USCI 0 Clock Source: 1 
UCSSEL_2	equ	0x80		; USCI 0 Clock Source: 2 
UCSSEL_3	equ	0xC0		; USCI 0 Clock Source: 3 

UCBRF3		equ	0x80		; USCI First Stage Modulation Select 3 
UCBRF2		equ	0x40		; USCI First Stage Modulation Select 2 
UCBRF1		equ	0x20		; USCI First Stage Modulation Select 1 
UCBRF0		equ	0x10		; USCI First Stage Modulation Select 0 
UCBRS2		equ	0x08		; USCI Second Stage Modulation Select 2 
UCBRS1		equ	0x04		; USCI Second Stage Modulation Select 1 
UCBRS0		equ	0x02		; USCI Second Stage Modulation Select 0 
UCOS16		equ	0x01		; USCI 16-times Oversampling enable 

UCBRF_0		equ	0x00		; USCI First Stage Modulation: 0 
UCBRF_1		equ	0x10		; USCI First Stage Modulation: 1 
UCBRF_2		equ	0x20		; USCI First Stage Modulation: 2 
UCBRF_3		equ	0x30		; USCI First Stage Modulation: 3 
UCBRF_4		equ	0x40		; USCI First Stage Modulation: 4 
UCBRF_5		equ	0x50		; USCI First Stage Modulation: 5 
UCBRF_6		equ	0x60		; USCI First Stage Modulation: 6 
UCBRF_7		equ	0x70		; USCI First Stage Modulation: 7 
UCBRF_8		equ	0x80		; USCI First Stage Modulation: 8 
UCBRF_9		equ	0x90		; USCI First Stage Modulation: 9 
UCBRF_10	equ	0xA0		; USCI First Stage Modulation: A 
UCBRF_11	equ	0xB0		; USCI First Stage Modulation: B 
UCBRF_12	equ	0xC0		; USCI First Stage Modulation: C 
UCBRF_13	equ	0xD0		; USCI First Stage Modulation: D 
UCBRF_14	equ	0xE0		; USCI First Stage Modulation: E 
UCBRF_15	equ	0xF0		; USCI First Stage Modulation: F 

UCBRS_0		equ	0x00		; USCI Second Stage Modulation: 0 
UCBRS_1		equ	0x02		; USCI Second Stage Modulation: 1 
UCBRS_2		equ	0x04		; USCI Second Stage Modulation: 2 
UCBRS_3		equ	0x06		; USCI Second Stage Modulation: 3 
UCBRS_4		equ	0x08		; USCI Second Stage Modulation: 4 
UCBRS_5		equ	0x0A		; USCI Second Stage Modulation: 5 
UCBRS_6		equ	0x0C		; USCI Second Stage Modulation: 6 
UCBRS_7		equ	0x0E		; USCI Second Stage Modulation: 7 

UCLISTEN	equ	0x80		; USCI Listen mode 
UCFE		equ	0x40		; USCI Frame Error Flag 
UCOE		equ	0x20		; USCI Overrun Error Flag 
UCPE		equ	0x10		; USCI Parity Error Flag 
UCBRK		equ	0x08		; USCI Break received 
UCRXERR		equ	0x04		; USCI RX Error Flag 
UCADDR		equ	0x02		; USCI Address received Flag 
UCBUSY		equ	0x01		; USCI Busy Flag 
UCIDLE		equ	0x02		; USCI Idle line detected Flag 

;			0x80		; reserved 
;			0x40		; reserved 
;			0x20		; reserved 
;			0x10		; reserved 
UCNACKIE	equ	0x08		; NACK Condition interrupt enable 
UCSTPIE		equ	0x04		; STOP Condition interrupt enable 
UCSTTIE		equ	0x02		; START Condition interrupt enable 
UCALIE		equ	0x01		; Arbitration Lost interrupt enable 

UCSCLLOW	equ	0x40		; SCL low 
UCGC		equ	0x20		; General Call address received Flag 
UCBBUSY		equ	0x10		; Bus Busy Flag 
UCNACKIFG	equ	0x08		; NAK Condition interrupt Flag 
UCSTPIFG	equ	0x04		; STOP Condition interrupt Flag 
UCSTTIFG	equ	0x02		; START Condition interrupt Flag 
UCALIFG		equ	0x01		; Arbitration Lost interrupt Flag 

UCIRTXPL5	equ	0x80		; IRDA Transmit Pulse Length 5 
UCIRTXPL4	equ	0x40		; IRDA Transmit Pulse Length 4 
UCIRTXPL3	equ	0x20		; IRDA Transmit Pulse Length 3 
UCIRTXPL2	equ	0x10		; IRDA Transmit Pulse Length 2 
UCIRTXPL1	equ	0x08		; IRDA Transmit Pulse Length 1 
UCIRTXPL0	equ	0x04		; IRDA Transmit Pulse Length 0 
UCIRTXCLK	equ	0x02		; IRDA Transmit Pulse Clock Select 
UCIREN		equ	0x01		; IRDA Encoder/Decoder enable 

UCIRRXFL5	equ	0x80		; IRDA Receive Filter Length 5 
UCIRRXFL4	equ	0x40		; IRDA Receive Filter Length 4 
UCIRRXFL3	equ	0x20		; IRDA Receive Filter Length 3 
UCIRRXFL2	equ	0x10		; IRDA Receive Filter Length 2 
UCIRRXFL1	equ	0x08		; IRDA Receive Filter Length 1 
UCIRRXFL0	equ	0x04		; IRDA Receive Filter Length 0 
UCIRRXPL	equ	0x02		; IRDA Receive Input Polarity 
UCIRRXFE	equ	0x01		; IRDA Receive Filter enable 

;			0x80		; reserved 
;			0x40		; reserved 
UCDELIM1	equ	0x20		; Break Sync Delimiter 1 
UCDELIM0	equ	0x10		; Break Sync Delimiter 0 
UCSTOE		equ	0x08		; Sync-Field Timeout error 
UCBTOE		equ	0x04		; Break Timeout error 
;			0x02		; reserved 
UCABDEN		equ	0x01		; Auto Baud Rate detect enable 

UCGCEN		equ	0x8000		; I2C General Call enable 
UCOA9		equ	0x0200		; I2C Own Address 9 
UCOA8		equ	0x0100		; I2C Own Address 8 
UCOA7		equ	0x0080		; I2C Own Address 7 
UCOA6		equ	0x0040		; I2C Own Address 6 
UCOA5		equ	0x0020		; I2C Own Address 5 
UCOA4		equ	0x0010		; I2C Own Address 4 
UCOA3		equ	0x0008		; I2C Own Address 3 
UCOA2		equ	0x0004		; I2C Own Address 2 
UCOA1		equ	0x0002		; I2C Own Address 1 
UCOA0		equ	0x0001		; I2C Own Address 0 

UCSA9		equ	0x0200		; I2C Slave Address 9 
UCSA8		equ	0x0100		; I2C Slave Address 8 
UCSA7		equ	0x0080		; I2C Slave Address 7 
UCSA6		equ	0x0040		; I2C Slave Address 6 
UCSA5		equ	0x0020		; I2C Slave Address 5 
UCSA4		equ	0x0010		; I2C Slave Address 4 
UCSA3		equ	0x0008		; I2C Slave Address 3 
UCSA2		equ	0x0004		; I2C Slave Address 2 
UCSA1		equ	0x0002		; I2C Slave Address 1 
UCSA0		equ	0x0001		; I2C Slave Address 0 

;;;
;;; WATCHDOG TIMER
;;;

#define __MSP430_HAS_WDT__		; Definition to show that Module is available 

WDTCTL		equ	0x0120		; Watchdog Timer Control 

; The bit names have been prefixed with "WDT" 
WDTIS0		equ	0x0001	
WDTIS1		equ	0x0002	
WDTSSEL		equ	0x0004	
WDTCNTCL	equ	0x0008	
WDTTMSEL	equ	0x0010	
WDTNMI		equ	0x0020	
WDTNMIES	equ	0x0040	
WDTHOLD		equ	0x0080	

WDTPW		equ	0x5A00	

; WDT-interval times [1ms] coded with Bits 0-2 
; WDT is clocked by fSMCLK assumed 1MHz 
WDT_MDLY_32	equ	WDTPW+WDTTMSEL+WDTCNTCL				; 32ms interval default 
WDT_MDLY_8	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTIS0			; 8ms	  " 
WDT_MDLY_0_5	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1			; 0.5ms	  " 
WDT_MDLY_0_064	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1+WDTIS0		; 0.064ms " 

; WDT is clocked by fACLK assumed 32KHz 
WDT_ADLY_1000	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL			; 1000ms  " 
WDT_ADLY_250	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS0		; 250ms	  " 
WDT_ADLY_16	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1		; 16ms	  " 
WDT_ADLY_1_9	equ	WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0	; 1.9ms	  " 

; Watchdog mode -> reset after expired time 
; WDT is clocked by fSMCLK assumed 1MHz 
WDT_MRST_32	equ	WDTPW+WDTCNTCL					; 32ms interval default 
WDT_MRST_8	equ	WDTPW+WDTCNTCL+WDTIS0				; 8ms	  " 
WDT_MRST_0_5	equ	WDTPW+WDTCNTCL+WDTIS1				; 0.5ms	  " 
WDT_MRST_0_064	equ	WDTPW+WDTCNTCL+WDTIS1+WDTIS0			; 0.064ms " 

; WDT is clocked by fACLK assumed 32KHz 
WDT_ARST_1000	equ	WDTPW+WDTCNTCL+WDTSSEL				; 1000ms  " 
WDT_ARST_250	equ	WDTPW+WDTCNTCL+WDTSSEL+WDTIS0			; 250ms	  " 
WDT_ARST_16	equ	WDTPW+WDTCNTCL+WDTSSEL+WDTIS1			; 16ms	  " 
WDT_ARST_1_9	equ	WDTPW+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0		; 1.9ms	  " 

; INTERRUPT CONTROL 
; These two bits are defined in the Special Function Registers 
; WDTIE   0x01 
; WDTIFG  0x01 

;;;
;;; Calibration Data in Info Mem
;;;

CALDCO_16MHZ	equ	0x10F8		; DCOCTL  Calibration Data for 16MHz 
CALBC1_16MHZ	equ	0x10F9		; BCSCTL1 Calibration Data for 16MHz 
CALDCO_12MHZ	equ	0x10FA		; DCOCTL  Calibration Data for 12MHz 
CALBC1_12MHZ	equ	0x10FB		; BCSCTL1 Calibration Data for 12MHz 
CALDCO_8MHZ	equ	0x10FC		; DCOCTL  Calibration Data for 8MHz 
CALBC1_8MHZ	equ	0x10FD		; BCSCTL1 Calibration Data for 8MHz 
CALDCO_1MHZ	equ	0x10FE		; DCOCTL  Calibration Data for 1MHz 
CALBC1_1MHZ	equ	0x10FF		; BCSCTL1 Calibration Data for 1MHz 


;;;
;;; Interrupt Vectors offset from 0xFFE0
;;;

PORT1_VECTOR		equ	0x0004		; 0xFFE4 Port 1 
PORT2_VECTOR		equ	0x0006		; 0xFFE6 Port 2 
ADC10_VECTOR		equ	0x000A		; 0xFFEA ADC10 
USCIAB0TX_VECTOR	equ	0x000C		; 0xFFEC USCI A0/B0 Transmit 
USCIAB0RX_VECTOR	equ	0x000E		; 0xFFEE USCI A0/B0 Receive 
TIMER0_A1_VECTOR	equ	0x0010		; 0xFFF0 Timer0A CC1, TA0 
TIMER0_A0_VECTOR	equ	0x0012		; 0xFFF2 Timer0_A CC0 
WDT_VECTOR		equ	0x0014		; 0xFFF4 Watchdog Timer 
COMPARATORA_VECTOR	equ	0x0016		; 0xFFF6 Comparator A 
TIMER1_A1_VECTOR	equ	0x0018		; 0xFFF8 Timer1_A CC1-4, TA1 
TIMER1_A0_VECTOR	equ	0x001A		; 0xFFFA Timer1_A CC0 
NMI_VECTOR		equ	0x001C		; 0xFFFC Non-maskable 
RESET_VECTOR		equ	0x001E		; 0xFFFE Reset [Highest Priority] 

;;;
;;; End of Modules
;;;
