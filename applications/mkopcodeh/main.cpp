#include <sqlite/compiler/tooling/mkopcodeh/generator.hpp>

#include <iostream>

int main() {
    std::cout << sqlite::compiler::tooling::mkopcodeh::generate(std::cin);
    return 0;
}
