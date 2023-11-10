/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include "table/f100_l.h"

struct _table_f100_l table_f100_l[] =
{
  { "nop",   0xf000, 0xffff, OP_NONE     },
  { "add",   0x9000, 0xf000, OP_ALU      },
  { "ads",   0x5000, 0xf000, OP_ALU      },
  { "and",   0xc000, 0xf000, OP_ALU      },
  { "cal",   0x2000, 0xf000, OP_ALU      },  // doesn't have /P+, /P-
  { "clr",   0x00f0, 0xfcf0, OP_BIT      },
  { "set",   0x00e0, 0xfcf0, OP_BIT      },
  { "cmp",   0xb000, 0xf000, OP_ALU      },
  { "halt",  0x0400, 0xfc00, OP_HALT     },
  { "incz",  0x7000, 0xf000, OP_INC      },
  { "jbc",   0x0080, 0xfcf0, OP_COND_JMP },
  { "jbs",   0x0090, 0xfcf0, OP_COND_JMP },
  { "jcs",   0x00a0, 0xfcf0, OP_COND_JMP },
  { "jsc",   0x00b0, 0xfcf0, OP_COND_JMP },
  { "jmp",   0xf000, 0xf000, OP_ALU      },
  { "lda",   0x8000, 0xf000, OP_ALU      },
  { "neq",   0xd000, 0xf000, OP_ALU      },
  { "rtn",   0x3000, 0xf800, OP_NONE     },
  { "rtc",   0x3800, 0xf800, OP_NONE     },
  { "sbs",   0x6000, 0xf000, OP_ALU      },
  { "sjm",   0x1000, 0xf000, OP_NONE     },
  { "sll",   0x0040, 0xfcc0, OP_SHIFT    },
  { "sla",   0x0040, 0xfcc0, OP_SHIFT    },
  { "srl",   0x0040, 0xfcc0, OP_SHIFT    },
  { "sra",   0x0040, 0xfcc0, OP_SHIFT    },
  { "sre",   0x0040, 0xfcc0, OP_SHIFT    },
  { "sla.d", 0x0040, 0xfcc0, OP_SHIFT_D    },
  { "sll.d", 0x0040, 0xfcc0, OP_SHIFT_D    },
  { "sra.d", 0x0040, 0xfcc0, OP_SHIFT_D    },
  { "srl.d", 0x0040, 0xfcc0, OP_SHIFT_D    },
  { "sto",   0x4000, 0xf000, OP_ALU      },
  { "sub",   0xa000, 0xf000, OP_ALU      },
  { NULL,    0x0000, 0x0000, OP_NONE     },
};

