#pragma once

#include <string>

enum class AsmFormat {
    Elf,
    Win
};

class Assembler
{
public:
    explicit Assembler(const std::string& outName, AsmFormat format);

    void generate(const std::string& asmStr);

private:
    std::string generateOutputFilename();

private:
    AsmFormat m_format;
    std::string m_outName;
};
