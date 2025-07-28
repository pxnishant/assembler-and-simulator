## Overview
This repository contains the implementation of a custom assembler and simulator for an ISA as part of the CSE 112 Computer Organization course at IIIT Delhi.<br>

I initially wrote the assembler in C, and the simulator in Python, but decided to port both to CPP because I wanted to relive this project again.

## Assembler
The assembler is responsible for translating assembly instructions into machine code that can be executed by the simulator. The assembly instructions follow a specific format and set of rules that were given to us as a part of our course.

## Simulator
The simulator simulates the execution of the machine code, as if it were a CPU. It also has its own memory. It executes instructions one by one depending on which instruction the program counter is pointing at. Some instructions make it do arithmetic and bitwise operations, while others make the program counter jump. After execution of each instruction, we print the memory and values of all registers.

## Sizes
- Each instruction is 16 bits in size, and each memory location is also 16 bits in size.
- Memory addresses are 7 bits long, which means that there are 2^7 locations in total.