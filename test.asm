; =======================================================================
; CCP6124 OOPDS - Virtual Machine Test Suite Program
; Purpose: Systematically tests all instruction classes and CPU flags
; =======================================================================

; -----------------------------------------------------------------------
; PART 1: Testing Basic IO, MOV, and Direct Memory Boundaries
; -----------------------------------------------------------------------
INPUT R0             ; Wait for keyboard input. Enter '5' to match sample logic
MOV R1, 5            ; Set R1 to 5 (Mode 1: Value to Register)
MOV R2, 6	     ; Set R2 to 5
ADD R1, R2           ; R1 = R1 + 6 = 11. Triggers math logic
MOV R3, R1           ; R3 = 11 (Mode 2: Register to Register)
MOV R4, 4	     ; Set R4 to 4
MUL R3, R4           ; R3 = R3 * 4 = 44
STORE R3, 20         ; Memory location [20] now holds 44
LOAD R4, [20]        ; R4 loads 44 back from memory (Direct Addressing)
DISPLAY R4           ; Outputs '44' to the terminal window

; -----------------------------------------------------------------------
; PART 2: Testing Register-Indirect Addressing (Square Brackets)
; -----------------------------------------------------------------------
MOV R5, 20           ; R5 holds the memory address 20
LOAD R6, [R5]        ; R6 reads address pointed to by R5 ([20]). R6 becomes 44
MOV R2, 30           ; R2 holds target destination address 30[cite: 111].
STORE R6, [R2]       ; Memory location [30] gets the value 44 (Indirect Store)

; -----------------------------------------------------------------------
; PART 3: Testing Shared Flag Sensor System (Overwriting behavior)
; -----------------------------------------------------------------------
MOV R0, 120          ; Prepare values close to signed limits
MOV R1, 10           ; Prepare addend
ADD R0, R1           ; 120 + 10 = 130. Triggers Overflow (OF=1)
RESET OF             ; Manually clear the Overflow Flag back to 0[cite: 231, 233].
MOV R2, 0            ; Move 0 into R2
DEC R2               ; Decrement R2. R2 becomes -1. ZF resets to 0

; -----------------------------------------------------------------------
; PART 4: Testing Bitwise Operations (Rotation)
; -----------------------------------------------------------------------
MOV R7, 1            ; Binary: 00000001
ROL R7, 1            ; Rotate left by 1. R7 becomes 2 (Binary: 00000010)
ROR R7, 2            ; Rotate right by 2. Wraps to 128 (Binary: 10000000)

; -----------------------------------------------------------------------
; PART 5: Testing System Stack Operations (LIFO Verification)
; -----------------------------------------------------------------------
MOV R0, 88           ; Set temporary storage values
MOV R1, 99           ; Set temporary storage values
PUSH R0              ; Stack index (SI) = 1, Stack top = 88
PUSH R1              ; Stack index (SI) = 2, Stack top = 99
POP R2               ; R2 retrieves the last item pushed (99). SI = 1
POP R3               ; R3 retrieves the next item (88). SI = 0

DISPLAY R2           ; Prints '99'
DISPLAY R3           ; Prints '88'
