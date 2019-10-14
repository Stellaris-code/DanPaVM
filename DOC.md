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

+ 0x00 		: `NOP`		: Does nothing
+ 0x01 a	: `SYSCALL`	: Calls the system procedure n°`a` (up to 256 system procedures)
+ 0x02 aa	: `BRT` 	: If the top stack value doesn't equal 0, jumps to the relative position `aa`
+ 0x03 aa	: `BRF` 	: If the top stack value equals 0, jumps to the relative position `aa`
+ 0x04 aa	: `JMP` 	: Jumps to the relative position `aa`
+ 0x05 aa	: `CALL` 	: Pushes the current IP to the call stack and jumps to the relative position `aa`
+ 0x06 		: `RET`		: Pops the first call stack address and returns to it
+ 0x07
+ 0x08 a	: `LDLOC`	: Pushes local variable n°`a` onto the data stack
+ 0x09 a	: `LDGLB`	: Pushes global variable n°`a` onto the data stack
+ 0x0A a	: `STLOC`	: Pops local variable n°`a` from the data stack
+ 0x0B a	: `STGLB`	: Pops global variable n°`a` from the data stack
+ 0x0C
+ 0x0D
+ 0x0E
+ 0x0F
+ 0x10		: `ADDI`	: Pops two integers, adds them and pushes the result back
+ 0x11		: `SUBI`	: Pops two integers, substracts the first one to the second and pushes the result back
+ 0x12		: `AND`		: Pops two values, ANDs them and pushes the result back
+ 0x12		: `OR`		: Pops two values, OR them and pushes the result back
+ 0x12		: `XOR`		: Pops two values, XORs them and pushes the result back
+ 0x12		: `AND`		: Pops a value, does a bitwise NOT on it and pushes the result back

### Glossary

+ DPVM : This project, Danmaku Paranoia Virtual Machine, or an instance of it
+ IP : Instruction pointer, program pointer


