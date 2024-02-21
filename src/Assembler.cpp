#include "Assembler.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

Assembler::Assembler(const std::string& outName, AsmFormat format) : m_outName(outName), m_format(format)
{
}

void Assembler::generate(const std::string& asmStr)
{
    const auto outputFilename = generateOutputFilename();

    // Output to a file for assemblage and linking
    {
        std::fstream outputFile(outputFilename, std::ios::out);
        outputFile << asmStr;
    }

    std::stringstream assembleCmd;
    assembleCmd << "nasm ";
    switch (m_format)
    {
        case AsmFormat::Elf:
            assembleCmd << "-felf64";
            break;
        case AsmFormat::Win:
            assembleCmd << "-fwin64";
            break;
    }
    assembleCmd << " " << outputFilename;
    std::cout << assembleCmd.str() << std::endl;
    std::system(assembleCmd.str().c_str());
}

std::string Assembler::generateOutputFilename()
{
    std::stringstream outputFilenameSs;
    outputFilenameSs << "./" << m_outName << ".asm";
    return outputFilenameSs.str();
}
