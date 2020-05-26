#include <fstream>
#include <iostream>

#include "yacis/yacis.hpp"

int main(int argc, char* argv[]) {
    if (argc == 2)
        std::cout << yacis::compile_to_asm(argv[1]);
    else if (argc == 3)
        std::ofstream(argv[2]) << yacis::compile_to_asm(argv[1]);
    else
        std::cout << "Unknown argument number.";
}
