#include <iostream>
#include <fstream>
#include <regex>

std::vector<std::string> &split(std::string s, std::vector<std::string> &v, char c = ' ') {
    v.clear();
    std::stringstream ss(s);
    std::string val;

    while(getline(ss, val, c)) {
        if (val.size() != 0) v.push_back(val);
    }
    return v;
}

int get_reg(std::string s, std::map<std::string, int> &reg_map) {
    std::regex d_e("\\d+");
    std::smatch sm;
    if (std::regex_match(s, sm, d_e)) {
        int r = std::stoi(s);
        if (r == 1 || r == 26 || r == 27) return -1;
        return std::stoi(s);
    }
    if (s == "zero" || s == "r0") return 0;
    if (reg_map[s] != 0 && reg_map[s] != 1 && reg_map[s] != 26 && reg_map[s] != 27) return reg_map[s];
    return -1;
}

void print_reg(int *regs, std::vector<std::pair<std::string, int>> &reg_vector) {
    for (int i = 0; i < reg_vector.size(); ++i) {
        std::cout << "$" << reg_vector[i].second << " : " << "$" << reg_vector[i].first << " : " << regs[reg_vector[i].second] << std::endl;
    }
}

void print_error(int line, std::string message = "Invalid instruction") {
    std::cout << "Error at line " << line << " : " << message << std::endl;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cout << "Error : Incorrect syntax. Correct syntax is \n\n\t$ ./exe file.s\n" << std::endl;
        return 0;
    }

    std::map<std::string, int> reg_map;
    reg_map["r0"] = 0;
    reg_map["at"] = 1;
    reg_map["v0"] = 2;
    reg_map["v1"] = 3;
    reg_map["a0"] = 4;
    reg_map["a1"] = 5;
    reg_map["a2"] = 6;
    reg_map["a3"] = 7;
    reg_map["t0"] = 8;
    reg_map["t1"] = 9;
    reg_map["t2"] = 10;
    reg_map["t3"] = 11;
    reg_map["t4"] = 12;
    reg_map["t5"] = 13;
    reg_map["t6"] = 14;
    reg_map["t7"] = 15;
    reg_map["s0"] = 16;
    reg_map["s1"] = 17;
    reg_map["s2"] = 18;
    reg_map["s3"] = 19;
    reg_map["s4"] = 20;
    reg_map["s5"] = 21;
    reg_map["s6"] = 22;
    reg_map["s7"] = 23;
    reg_map["t8"] = 24;
    reg_map["t9"] = 25;
    reg_map["k0"] = 26;
    reg_map["k1"] = 27;
    reg_map["gp"] = 28;
    reg_map["sp"] = 29;
    reg_map["fp"] = 30;
    reg_map["ra"] = 31;

    std::vector<std::pair<std::string, int>> reg_vector;
    for (std::pair<std::string, int> i:reg_map) {
        reg_vector.push_back(i);
    }
    std::sort(reg_vector.begin(), reg_vector.end(), [](const std::pair<std::string, int> &left, const std::pair<std::string, int> &right) {
        return left.second < right.second;
    });

    std::ifstream infile(argv[1]);
    std::cout << "MIPS Simulator" << std::endl << std::endl;

    std::regex mode_e("\\s*\\.(\\w+)\\s*(\\w+)?\\s*(#.*)?");
    std::regex empty_e("\\s*(#.*)?");
    std::regex branch_e("\\s*(\\w+)\\s*:.*");
    std::regex br_empty_e("\\s*(\\w+)\\s*:\\s*(#.*)?");
    // add sub mul and or nor slt
    std::regex r_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*(#.*)?");
    // addi andi ori slti
    std::regex i_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\d+)\\s*(#.*)?");
    // lw, sw
    std::regex load_e("\\s*(\\w+\\s*:)?\\s*([a-z][a-z])\\s+\\$(\\w+)\\s*,\\s*(\\d+)\\s*\\(\\s*\\$(\\w+)\\s*\\)\\s*(#.*)?");
    // la
    std::regex la_e("\\s*(\\w+\\s*:)?\\s*la\\s+\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
    // beq, bne
    std::regex cond_branch_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
    // j
    std::regex jump_e("\\s*(\\w+\\s*:)?\\s*j\\s+(\\w+)\\s*(#.*)?");
    // halt
    std::regex halt_e("\\s*(\\w+\\s*:)?\\s*halt\\s*(#.*)?");
    std::regex data_word_e("\\s*(\\w+)\\s*:\\s*\\.(\\w+)\\s+([\\w\"'\\s,]+)\\s*(#.*)?");
    std::smatch sm;

    std::vector<std::string> code;
    int *regs = new int[32];
    regs[0] = 0;
    for (int k = 0; k < 32; ++k) {
        regs[k] = 0;
    }
    std::map<int, int> data;
    std::map<std::string, int> data_r;
    int cp = 15000000;
    regs[get_reg("gp", reg_map)] = cp;
    std::string line;
    int mode = -1; // 0 - data, 1 - text

    std::map<std::string, int> branches;
    int line_no = -1;

    while (std::getline(infile, line)) {
        line_no++;
        code.push_back("");
        if (std::regex_match(line, sm, empty_e)) continue;
        if(std::regex_match(line, sm, mode_e)) {
            if (sm[1] == "data" && sm[2] == "") mode = 0;
            else if (sm[1] == "text" && sm[2] == "") mode = 1;
            else if (sm[1] == "globl" && sm[2] == "main") {

            }
            else {
                print_error(line_no+1);
                return 0;
            }
            continue;
        }
        if (mode == 0) {
            if (std::regex_match(line, sm, data_word_e)) {
                if (sm[2] == "word") {
                    std::string els = sm[3];
                    els = std::regex_replace(els, std::regex("\\s+$"), "");
                    els = std::regex_replace(els, std::regex(",\\s*"), "|");
                    std::vector<std::string> v;
                    split(els, v, '|');
                    for (std::string s : v) {
                        int t = std::stoi(s);
                        data[cp] = t;
                        data_r[sm[1]] = cp;
                        cp += 4;
                    }
                }
                else {
                    print_error(line_no+1);
                    return 0;
                }
            }
        }
        else if (mode == 1) {
            if (std::regex_match(line, sm, branch_e)) {
                if(branches[sm[1]] == 0) branches[sm[1]] = line_no;
                else {
                    print_error(line_no+1, "Two instructions with same label");
                    return 0;
                }
            }
            code[line_no] = line;
        }
        else {
            print_error(line_no+1);
            return 0;
        }
    }


//        char cmd;
//        std::cout << "Enter h to see list of commands, r to run the program until halt and n to execute step by step." << std::endl << std::endl;
//        std::cout << "mips> ";
//        std::cin >> cmd;
//        std::cout << std::endl;

    for (int i = 0; i < code.size(); ++i) {
        if (code[i] == "") continue;
        else if (std::regex_match(code[i], sm, br_empty_e)) continue;
        else if (std::regex_match(code[i], sm, r_format_e)) {
            int reg_d = get_reg(sm[3], reg_map);
            int reg_s = get_reg(sm[4], reg_map);
            int reg_t = get_reg(sm[5], reg_map);
            if (reg_d == -1 || reg_s == -1 || reg_t == -1) {
                print_error(i+1);
                return 0;
            }
            if (sm[2] == "add") {
                regs[reg_d] = regs[reg_s] + regs[reg_t];
            }
            else if (sm[2] == "sub") {
                regs[reg_d] = regs[reg_s] - regs[reg_t];
            }
            else if (sm[2] == "mul") {
                regs[reg_d] = regs[reg_s] * regs[reg_t];
            }
            else if (sm[2] == "and") {
                regs[reg_d] = regs[reg_s] & regs[reg_t];
            }
            else if (sm[2] == "or") {
                regs[reg_d] = regs[reg_s] | regs[reg_t];
            }
            else if (sm[2] == "nor") {
                regs[reg_d] = ~(regs[reg_s] | regs[reg_t]);
            }
            else if (sm[2] == "slt") {
                regs[reg_d] = regs[reg_s] < regs[reg_t];
            }
            else {
                print_error(i+1);
                return 0;
            }
            regs[0] = 0;
        }
        else if (std::regex_match(code[i], sm, i_format_e)) {
            int reg_d = get_reg(sm[3], reg_map);
            int reg_s = get_reg(sm[4], reg_map);
            int imm = std::stoi(sm[5]);
            if (reg_d == -1 || reg_s == -1) {
                print_error(i+1);
                return 0;
            }
            if (sm[2] == "addi") {
                regs[reg_d] = regs[reg_s] + imm;
            }
            else if (sm[2] == "andi") {
                regs[reg_d] = regs[reg_s] & imm;
            }
            else if (sm[2] == "ori") {
                regs[reg_d] = regs[reg_s] | imm;
            }
            else if (sm[2] == "slti") {
                regs[reg_d] = regs[reg_s] < imm;
            }
            else {
                print_error(i+1);
                return 0;
            }
        }
        else if (std::regex_match(code[i], sm, la_e)) {
            int reg_d = get_reg(sm[2], reg_map);
            std::string dt = sm[3];
            int adr = data_r[dt];
            if (adr == 0 || reg_d == -1) {
                print_error(i+1);
                return 0;
            }
            regs[reg_d] = adr;
        }
        else if (std::regex_match(code[i], sm, load_e)) {
            int reg_d = get_reg(sm[3], reg_map);
            int reg_s = get_reg(sm[5], reg_map);
            int imm = std::stoi(sm[4]);
            if (reg_d == -1 || reg_s == -1 || imm%4 != 0) {
                print_error(i+1);
                return 0;
            }
            if (sm[2] == "lw") {
                regs[reg_d] = data[imm+regs[reg_s]];
            }
            else if (sm[2] == "sw") {
                data[imm+regs[reg_s]] = regs[reg_d];
            }
            else {
                print_error(i+1);
                return 0;
            }
        }
        else if (std::regex_match(code[i], sm, cond_branch_e)) {
            int reg_1 = get_reg(sm[3], reg_map);
            int reg_2 = get_reg(sm[4], reg_map);
            int br = branches[sm[5]];
            bool eq = (reg_1 == reg_2);
            if (reg_1 == -1 || reg_2 == -1) {
                print_error(i+1);
                return 0;
            }
            if (br == 0) {
                print_error(i+1, "No such label exists");
                return 0;
            }
            if (sm[2] == "beq") {
                if (eq) {
                    i = br-1;
                }
            }
            else if (sm[2] == "bne") {
                if (!eq) {
                    i = br-1;
                }
            }
            else {
                print_error(i+1);
                return 0;
            }
        }
        else if (std::regex_match(code[i], sm, jump_e)) {
            int br = branches[sm[2]];
            if (br == 0) {
                print_error(i+1, "No such label exists");
                return 0;
            }
            i = br-1;
        }
        else if (std::regex_match(code[i], sm, halt_e)) {
            break;
        }
        else {
            print_error(i+1);
            return 0;
        }
        std::cout << std::endl;
    }
    print_reg(regs, reg_vector);

    return 0;
}
