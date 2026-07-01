; Prompt and read 4 values from the keyboard into registers R0-R3
INPUT R0
INPUT R1
INPUT R2
INPUT R3

; Accumulate the sum into R0
ADD R0, R1
ADD R0, R2
ADD R0, R3

; Set up a temporary register with the divisor (4)
MOV R4, 4

; Divide the total sum (R0) by the divisor (R4)
DIV R0, R4

; Display the calculated average
DISPLAY R0

; Store the average into memory address 30
STORE R0, 30
