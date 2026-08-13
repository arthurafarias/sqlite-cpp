#include <sqlite/compiler/tooling/mkkeywordhash/generator.hpp>

#include <iostream>

int main() {
    std::cout << sqlite::compiler::tooling::mkkeywordhash::generate();
    return 0;
}
