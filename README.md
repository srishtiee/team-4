# CPU Emulator: Function Calls and Recursion

A software CPU emulator demonstrating **function calling and recursion** through a recursive factorial program. This project shows how executables are laid out in memory, how function calls are handled, and how recursion is carried out using stack frames.

## Video Link
[Program Layout and Execution Walkthrough](https://drive.google.com/file/d/1vOFgQTE7auNNG0jvVXWGBovKbeqhjMGr/view?usp=drive_link)

## Overview

This project implements a complete CPU emulator with support for:
- **Stack-based function calls**: Parameters, return addresses, and local variables on the stack
- **Recursion**: Multiple nested function calls with proper stack frame management
- **Memory layout visualization**: Code, data, and stack regions
- **Execution tracing**: Step-by-step instruction execution with stack frame inspection

The factorial program computes `factorial(5) = 120` using a recursive function, demonstrating the complete function calling mechanism.

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/srishtiee/team-4.git
cd team-4
```

### 2. Switch to the Correct Branch

```bash
git checkout program-layout-and-execution
```

### 3. Build the Emulator

```bash
make
```

This will create the `cpu_emulator` executable.

## Running the Factorial Program

### Quick Start: Load and Run

```bash
./cpu_emulator programs/factorial.asm run
```

This loads the factorial program, runs it to completion, and halts. The result (120) is stored at memory address `0x0040`.

### Interactive Mode: Step-by-Step Execution

For detailed inspection and tracing:

```bash
./cpu_emulator
```

You'll enter the interactive emulator. Available commands:

#### Basic Commands
- `load <file>` - Load and assemble program from file
- `run` - Run program until halt
- `step` - Execute one instruction
- `reset` - Reset CPU to initial state
- `quit/exit` - Exit emulator
- `help` - Show all available commands

#### Inspection Commands
- `gpr` - Print General Purpose Registers (R0-R7)
- `spr` - Print Special Purpose Registers (PC, SP, FLAGS)
- `state` - Print complete CPU state
- `ram [addr] [len]` - Print RAM dump (default: 0x0000, 256 bytes)
- `dec [addr] [cnt]` - Print memory as decimal numbers (default: 0x0040, 10 words)
- `stack [cnt]` - Print stack contents (default: 20 words)
- `frames` - Print stack frames (for function calls)

#### Tracing Commands
- `trace on` - Enable instruction tracing (shows fetch-decode-execute cycle)
- `trace off` - Disable instruction tracing

## Complete Workflow: Load, Trace, and Run

### Step 1: Load the Factorial Program

```bash
$ ./cpu_emulator
> load programs/factorial.asm
```

**Expected output:**
```
Program loaded: 74 instructions
Labels:
  main: 0x0
  factorial: 0x28
  fact_return: 0x5c
  base_case: 0x9e
  mult_loop: 0x6c
  mult_done: 0x78
```

This shows:
- The program has been assembled into 74 instructions
- Label addresses: `main` starts at 0x0, `factorial` function at 0x28, etc.

### Step 2: Enable Tracing
```bash
> trace on
```

Tracing shows detailed execution information for each instruction:
- **FETCH**: Instruction fetched from memory
- **DECODE**: Instruction decoded (opcode, registers, immediate values)
- **EXECUTE**: Operation performed and result
- **STORE**: Register/memory updates and PC changes

### Step 3: Run the Program

**Option A: Run to completion**
```bash
> run
```

**Option B: Step through execution (for detailed inspection)**
```bash
> step    # Execute one instruction
> step    # Execute another instruction
# ... continue stepping to see execution flow
```

### Step 4: View the Result

After the program halts, check the result:

```bash
> dec 0x0040 1
```

**Expected output:**
```
=== Memory as Decimal Numbers ===
Starting address: 0x0040
[00] 0x0040: 120
```

This confirms that `factorial(5) = 120` was computed and stored.


## Understanding the Execution

### Memory Layout

The executable is laid out in memory as follows:

```
0x0000 - 0x003F:  Program Code (64 bytes)
0x0040 - 0xFEFF:  Data Section
0xFF00 - 0xFFFF:  Stack (grows downward from 0xFFFF)
```

- **Code Section**: Machine instructions starting at 0x0000
- **Data Section**: Variables and results (factorial result stored at 0x0040)
- **Stack**: Function call frames, grows downward from 0xFFFF

### Function Call Sequence

The factorial program demonstrates the following call sequence:

```
main() calls factorial(5)
  └─> factorial(5) calls factorial(4)
      └─> factorial(4) calls factorial(3)
          └─> factorial(3) calls factorial(2)
              └─> factorial(2) calls factorial(1)
                  └─> factorial(1) returns 1 (base case)
              └─> factorial(2) returns 2 * 1 = 2
          └─> factorial(3) returns 3 * 2 = 6
      └─> factorial(4) returns 4 * 6 = 24
  └─> factorial(5) returns 5 * 24 = 120
main() stores result at 0x0040 and halts
```

### Stack Frame Structure

Each recursive call creates a stack frame containing:

1. **Return Address**: Where to return after function completes
2. **Saved Parameter (n)**: The value of n for this call
3. **Local Variables**: Any temporary values needed

Stack frames grow downward:
```
0xFFFF: [Initial stack top]
0xFFFE: [Return address from main]
0xFFFC: [Saved n = 5]
0xFFFA: [Return address from factorial(5)]
0xFFF8: [Saved n = 4]
...
```

## C Reference Function

The assembly program implements the following C function:

```c
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    int result = factorial(5);
    // Store result at address 0x0040
    return 0;
}
```

The assembly implementation (`programs/factorial.asm`) demonstrates how this high-level recursive function is translated to low-level assembly code with:
- Stack-based parameter passing
- Return address management
- Recursive call handling
- Stack frame creation and cleanup

## Documentation

For detailed information, see:

- **[Calling Convention](docs/CALLING_CONVENTION.md)**: Complete specification of how function calls work
  - Parameter passing (R1, R2)
  - Return values (R0)
  - Stack pointer management (R5)
  - Return address handling (R6)

- **[Memory Layout](docs/MEMORY_LAYOUT.md)**: Detailed memory organization
  - Code section layout
  - Data section usage
  - Stack frame structure
  - Memory-mapped I/O

- **[Recursion Trace](docs/RECURSION_TRACE.md)**: Step-by-step execution trace
  - Cycle-by-cycle execution
  - Stack frame evolution
  - Register state changes
  - Complete trace of factorial(5)

- **[ISA Documentation](docs/ISA.md)**: Instruction Set Architecture
  - Available instructions
  - Instruction formats
  - Addressing modes
  - Extended jump offset range (-256 to +255)

## Example Session

Complete example session:

```bash
$ ./cpu_emulator
> load programs/factorial.asm
Program loaded: 74 instructions
Labels:
  main: 0x0
  factorial: 0x28
  fact_return: 0x5c
  base_case: 0x9e
  mult_loop: 0x6c
  mult_done: 0x78

