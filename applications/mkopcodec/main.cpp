#include <sqlite/compiler/tooling/mkopcodec/generator.hpp>

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: mkopcodec <opcodes.h>\n";
        return 1;
    }
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "mkopcodec: cannot open " << argv[1] << "\n";
        return 1;
    }
    std::cout << sqlite::compiler::tooling::mkopcodec::generate(in);
    return 0;
}
