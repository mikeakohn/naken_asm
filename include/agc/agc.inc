
;; Apollo Guidance Computer
;; Addresses are in octal.

;; Registers.
REG_A     equ 000
REG_L     equ 001
REG_Q     equ 002
EB        equ 003
FB        equ 004
REG_Z     equ 005
BB        equ 006
ZERO      equ 007 ; hardwired to 0?
ARUPT     equ 010
LRUPT     equ 011
QRUPT     equ 012
SAMPTIME1 equ 013
SAMPTIME2 equ 014
ZRUPT     equ 015
BBRUPT    equ 016
BRUPT     equ 017
CYR       equ 020
SR        equ 021
CYL       equ 022
EDOP      equ 023
TIME2     equ 024
TIME1     equ 025
TIME3     equ 026
TIME4     equ 027
TIME5     equ 030
TIME6     equ 031
CDUX      equ 032
CDUY      equ 033
CDUZ      equ 034
OPTY      equ 035
OPTX      equ 036
PIPAX     equ 037
PIPAY     equ 040
PIPAZ     equ 041
RHCP      equ 042
RHCY      equ 043
RHCR      equ 044
INLINK    equ 045
RNRAD     equ 046
GRYOCTR   equ 047
CDUXCMD   equ 050
CDUYCMD   equ 051
CDUZCMD   equ 052
OPTYCMD   equ 053
OPTXCMD   equ 054
THRUST    equ 055
LEMONM    equ 056
OUTLINK   equ 057
ALTM      equ 060

;; Vectors.
BOOT      equ 04000
T6RUPT    equ 04004
T5RUPT    equ 04010
T3RUPT    equ 04014
T4RUPT    equ 04020
KEYRUPT1  equ 04024
KEYRUPT2  equ 04030
UPRUPT    equ 04034
DOWNRUPT  equ 04040
RADARRUPT equ 04044
HANDRUPT  equ 04050
RUPT10    equ 04050 ; Alias of HANDRUPT

