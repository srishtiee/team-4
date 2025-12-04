# Recursion Trace Documentation

This document demonstrates how recursion works in the factorial program, showing the execution trace, stack frames, and how recursive calls are handled.

## Factorial Function Overview

**C Reference**:
```c
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

**Assembly Implementation**: See `programs/factorial.asm`

## Execution Trace: factorial(5)

### Call Sequence

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
main() receives 120
```

## Stack Frame Evolution

### Initial State (Before Call)

```
SP = 0xFFFF
Stack: [empty]
```

### Step 1: main() calls factorial(5)

**Stack after call setup**:
```
0xFFFF: [Return Address to main: 0x002A]
SP = 0xFFFE
```

**Registers**:
- R1 = 5 (parameter n)
- R0 = (will contain return value)

### Step 2: factorial(5) calls factorial(4)

**Stack after recursive call setup**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
SP = 0xFFFA
```

**Registers**:
- R1 = 4 (new parameter for recursive call)
- R0 = (will contain factorial(4))

### Step 3: factorial(4) calls factorial(3)

**Stack**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
0xFFFA: [Saved n = 4]
0xFFF8: [Return Address to factorial(4): 0x0042]
SP = 0xFFF6
```

**Registers**:
- R1 = 3 (new parameter)
- R0 = (will contain factorial(3))

### Step 4: factorial(3) calls factorial(2)

**Stack**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
0xFFFA: [Saved n = 4]
0xFFF8: [Return Address to factorial(4): 0x0042]
0xFFF6: [Saved n = 3]
0xFFF4: [Return Address to factorial(3): 0x0042]
SP = 0xFFF2
```

**Registers**:
- R1 = 2 (new parameter)
- R0 = (will contain factorial(2))

### Step 5: factorial(2) calls factorial(1)

**Stack**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
0xFFFA: [Saved n = 4]
0xFFF8: [Return Address to factorial(4): 0x0042]
0xFFF6: [Saved n = 3]
0xFFF4: [Return Address to factorial(3): 0x0042]
0xFFF2: [Saved n = 2]
0xFFF0: [Return Address to factorial(2): 0x0042]
SP = 0xFFEE
```

**Registers**:
- R1 = 1 (new parameter)
- R0 = (will contain factorial(1))

### Step 6: factorial(1) - Base Case

**Stack** (unchanged):
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
0xFFFA: [Saved n = 4]
0xFFF8: [Return Address to factorial(4): 0x0042]
0xFFF6: [Saved n = 3]
0xFFF4: [Return Address to factorial(3): 0x0042]
0xFFF2: [Saved n = 2]
0xFFF0: [Return Address to factorial(2): 0x0042]
SP = 0xFFEE
```

**Execution**:
- Checks `n <= 1` → true
- Sets R0 = 1 (return value)
- Pops return address from stack
- Returns to factorial(2)

### Step 7: factorial(2) Returns

**After popping return address and saved n**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
0xFFFA: [Saved n = 4]
0xFFF8: [Return Address to factorial(4): 0x0042]
0xFFF6: [Saved n = 3]
0xFFF4: [Return Address to factorial(3): 0x0042]
SP = 0xFFF6
```

**Execution**:
- Restores n = 2 from stack
- Computes: 2 * factorial(1) = 2 * 1 = 2
- Sets R0 = 2
- Returns to factorial(3)

### Step 8: factorial(3) Returns

