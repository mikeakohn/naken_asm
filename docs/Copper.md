
Copper
======

The Copper is a co-processor in the Amiga computer.  It seems there's
no defined syntax for this instruction set so I made up my own.

There are just three instructions:

* WAIT
* SKIP
* MOVE

WAIT
----

Pauses the co-processor until the video beam hits a certain X,Y
location on the screen.  The syntax is:

wait <bfd>, <x>, <y>, <compare x>, <compare y>

* bfd is a blitter finish disable bit.  If it's not included in the instruction it's set to 0.  It tells the co-coprocessor to also wait until the blitter-finished flag is set.

* x is the horizontal position of the video beam (x4 in low-res mode and x8 in hi-res.. valid values 0 to 127).
* y is the vertical position of the video beam (valid values 0 to 255).
* compare x sets the horizontal comparison enable bits (valid values 0 to 0x7f).  If excluded they are set to 1's.
* compare y sets the vertical comparison enable bits (valid values 0 to 0x7f).  If excluded they are set to 1's.

- Note: The x and y values include full overscan, including blanking areas.
- Note: If there is no x and y value defined, then x=0xff and y=0xfe which
will pause the copper until video beam moves back to the top of of the display.

Examples:

    wait 50, 50, 0x7f, 0x7f
    wait 50, 50
    wait bfd, 50, 50, 0x7f, 0x7f
    wait bfd, 50, 50
    wait

MOVE
----

Moves data into one of the custom chip registers.  The syntax is:

move <register>, <value>

* register is one of the graphics sound registers in the system (example:
the color palette, sound chip, sprites, etc). Valid value must be an even
number between 0 to 510.

* value is a 16 bit number to into that register.

Examples:
    move 0xe2, 0x1000

SKIP
----

Skips the next instruction  if the video beam is equal to or greater than
a certain X,Y location on the screen.  The syntax is:

wait <bfd>, <x>, <y>, <compare x>, <compare y>

Examples:

    skip 50, 50, 0x7f, 0x7f
    skip 50, 50
    skip bfd, 50, 50, 0x7f, 0x7f
    skip bfd, 50, 50

