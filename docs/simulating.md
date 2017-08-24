Simulating
==========
The simulator (as of the writing of this file only MSP430, AVR8, and 6502 is
supported) allows the user to show how their program will run. It gives a view
of all the registers and how many CPU cycles have passed, along with the current
instruction that executed and the next few in memory. To test the simulation,
after assembling launchpad.asm type:

    ./naken_util launchpad_blink.hex
    speed 1
    run

This will load the laundpad_blink.hex file and set the CPU speed to 1Hz.
The run command of course will start the simulation. To pause it just press
Ctrl-C. To run in single step mode:

    speed 0
    run

One instruction is executed and the simulator breaks. Pressing enter or
typing 'run' again will execute one more instruction. If there is a
function needing to be profiled at location 0xf010, typing: call 0xf010
will execute it. The program will stop running at the RET of the function.

Three other useful commands are call, push, and set. To push something on
the stack just do: push 0x1000 for example to push 0x1000 to the stack.
To set the value of a register: set pc=1000 or set r5=0xffff. To call a
function: call 0xf034 to call a function at location 0xf034. This pushes
the return address 0xffff on the stack and sets PC to 0xf034. When the
simulator sees PC==0xffff it returns back to you so the number of clock
cycles it took to run the function.

There are 4 commands for reading and writing memory: bprint, wprint, bwrite,
wwrite. So to write 5 bytes to location 0x1000, I could type:

It can confirm it's written by typing: bprint 0x1000

For reading and writing word sized data, wprint and wwrite work the same way.

As of March 16, 2014, I added a -run command to naken_util for MSP430.
A program can now be assembled and then run directly from the command-line
without entering the simulator:

    naken_util -run program.hex

If when the program hits a ret instruction that didn't have a matching
call instruction, the program will stop and the registers will be dumped
along with number of cycles passed.  This was added for automated testing
in Java Grinder but could be pretty useful other places.

To assist with testing software there is a newer option "break_io"
which allows the user to set an address as an I/O port that will
exit the simulator if written so:

    naken_util -msp430 -break_io 0x0000 -run program.hex

So now if the running program.hex does a mov.b #5, 0x0000 then the
simulator exit with a return code of 5.

