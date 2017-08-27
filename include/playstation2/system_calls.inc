;;
;; Playstation 2 System Calls
;;
;; A part of the naken_asm assembler 
;;
;; For more information:
;; http://www.mikekohn.net/
;;

;; System calls
SetGsCrt equ 2
AddIntcHandler equ 16
RemoveIntcHandler equ 17
_EnableIntc equ 20
_DisableIntc equ 21
GsGetIMR equ 112
GsPutIMR equ 113

