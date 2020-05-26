#include <iostream>

#include "yacis/yacis.hpp"

// Temporary main
int main() {
    std::cout << yacis::compile_to_asm("../examples/primality_test.yac");
}
