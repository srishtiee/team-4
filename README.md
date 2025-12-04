# Simple CPU Emulator

A complete software CPU emulator implemented in C++ with modular design, featuring a custom instruction set, assembler, and example programs. This project demonstrates CPU architecture, instruction execution, and **function calling with recursion**.

## Features

- **Modular Architecture**: Each CPU component is represented by appropriate classes/structs
- **Complete ISA**: 16-bit instruction set with arithmetic, logic, memory, and control flow operations
- **Assembler**: Full assembler with label and literal support
- **Debugging**: Instruction tracing and state inspection
- **Memory-Mapped I/O**: Character output support
- **Function Calls & Recursion**: Stack-based function calling mechanism with recursive support
- **Stack Inspection**: Commands to view stack contents and stack frames
- **Example Programs**: Timer, Hello World, and **Factorial with recursion**


## Building

```bash
make
```

This will create the `cpu_emulator` executable.

## Usage

### Interactive Mode

```bash
./cpu_emulator
```

Available commands:
- `load <file>` - Load and assemble program from file
- `run` - Run program until halt
- `step` - Execute one instruction
- `gpr` - Print General Purpose Registers
- `spr` - Print Special Purpose Registers
- `ram [addr] [len]` - Print RAM dump (default: 0x0000, 256 bytes)
- `dec [addr] [cnt]` - Print memory as decimal numbers (default: 0x0040, 10 words)
- `stack [cnt]` - Print stack contents (default: 20 words)
- `frames` - Print stack frames (for function calls)
- `state` - Print complete CPU state
- `trace on/off` - Enable/disable instruction tracing
- `reset` - Reset CPU to initial state
- `help` - Show help message
- `quit/exit` - Exit emulator

### Direct Execution

```bash
# Load and run a program directly
./cpu_emulator programs/hello.asm run
./cpu_emulator programs/timer.asm run
./cpu_emulator programs/factorial.asm run
```

### Example Session: Factorial with Recursion

```bash
$ ./cpu_emulator
> load programs/factorial.asm
Program loaded: 74 instructions
Labels:
  main: 0x0
  factorial: 0x28
  fact_return: 0x60
  base_case: 0x8A

> run
> dec 0x0040 1
=== Memory as Decimal Numbers ===
Starting address: 0x0040
[00] 0x0040: 120  (factorial(5) = 120)

> stack 20
=== Stack Contents ===
Stack Pointer (SP): 0xFFFF
Stack grows downward (from 0xFFFF)
...

> frames
=== Stack Frames ===
Stack Pointer (SP): 0xFFFF
(No stack frames detected - program completed)
```

## CPU Components

### Registers
- **GPRs (General Purpose Registers)**: 8 registers (R0-R7), 16-bit each
- **SPRs (Special Purpose Registers)**: PC (Program Counter), SP (Stack Pointer), FLAGS

### ALU
Performs arithmetic and logical operations, returns result with flags (overflow, carry, zero, negative).

### Control Unit
Orchestrates the fetch-decode-execute cycle, manages program counter, and controls all CPU components.

### Bus System
- **Instruction Bus**: Two-way communication for instruction fetch
- **Info Bus**: Data transfer between components
- **Control Bus**: One-way control signals from Control Unit

### Memory
- 64KB address space
- Memory-mapped I/O at 0xFF00-0xFFFF
- Byte-addressable, word-aligned
- **Stack**: Grows downward from 0xFFFF (256 words available)
- **Memory Layout**: 
  - 0x0000-0x003F: Program code
  - 0x0040-0xFEFF: Data section
  - 0xFF00-0xFFFF: Stack (grows downward)

## Instruction Set

See [docs/ISA.md](docs/ISA.md) for complete instruction set documentation.

Key instructions:
- **Arithmetic**: ADD, SUB
- **Logic**: AND, OR, XOR, NOT
- **Shifts**: SHL, SHR
- **Memory**: LD, ST
- **Immediate**: LDI
- **Control Flow**: JMP, JZ, JNZ, HLT

## Example Programs

### Hello World (`programs/hello.asm`)
Outputs "Hello, World!" using memory-mapped I/O.

### Timer (`programs/timer.asm`)
Demonstrates Fetch/Compute/Store cycles by counting down from 10 to 0.

### Factorial (`programs/factorial.asm`) ⭐ **NEW**
**Demonstrates function calls and recursion!**

Computes factorial(5) = 120 using a recursive function. This program demonstrates:
- **Function calling mechanism**: How functions are called and return
- **Stack frames**: How each recursive call creates a new stack frame
- **Recursion**: Multiple nested function calls with proper stack management
- **Memory layout**: How executable code, data, and stack are organized

