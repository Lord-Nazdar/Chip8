#pragma once
#include <fstream>
#include <vector>
#include <string>

class ProgramReader {
public:
    void Load(const std::string &filename);

    std::vector<uint8_t> Program;
};
