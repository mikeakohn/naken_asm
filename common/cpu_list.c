/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include "cpu_list.h"

#include "asm_65xx.h"
#include "asm_680x.h"
#include "asm_68hc08.h"
#include "asm_680x0.h"
#include "asm_805x.h"
#include "asm_arm.h"
#include "asm_avr8.h"
#include "asm_common.h"
#include "asm_dspic.h"
#include "asm_mips.h"
#include "asm_msp430.h"
#include "asm_powerpc.h"
#include "asm_stm8.h"
#include "asm_thumb.h"
#include "asm_tms1000.h"
#include "asm_tms9900.h"
#include "asm_z80.h"
//#include "assembler.h"
#include "disasm_65xx.h"
#include "disasm_680x.h"
#include "disasm_68hc08.h"
#include "disasm_680x0.h"
#include "disasm_805x.h"
#include "disasm_arm.h"
#include "disasm_avr8.h"
#include "disasm_dspic.h"
#include "disasm_mips.h"
#include "disasm_msp430.h"
#include "disasm_powerpc.h"
#include "disasm_stm8.h"
#include "disasm_thumb.h"
#include "disasm_tms1000.h"
#include "disasm_tms9900.h"
#include "disasm_z80.h"

struct _cpu_list cpu_list[] =
{
#ifdef ENABLE_65XX
  { "65xx", CPU_TYPE_65XX, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_65xx, list_output_65xx, disasm_range_65xx },
#endif
#ifdef ENABLE_680X
  { "680x", CPU_TYPE_680X, ENDIAN_BIG, 1, 1, 0, parse_instruction_680x, list_output_680x, disasm_range_680x },
#endif
#ifdef ENABLE_68HC08
  { "68hc08", CPU_TYPE_68HC08, ENDIAN_BIG, 1, 1, 0, parse_instruction_68hc08, list_output_68hc08, disasm_range_68hc08 },
#endif
#ifdef ENABLE_680X0
  { "680x0", CPU_TYPE_680X0, ENDIAN_BIG, 1, 1, 0, parse_instruction_680x0, list_output_680x0, disasm_range_680x0 },
#endif
#ifdef ENABLE_805X
  { "805x", CPU_TYPE_805X, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_805x, list_output_805x, disasm_range_805x },
#endif
#ifdef ENABLE_ARM
  { "arm", CPU_TYPE_ARM, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_arm, list_output_arm, disasm_range_arm },
#endif
#ifdef ENABLE_AVR8
  { "avr8", CPU_TYPE_AVR8, ENDIAN_LITTLE, 2, 0, 0, parse_instruction_avr8, list_output_avr8, disasm_range_avr8 },
#endif
#ifdef ENABLE_DSPIC
  { "dspic", CPU_TYPE_DSPIC, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_dspic, list_output_dspic, disasm_range_dspic },
#endif
#ifdef ENABLE_MIPS
  { "mips", CPU_TYPE_MIPS, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_mips, list_output_mips, disasm_range_mips },
#endif
  { "msp430", CPU_TYPE_MSP430, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_msp430, list_output_msp430, disasm_range_msp430 },
#ifdef ENABLE_POWERPC
  { "powerpc", CPU_TYPE_POWERPC, ENDIAN_BIG, 1, 0, 0, parse_instruction_powerpc, list_output_powerpc, disasm_range_powerpc },
#endif
#ifdef ENABLE_STM8
  { "stm8", CPU_TYPE_STM8, ENDIAN_LITTLE, 1, 1, 0, parse_instruction_stm8, list_output_stm8, disasm_range_stm8 },
#endif
#ifdef ENABLE_THUMB
  { "thumb", CPU_TYPE_THUMB, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_thumb, list_output_thumb, disasm_range_thumb },
#endif
#ifdef ENABLE_TMS1000
  { "tms1000", CPU_TYPE_TMS1000, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_tms1000, list_output_tms1000, disasm_range_tms1000 },
  { "tms1100", CPU_TYPE_TMS1100, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_tms1100, list_output_tms1100, disasm_range_tms1100 },
#endif
#ifdef ENABLE_TMS9900
  { "tms9900", CPU_TYPE_TMS9900, ENDIAN_LITTLE, 1, 0, 0, parse_instruction_tms9900, list_output_tms9900, disasm_range_tms9900 },
#endif
#ifdef ENABLE_Z80
  { "z80", CPU_TYPE_Z80, ENDIAN_LITTLE, 1, 0, 1, parse_instruction_z80, list_output_z80, disasm_range_z80 },
#endif
  { NULL },
};


