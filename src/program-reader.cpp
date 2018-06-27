#include "program-reader.h"
#include <iostream>

void ProgramReader::Load(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cout << "Could not open requested program file" << std::endl;
    }

    char ch;

    while (file >> std::noskipws >> ch) {
        std::cout << std::hex << ch; // Or whatever
    }
}
