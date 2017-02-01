/**
 * CS2323 :: Computer Architecture :: Programming Assignment 2
 * Author : Prateek Kumar
 * Email : cs15btech11031@iith.ac.in
 */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>

/**
 * Take input from user
 */
void cmd(std::string &s) {
    std::cout << "mips> ";
    std::cin >> s;
}

/**
 * Split the string with deliminator
 */
std::vector<std::string> &split(std::string s, std::vector<std::string> &v, char c = ' ') {
    v.clear();
    std::stringstream ss(s);
    std::string val;

    while(getline(ss, val, c)) {
        if (val.size() != 0) v.push_back(val);
    }
    return v;
}

/** 
 * Get register number based on the register name like 8 for t0
 */
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

/**
 * Print the values in the registers
 */
void print_reg(int *regs, std::vector<std::pair<std::string, int>> &reg_vector) {
    for (int i = 0; i < reg_vector.size(); ++i) {
        std::cout << "$" <<  reg_vector[i].first << "[" << std::setw(2) << reg_vector[i].second << "] : " << regs[reg_vector[i].second] << std::endl;
    }
}

/**
 * Print the values from the memory
 */
void print_data(std::map<int, int> &data, int b, int e) {
    for (int i = b; i < e; i+=4) {
        std::cout << "<Memory> " << "0x" << std::setfill('0') << std::setw(sizeof(int)*2) << std::hex << i << " : " << std::dec << data[i] << std::endl;
    }
}

/**
 * Print the error if present in the program
 */
void print_error(int line, std::string message = "Invalid instruction") {
    std::cout << "Error at line " << line << " : " << message << std::endl;
}

/**
 * Main function of the program
 */
