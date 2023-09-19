#include "Linker.hpp"

#include <sstream>
#include <iostream>

Linker::Linker(const std::string& outName) : m_outName(outName)
{
}

void Linker::link()
{
    std::stringstream linkCmd;
    linkCmd << "ld " << m_outName << ".o -o " << m_outName;
    std::cout << linkCmd.str() << std::endl;
    std::system(linkCmd.str().c_str());
}
