# Memory Layout

This document describes the memory layout of the CPU emulator, including where program code, data, and the stack are located in the 64KB address space.

## Memory Map Overview

```
0x0000 ┌─────────────────────────┐
       │                         │
       │   Program Code           │
       │   (Instructions)         │
       │                         │
0x0040 ├─────────────────────────┤
       │                         │
       │   Data Section          │
       │   (Variables, Results)  │
       │                         │
       │                         │
       │   ...                   │
       │                         │
0xFE00 ├─────────────────────────┤
       │                         │
       │   Stack                 │
       │   (Grows downward)      │
       │                         │
0xFFFF └─────────────────────────┘
```

## Detailed Memory Regions

### 1. Program Code Section (0x0000 - 0x003F)

**Purpose**: Stores executable instructions

**Size**: 64 bytes (32 instructions, 2 bytes each)

**Usage**:
- Program instructions are loaded starting at address `0x0000`
- Each instruction is 16 bits (2 bytes)
- Instructions are stored in little-endian format
- Labels resolve to addresses in this region

**Example**:
```
0x0000: 0xB000  (LDI R0, #0)
0x0002: 0xB105  (LDI R1, #5)
0x0004: 0xC000  (JMP R0, factorial)
...
```

### 2. Data Section (0x0040 - 0xFEFF)

**Purpose**: Stores program data, variables, and results

**Size**: ~65,200 bytes

**Usage**:
- Global variables
- Arrays
- Function results
- Temporary data storage

**Example for Factorial Program**:
```
0x0040: 0x0078  (120 decimal - factorial(5) result)
```

### 3. Stack Section (0xFF00 - 0xFFFF)

**Purpose**: Runtime stack for function calls and local variables

**Size**: 256 bytes (128 words)

**Characteristics**:
- **Stack Pointer (SP)**: Initialized to `0xFFFF` (top of memory)
- **Growth Direction**: Grows **downward** (from high to low addresses)
- **Alignment**: Word-aligned (2-byte boundaries)

**Stack Layout**:
```
0xFFFF ┌─────────────────┐ ← Initial SP (top of stack)
       │                 │
       │   Stack Frame 1 │
       │   - Return Addr │
       │   - Saved Regs  │
       │   - Local Vars  │
       ├─────────────────┤
       │   Stack Frame 2 │
       │   - Return Addr │
       │   - Saved Regs  │
       │   - Local Vars  │
       ├─────────────────┤
       │   ...           │
       ├─────────────────┤
       │   Stack Frame N │
       │   - Return Addr │
       │   - Saved Regs  │
       │   - Local Vars  │
       ├─────────────────┤
       │                 │
       │   (unused)      │
       │                 │
0xFF00 └─────────────────┘ ← Bottom of stack region
```

## Stack Frame Structure

Each function call creates a stack frame with the following structure (from high to low addresses):

```
High Address (closer to 0xFFFF)
    ↓
[Return Address]      ← SP points here when function is called
[Saved Registers]     ← If function needs to save caller's registers
[Local Variables]     ← Function's local variables
[Parameters]          ← Additional parameters (if passed on stack)
    ↓
Low Address (further from 0xFFFF)
```

### Example: Factorial Function Stack Frames

For `factorial(5)` calling `factorial(4)` calling `factorial(3)`:

```
0xFFFF ┌─────────────────────────┐
       │ Return to main (0x002A) │ ← Frame 1: factorial(5)
0xFFFE ├─────────────────────────┤
       │ Saved n = 5             │
0xFFFC ├─────────────────────────┤
       │ Return to fact(5) (0x??) │ ← Frame 2: factorial(4)
0xFFFA ├─────────────────────────┤
       │ Saved n = 4             │
0xFFF8 ├─────────────────────────┤
       │ Return to fact(4) (0x??) │ ← Frame 3: factorial(3)
0xFFF6 ├─────────────────────────┤
       │ Saved n = 3             │
       │ ...                     │
       └─────────────────────────┘
```

## Memory-Mapped I/O (0xFF00 - 0xFFFF)

The stack region overlaps with I/O addresses. I/O takes precedence:

| Address | Purpose | Description |
|---------|---------|------------|
| 0xFF00 | STDOUT | Character output (writing a byte outputs the character) |
| 0xFF01 | STDIN | Character input (reading returns input byte) |
| 0xFF02 | STATUS | I/O status register |
| 0xFF03-0xFFFF | Reserved | Reserved for future I/O |

**Note**: The stack should not grow below `0xFE00` to avoid conflicts with I/O.

## Memory Access Patterns

### Reading Instructions
- **Fetch**: `PC` points to instruction address
- **Format**: 16-bit little-endian
- **Range**: 0x0000 - 0x003F (for typical programs)

### Reading/Writing Data
- **Load**: `LD RD, RS1, IMM` → `RD = MEM[RS1 + IMM]`
- **Store**: `ST RD, RS1, IMM` → `MEM[RS1 + IMM] = RD`
- **Range**: 0x0040 - 0xFEFF (data section)

### Stack Operations
- **Push**: `ST R, SP, #-2` then `SUB SP, SP, #2`
- **Pop**: `ADD SP, SP, #2` then `LD R, SP, #0`
- **Range**: 0xFF00 - 0xFFFF (stack grows downward)

## Example: Factorial Program Memory Layout

### At Program Start
```
0x0000: [Program code - main function]
0x0020: [Program code - factorial function]
0x0040: [Data - will store result]
0xFF00: [Stack - empty, SP = 0xFFFF]
```

### During Execution (factorial(5) → factorial(4) → factorial(3))
```
0x0000: [Program code]
0x0040: [Data - result not yet stored]
0xFF00: [Stack]
0xFFFE: Return address (to main)
0xFFFC: Saved n = 5
0xFFFA: Return address (to factorial(5))
0xFFF8: Saved n = 4
0xFFF6: Return address (to factorial(4))
0xFFF4: Saved n = 3
        ...
        SP = 0xFFF4 (current stack pointer)
```

### After Execution
```
0x0000: [Program code]
0x0040: 0x0078 (120 decimal - factorial(5) result)
0xFF00: [Stack - cleaned up, SP = 0xFFFF]
```

## Memory Safety Considerations

1. **Stack Overflow**: If SP goes below `0xFE00`, stack may overflow into data section
2. **Code Overwrite**: Storing data at addresses < 0x0040 may overwrite program code
3. **I/O Conflicts**: Stack should not grow into I/O region (0xFF00-0xFF03)
4. **Address Alignment**: All memory accesses should be word-aligned (even addresses)

## Best Practices

1. **Code**: Keep program code in 0x0000-0x003F
2. **Data**: Use 0x0040+ for data storage
3. **Stack**: Ensure stack doesn't grow below 0xFE00
4. **I/O**: Avoid using 0xFF00-0xFF03 for stack operations

