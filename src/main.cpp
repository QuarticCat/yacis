#include <fstream>
#include <iostream>

#include "yacis/yacis.hpp"

int main(int argc, char* argv[]) {
    try {
        if (argc == 2)
            std::cout << yacis::compile_to_asm(argv[1]) << std::endl;
        else if (argc == 3)
            std::ofstream(argv[2])
                << yacis::compile_to_asm(argv[1]) << std::endl;
        else
            std::cerr << "Unknown argument number." << std::endl;
    } catch (const yacis::analysis::CompileError& e) {
        std::cerr << e.what() << std::endl;
    }
}
