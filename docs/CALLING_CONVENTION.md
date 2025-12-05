# Calling Convention

This document describes the function calling convention for the CPU emulator, including how functions are called, how parameters are passed, how return values are handled, and how the stack is managed.

## Overview

The calling convention uses a **stack-based approach** where:
- The stack grows downward (from high memory to low memory)
- Stack Pointer (SP) starts at `0xFFFF` (top of memory)
- Each stack frame contains: return address, saved registers, local variables, and parameters

## Register Usage

### General Purpose Registers (R0-R7)

| Register | Purpose | Saved by |
|----------|---------|----------|
| R0 | Return value | Caller |
| R1 | First parameter / temporary | Caller |
| R2 | Second parameter / temporary | Caller |
| R3-R6 | General purpose / temporaries | Caller |
| R7 | Zero register (convention) | N/A |

**Caller-saved registers**: R0-R6 (caller must save if needed)
**Callee-saved registers**: None (all registers are caller-saved)

### Special Purpose Registers

- **PC (Program Counter)**: Points to current instruction
- **SP (Stack Pointer)**: Points to top of stack (grows downward)
  - Initialized to `0xFFFF`
  - Decremented when pushing (stack grows down)
  - Incremented when popping (stack shrinks up)

## Stack Frame Layout

```
High Memory (0xFFFF)
    ↓ (stack grows downward)
    
[Return Address]    ← SP points here after push
[Saved Registers]   ← If callee needs to save registers
[Local Variables]   ← Function's local variables
[Parameters]        ← Additional parameters (if passed on stack)
    ↓
Low Memory (0x0000)
```

### Stack Frame Structure

For a typical function call:

```
Before Call:
    SP = 0xFFFF (or current stack top)
    
During Call Setup:
    1. Save return address: ST return_addr, SP, #-2
    2. Decrement SP: SUB SP, SP, #2
    3. Save caller's registers if needed
    4. Pass parameters in R1, R2, etc.
    5. Jump to function: JMP R0, function_label
    
Inside Function:
    SP points to saved return address
    Can save more registers/local vars by decrementing SP
    
During Return:
    1. Put return value in R0
    2. Restore saved registers
    3. Load return address: LD temp_reg, SP, #0
    4. Increment SP: ADD SP, SP, #2
    5. Return: JMP R0, temp_reg
```

## Function Call Protocol

### Caller Responsibilities

1. **Save return address**:
   ```asm
   ; Calculate return address (PC + 2 for instruction after JMP)
   LDI R0, #0          ; Clear R0
   ADD R0, R7, PC      ; R0 = PC (current instruction)
   LDI R1, #2          ; Load 2
   ADD R0, R0, R1      ; R0 = PC + 2 (return address)
   
   ; Save to stack
   ST R0, SP, #-2      ; Store return address at SP-2
   SUB SP, SP, #2      ; Decrement SP
   ```

2. **Pass parameters**:
   - First parameter in R1
   - Second parameter in R2
   - Additional parameters on stack (if needed)

3. **Call function**:
   ```asm
   JMP R0, function_label  ; Jump to function
   ```

4. **After return**:
   - Get return value from R0
   - Restore any saved registers

### Callee Responsibilities

1. **Function prologue** (save state):
   ```asm
   function:
       ; Return address is already on stack
       ; Save any registers this function will modify
       ; (if needed, save R1, R2, etc. to stack)
   ```

2. **Function body**:
   - Use parameters from R1, R2, etc.
   - Use stack for local variables if needed
   - Perform computation

3. **Function epilogue** (restore state and return):
   ```asm
       ; Put return value in R0
       LDI R0, #result_value
       
       ; Restore saved registers (if any)
       
       ; Load return address
       LD R1, SP, #0      ; Load return address from stack
       ADD SP, SP, #2     ; Increment SP (pop return address)
       
       ; Return
       JMP R0, R1         ; Jump to return address
   ```

## Recursive Function Example

For recursive functions, each call creates a new stack frame:

