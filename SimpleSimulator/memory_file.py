from register_file import *

def memory_dump(memory_inst):
    for i in memory_inst:
        print(i)
        
def instr_at_pc(memory_inst, pc):
    return memory_inst[bintodec(pc)]