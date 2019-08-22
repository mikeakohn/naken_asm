/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_AMIGA_HUNK_H
#define NAKEN_ASM_AMIGA_HUNK_H

#define HUNK_UNIT         0x3e7
#define HUNK_NAME         0x3e8
#define HUNK_CODE         0x3e9
#define HUNK_DATA         0x3ea
#define HUNK_BSS          0x3eb
#define HUNK_RELOC32      0x3ec
#define HUNK_RELOC16      0x3ed
#define HUNK_RELOC8       0x3ee
#define HUNK_EXT          0x3ef
#define HUNK_SYMBOL       0x3f0
#define HUNK_DEBUG        0x3f1
#define HUNK_END          0x3f2
#define HUNK_HEADER       0x3f3
#define HUNK_OVERLAY      0x3f5
#define HUNK_BREAK        0x3f6
#define HUNK_DREL32       0x3f7
#define HUNK_DREL16       0x3f8
#define HUNK_DREL8        0x3f9
#define HUNK_LIB          0x3fa
#define HUNK_INDEX        0x3fb
#define HUNK_RELOC32SHORT 0x3fc
#define HUNK_RELRELOC32   0x3fd
#define HUNK_ABSRELOC16   0x3fe
#define HUNK_PPC_CODE     0x4e9
#define HUNK_RELRELOC26   0x4ec

#endif