int main(int argc, const char* argv[]) {
    std::string file; // File name of the program
    int mode_exec = -1; // Mode of execution step(1) or till halt(0)
    switch (argc) { // Switching over number of arguments given in cli
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
            else { // Error if mode of execution is neither s nor e
                std::cerr << "Error : Use as '$ ./mips file.s m' where m is 's' for step execution and 'e' for execution till end." << std::endl;
                return 0;
            }
            break;
        default: // Error if number of arguements is neither 2 nor 3
            std::cerr << "Error : Use as '$ ./mips file.s m' where m is 's' for step execution and 'e' for execution till end." << std::endl;
            return 0;
    }

    std::ifstream infile(file); // Input file stream
    if (!infile.is_open()) { // If file not found then error
        std::cerr << "Error : File does not exists" << std::endl;
        return 0;
    }

    std::string reg_str[] = { // List of register names
            "r0", "at", "v0", "v1", "a0", "a1", "a2", "a3",
            "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
            "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
            "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
    };

    std::map<std::string, int> reg_map; // Map between register numbers and names

    for (int j = 0; j < 32; ++j) {
        reg_map[reg_str[j]] = j;
    }

    std::vector<std::pair<std::string, int>> reg_vector; // Register number and name pairs in a vector
    for (std::pair<std::string, int> i:reg_map) {
        reg_vector.push_back(i);
    }
    // Sort the register vector
    std::sort(reg_vector.begin(), reg_vector.end(), [](const std::pair<std::string, int> &left, const std::pair<std::string, int> &right) {
        return left.second < right.second;
    });


    std::cout << "MIPS Simulator" << std::endl << std::endl;

    std::regex mode_e("\\s*\\.(\\w+)\\s*(\\w+)?\\s*(#.*)?"); // Match .data or .text
    std::regex empty_e("\\s*(#.*)?"); // Match lines with no instruction
    std::regex branch_e("\\s*(\\w+)\\s*:.*"); // Match labels
    std::regex br_empty_e("\\s*(\\w+)\\s*:\\s*(#.*)?"); // Match labels with no instructuions
    //Match  add sub mul and or nor slt
    std::regex r_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*(#.*)?");
    //Match addi andi ori slti
    std::regex i_format_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\-?\\d+)\\s*(#.*)?");
    //Match lw, sw
    std::regex load_e("\\s*(\\w+\\s*:)?\\s*([a-z][a-z])\\s+\\$(\\w+)\\s*,\\s*(\\-?\\d+)\\s*\\(\\s*\\$(\\w+)\\s*\\)\\s*(#.*)?");
    //Match la, ls, ls having label in instruction
    std::regex la_e("\\s*(\\w+\\s*:)?\\s*la\\s+\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
    std::regex lw_e("\\s*(\\w+\\s*:)?\\s*lw\\s+\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
    std::regex sw_e("\\s*(\\w+\\s*:)?\\s*sw\\s+\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
    //Match beq, bne
    std::regex cond_branch_e("\\s*(\\w+\\s*:)?\\s*(\\w+)\\s+\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\w+)\\s*(#.*)?");
    //Match j
    std::regex jump_e("\\s*(\\w+\\s*:)?\\s*j\\s+(\\w+)\\s*(#.*)?");
    //Match halt
    std::regex halt_e("\\s*(\\w+\\s*:)?\\s*halt\\s*(#.*)?");
    // Match labels in data
    std::regex data_word_e("\\s*(\\w+)\\s*:\\s*\\.(\\w+)\\s+([\\-\\w\"'\\s,]+)\\s*(#.*)?");
    std::smatch sm;

    std::vector<std::string> code; // Vector to store program
    int *regs = new int[32]; // Array to store register values
    for (int k = 0; k < 32; ++k) { // Initialize every register to 0
        regs[k] = 0;
    }

    std::map<int, int> data; // Vector to store data
    std::map<std::string, int> data_r; // Vector to store labels in data

    // Memory address of data, global and stack
    int data_p_st = 10000000; 
    int data_p = data_p_st;
    int global_p = 20000000;
    int stack_p = 90000000;
    regs[get_reg("gp", reg_map)] = global_p;
    regs[get_reg("sp", reg_map)] = stack_p;

    std::string line;
    int mode = -1; // 0 - data, 1 - text

    std::map<std::string, int> branches;
    int line_no = -1;

    while (std::getline(infile, line)) { // Parse each line and store data and text labels
        line_no++;
        code.push_back("");
        if (std::regex_match(line, sm, empty_e)) continue; // Ignore empty instructions
        if(std::regex_match(line, sm, mode_e)) { // .data & .text set mode
            if (sm[1] == "data" && sm[2] == "") mode = 0; 
            else if (sm[1] == "text" && sm[2] == "") mode = 1;
            else if (sm[1] == "globl" && sm[2] == "main") { // .globl main

            }
            else { // Error
                print_error(line_no+1);
                return 0;
            }
            continue;
        }
        if (mode == 0) { // .data
            if (std::regex_match(line, sm, data_word_e)) {
                if (sm[2] == "word") {
                    std::string els = sm[3];
                    els = std::regex_replace(els, std::regex("\\s+$"), "");
                    els = std::regex_replace(els, std::regex(",\\s*"), "|");
                    std::vector<std::string> v;
                    split(els, v, '|'); // Store numbers in array from data label
                    data_r[sm[1]] = data_p;
                    for (std::string s : v) {
                        int t = std::stoi(s);
                        data[data_p] = t;
                        data_p += 4;
                    }
                }
                else {
                    print_error(line_no+1);
                    return 0;
                }
            }
        }
        else if (mode == 1) { // .text
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

    while (mode_exec == -1) { // Mode of execution not specified in cli arguement
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

    for (int i = 0; i < code.size(); ++i) { // Parse instructions
        if (code[i] == "") continue; // Empty instructions
        else if (std::regex_match(code[i], sm, br_empty_e)) continue;
        else if (std::regex_match(code[i], sm, r_format_e)) { // R - format
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
        else if (std::regex_match(code[i], sm, i_format_e)) { // I - format
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
        else if (std::regex_match(code[i], sm, la_e)) { // la $r, lbl
            int reg_d = get_reg(sm[2], reg_map);
            std::string dt = sm[3];
            int adr = data_r[dt];
            if (adr == 0 || reg_d == -1) {
                print_error(i+1);
                return 0;
            }
            regs[reg_d] = adr;
        }
        else if (std::regex_match(code[i], sm, lw_e)) { // lw $r, lbl
            int reg_d = get_reg(sm[2], reg_map);
            std::string dt = sm[3];
            int adr = data_r[dt];
            if (adr == 0 || reg_d == -1) {
                print_error(i+1);
                return 0;
            }
            regs[reg_d] = data[adr];
        }
        else if (std::regex_match(code[i], sm, sw_e)) { // sw $r, lbl
            int reg_d = get_reg(sm[2], reg_map);
            std::string dt = sm[3];
            int adr = data_r[dt];
            if (adr == 0 || reg_d == -1) {
                print_error(i+1);
                return 0;
            }
            data[adr] = regs[reg_d];
        }
        else if (std::regex_match(code[i], sm, load_e)) { // lw, sw
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
        else if (std::regex_match(code[i], sm, cond_branch_e)) { // beq, bne
            int reg_1 = get_reg(sm[3], reg_map);
            int reg_2 = get_reg(sm[4], reg_map);
            int br = branches[sm[5]];
            if (reg_1 == -1 || reg_2 == -1) {
                print_error(i+1);
                return 0;
            }
            if (br == 0) {
                print_error(i+1, "No such label exists");
                return 0;
            }
            bool eq = (regs[reg_1] == regs[reg_2]);
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
        else if (std::regex_match(code[i], sm, jump_e)) { // j
            int br = branches[sm[2]];
            if (br == 0) {
                print_error(i+1, "No such label exists");
                return 0;
            }
            i = br-1;
        }
        else if (std::regex_match(code[i], sm, halt_e)) { // halt
            break;
        }
        else {
            print_error(i+1);
            return 0;
        }

        // If step execution mode ask for step or till end execution
        if (mode_exec == 1) {
            std::cout << "Code executed - line " << i+1 << " : " << code[i] << std::endl;
            mode_exec = -1;
            print_reg(regs, reg_vector);
        }

        while (mode_exec == -1) {
            std::string mode_temp;
            std::cout << "Enter s for step execution, e to execute till end, d to display data section, g to display global data and t to display stack data" << std::endl;
            cmd(mode_temp);
            if (mode_temp == "s") {
                mode_exec = 1;
            }
            else if (mode_temp == "e") {
                mode_exec = 0;
            }
            else if (mode_temp == "d") {
                print_data(data, data_p_st, data_p);
            }
            else if (mode_temp == "g") {
                print_data(data, global_p, global_p+200);
            }
            else if (mode_temp == "t") {
                print_data(data, stack_p-200, stack_p);
            }
            else {
                std::cout << "Invalid Input" << std::endl;
            }
        }

    }
    print_reg(regs, reg_vector); // Print register values
    std::cout << std::endl << "DATA SECTION" << std::endl;
    print_data(data, data_p_st, data_p); // Print DATA section
    std::cout << std::endl << "GLOBAL DATA" << std::endl;
    print_data(data, global_p, global_p+200); // Print GLOBAL data
    std::cout << std::endl << "STACK DATA" << std::endl;
    print_data(data, stack_p-200, stack_p); // Print STACK data
    return 0;
}