**Usage**:
```bash
./cpu_emulator programs/factorial.asm run
> dec 0x0040 1        # View result (should be 120)
> stack 20           # View stack contents
> frames              # View stack frames during execution
```

**See also**:
- [Calling Convention Documentation](docs/CALLING_CONVENTION.md) - How function calls work
- [Memory Layout Documentation](docs/MEMORY_LAYOUT.md) - Memory organization
- [Recursion Trace Documentation](docs/RECURSION_TRACE.md) - Detailed execution trace

## Function Calls and Recursion

This emulator supports **stack-based function calling** with recursion. The implementation demonstrates:

1. **Calling Convention**: Parameters passed in registers (R1, R2), return value in R0
2. **Stack Management**: Stack grows downward from 0xFFFF, each frame contains return address and saved registers
3. **Recursion**: Multiple nested calls create multiple stack frames
4. **Stack Inspection**: Use `stack` and `frames` commands to view stack state

### Documentation

- **[Calling Convention](docs/CALLING_CONVENTION.md)**: Complete specification of function calling protocol
- **[Memory Layout](docs/MEMORY_LAYOUT.md)**: Detailed memory map and stack frame structure
- **[Recursion Trace](docs/RECURSION_TRACE.md)**: Step-by-step execution trace of factorial(5)

### Example: Understanding Recursion

The factorial program shows how recursion works:

```
factorial(5) calls factorial(4)
  └─> factorial(4) calls factorial(3)
      └─> factorial(3) calls factorial(2)
          └─> factorial(2) calls factorial(1)
              └─> factorial(1) returns 1 (base case)
          └─> factorial(2) returns 2
      └─> factorial(3) returns 6
  └─> factorial(4) returns 24
└─> factorial(5) returns 120
```

Each call creates a stack frame with:
- Return address (where to return after function completes)
- Saved parameter value (n)
- Local variables (if any)

## Architecture

See [docs/CPU_SCHEMATIC.md](docs/CPU_SCHEMATIC.md) for detailed architecture documentation.

### Architecture Diagram

![CPU Architecture Diagram](docs/architecture_diagram.png)

## Design Principles

1. **Modularity**: Each component is self-contained and well-defined
2. **Traceability**: Control flow is visible through trace output
3. **Debuggability**: Easy inspection of registers, memory, and CPU state
4. **Extensibility**: Easy to add new instructions or components

## Videos
1. **Architecture**:
[![Architecture](https://img.shields.io/badge/Architecture%20Diagram-Google%20Drive-green)](https://drive.google.com/file/d/13vMmB9d0Ckdk_mBC7ShI4nOnaPx3Hor0/view?usp=drive_link)
## Assignment: Function Calls and Recursion

This branch implements **function calling and recursion** as a follow-up assignment. The implementation includes:

### Requirements Met

✅ **C Function Reference**: Factorial function with recursion (see `programs/factorial.asm`)  
✅ **Executable Memory Layout**: Documented in [Memory Layout](docs/MEMORY_LAYOUT.md)  
✅ **Function Call Handling**: Demonstrated with stack frames and calling convention  
✅ **Recursion Demonstration**: Multiple stack frames shown in [Recursion Trace](docs/RECURSION_TRACE.md)  

### Key Features

- **Stack-based calling convention**: Return addresses and parameters on stack
- **Recursive factorial**: Computes factorial(5) = 120
- **Stack inspection tools**: `stack` and `frames` commands
- **Complete documentation**: Calling convention, memory layout, and execution traces

### How to Demonstrate

1. **Run the factorial program**:
   ```bash
   ./cpu_emulator programs/factorial.asm run
   ```

2. **View the result**:
   ```bash
   > dec 0x0040 1
   ```

3. **Inspect stack during execution** (run with trace, then check stack):
   ```bash
   > load programs/factorial.asm
   > trace on
   > step  # Step through a few cycles
   > stack 20  # View stack contents
   > frames    # View stack frames
   ```

4. **Review documentation**:
   - [Calling Convention](docs/CALLING_CONVENTION.md) - Function call protocol
   - [Memory Layout](docs/MEMORY_LAYOUT.md) - Executable layout in memory
   - [Recursion Trace](docs/RECURSION_TRACE.md) - Detailed execution trace

## Notes

- Instructions are 16 bits wide
- Memory is little-endian
- Immediate values are 6-bit signed (-32 to 31)
- **Jump offsets are 9-bit signed (-256 to +255)** - extended range for function calls
- Program counter increments by 2 (instruction size)
- Stack pointer initialized to 0xFFFF (top of memory)
- **Stack grows downward** (from 0xFFFF toward lower addresses)
- **Function calls**: Use R5 as stack pointer (SP not directly accessible in assembly)
- **Return addresses**: Passed via R6 register (limitation: no PC access in assembly)

## License

This project is part of a course assignment.

