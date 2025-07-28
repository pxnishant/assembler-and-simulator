#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <bitset>
using namespace std;

map<string, string> OPCODES = {
    {"add", "00000"}, {"sub", "00001"}, {"mov", "00010"}, {"movr", "00011"},
    {"ld", "00100"}, {"st", "00101"}, {"mul", "00110"}, {"div", "00111"},
    {"rs", "01000"}, {"ls", "01001"}, {"xor", "01010"}, {"or", "01011"},
    {"and", "01100"}, {"not", "01101"}, {"cmp", "01110"}, {"jmp", "01111"},
    {"jlt", "11100"}, {"jgt", "11101"}, {"je", "11111"}, {"hlt", "11010"}
};

map<string, char> TYPE_MAP = {
    {"add", 'A'}, {"sub", 'A'}, {"mul", 'A'}, {"xor", 'A'},
    {"or", 'A'}, {"and", 'A'}, {"mov", 'B'}, {"movr", 'C'},
    {"ld", 'D'}, {"st", 'D'}, {"rs", 'B'}, {"ls", 'B'},
    {"div", 'C'}, {"not", 'C'}, {"cmp", 'C'}, {"jmp", 'E'},
    {"jlt", 'E'}, {"jgt", 'E'}, {"je", 'E'}, {"hlt", 'F'}
};

map<string, string> REGISTER_CODES = {
    {"R0", "000"}, {"R1", "001"}, {"R2", "010"}, {"R3", "011"},
    {"R4", "100"}, {"R5", "101"}, {"R6", "110"}, {"FLAGS", "111"}
};

bool isValidRegister(const string &reg) {
    return REGISTER_CODES.find(reg) != REGISTER_CODES.end();
}

//checks if the value can fit in 7 bits
bool isImmediateValid(const string &imm) {
    if (imm.empty() || imm[0] != '$') return false;
    try {
        int val = stoi(imm.substr(1));
        return val >= 0 && val <= 127;
    } catch (...) {
        return false;
    }
}

string toBinary(int number, int bits) {
    string result = "";
    for (int i = bits - 1; i >= 0; --i) {
        result += ((number >> i) & 1) ? '1' : '0';
    }
    return result;
}

//manually splitting - building a string character by character and splitting on space
vector<string> tokenize(const string &line) {
    vector<string> tokens;
    string token;
    for (char c : line) {
        if (c == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

int main() {
    int N;
    cin >> N;
    cin.ignore();   //skips the next 1 character, which will be newline here

    vector<string> lines(N);
    map<string, int> labelAddress;
    map<string, int> variableAddress;
    vector<string> errors;
    set<string> variables;
    int varEnd = 0;
    bool hltFound = false;

    for (int i = 0; i < N; ++i) {
        getline(cin, lines[i]);
    }

    for (int i = 0; i < N; ++i) {
        vector<string> tokens = tokenize(lines[i]);
        if (tokens.empty()) continue;

        if (tokens[0] == "var") {
            if (varEnd) {
                errors.push_back("Error on line " + to_string(i + 1) + ": Variable declared after instruction.");
            } else if (tokens.size() == 2) {
                variables.insert(tokens[1]);
            } else {
                errors.push_back("Error on line " + to_string(i + 1) + ": Invalid variable declaration.");
            }
        } else {
            varEnd = 1;
            if (tokens[0].back() == ':') {
                string label = tokens[0].substr(0, tokens[0].size() - 1);
                //we are at the i'th instruction, but in memory this is stored at i - variables.size()
                //since variables are stored after all instructions in memory
                labelAddress[label] = i - variables.size();
            }
        }
    }
    
    //alloting an address to each var, starting at the first line after instructions end
    int instrCount = 0;
    for (const string &v : variables) {
        variableAddress[v] = N - variables.size() + instrCount++;
    }

    for (int i = 0; i < N; ++i) {
        vector<string> tokens = tokenize(lines[i]);
        if (tokens.empty() || tokens[0] == "var") continue;

        int idx = 0;
        if (tokens[0].back() == ':') idx = 1;

        if (idx >= tokens.size()) continue;
        string opcode = tokens[idx];

        if (opcode == "mov" && tokens.size() > idx + 2 && tokens[idx + 2][0] != '$') {
            opcode = "movr";
        }

        if (OPCODES.find(opcode) == OPCODES.end()) {
            errors.push_back("Error on line " + to_string(i + 1) + ": Invalid opcode.");
            continue;
        }

        char type = TYPE_MAP.at(opcode);
        string bin = OPCODES.at(opcode);

        try {
            if (type == 'A') {
                if (tokens.size() < idx + 4) throw runtime_error("Missing operands");
                string r1 = tokens[idx + 1], r2 = tokens[idx + 2], r3 = tokens[idx + 3];
                if (!isValidRegister(r1) || !isValidRegister(r2) || !isValidRegister(r3))
                    throw runtime_error("Invalid register");
                if (r1 == "FLAGS" || r2 == "FLAGS" || r3 == "FLAGS")
                    throw runtime_error("Illegal use of FLAGS");
                bin += "00" + REGISTER_CODES.at(r1) + REGISTER_CODES.at(r2) + REGISTER_CODES.at(r3);
            } else if (type == 'B') {
                if (tokens.size() < idx + 3) throw runtime_error("Missing operands");
                string r1 = tokens[idx + 1], imm = tokens[idx + 2];
                if (!isValidRegister(r1) || !isImmediateValid(imm))
                    throw runtime_error("Invalid register or immediate");
                if (r1 == "FLAGS") throw runtime_error("Illegal use of FLAGS");
                bin += "0" + REGISTER_CODES.at(r1) + toBinary(stoi(imm.substr(1)), 7);
            } else if (type == 'C') {
                if (tokens.size() < idx + 3) throw runtime_error("Missing operands");
                string r1 = tokens[idx + 1], r2 = tokens[idx + 2];
                if (!isValidRegister(r1) || !isValidRegister(r2))
                    throw runtime_error("Invalid register");
                if (r1 == "FLAGS" && r2 == "FLAGS") throw runtime_error("Illegal FLAGS move");
                bin += "00000" + REGISTER_CODES.at(r1) + REGISTER_CODES.at(r2);
            } else if (type == 'D') {
                if (tokens.size() < idx + 3) throw runtime_error("Missing operands");
                string r1 = tokens[idx + 1], var = tokens[idx + 2];
                if (!isValidRegister(r1) || variableAddress.find(var) == variableAddress.end())
                    throw runtime_error("Invalid register or variable");
                bin += "0" + REGISTER_CODES.at(r1) + toBinary(variableAddress[var], 7);
            } else if (type == 'E') {
                if (tokens.size() < idx + 2) throw runtime_error("Missing label");
                string lbl = tokens[idx + 1];
                if (labelAddress.find(lbl) == labelAddress.end())
                    throw runtime_error("Undefined label");
                bin += "0000" + toBinary(labelAddress[lbl], 7);
            } else if (type == 'F') {
                if (i != N - 1)
                    throw runtime_error("hlt not last instruction");
                bin += "00000000000";
                hltFound = true;
            }
        } catch (const exception &e) {
            errors.push_back("Error on line " + to_string(i + 1) + ": " + e.what());
            continue;
        }

        cout << bin << endl;
    }

    if (!hltFound) {
        cerr << "Error: Missing hlt instruction." << endl;
        return 1;
    }

    if (!errors.empty()) {
        for (const string &err : errors) {
            cerr << err << endl;
        }
        return 1;
    }

    return 0;
}