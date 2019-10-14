# Danmaku Paranoia VM Documentation

### General description

The DPVM is a stack based machine, which means it doesn't have any registers.

It is a 32 bits machine, so it works on 32 bits addresses and values.

The instructions are made of a single byte, sometimes followed by few other bytes for "arguments".

### The stacks

The DPVM has two stacks : the data stack and the call stack. We work on both stack from the top.

The data stack can hold any type of 32 bits values, and has a size of 128 kibi-bytes (32768‬ values). A lot of the opcodes work directly on it.

The call stack is used for returns from calls. It can hold 512 32 bit addresses. This stack is only accessed by `CALL` and `RET`.

### Variables

Aside from the stack, variables can be found in two arrays of 256 32 bit values : the global variables and the local variables.

The global variables are shared by all virtual machines, whereas the local variables should only be visible by the owner VM.

Variables can be accessed using `LDLOC`, `LDGLB`, `STLOC` and `STGLB`, to and from the stack. LD = load and ST = store, LOC = local, GLB = global.

### Data section

A part of the program can be dedicated to storing constants of any type, which can be then copied to variables. This part is read-only.

This space can be accessed with operations such as `LDAR`(?).

### Instruction set

The letters next to the opcode each indicate one byte that will be read as an argument for that operation.

Relative jumps are made by adding a 2 bytes integer to the current program pointer.

If it isn't precised, the words pop and push refer to the data stack

| OPCODE	| NAME		| DESCRIPTION
| --------- | --------- | ----
| 0x00 		| `NOP`		| Does nothing
| 0x01 a	| `SYSCALL`	| Calls the system procedure n°`a` (up to 256 system procedures)
| 0x02 aa	| `BRT` 	| If the top stack value doesn't equal 0, jumps to the relative position `aa`
| 0x03 aa	| `BRF` 	| If the top stack value equals 0, jumps to the relative position `aa`
| 0x04 aa	| `JMP` 	| Jumps to the relative position `aa`
| 0x05 aa	| `CALL` 	| Pushes the current IP to the call stack and jumps to the relative position `aa`
| 0x06 		| `RET`		| Pops the first call stack address and returns to it
| 0x07		| ?			|
| 0x08 a	| `LDLOC`	| Pushes local variable n°`a` onto the data stack
| 0x09 a	| `LDGLB`	| Pushes global variable n°`a` onto the data stack
| 0x0A a	| `STLOC`	| Pops local variable n°`a` from the data stack
| 0x0B a	| `STGLB`	| Pops global variable n°`a` from the data stack
| 0x0C		| `ARLD`	| ?
| 0x0D		| `ARST`	| ?
| 0x0E		| `ARLEN`	| ?
| 0x0F		| `ARRESIZE`| ?
| 0x10		| `ADDI`	| Pops two integers, adds them and pushes the result back
| 0x11		| `SUBI`	| Pops two integers, substracts the first one to the second and pushes the result back (?)
| 0x12		| `AND`		| Pops two values, ANDs them and pushes the result back
| 0x13		| `OR`		| Pops two values, OR them and pushes the result back
| 0x14		| `XOR`		| Pops two values, XORs them and pushes the result back
| 0x15		| `NOT`		| Pops a value, does a bitwise NOT on it and pushes the result back
| 0x16		| `SHL`		| Pops a value, shift it left once, pushes it back
| 0x17		| `SHR`		| Pops a value, shift it right once, pushes it back
| 0x18		| `LSR`		| Pops a value, shift it right once (with sign), pushes it back
| 0x19		| `NEGI`	| Pops an integer, negates it and pushes it back
| 0x1A		| `MULI`	| Pops two integers, multiplies them and pushes the result back
| 0x1B		| `DIVI`	| Pops two integers, divides the first one to the second and pushes the result back	(?)
| 0x1C		| `MODI`	| ?
| 0x1D		| ?			|
| 0x1E		| ?			|
| 0x1F		| ?			|
| 0x20		| `ADDF`	| Pops two floats, adds them and pushes the result
| 0x21		| `SUBF`	| Pops two floats, substract the first one to the second and pushes the result (?)
| 0x22		| ?			|
| 0x23		| ?			|
| 0x24		| ?			|
| 0x25		| ?			|
| 0x26		| ?			|
| 0x27		| ?			|
| 0x28		| ?			|
| 0x29		| `NEGF`	| Pops a float, negates it and pushes it back
| 0x2A		| `MULF`	| Pops two floats, multiplies them and pushes the result
| 0x2B		| `DIVF`	| Pops two floats, divides the first one to the second and pushes the result back	(?)
| 0x2C		| `MODF`	| ?
| 0x2D		| ?			|
| 0x2E		| ?			|
| 0x2F		| ?			|
| 0x30 a	| `PUSH`	| Pushes `a` to the stack (?)
| 0x31		| `SWAP`	| Swaps the two top-most values of the stack
| 0x32		| `DUP`		| Duplicates the top-most value of the stack
| 0x33		| ?			|
| 0x34		| ?			|
| 0x35		| ?			|
| 0x36		| `I2F`		| Pops an integer, converts it to a float and pushes it back
| 0x37		| `F2I`		| Pops a float, converts it to an integer and pushes it back
| 0x38		| `I2C`		| ?
| 0x39		| `C2I`		| ?
| 0x3A		| ?			|
| 0x3B		| ?			|
| ...		| ...		| ...
| 0xFF		| `DBG`		| Prints miscellaneous info about the vm for debugging purposes

### Glossary

- DPVM | This project, Danmaku Paranoia Virtual Machine, or an instance of it
- IP | Instruction pointer, program pointer


