#include "Assembler.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

Assembler::Assembler(const std::string& outName) : m_outName(outName)
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
    assembleCmd << "nasm -felf64 " << outputFilename;
    std::cout << assembleCmd.str() << std::endl;
    std::system(assembleCmd.str().c_str());
}

std::string Assembler::generateOutputFilename()
{
    std::stringstream outputFilenameSs;
    outputFilenameSs << "./" << m_outName << ".asm";
    return outputFilenameSs.str();
}
