
This test checks to make sure the symbol addesses (labels) have
the same value at the end of pass 1 and pass 2.

The reason for this test is because some instructions in some assemblers
can have a short or longer version.  On pass 1 if the assembler can't
determine the value of a constant for example, then it must use the long
version.  On pass 2 when it does figure out the constant it has to STILL
use the long version.  For example:

.org 0x1000
mov.w #label, &0x100
.org 2
label:
  dw 100

The symbol label is equal to 2, so if the assembler had known this
it could have made the mov.w 2 bytes long.  Because it didn't know it
was 2, it had to assume it was the longer version of the instruction
which is 4 bytes long.  So on both passes it needs to use the long
version.

To add a new test for this test, create a .h file for that specific
CPU and add some code (as seen in the example for msp430_test.h).  There
needs to be 5 labels in the test program labeled first, second, third,
fourth, and fifth.  Then in the symbol_address.c file add an include for
the CPU (if there is a new include) and in main() add:

errors += test_symbols("My CPU", code_constant_string);

The main function test_symbols will assemble the program pass 1 and
figure out the addresses of labels mentioned above and then assemble
for pass 2 and test to make sure they didn't change.

