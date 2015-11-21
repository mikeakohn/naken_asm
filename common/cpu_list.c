/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#include "cpu_list.h"

#include "asm/65xx.h"
#include "asm/65816.h"
#include "asm/680x.h"
#include "asm/68hc08.h"
#include "asm/680x0.h"
#include "asm/805x.h"
#include "asm/arm.h"
#include "asm/avr8.h"
#include "asm/common.h"
#include "asm/dspic.h"
#include "asm/epiphany.h"
#include "asm/mips32.h"
#include "asm/msp430.h"
#include "asm/powerpc.h"
#include "asm/stm8.h"
#include "asm/thumb.h"
#include "asm/tms1000.h"
#include "asm/tms9900.h"
#include "asm/z80.h"
#include "disasm/65xx.h"
#include "disasm/65816.h"
#include "disasm/680x.h"
#include "disasm/68hc08.h"
#include "disasm/680x0.h"
#include "disasm/805x.h"
#include "disasm/arm.h"
#include "disasm/avr8.h"
#include "disasm/dspic.h"
#include "disasm/epiphany.h"
#include "disasm/mips32.h"
#include "disasm/msp430.h"
#include "disasm/powerpc.h"
#include "disasm/stm8.h"
#include "disasm/thumb.h"
#include "disasm/tms1000.h"
#include "disasm/tms9900.h"
#include "disasm/z80.h"
#include "simulate/65xx.h"
#include "simulate/avr8.h"
#include "simulate/msp430.h"
#include "simulate/tms9900.h"
#include "simulate/z80.h"

struct _cpu_list cpu_list[] =
{
#ifdef ENABLE_65XX
  { "65xx", CPU_TYPE_65XX, ENDIAN_LITTLE, 1, 1, 0, 0, parse_instruction_65xx, NULL, list_output_65xx, disasm_range_65xx, simulate_init_65xx },
#endif
#ifdef ENABLE_65816
  { "65816", CPU_TYPE_65816, ENDIAN_LITTLE, 1, 1, 0, 0, parse_instruction_65816, NULL, list_output_65816, disasm_range_65816, NULL },
#endif
#ifdef ENABLE_680X
  { "680x", CPU_TYPE_680X, ENDIAN_BIG, 1, 1, 0, 0, parse_instruction_680x, NULL, list_output_680x, disasm_range_680x, NULL },
#endif
#ifdef ENABLE_68HC08
  { "68hc08", CPU_TYPE_68HC08, ENDIAN_BIG, 1, 1, 0, 0, parse_instruction_68hc08, NULL, list_output_68hc08, disasm_range_68hc08, NULL },
#endif
#ifdef ENABLE_680X0
  { "680x0", CPU_TYPE_680X0, ENDIAN_BIG, 1, 1, 0, 1, parse_instruction_680x0, NULL, list_output_680x0, disasm_range_680x0, NULL },
#endif
#ifdef ENABLE_805X
  { "805x", CPU_TYPE_805X, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_805x, NULL, list_output_805x, disasm_range_805x, NULL },
#endif
#ifdef ENABLE_ARM
  { "arm", CPU_TYPE_ARM, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_arm, NULL, list_output_arm, disasm_range_arm, NULL },
#endif
#ifdef ENABLE_AVR8
  { "avr8", CPU_TYPE_AVR8, ENDIAN_LITTLE, 2, 0, 0, 0, parse_instruction_avr8, NULL, list_output_avr8, disasm_range_avr8, simulate_init_avr8 },
#endif
#ifdef ENABLE_DSPIC
  { "dspic", CPU_TYPE_DSPIC, ENDIAN_LITTLE, 2, 0, 0, 0, parse_instruction_dspic, NULL, list_output_dspic, disasm_range_dspic, NULL },
#endif
#ifdef ENABLE_EPIPHANY
  { "epiphany", CPU_TYPE_EPIPHANY, ENDIAN_LITTLE, 1, 0, 0, 1, parse_instruction_epiphany, NULL, list_output_epiphany, disasm_range_epiphany, NULL },
#endif
#ifdef ENABLE_MIPS
  { "mips32", CPU_TYPE_MIPS32, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_mips, NULL, list_output_mips, disasm_range_mips, NULL },
#endif
#ifdef ENABLE_MSP430
  { "msp430", CPU_TYPE_MSP430, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_msp430, NULL, list_output_msp430, disasm_range_msp430, simulate_init_msp430 },
  { "msp430x", CPU_TYPE_MSP430X, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_msp430, NULL, list_output_msp430x, disasm_range_msp430x, simulate_init_msp430},
#endif
#ifdef ENABLE_DSPIC
  { "pic24", CPU_TYPE_PIC24, ENDIAN_LITTLE, 2, 0, 0, 0, parse_instruction_dspic, NULL, list_output_dspic, disasm_range_dspic, NULL },
#endif
#ifdef ENABLE_POWERPC
  { "powerpc", CPU_TYPE_POWERPC, ENDIAN_BIG, 1, 0, 0, 0, parse_instruction_powerpc, NULL, list_output_powerpc, disasm_range_powerpc, NULL },
#endif
#ifdef ENABLE_STM8
  { "stm8", CPU_TYPE_STM8, ENDIAN_BIG, 1, 1, 0, 1, parse_instruction_stm8, NULL, list_output_stm8, disasm_range_stm8, NULL },
#endif
#ifdef ENABLE_THUMB
  { "thumb", CPU_TYPE_THUMB, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_thumb, NULL, list_output_thumb, disasm_range_thumb, NULL },
#endif
#ifdef ENABLE_TMS1000
  { "tms1000", CPU_TYPE_TMS1000, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_tms1000, NULL, list_output_tms1000, disasm_range_tms1000, NULL },
  { "tms1100", CPU_TYPE_TMS1100, ENDIAN_LITTLE, 1, 0, 0, 0, parse_instruction_tms1100, NULL, list_output_tms1100, disasm_range_tms1100, NULL },
#endif
#ifdef ENABLE_TMS9900
  { "tms9900", CPU_TYPE_TMS9900, ENDIAN_BIG, 1, 0, 0, 0, parse_instruction_tms9900, NULL, list_output_tms9900, disasm_range_tms9900, simulate_init_tms9900 },
#endif
#ifdef ENABLE_Z80
  { "z80", CPU_TYPE_Z80, ENDIAN_LITTLE, 1, 0, 1, 0, parse_instruction_z80, NULL, list_output_z80, disasm_range_z80, simulate_init_z80 },
#endif
  { NULL },
};


