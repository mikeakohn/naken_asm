
;; RAM
appData equ 0x8000
ramCode equ 0x8100
curBGColor equ 0xa017
curCol equ equ 0x8459
curFGColor equ 0xa015
curRow equ equ 0x845a
fillRectColor equ 0xa5f4
penBGColor equ equ 0xa036
penFGColor equ equ 0xa038

;; Functions
CenterPutS equ 0x55a5
ClearRect equ 0x4d44
DispHL equ 0x44fe
FillRect equ 0x4d4a
IPoint equ 0x47ce
PutS equ 0x4501
SetPenBG_White equ 0x563b

