Installing
=========

Compiling
---------

By default the configure script will include support for all CPU's.  If
only a subset of CPU's is desired, the --enable-<cpu arch> option can
be added to the configure script.  Type ./configure --help for all options.
It's recommended that libreadline dev files are installed.  If they are
installed naken_util in interactive mode will allow the up-arrow to
bring back previous commands.  On Ubuntu/Debian this can be installed
by typing:  sudo apt-get install libreadline-dev

To compile naken_asm and naken_util (the disassembler / simulator) type:

    ./configure
    make

As of 2013-Feb-23 there is no installer yet.  The binaries can be copied
to /usr/local/bin.  This will be fixed later.


