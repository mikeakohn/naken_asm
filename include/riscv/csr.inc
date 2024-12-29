;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; RISC-V CSR include.
;; Part of the naken_asm assembler
;;
;; Create by: Michael Kohn (mike@mikekohn.net)
;;      Date: 2024-Dec-28
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Vector unit config.
vstart equ 0x008
vxsat  equ 0x009
vxrm   equ 0x00a
vcsr   equ 0x00f
vl     equ 0xc20
vtype  equ 0xc21
vlenb  equ 0xc22

