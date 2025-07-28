#-------------------CONVERSIONS----------------------#

def bintodec(ssnumber):    #input:string
    sum=0
    for i in range(0,len(ssnumber)):
        if ssnumber[i]=="1":
            sum+=2**(len(ssnumber)-1-i)
    return sum             #output: integer

def dectobin(dec, n):        #input:integer
    binary=format(dec,'b')
    bin_num="0"*(n-len(binary))+binary
    return bin_num       #output:string

#------------- INITIALISING REGISTER VALUES -----------#

regs={
    "000":"0000000000000000",
    "001":"0000000000000000",
    "010":"0000000000000000",
    "011":"0000000000000000",
    "100":"0000000000000000",
    "101":"0000000000000000",
    "110":"0000000000000000",
    "111":"0000000000000000" 
    }

#--------------- REGISTER FUNCTIONS ------------------- #

def invert(registervalue):
    invList = list(registervalue) 
    for i in range(0,16):
        if invList[i]=='0':
            invList[i]='1'
        elif invList[i]=='1':
            invList[i]='0'
    val="".join(invList)
    return val



''' printing the values of all the registers '''

def register_dump(register_dict):

    print(register_dict['000'],end=" ")
    print(register_dict['001'],end=" ")
    print(register_dict['010'],end=" ")
    print(register_dict['011'],end=" ")
    print(register_dict['100'],end=" ")
    print(register_dict['101'],end=" ")
    print(register_dict['110'],end=" ")
    print(register_dict['111'])



#-------------------- FLAG FUNCTIONS ------------------- #
    # _ _ _ _ _ _ _ _ _ _ _  __ __ __ __ __
    # 0 0 0 0 0 0 0 0 0 0 0  0  V  L  G  E
    # _ _ _ _ _ _ _ _ _ _ _  __ __ __ __ __
    # 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

def set_flag(register_dict,index):
    slice1=register_dict['111'][0:index]
    slice2=register_dict['111'][index+1:]
    flags=slice1+'1'+slice2
    register_dict['111']=flags 


def check_overflow(dec,register_dict,register_no):
    if (dec>65535 or dec<0):
        set_flag(register_dict,12)
        if (dec<0):
            register_dict[register_no]='0000000000000000'

    else:
        register_dict[register_no]=dectobin(dec, 16)

def check_lessThan(n1,n2,register_dict):
    if(n1<n2):
        set_flag(register_dict,13)

def check_greaterThan(n1,n2,register_dict):
    if(n1>n2):
        set_flag(register_dict,14)

def check_equal(n1,n2,register_dict):
    if(n1==n2):
        set_flag(register_dict,15)