**Stack**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
0xFFFA: [Saved n = 4]
0xFFF8: [Return Address to factorial(4): 0x0042]
SP = 0xFFFA
```

**Execution**:
- Restores n = 3
- Computes: 3 * factorial(2) = 3 * 2 = 6
- Sets R0 = 6
- Returns to factorial(4)

### Step 9: factorial(4) Returns

**Stack**:
```
0xFFFF: [Return Address to main: 0x002A]
0xFFFE: [Saved n = 5]
0xFFFC: [Return Address to factorial(5): 0x0042]
SP = 0xFFFE
```

**Execution**:
- Restores n = 4
- Computes: 4 * factorial(3) = 4 * 6 = 24
- Sets R0 = 24
- Returns to factorial(5)

### Step 10: factorial(5) Returns

**Stack**:
```
0xFFFF: [Return Address to main: 0x002A]
SP = 0xFFFF
```

**Execution**:
- Restores n = 5
- Computes: 5 * factorial(4) = 5 * 24 = 120
- Sets R0 = 120
- Returns to main

### Final State

**Stack**: Empty (SP = 0xFFFF)
**Registers**: R0 = 120 (result)
**Memory**: 0x0040 = 120 (stored result)

## Detailed Cycle Trace (Sample)

### Cycle 1: Entering factorial(5)

```
PC: 0x0020 (factorial function entry)
Instruction: LDI R2, #1
Registers: R1 = 5 (parameter)
Stack: [Return Address: 0x002A] at 0xFFFF
SP: 0xFFFE
```

### Cycle 2: Checking Base Case

```
PC: 0x0022
Instruction: SUB R3, R1, R2  (R3 = 5 - 1 = 4)
Registers: R1 = 5, R2 = 1, R3 = 4
Flags: Z = 0 (not zero, continue)
Stack: [Return Address: 0x002A] at 0xFFFF
SP: 0xFFFE
```

### Cycle 3: Saving n to Stack

```
PC: 0x0024
Instruction: ST R1, SP, #-2  (Save n=5 to stack)
Registers: R1 = 5
Stack: 
  - [Return Address: 0x002A] at 0xFFFF
  - [Saved n = 5] at 0xFFFE
SP: 0xFFFE (before), 0xFFFC (after SUB SP)
```

### Cycle 4: Preparing Recursive Call

```
PC: 0x0026
Instruction: SUB R1, R1, R2  (R1 = 5 - 1 = 4)
Registers: R1 = 4 (new parameter)
Stack: [Return Address: 0x002A, Saved n = 5]
SP: 0xFFFC
```

### Cycle 5: Saving Return Address for Recursive Call

```
PC: 0x0028
Instruction: ST R0, SP, #-2  (Save return address)
Registers: R0 = 0x0042 (calculated return address)
Stack:
  - [Return Address: 0x002A] at 0xFFFF
  - [Saved n = 5] at 0xFFFE
  - [Return Address: 0x0042] at 0xFFFC
SP: 0xFFFC (before), 0xFFFA (after)
```

### Cycle 6: Recursive Call

```
PC: 0x002A
Instruction: JMP R0, factorial
Registers: R0 = 0x0020 (factorial function address)
PC: 0x0020 (jump to factorial)
Stack: [Return Address: 0x002A, Saved n = 5, Return Address: 0x0042]
SP: 0xFFFA
```

## Visual Stack Frame Diagram

```
┌─────────────────────────────────────────┐
│ 0xFFFF: Return to main (0x002A)         │ ← Frame 1: factorial(5)
├─────────────────────────────────────────┤
│ 0xFFFE: Saved n = 5                     │
├─────────────────────────────────────────┤
│ 0xFFFC: Return to fact(5) (0x0042)     │ ← Frame 2: factorial(4)
├─────────────────────────────────────────┤
│ 0xFFFA: Saved n = 4                     │
├─────────────────────────────────────────┤
│ 0xFFF8: Return to fact(4) (0x0042)     │ ← Frame 3: factorial(3)
├─────────────────────────────────────────┤
│ 0xFFF6: Saved n = 3                     │
├─────────────────────────────────────────┤
│ 0xFFF4: Return to fact(3) (0x0042)      │ ← Frame 4: factorial(2)
├─────────────────────────────────────────┤
│ 0xFFF2: Saved n = 2                     │
├─────────────────────────────────────────┤
│ 0xFFF0: Return to fact(2) (0x0042)     │ ← Frame 5: factorial(1)
├─────────────────────────────────────────┤
│ 0xFFEE: (base case, returns immediately)│
└─────────────────────────────────────────┘
```

## Key Observations

1. **Stack Growth**: Each recursive call adds 2 words to the stack (return address + saved n)
2. **Frame Isolation**: Each frame is independent, allowing proper return
3. **Parameter Passing**: Parameter n is passed in R1, saved to stack before recursive call
4. **Return Value**: Return value is always in R0
5. **Stack Cleanup**: Each return pops its frame, restoring the previous state

## Testing the Trace

To see the recursion in action:

```bash
# Load and run factorial program
./cpu_emulator programs/factorial.asm run

# Check stack frames
> frames

# Check stack contents
> stack 20

# Check result
> dec 0x0040 1
```

## Recursion Depth Limits

- **Maximum Stack Size**: ~256 words (0xFF00 to 0xFFFF)
- **Words per Frame**: 2 (return address + saved n)
- **Maximum Recursion Depth**: ~128 nested calls
- **Factorial(5) Uses**: 5 frames = 10 words (well within limits)

