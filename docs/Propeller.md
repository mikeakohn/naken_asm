
Propeller
=========

There were a couple minor adjustments to the assembler that comes
with the Parallax Propeller devkit.  They were mostly due to the
(generic) way naken_asm parses things.  The samples/propeller directory
in the git repository has examples.

Effects
-------

In the Parallax assembler, effects are put at the end of the line
with no comma seperating them from the instruction:

add dest, #immediate wz wc

In naken_asm a comma is needed:

add dest, #immediate, wz ec

Calls
-----

The Parallax assembler automatically figures out where the "ret"
instruction is within a function call.  This is done because the
Propeller chip doesn't use a stack or "normal" registers.  In order
to know where to return to after a "call" instruction, the code is
modified:

call #myfunction

myfunction:
  mov something, #value
myfunction_ret:
  ret

With naken_asm, it won't automatically scan for myfunction_ret and
needs to be specified.  The positive part is the label can be named
anything:

call myfunction_ret, #myfunction

myfunction:
  mov something, #value
myfunction_ret:
  ret






