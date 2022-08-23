;; SGP30 - Gas Sensor
;;
;; Date: July 27, 2019
;; Author: Michael Kohn
;; Email: mike@mikekohn.net
;; Web: https://www.mikekohn.net/
;;
;; Part of the naken_asm project

;; Sensor registers.

SGP30_INIT_AIR_QUALITY        equ 0x2003
SGP30_MEASURE_AIR_QUALITY     equ 0x2008
SGP30_GET_BASELINE            equ 0x2015
SGP30_SET_BASELINE            equ 0x201e
SGP30_SET_HUMIDITY            equ 0x2061
SGP30_MEASURE_TEST            equ 0x2032
SGP30_GET_FEATURE_SET_VERSION equ 0x202f
SGP30_MEASURE_RAW_SIGNALS     equ 0x2050

SGP30_SOFT_RESET              equ 0x0006
SGP30_GET_SERIAL_ID           equ 0x3682

