#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <bitset>
using namespace std;

//each instruction exists as a string, so we must convert the strings to integers first

int bintodec(const string &s) {
    int val = 0;
    for (char c : s) {
        val = val * 2 + (c - '0');
    }
    return val;
}

string dectobin(int n, int bits = 16) {
    string res = "";
    for (int i = bits - 1; i >= 0; --i) {
        res += ((n >> i) & 1) ? '1' : '0';
    }
    return res;
}

string invert(const string &s) {
    string res = s;
    for (char &c : res) {
        c = (c == '0') ? '1' : '0';
    }
    return res;
}

void set_flag(string &FLAGS, int idx) {
    FLAGS[idx] = '1';
}

void clear_flags(string &FLAGS) {
    FLAGS = "0000000000000000";
}

//in a map, .at throws an exception when the key doesn't exist
void register_dump(map<string, string> &regs) {
    for (int i = 0; i < 8; ++i) {
        cout << regs.at(dectobin(i, 3)) << " ";
    }
    cout << endl;
}

void memory_dump(vector<string> &mem) {
    for (string &line : mem) {
        cout << line << endl;
    }
}

void check_overflow(int val, map<string, string> &regs, string &dest) {
    if (val > 65535 || val < 0) {
        set_flag(regs["111"], 12);
        regs[dest] = "0000000000000000";
    } else {
        regs[dest] = dectobin(val, 16);
    }
}

//inst reg reg reg
void typeA(const string &inst, map<string, string> &regs) {
    clear_flags(regs["111"]);
    int val1 = bintodec(regs[inst.substr(10, 3)]);  //R2
    int val2 = bintodec(regs[inst.substr(13, 3)]);  //R3
    string dest = inst.substr(7, 3);    //R1
    if (inst.substr(0, 5) == "00000") check_overflow(val1 + val2, regs, dest); // ADD
    else if (inst.substr(0, 5) == "00001") check_overflow(val1 - val2, regs, dest); // SUB
    else if (inst.substr(0, 5) == "00110") check_overflow(val1 * val2, regs, dest); // MUL
    else if (inst.substr(0, 5) == "01010") regs[dest] = dectobin(val1 ^ val2, 16); // XOR
    else if (inst.substr(0, 5) == "01011") regs[dest] = dectobin(val1 | val2, 16); // OR
    else if (inst.substr(0, 5) == "01100") regs[dest] = dectobin(val1 & val2, 16); // AND
}

//inst reg $val 
void typeB(string &inst, map<string, string> &regs) {
    clear_flags(regs["111"]);
    string reg = inst.substr(6, 3);
    int imm = bintodec(inst.substr(9));
    int reg_val = bintodec(regs[reg]);

    if (inst.substr(0, 5) == "00010") regs[reg] = string(9, '0') + inst.substr(9); // MOV Imm
    else if (inst.substr(0, 5) == "01000") regs[reg] = dectobin(reg_val >> imm, 16); // RS
    else if (inst.substr(0, 5) == "01001") regs[reg] = dectobin(reg_val << imm, 16); // LS
}

//inst reg reg
void typeC(const string &inst, map<string, string> &regs) {
    clear_flags(regs["111"]);
    string r1 = inst.substr(10, 3);
    string r2 = inst.substr(13, 3);
    int v1 = bintodec(regs[r1]);
    int v2 = bintodec(regs[r2]);

    if (inst.substr(0, 5) == "00011") regs[r1] = regs[r2]; // MOV Reg
    else if (inst.substr(0, 5) == "00111") { // DIV
        if (regs[r2] == "0000000000000000") {
            regs["000"] = regs["001"] = "0000000000000000";
            set_flag(regs["111"], 12);
        } else {
            regs["000"] = dectobin(v1 / v2, 16);
            regs["001"] = dectobin(v1 % v2, 16);
        }
    }
    else if (inst.substr(0, 5) == "01101") regs[r1] = invert(regs[r2]); // NOT
    else if (inst.substr(0, 5) == "01110") { // CMP
        if (v1 < v2) set_flag(regs["111"], 13);
        if (v1 > v2) set_flag(regs["111"], 14);
        if (v1 == v2) set_flag(regs["111"], 15);
    }
}

//inst reg mem_add
void typeD(const string &inst, map<string, string> &regs, vector<string> &mem) {
    clear_flags(regs["111"]);
    string r = inst.substr(6, 3);
    int addr = bintodec(inst.substr(9));
    if (inst.substr(0, 5) == "00100") regs[r] = mem[addr]; // LD
    else if (inst.substr(0, 5) == "00101") mem[addr] = regs[r]; // ST
}

//inst mem_add
pair<string, bool> typeE(const string &inst, map<string, string> &regs, const string &pc) {
    clear_flags(regs["111"]);
    string target = inst.substr(9);
    string FLAGS = regs["111"];

    if (inst.substr(0, 5) == "01111") return {target, true};  // JMP
    if (inst.substr(0, 5) == "11100" && FLAGS[13] == '1') return {target, true}; // JLT
    if (inst.substr(0, 5) == "11101" && FLAGS[14] == '1') return {target, true}; // JGT
    if (inst.substr(0, 5) == "11111" && FLAGS[15] == '1') return {target, true}; // JE

    return {dectobin(bintodec(pc) + 1, 7), false};
}

void simulate(vector<string> &mem, map<string, string> &regs, string pc) {
    while (true) {
        string inst = mem[bintodec(pc)];
        string op = inst.substr(0, 5);
        cout << pc << "        ";

        if (op == "11010") { // HLT
            clear_flags(regs["111"]);
            register_dump(regs);
            memory_dump(mem);
            break;
        }

        if (op == "00000" || op == "00001" || op == "00110" || op == "01010" || op == "01011" || op == "01100") typeA(inst, regs);
        else if (op == "00010" || op == "01000" || op == "01001") typeB(inst, regs);
        else if (op == "00011" || op == "00111" || op == "01101" || op == "01110") typeC(inst, regs);
        else if (op == "00100" || op == "00101") typeD(inst, regs, mem);
        else if (op == "01111" || op == "11100" || op == "11101" || op == "11111") {
            auto [newPC, jump] = typeE(inst, regs, pc);
            register_dump(regs);
            pc = jump ? newPC : dectobin(bintodec(pc) + 1, 7);
            continue;
        }

        register_dump(regs);
        pc = dectobin(bintodec(pc) + 1, 7);
    }
}

int main() {
    vector<string> memory;
    string line;
    while (getline(cin, line)) {
        if (!line.empty())
            memory.push_back(line);
    }

    //2^7 = size of address space, and each position is 16 bits
    while (memory.size() < 128)
        memory.push_back("0000000000000000");
        
    //values stored in registers
    map<string, string> regs = {
        {"000","0000000000000000"}, {"001","0000000000000000"},
        {"010","0000000000000000"}, {"011","0000000000000000"},
        {"100","0000000000000000"}, {"101","0000000000000000"},
        {"110","0000000000000000"}, {"111","0000000000000000"}
    };

    //program counter is 7 bits only, initially at 0000000
    simulate(memory, regs, "0000000");
    return 0;
}