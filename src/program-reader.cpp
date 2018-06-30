#include "program-reader.h"
#include <iostream>

void ProgramReader::Load(const std::string &filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file) {
        std::cout << "Could not open requested program file" << std::endl;
    }

    char ch;

    Program.clear();

    while (file >> std::noskipws >> ch) {
        Program.push_back(ch);
    }
}
