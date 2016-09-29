#include <iostream>
#include <fstream>
#include <regex>

int get_reg(std::string s) {
    if (s == "0") return 0;
    if (s == "1") return 1;
    if (s == "2") return 2;
    if (s == "3") return 3;
    if (s == "4") return 4;
    if (s == "5") return 5;
    if (s == "6") return 6;
    if (s == "7") return 7;
    if (s == "8") return 8;
    if (s == "9") return 9;
    if (s == "10") return 10;
    if (s == "11") return 11;
    if (s == "12") return 12;
    if (s == "13") return 13;
    if (s == "14") return 14;
    if (s == "15") return 15;
    if (s == "16") return 16;
    if (s == "17") return 17;
    if (s == "18") return 18;
    if (s == "19") return 19;
    if (s == "20") return 20;
    if (s == "21") return 21;
    if (s == "22") return 22;
    if (s == "23") return 23;
    if (s == "24") return 24;
    if (s == "25") return 25;
    if (s == "26") return 26;
    if (s == "27") return 27;
    if (s == "28") return 28;
    if (s == "29") return 29;
    if (s == "30") return 30;
    if (s == "31") return 31;
    if (s == "r0") return 0;
    if (s == "zero") return 0;
    if (s == "at") return 1;
    if (s == "v0") return 2;
    if (s == "v1") return 3;
    if (s == "a0") return 4;
    if (s == "a1") return 5;
    if (s == "a2") return 6;
    if (s == "a3") return 7;
    if (s == "t0") return 8;
    if (s == "t1") return 9;
    if (s == "t2") return 10;
    if (s == "t3") return 11;
    if (s == "t4") return 12;
    if (s == "t5") return 13;
    if (s == "t6") return 14;
    if (s == "t7") return 15;
    if (s == "s0") return 16;
    if (s == "s1") return 17;
    if (s == "s2") return 18;
    if (s == "s3") return 19;
    if (s == "s4") return 20;
    if (s == "s5") return 21;
    if (s == "s6") return 22;
    if (s == "s7") return 23;
    if (s == "t8") return 24;
    if (s == "t9") return 25;
    if (s == "k0") return 26;
    if (s == "k1") return 27;
    if (s == "gp") return 28;
    if (s == "sp") return 29;
    if (s == "fp") return 30;
    if (s == "s8") return 30;
    if (s == "ra") return 31;
    return -1;
}

