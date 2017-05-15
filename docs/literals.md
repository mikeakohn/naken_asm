Literals
========

All numbers can be spaced out with underscores.  For example 100_100
will be interpreted as 100000 and 0x1234_3333 will be interpreted
as 0x1234333.

Hexadecimal
-----------

Hexadecimals values can be prefixed with 0x or postfixed with h.  For
example the following are valid and the same:

0x1234, 1234h, 0x12_34

Binary
------

Binary numbers can be prefixed with 0b or postfixed with b.  For
example the following are valid and the same:

0b1001, 1001b, 0xb10_01

Octal
-----

Octal numbers are postfixed with q.  For example the following is
a valid octal number:

17q

Special Characters
------------------

As with other assemblers, $ means "current address".  So a jmp $ will be
an infinite loop.



