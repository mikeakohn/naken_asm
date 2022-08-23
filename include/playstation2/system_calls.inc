;;
;; Playstation 2 System Calls
;;
;; A part of the naken_asm assembler 
;;
;; For more information:
;; https://www.mikekohn.net/
;;

;; System calls
_RFU000_FullReset equ 0
_ResetEE equ 1
_SetGsCrt equ 2
_AddIntcHandler equ 16
_RemoveIntcHandler equ 17
__EnableIntc equ 20
__DisableIntc equ 21
_EnableCache equ 97
_DisableCache equ 98
_FlushCache equ 100
_GsGetIMR equ 112
_GsPutIMR equ 113