> trace on
> run
[Detailed trace output showing fetch-decode-execute cycles...]
=== CPU State ===
Cycle: 260
=== General Purpose Registers ===
R0: 0x0078 (120)
R1: 0x0005 (5)
R2: 0x0040 (64)
R3: 0x0002 (2)
R4: 0x0018 (24)
R5: 0xffff (-1)
R6: 0x0026 (38)
R7: 0x0000 (0)
=== Special Purpose Registers ===
PC:  0x002e
SP:  0xffff
FLAGS: Z=0 N=1 C=0 V=0

> dec 0x0040 1
=== Memory as Decimal Numbers ===
Starting address: 0x0040
[00] 0x0040: 120

> stack 20
=== Stack Contents ===
Stack Pointer (SP): 0xFFFF
Stack grows downward (from 0xFFFF)
[Stack contents after program completion]

> frames
=== Stack Frames ===
Stack Pointer (SP): 0xFFFF
(No stack frames detected - program completed)

> quit
```

## Key Features Demonstrated

**Executable Memory Layout**: Code, data, and stack regions clearly separated  
**Function Call Mechanism**: Stack-based calling convention with return addresses  
**Recursion**: Multiple nested calls with proper stack frame management  
**Stack Inspection**: Tools to view stack contents and frames during execution  
**Execution Tracing**: Detailed cycle-by-cycle execution information  

## Architecture Notes

- **16-bit instruction set** with custom ISA
- **Stack grows downward** from 0xFFFF
- **Jump offsets**: 9-bit signed (-256 to +255 bytes) for function calls
- **Calling convention**: Parameters in R1/R2, return value in R0, stack pointer in R5
- **No direct PC/SP access** in assembly (uses R5 for SP, R6 for return addresses)

## Troubleshooting

**Program doesn't halt:**
- Check if you're on the correct branch: `git branch`
- Rebuild: `make clean && make`
- Check trace output for infinite loops

**Wrong result:**
- Verify you're checking address 0x0040: `dec 0x0040 1`
- Check register state: `gpr` and `spr`
- Review trace output for incorrect operations

**Stack inspection shows nothing:**
- Stack frames are only visible **during execution**
- After program completes, stack is cleaned up
- Use `step` to pause execution and inspect stack mid-execution

## Requirements Met

This implementation demonstrates:

1. **C Function with Recursion**: Factorial function reference (see C code above)
2. **Executable Memory Layout**: Documented code/data/stack regions
3. **Function Call Handling**: Stack frames, return addresses, parameter passing
4. **Recursion Demonstration**: Multiple nested calls with stack frame visualization
