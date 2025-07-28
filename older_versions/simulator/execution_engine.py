from register_file import *
from memory_file import *


def typeA(inst,register_dict):

    register_dict['111'] = '0000000000000000'

    ''' 
    fetching value of source registers and updating the value of destination register 
    after cheching overflow 
    ''' 
    
    val1_bin = register_dict[inst[10:13]]
    val2_bin = register_dict[inst[13:16]]
    val1_dec = bintodec(val1_bin)
    val2_dec = bintodec(val2_bin)
    #print(val1_bin , val2_bin , val1_dec , val2_dec )

    #ADD 
    if inst[0:5]=='00000':
        dec=val1_dec+val2_dec
        check_overflow(dec,register_dict,inst[7:10])

    #SUBTRACT
    elif inst[0:5]=='00001':
        dec=val1_dec-val2_dec
        check_overflow(dec,register_dict,inst[7:10])
    
    #MULTIPLY
    elif inst[0:5]=='00110':
        dec=val1_dec*val2_dec
        check_overflow(dec,register_dict,inst[7:10])
    
    #OR 
    elif inst[0:5]=='01011':
        result=val1_dec | val2_dec
        result=dectobin(result, 16)
        register_dict[inst[7:10]] = result
    
    #AND
    elif inst[0:5]=='01100':
        result=val1_dec & val2_dec
        result=dectobin(result, 16)
        register_dict[inst[7:10]] = result
        
    #XOR
    elif inst[0:5]=='01010':
        result=val1_dec ^ val2_dec
        result=dectobin(result, 16)
        register_dict[inst[7:10]] = result
        
        
def typeB(inst, register_dict):

    register_dict['111'] = '0000000000000000'

    # mov immediate
    if inst[0:5] == "00010":
        register_dict[inst[6:9]] = "0"*9 + inst[9:]

    #right shift
    if inst[0:5] == "01000": 
        shift_value = int(bintodec(inst[9:]))
        register_val = bintodec(register_dict[inst[6:9]])
        right_shifted = register_val >> shift_value
        #right_shifted = "0"*shift_value + str(dectobin(right_shifted,16))
        right_shifted=str(dectobin(right_shifted,16))

        register_dict[inst[6:9]] = right_shifted

    #left shift
    if inst[0:5] == "01001":
        shift_value = int(bintodec(inst[9:]))
        register_val = bintodec(register_dict[inst[6:9]])
        left_shifted = register_val << shift_value
        left_shifted = left_shifted[shift_value:]

        register_dict[inst[6:9]] = left_shifted 

# TYPE C
def typeC(line,reg_dict):
    opcode=line[0:5]
    reg1=line[10:13]
    reg2=line[13:16]
    binval_reg1=reg_dict[reg1]
    binval_reg2=reg_dict[reg2]
    decval_reg1=bintodec(str(binval_reg1))
    decval_reg2=bintodec(str(binval_reg2))

    reg_dict["111"]="0000000000000000"

    #mov
    if opcode=="00011":
        reg_dict[reg1]=binval_reg2
    #div
    elif opcode=="00111":
        if binval_reg2=="0000000000000000":
            reg_dict[reg1]="0000000000000000"
            set_flag(reg_dict,12)
        else:
            r0=dectobin(int(decval_reg1/decval_reg2), 16)  #quotient
            r1=dectobin(int(decval_reg1%decval_reg2), 16)  #remainder
            reg_dict["000"]=r0
            reg_dict["001"]=r1
    #invert/not
    elif opcode=="01101":
        inv_val=invert(binval_reg2)
        reg_dict[reg1]=inv_val
    #compare
    elif opcode=="01110":
        check_lessThan(decval_reg1,decval_reg2,reg_dict)
        check_greaterThan(decval_reg1,decval_reg2,reg_dict)
        check_equal(decval_reg1,decval_reg2,reg_dict)
        
def typeD(inst, register_dict, memory_inst):

    register_dict['111'] = '0000000000000000'

    if inst[0:5] == "00101":            #store
        memory_inst[bintodec(inst[9:])] = register_dict[inst[6:9]]
    if inst[0:5] == "00100":            #load
        register_dict[inst[6:9]] = memory_inst[bintodec(inst[9:])]

def typeE(inst, register_dict, pc):
        
    if inst[0:5] == "01111":
        register_dict['111'] = '0000000000000000'
        return inst[9:], pc, True
    
    if inst[0:5] == "11100":
        if regs['111'][13] == "1":
            register_dict['111'] = '0000000000000000'
            return inst[9:], pc, True
        else:
            register_dict['111'] = '0000000000000000'
            return dectobin(bintodec(pc) + 1, 7), pc, False
        
    if inst[0:5] == "11101":
        if regs['111'][14] == "1":
            register_dict['111'] = '0000000000000000'
            return inst[9:], pc, True
        else:
            register_dict['111'] = '0000000000000000'
            return dectobin(bintodec(pc) + 1, 7), pc, False
        
    if inst[0:5] == "11111":
        if regs['111'][15] == "1":
            register_dict['111'] = '0000000000000000'
            return inst[9:], pc, True
        else:
            register_dict['111'] = '0000000000000000'
            return dectobin(bintodec(pc) + 1, 7), pc, False
        

def cute(memory_inst,regs, pc):   # list of memory instructions
    
    #making the lists of the op codes and defining the types of function

    a = ["00000","00001","00110","01010","01011","01100"]
    b = ["00010","01000","01001"]
    c = ["00011","00111","01101","01110"]
    d = ["00100","00101"]
    e = ["01111","11100","11101","11111"]
    f = ["11010"] 

    while True:
        inst = memory_inst[bintodec(pc)]
        
        if inst[0:5] in a:
            typeA(inst,regs)
            print(pc, end="        ")
            pc = dectobin(bintodec(pc) + 1, 7)
            register_dump(regs)
            

        if inst[0:5] in b:
            typeB(inst,regs)
            print(pc, end="        ")
            pc = dectobin(bintodec(pc) + 1, 7)
            register_dump(regs)
            

        if inst[0:5] in c:
            typeC(inst,regs)
            print(pc, end="        ")
            pc = dectobin(bintodec(pc) + 1, 7)
            register_dump(regs)
            

        if inst[0:5] in d:
            typeD(inst,regs, memory_inst)
            print(pc, end="        ")
            pc = dectobin(bintodec(pc) + 1, 7)
            register_dump(regs)
            
        
        if inst[0:5] in e:
            newpc, oldpc, jump_flag = typeE(inst,regs,pc)
            print(pc, end="        ")
            if jump_flag == True:
                pc = newpc
            else:
                pc = dectobin(bintodec(oldpc) + 1, 7)
            
            register_dump(regs)
        
        if inst[0:5] == "11010":        # --------- halt --------
            regs['111'] = '0000000000000000'

            print(pc, end="        ")
            register_dump(regs)
            for i in range(128-len(memory_inst)):  
                memory_inst.append('0000000000000000')
            memory_dump(memory_inst)
            break