void print_reg(int *regs) {
    std::cout << "$0 : " << " $0/$zero : " << regs[0] << std::endl;
    std::cout << "$1 : " << " $at : " << regs[1] << std::endl;
    std::cout << "$2 : " << " $v0 : " << regs[2] << std::endl;
    std::cout << "$3 : " << " $v1 : " << regs[3] << std::endl;
    std::cout << "$4 : " << " $a0 : " << regs[4] << std::endl;
    std::cout << "$5 : " << " $a1 : " << regs[5] << std::endl;
    std::cout << "$6 : " << " $a2 : " << regs[6] << std::endl;
    std::cout << "$7 : " << " $a3 : " << regs[7] << std::endl;
    std::cout << "$8 : " << " $t0 : " << regs[8] << std::endl;
    std::cout << "$9 : " << " $t1 : " << regs[9] << std::endl;
    std::cout << "$10 : " << " $t2 : " << regs[10] << std::endl;
    std::cout << "$11 : " << " $t3 : " << regs[11] << std::endl;
    std::cout << "$12 : " << " $t4 : " << regs[12] << std::endl;
    std::cout << "$13 : " << " $t5 : " << regs[13] << std::endl;
    std::cout << "$14 : " << " $t6 : " << regs[14] << std::endl;
    std::cout << "$15 : " << " $t7 : " << regs[15] << std::endl;
    std::cout << "$16 : " << " $s0 : " << regs[16] << std::endl;
    std::cout << "$17 : " << " $s1 : " << regs[17] << std::endl;
    std::cout << "$18 : " << " $s2 : " << regs[18] << std::endl;
    std::cout << "$19 : " << " $s3 : " << regs[19] << std::endl;
    std::cout << "$20 : " << " $s4 : " << regs[20] << std::endl;
    std::cout << "$21 : " << " $s5 : " << regs[21] << std::endl;
    std::cout << "$22 : " << " $s6 : " << regs[22] << std::endl;
    std::cout << "$23 : " << " $s7 : " << regs[23] << std::endl;
    std::cout << "$24 : " << " $t8 : " << regs[24] << std::endl;
    std::cout << "$25 : " << " $t9 : " << regs[25] << std::endl;
    std::cout << "$26 : " << " $k0 : " << regs[26] << std::endl;
    std::cout << "$27 : " << " $k1 : " << regs[27] << std::endl;
    std::cout << "$28 : " << " $gp : " << regs[28] << std::endl;
    std::cout << "$29 : " << " $sp : " << regs[29] << std::endl;
    std::cout << "$30 : " << " $fp/$s8 : " << regs[30] << std::endl;
    std::cout << "$31 : " << " $ra : " << regs[31] << std::endl;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cout << "Error : Incorrect syntax. Correct syntax is \n\n\t$ ./exe file.s\n" << std::endl;
    }
    else {
        std::string input_file_name = argv[1];
        std::ifstream infile(input_file_name);
        std::cout << "MIPS Simulator" << std::endl << std::endl;

        std::regex mode_e("\\.(\\w+)\\s*(#.*)?");
        std::regex empty_e("\\s*(#.*)?");
        std::regex branch_e("\\s*(\\w+)\\s*:.*");
        std::regex br_empty_e("\\s*(\\w+)\\s*:\\s*(#.*)?");
        // add sub mul and or nor slt
        std::regex r_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*(#.*)?");
        // addi andi ori slti
        std::regex i_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\d+)\\s*(#.*)?");
        // lw, sw
        std::regex load_e("\\s*(\\w+\\s*:)?\\s*([ls]w)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*(#.*)?");
        // beq, bne
        std::regex cond_branch_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
        // j
        std::regex jump_e("\\s*(\\w+\\s*:)?\\s*j\\s+(\\w+)\\s*(#.*)?");
        // halt
        std::regex halt_e("\\s*(\\w+\\s*:)?\\s*halt\\s*(#.*)?");
        std::smatch sm;

        std::vector<std::string> code;
        std::string line("");
        int mode = -1; // 0 - data, 1 - text

        std::map<std::string, int> branches;
        int line_no = -1;

        while (std::getline(infile, line)) {
            line_no++;
            code.push_back("");
            if (std::regex_match(line, sm, empty_e)) continue;
            if(std::regex_match(line, sm, mode_e)) {
                if (sm[1] == "data") mode = 0;
                else if (sm[1] == "text") mode = 1;
                else {
                    std::cout << "Error : Invalid instruction at line " << line_no+1 << std::endl;
                    return 0;
                }
                continue;
            }
            if (mode == 0) {

            }
            else if (mode == 1) {
                if (std::regex_match(line, sm, branch_e)) {
                    if(branches[sm[1]] == 0) branches[sm[1]] = line_no;
                    else {
                        std::cout << "Error : Two instructions with same label at line " << line_no+1 << std::endl;
                        return 0;
                    }
                }
                code[line_no] = line;
            }
            else {
                std::cout << "Error : Invalid instruction at line " << line_no+1 << std::endl;
                return 0;
            }
        }

        int *regs = new int[32];
        regs[0] = 0;
        for (int k = 0; k < 32; ++k) {
            regs[k] = 0;
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
                int reg_d = get_reg(sm[3]);
                int reg_s = get_reg(sm[4]);
                int reg_t = get_reg(sm[5]);
                if (reg_d == -1 || reg_s == -1 || reg_t == -1) {
                    std::cout << "Error : Invalid instruction at line " << i+1 << std::endl;
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
                    std::cout << "Error : Invalid instruction at line " << i+1 << std::endl;
                    return 0;
                }
                regs[0] = 0;
            }
            else if (std::regex_match(code[i], sm, i_format_e)) {
                int reg_d = get_reg(sm[3]);
                int reg_s = get_reg(sm[4]);
                int imm = std::stoi(sm[5]);
                if (reg_d == -1 || reg_s == -1) {
                    std::cout << "Error : Invalid instruction at line " << i+1 << std::endl;
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
                    std::cout << "Error : Invalid instruction at line " << i+1 << std::endl;
                    return 0;
                }
            }
            else if (std::regex_match(code[i], sm, load_e)) {
                for (unsigned j = 0; j < sm.size(); ++j) {
                    std::cout << "[" << sm[j] << "] ";
                }
            }
            else if (std::regex_match(code[i], sm, cond_branch_e)) {
                for (unsigned j = 0; j < sm.size(); ++j) {
                    std::cout << "[" << sm[j] << "] ";
                }
            }
            else if (std::regex_match(code[i], sm, jump_e)) {
                for (unsigned j = 0; j < sm.size(); ++j) {
                    std::cout << "[" << sm[j] << "] ";
                }
            }
            else if (std::regex_match(code[i], sm, halt_e)) {
                for (unsigned j = 0; j < sm.size(); ++j) {
                    std::cout << "[" << sm[j] << "] ";
                }
            }
            else {
                std::cout << "Error : Invalid instruction at line " << i+1 << std::endl;
                return 0;
            }
            std::cout << std::endl;
        }
        print_reg(regs);
    }
    return 0;
}
