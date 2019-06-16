# simple-lc3-emu
A simple LC-3 emulator written in C

Emulates a basic LC-3 machine with memory and registers. It can correctly process LD, LEA, LDI, AND, NOT, ADD and BR instructions. 
As input, it takes compiled LC-3 machine code through a command-line argument.

The example file input.obj was generated using the following machine code:
```
.ORIG xAFFF
      LD R7, BEE
      LDI R3, COW
      BRnzp BIRD
DOG   ADD R3, R3, R4
      BRnzp CAT
BIRD  LEA R3, #112
      BRn DOG
CAT   LD R2, BEE
      LD R6, HORSE
      ADD R0, R2, R6
      ADD R0, R0, #-9
      BRnz DOG
      NOT R0, R0
      NOT R0, R6
      HALT
BEE   .FILL xFFFF
FISH  .FILL x0000
HORSE .FILL x0FE7
BEAR  .FILL x0001
COW   .FILL xB003
      .END
```

The program outputs its inital state, and then its state after the execution of each LC-3 instruction:

```
Initial state
R0      0x0000
R1      0x0000
R2      0x0000
R3      0x0000
R4      0x0000
R5      0x0000
R6      0x0000
R7      0x0000
PC      0xafff
IR      0x0000
CC      Z
==================
after executing instruction     0x2e0e
R0      0x0000
R1      0x0000
R2      0x0000
R3      0x0000
R4      0x0000
R5      0x0000
R6      0x0000
R7      0xffff
PC      0xb000
IR      0x2e0e
CC      N
==================
after executing instruction     0xa611
R0      0x0000
R1      0x0000
R2      0x0000
R3      0x0e02
R4      0x0000
R5      0x0000
R6      0x0000
R7      0xffff
PC      0xb001
IR      0xa611
CC      P
```
and so on for the remaining instructions.
