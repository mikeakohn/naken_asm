Adding a simulator to naken_asm
========

Another feature in naken_asm is the ability to simulate MCUs in some of the 
architectures it supports, for example (MSP430, AVR8, 6502...etc). This page 
will introduce how to go about adding a simulator to the code base.



Example prototype of adding a MCU simulator.
------
There is code in simulate/ directory that can help in this process. namely the
source and header files null.c and null.h

Creating similar files under directory simulate, my_mcu.c and my_mcu.h, and 
including headers in the my_mcu.h:
``` c
#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"
```

Next, defining a struct that holds the registers for the microcontroller:

``` c
struct _simulate_null
{
    uint16_t reg[16];
};

```

Finally, defining a set of functions that control the simulation, for example:
* A function to initiates the simulation. 
* A function to dump memory, for debugging purposes.
* A function to push values of registers onto stack.
* A function to set a register, another function to set the program counter.
* A function to reset simulation of our microcontroller.
* A function to run the simulation.

There can be other customized functions for the simulation later.

Example of the code:
``` c
struct _simulate *simulate_init_null();
void simulate_free_null(struct _simulate *simulate);
int simulate_dumpram_null(struct _simulate *simulate, int start, int end);
void simulate_push_null(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_null(struct _simulate *simulate, char *reg_string, 
                                                        uint32_t value);
uint32_t simulate_get_reg_null(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_null(struct _simulate *simulate, uint32_t value);
void simulate_reset_null(struct _simulate *simulate);
void simulate_dump_registers_null(struct _simulate *simulate);
int simulate_run_null(struct _simulate *simulate, int max_cycles, int step);


```

That was for code definitions inside the header files for our my_mcu.h, the 
actual code will be written in my_mcu.c. There can be other architecture 
specific definitions for example to keep track of the running state of 
the simulator or to send signals and interrupts, but the code here is 
the bare minimum needed in this example.



Configure scripts
------
To reflect the changes of adding my_mcu simulator, the following files 
need to be edited:

* common/cpu_list.c: To add the headers of the new microcontroller header file
(add the headers to common/naken_util.c as well) and add the name of the 
microcontroller architecture using an #ifdef such as the one in line 342:
``` c
#ifdef ENABLE_my_mcu
  {
    "my_mcu",
    CPU_TYPE_my_mcu,
    ENDIAN_BIG,
    1,
    ALIGN_1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    SREC_16,
    parse_instruction_my_mcu,
    NULL,
    link_not_supported,
    list_output_my_mcu,
    disasm_range_my_mcu,
    simulate_init_my_mcu,
    NO_FLAGS,
  },
#endif
``` 

* Including the name of the object file to the source in my_mcu.c to 
configure script:
``` 
SIM_OBJS="${SIM_OBJS} my_mcu.o
```

Compiling
------
Compiling is just the same way as the program is normally compiled, no need 
for further changes or additions, if the compiler stops for some reason 
it would be due to not adding the proper header or having a function that 
wasn't defined in the header file before.
``` 
$ ./configure
$ make
```
