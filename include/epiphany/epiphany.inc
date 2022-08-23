;;
;; This is an include file for Adapteva's Ephiphany III chip found on a
;; Parallella board.
;;
;; For more info:
;; https://www.parallella.org/
;;

CONFIG      equ 0xf0400 ; Core configuration
STATUS      equ 0xf0404 ; Core status
PC          equ 0xf0408 ; Program counter
DEBUGSTATUS equ 0xf040c ; Debug status
LC          equ 0xf0414 ; Hardware loop counter
LS          equ 0xf0418 ; Hardware loop start address
LE          equ 0xf041c ; Hardware loop end address
IRET        equ 0xf0420 ; Interrupt PC return value
IMASK       equ 0xf0424 ; Interrupt mask
ILAT        equ 0xf0428 ; Interrupt latch
ILATST      equ 0xf042c ; Alias for setting interrupts
ILATCL      equ 0xf0430 ; Alias for clearing interrupts
IPEND       equ 0xf0434 ; Interrupts currently in process
FSTATUS     equ 0xf0440 ; Alias for writing to all STATUS bits
CTIMER0     equ 0xf0438 ; Core timer0
CTIMER1     equ 0xf043c ; Core timer1
MEMSTATUS   equ 0xf0604 ; Memory protection status
MEMPROTECT  equ 0xf0608 ; Memory protection configuration
DMA0CONFIG  equ 0xf0500 ; DMA channel0 configuration
DMA0STRIDE  equ 0xf0504 ; DMA channel0 stride
DMA0COUNT   equ 0xf0508 ; DMA channel0 count
DMA0SRCADDR equ 0xf050c ; DMA channel0 source address
DMA0DSTADDR equ 0xf0510 ; DMA channel0 destination address
DMA0AUTO0   equ 0xf0514 ; DMA channel0 slave lower data
DMA0AUTO1   equ 0xf0518 ; DMA channel0 slave upper data
DMA0STATUS  equ 0xf051c ; DMA channel0 status
DMA1CONFIG  equ 0xf0520 ; DMA channel1 configuration
DMA1STRIDE  equ 0xf0524 ; DMA channel1 stride
DMA1COUNT   equ 0xf0528 ; DMA channel1 count
DMA1SRCADDR equ 0xf052c ; DMA channel1 source address
DMA1DSTADDR equ 0xf0530 ; DMA channel1 destination address
DMA1AUTO0   equ 0xf0534 ; DMA channel1 slave lower data
DMA1AUTO1   equ 0xf0538 ; DMA channel1 slave upper data
DMA1STATUS  equ 0xf053c ; DMA channel1 status
MESHCONFIG  equ 0xf0700 ; Mesh node configuration
COREID      equ 0xf0704 ; Processor node ID
MULTICAST   equ 0xf0708 ; Multicast configuration
CMESHROUTE  equ 0xf0710 ; cMesh routing configuration
XMESHROUTE  equ 0xf0714 ; xMesh routing configuration
RMESHROUTE  equ 0xf0718 ; rMesh routing configuration

