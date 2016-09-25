#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cout << "Error : Syntax to run the simulator is \n\n\t$ ./exe file.s\n" << std::endl;
    }
    else {
        std::string input_file_name = argv[1];
        std::ifstream ifile(input_file_name);
    }
    return 0;
}