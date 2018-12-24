Developer Guidelines
====================

For anyone interested in adding a new assembler to naken_asm,
fixing bugs, or adding features, this document gives coding
standards that I will accept when doing a merge request and a
small tutorial on how to add a new CPU from scratch.

Adding A New CPU
----------------

In order to add a new CPU to naken_asm, the following steps are
needed (assume as an example the new cup is called "mycpu":

1. Update ./configure script (making sure to preserve alphabetical order):
  * Add the CPU to the CPU= list (all uppercaes).
  * Add the CPU to show_help() (all lowercaes).
  * Add an "--enable-" option in the "for option in" section.
2. Update common/cpulist.c:
  * Add includes for the asm/disasm and sim if needed (alphabetical order).
  * Add entry for CPU in list (alphabetical order).
3. Update common/cpulist.h:
  * Add entry in CPU_TYPE_ enum (alphabetical order).
4. Add files asm/mycpu.h and asm/mycpu.c.
  * Implement parse_instruction_mycpu().
  * Make sure in the .h file the #ifndef has the proper CPU name for guards.
5. Add files disasm/mycpu.h and disasm/mycpu.c
  * Implement 
  * Make sure in the .h file the #ifndef has the proper CPU name for guards.
6. Add files table/mycpu.h and table/mycpu.c (OPTIONAL)
  * Create any tables neeed.  Again this is optional. 
  * Make sure in the .h file the #ifndef has the proper CPU name for guards.
7. Add include to common/naken_util.c (alphabetical order).
  * Add to list of parse_instruction_t the new CPU.
  * Add CPU to list of supported CPU's in the "Usage:" list.

Coding Style
------------

Most of the files follow this coding standard so some of the other
assemblers can be used as an example, but here is the basic coding
standard:

* Indentation is 2 spaces (no tabs).
* Make sure there are no trailing spaces at the end of lines.
* 1 space always after the keyword "if" and "for".  For example:

This is okay:

    for (i = 0; i < 1; i++)
    if (i < 5)

This isn't:

    for(i = 0; i < 10; i++)
    if(i < 5)

This is also isn't okay:

    if ( i < 5 )
    if (i < 5 )
    if ( i < 5)

* Always leave (only) 1 space between functions and blocks of if / for while statements.

This is okay:

    if (i < 5)
    {
    }
      else
    {
    }

    if (n < 10)
    {
    }

Not okay:


    if (i < 5)
    {
    }
      else
    {
    }
    if (n < 10)
    {
    }

* Function names are always lowercase with _ separating words.
* Variable names are always lowercase with _ separating words.

