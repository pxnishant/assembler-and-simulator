import sys
from register_file import *
import execution_engine
# --------------- INITIALISING MEMORY ---------------- #

pc = "0000000"                 #will store memory address

memory_inst = []

while True:
    line = sys.stdin.readline().rstrip('\n')
    if not line:
        break
    memory_inst.append(line)

# remaining zeroes 
for i in range(128-len(memory_inst)):  
    memory_inst.append('0000000000000000')

execution_engine.cute(memory_inst, regs, pc)
