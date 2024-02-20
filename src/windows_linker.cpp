#include "windows_linker.hpp"

#include <sstream>
#include <iostream>

windows_linker::windows_linker(const std::string& out_name) : m_out_name(out_name)
{
}

void windows_linker::link()
{
    std::stringstream linkCmd;
    linkCmd << "ld " << m_out_name << ".o -o " << m_out_name;
    std::cout << linkCmd.str() << std::endl;
    std::system(linkCmd.str().c_str());
}
