#include <iostream>
#include <fstream>
#include <regex>

void cmd(std::string &s) {
    std::cout << "mips> ";
    std::cin >> s;
}

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
    std::string file;
    int mode_exec = -1;
    switch (argc) {
        case 2:
            file = argv[1];
            break;
        case 3:
            file = argv[1];
            if (std::string(argv[2]) == "s") {
                mode_exec = 1;
            }
            else if (std::string(argv[2]) == "e") {
                mode_exec = 0;
            }
            else {
                std::cerr << "Error : Use as '$ ./mips file.s m' where m is 's' for step execution and 'e' for execution till end." << std::endl;
                return 0;
            }
            break;
        default:
            std::cerr << "Error : Use as '$ ./mips file.s m' where m is 's' for step execution and 'e' for execution till end." << std::endl;
            return 0;
    }

    std::ifstream infile(file);
    if (!infile.is_open()) {
        std::cerr << "Error : File does not exists" << std::endl;
        return 0;
    }

    std::string reg_str[] = {
            "r0", "at", "v0", "v1", "a0", "a1", "a2", "a3",
            "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
            "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
            "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
    };

    std::map<std::string, int> reg_map;

    for (int j = 0; j < 32; ++j) {
        reg_map[reg_str[j]] = j;
    }

    std::vector<std::pair<std::string, int>> reg_vector;
    for (std::pair<std::string, int> i:reg_map) {
        reg_vector.push_back(i);
    }
    std::sort(reg_vector.begin(), reg_vector.end(), [](const std::pair<std::string, int> &left, const std::pair<std::string, int> &right) {
        return left.second < right.second;
    });


    std::cout << "MIPS Simulator" << std::endl;

    std::regex mode_e("\\s*\\.(\\w+)\\s*(\\w+)?\\s*(#.*)?");
    std::regex empty_e("\\s*(#.*)?");
    std::regex branch_e("\\s*(\\w+)\\s*:.*");
    std::regex br_empty_e("\\s*(\\w+)\\s*:\\s*(#.*)?");
    // add sub mul and or nor slt
    std::regex r_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*(#.*)?");
    // addi andi ori slti
    std::regex i_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\-?\\d+)\\s*(#.*)?");
    // lw, sw
    std::regex load_e("\\s*(\\w+\\s*:)?\\s*([a-z][a-z])\\s+\\$(\\w+)\\s*,\\s*(\\-?\\d+)\\s*\\(\\s*\\$(\\w+)\\s*\\)\\s*(#.*)?");
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

    // TODO Decide mode of execution
    while (mode_exec == -1) {
        std::string mode_temp;
        std::cout << "Enter s for step execution and e for execution till end" << std::endl;
        cmd(mode_temp);
        if (mode_temp == "s") {
            mode_exec = 1;
        }
        else if (mode_temp == "e") {
            mode_exec = 0;
        }
    }

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

        if (mode_exec == 1) {
            std::cout << "Code executed - line " << i+1 << " : " << code[i] << std::endl;
            mode_exec = -1;
            print_reg(regs, reg_vector);
        }

        while (mode_exec == -1) {
            std::string mode_temp;
            std::cout << "Enter s for step execution and e for execution till end" << std::endl;
            cmd(mode_temp);
            if (mode_temp == "s") {
                mode_exec = 1;
            }
            else if (mode_temp == "e") {
                mode_exec = 0;
            }
        }

    }
    print_reg(regs, reg_vector);



    return 0;
}
