
;; Apollo Guidance Computer
;; Addresses are in octal.

;; Registers.
REG_A     equ 0o00
REG_L     equ 0o01
REG_Q     equ 0o02
EB        equ 0o03
FB        equ 0o04
REG_Z     equ 0o05
BB        equ 0o06
ZERO      equ 0o07 ; hardwired to 0?
ARUPT     equ 0o10
LRUPT     equ 0o11
QRUPT     equ 0o12
SAMPTIME1 equ 0o13
SAMPTIME2 equ 0o14
ZRUPT     equ 0o15
BBRUPT    equ 0o16
BRUPT     equ 0o17
CYR       equ 0o20
SR        equ 0o21
CYL       equ 0o22
EDOP      equ 0o23
TIME2     equ 0o24
TIME1     equ 0o25
TIME3     equ 0o26
TIME4     equ 0o27
TIME5     equ 0o30
TIME6     equ 0o31
CDUX      equ 0o32
CDUY      equ 0o33
CDUZ      equ 0o34
OPTY      equ 0o35
OPTX      equ 0o36
PIPAX     equ 0o37
PIPAY     equ 0o40
PIPAZ     equ 0o41
RHCP      equ 0o42
RHCY      equ 0o43
RHCR      equ 0o44
INLINK    equ 0o45
RNRAD     equ 0o46
GRYOCTR   equ 0o47
CDUXCMD   equ 0o50
CDUYCMD   equ 0o51
CDUZCMD   equ 0o52
OPTYCMD   equ 0o53
OPTXCMD   equ 0o54
THRUST    equ 0o55
LEMONM    equ 0o56
OUTLINK   equ 0o57
ALTM      equ 0o60

;; Vectors.
BOOT      equ 0o4000
T6RUPT    equ 0o4004
T5RUPT    equ 0o4010
T3RUPT    equ 0o4014
T4RUPT    equ 0o4020
KEYRUPT1  equ 0o4024
KEYRUPT2  equ 0o4030
UPRUPT    equ 0o4034
DOWNRUPT  equ 0o4040
RADARRUPT equ 0o4044
HANDRUPT  equ 0o4050
RUPT10    equ 0o4050 ; Alias of HANDRUPT

