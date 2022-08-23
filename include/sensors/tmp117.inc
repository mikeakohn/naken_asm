;; TMP117 - High-Accuracy, Low-Power, Digital Temperature Sensor
;;
;; Date: August 2, 2019
;; Author: Michael Kohn
;; Email: mike@mikekohn.net
;; Web: https://www.mikekohn.net/
;;
;; Part of the naken_asm project

;; Sensor registers.

TMP117_TEMPERATURE        equ 0x00
TMP117_CONFIGURATION      equ 0x01
TMP117_HIGH_LIMIT         equ 0x02
TMP117_LOW_LIMIT          equ 0x03
TMP117_EEPROM_UNLOCK      equ 0x04
TMP117_EEPROM1            equ 0x05
TMP117_EEPROM2            equ 0x06
TMP117_TEMPERATURE_OFFSET equ 0x07
TMP117_EEPROM3            equ 0x08
TMP117_DEVICE_ID          equ 0x0f

;; Bit masks
TMP117_HIGH_ALERT         equ 0x8000
TMP117_LOW_ALERT          equ 0x4000
TMP117_DATA_READY         equ 0x2000
TMP117_EEPROM_BUSY        equ 0x1000
TMP117_TNA                equ 0x0010
TMP117_POL                equ 0x0008
TMP117_DR_ALERT           equ 0x0004
TMP117_SOFT_RESET         equ 0x0002

TMP117_EEPROM_UNLOCK_EUN  equ 0x8000
TMP117_EEPROM_UNLOCK_BUSY equ 0x4000

;; Bit offsets
TMP117_MOD equ 10
TMP117_CONV equ 7
TMP117_AVG equ 5