```
factorial(5):
    Stack Frame 1: [return_to_main, saved_regs, n=5]
    └─> calls factorial(4)
        Stack Frame 2: [return_to_fact5, saved_regs, n=4]
        └─> calls factorial(3)
            Stack Frame 3: [return_to_fact4, saved_regs, n=3]
            └─> calls factorial(2)
                Stack Frame 4: [return_to_fact3, saved_regs, n=2]
                └─> calls factorial(1)
                    Stack Frame 5: [return_to_fact2, saved_regs, n=1]
                    └─> returns 1
                └─> returns 2
            └─> returns 6
        └─> returns 24
    └─> returns 120
```

## Stack Operations

### Push (Save to Stack)

```asm
; Push register R1 to stack
ST R1, SP, #-2      ; Store R1 at SP-2
SUB SP, SP, #2      ; Decrement SP by 2
```

### Pop (Restore from Stack)

```asm
; Pop from stack to register R1
ADD SP, SP, #2      ; Increment SP by 2
LD R1, SP, #0       ; Load from SP (now points to saved value)
```

## Memory Layout

```
0x0000 - 0x003F: Program Code
0x0040 - 0xFEFF: Data Section / Heap
0xFF00 - 0xFFFF: Stack (grows downward from 0xFFFF)
```

The stack has approximately **256 words** (512 bytes) of space from `0xFFFF` down to `0xFE00`, which allows for deep recursion (up to ~128 nested calls if each uses 2 words).

## Example: Simple Function Call

```asm
; Caller code
main:
    ; Setup: pass parameter 5 in R1
    LDI R1, #5
    
    ; Save return address
    LDI R0, #0
    ADD R0, R7, PC      ; R0 = current PC
    LDI R2, #2
    ADD R0, R0, R2      ; R0 = PC + 2 (return address)
    ST R0, SP, #-2     ; Save return address
    SUB SP, SP, #2     ; Decrement SP
    
    ; Call function
    JMP R0, factorial  ; Jump to factorial
    
    ; After return: result in R0
    ; Store result
    ST R0, R2, #0      ; Store result (assuming R2 has data address)
    HLT

; Callee code
factorial:
    ; Prologue: return address already on stack
    ; Parameter n is in R1
    
    ; Check base case: if n <= 1, return 1
    LDI R2, #1
    SUB R3, R1, R2     ; R3 = n - 1
    JZ R7, base_case   ; If n <= 1 (n-1 <= 0), jump to base case
    
    ; Recursive case: n * factorial(n-1)
    ; Save n to stack
    ST R1, SP, #-2     ; Save n
    SUB SP, SP, #2
    
    ; Compute n-1
    SUB R1, R1, R2     ; R1 = n - 1
    
    ; Recursive call (save return address first)
    LDI R0, #0
    ADD R0, R7, PC
    LDI R2, #2
    ADD R0, R0, R2      ; Return address
    ST R0, SP, #-2
    SUB SP, SP, #2
    JMP R0, factorial   ; Recursive call
    
    ; After recursive call: result in R0, restore n
    ADD SP, SP, #2      ; Pop return address
    ADD SP, SP, #2      ; Pop saved n
    LD R1, SP, #0       ; Restore n
    
    ; Multiply n * result
    ; (multiplication using repeated addition)
    ; ... multiplication code ...
    
    ; Return
    LD R1, SP, #0       ; Load return address
    ADD SP, SP, #2
    JMP R0, R1
    
base_case:
    ; Return 1
    LDI R0, #1
    LD R1, SP, #0       ; Load return address
    ADD SP, SP, #2
    JMP R0, R1
```

## Notes

- **Stack overflow**: If SP goes below `0xFE00`, stack overflow may occur
- **Register preservation**: Since all registers are caller-saved, the caller must save any registers it needs before making a call
- **Return value**: Always returned in R0
- **Parameters**: First 2 parameters in R1, R2; additional parameters on stack
- **Stack alignment**: Stack is word-aligned (2-byte boundaries)

