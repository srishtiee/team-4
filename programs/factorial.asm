; Factorial Program with Recursion
; Computes factorial(5) = 120
;
; Memory Layout:
;   0x0000-0x003F: Program code
;   0x0040: Result storage
;   0xFF00-0xFFFF: Stack (grows downward from 0xFFFF)
;
; Calling Convention:
;   - Parameters: R1 (first parameter)
;   - Return value: R0
;   - Return address: R6 (caller sets R6 before call)
;   - Stack pointer: R5 (holds current stack pointer, initialized to 0xFFFF)
;
; Note: In a real system, you'd have CALL/RET instructions or PC access.
; For this demo, we use R6 to pass return addresses and R5 for stack pointer.

main:
    ; Initialize stack pointer in R5
    ; Build 0xFFFF: -1 in 16-bit signed = 0xFFFF
    LDI R5, #-1        ; R5 = -1 = 0xFFFF (stack top)
    
    ; Build data address 0x0040 for storing result
    LDI R2, #31        ; Start building 0x0040
    LDI R3, #31
    ADD R2, R2, R3     ; R2 = 62
    LDI R3, #2
    ADD R2, R2, R3     ; R2 = 64 = 0x0040
    
    ; Pass parameter: n = 5
    LDI R1, #5
    
    ; Save result address (0x0040 in R2) to stack before calling factorial
    ; R2 will be overwritten during factorial execution
    ST R2, R5, #-2     ; Save result address at R5-2
    LDI R0, #-2
    ADD R5, R5, R0     ; Decrement R5 by 2
    
    ; Set return address in R6 (address of main_return)
    ; main_return is at 0x26 = 38 decimal (from assembler output)
    ; Build 38: 31 + 7 = 38 (can't use LDI #38 directly, it's out of 6-bit range)
    LDI R6, #0
    LDI R0, #31        ; Start building 38
    LDI R3, #7
    ADD R0, R0, R3     ; R0 = 38
    ADD R6, R7, R0     ; R6 = return address
    
    ; Save return address to stack
    ST R6, R5, #-2     ; Save return address at R5-2
    LDI R0, #-2
    ADD R5, R5, R0     ; Decrement R5 by 2
    
    ; Call factorial (use R7=0 for relative jump)
    JMP R7, factorial
    
main_return:
    ; After return: result is in R0
    ; Restore result address from stack
    LD R2, R5, #0      ; Restore result address (0x0040)
    LDI R3, #2
    ADD R5, R5, R3     ; Pop result address (increment R5 by 2)
    ; Store result at address 0x0040
    ST R0, R2, #0      ; Store result
    
    ; Halt
    HLT

; Factorial function: factorial(n) where n is in R1
; Returns: factorial(n) in R0
; Calling convention: Return address in R6 (saved on stack by caller)
factorial:
    ; Base case: if n <= 1, return 1
    LDI R2, #1         ; Load 1
    SUB R3, R1, R2     ; R3 = n - 1
    JZ R7, base_case   ; If n == 1 (n-1 == 0), jump to base case
    
    ; Check if n == 0
    LDI R3, #0
    SUB R3, R1, R3     ; R3 = n - 0 = n
    JZ R7, base_case   ; If n == 0, return 1
    
    ; Recursive case: return n * factorial(n-1)
    ; Save n to stack (we'll need it after recursive call)
    ST R1, R5, #-2     ; Save n to stack at R5-2
    LDI R0, #-2
    ADD R5, R5, R0     ; Decrement R5 by 2
    
    ; Compute n-1
    LDI R2, #1
    SUB R1, R1, R2     ; R1 = n - 1 (new parameter)
    
    ; Set return address for recursive call (fact_return)
    ; fact_return is at 0x5c = 92 decimal (from assembler output)
    ; Build 92: 31 + 31 + 30 = 92
    LDI R6, #0
    LDI R0, #31
    LDI R3, #31
    ADD R0, R0, R3     ; R0 = 62
    LDI R3, #30        ; 62 + 30 = 92 = 0x5C
    ADD R0, R0, R3     ; R0 = 92
    ADD R6, R7, R0     ; R6 = 92 (0x5C)
    ST R6, R5, #-2     ; Save return address
    LDI R0, #-2
    ADD R5, R5, R0     ; Decrement R5 by 2
    
    ; Recursive call (use R7=0 for relative jump)
    JMP R7, factorial
    
fact_return:
    ; After recursive call returns: result is in R0 (factorial(n-1))
    ; Note: Return address was already popped when we returned from recursive call
    ; So R5 now points to where n is stored
    ; Restore n from stack
    LD R1, R5, #0      ; Restore n to R1
    LDI R2, #2
    ADD R5, R5, R2     ; Pop saved n (increment R5 by 2)
    
    ; Now compute n * result (result is in R0, n is in R1)
    ; Multiplication using repeated addition: result = result * n
    ; Algorithm: product = 0; for i = 0 to n-1: product += result
    ; Save the recursive result to a safe location first
    ; Use R4 to temporarily hold the recursive result
    LDI R4, #0
    ADD R4, R7, R0     ; R4 = recursive result (factorial(n-1))
    
    ; Initialize: product = 0, counter = 0
    LDI R0, #0         ; R0 = 0 (will accumulate product)
    LDI R2, #0         ; R2 = counter (i)
    LDI R3, #1         ; R3 = 1 (increment)
    
mult_loop:
    ; Check if counter >= n (i >= n)
    ; Use R3 for comparison (save R6 for return address, R2 for counter)
    SUB R3, R1, R2     ; R3 = n - i
    JZ R7, mult_done   ; If n - i == 0, done (i == n)
    
    ; Add recursive result to accumulator
    ADD R0, R0, R4     ; R0 += factorial(n-1) (from R4)
    
    ; Increment counter
    LDI R3, #1         ; Reload increment (R3 was used for comparison)
    ADD R2, R2, R3     ; i++
    
    ; Loop back (use R7=0 for relative jump)
    JMP R7, mult_loop
    
mult_done:
    ; R0 now contains n * factorial(n-1)
    ; Return to caller
    ; Load return address from stack
    LD R6, R5, #0      ; Load return address
    LDI R2, #2         ; Reload 2 for stack pop (R2 was counter)
    ADD R5, R5, R2     ; Pop return address (increment R5 by 2)
    JMP R6, #0         ; Return (jump to address in R6)
    
base_case:
    ; Return 1
    LDI R0, #1         ; Return value = 1
    
    ; Load return address and return
    LD R6, R5, #0      ; Load return address
    LDI R2, #2
    ADD R5, R5, R2     ; Pop return address (increment R5 by 2)
    JMP R6, #0         ; Return (jump to address in R6)
