#pragma once

#include <string>

class Assembler
{
public:
    explicit Assembler(const std::string& outName);

    void generate(const std::string& asmStr);

private:
    std::string generateOutputFilename();

private:
    std::string m_outName;
};
