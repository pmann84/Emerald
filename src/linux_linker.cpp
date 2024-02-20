#include "linux_linker.hpp"

#include <sstream>
#include <iostream>

linux_linker::linux_linker(const std::string& out_name) : m_out_name(out_name)
{
}

void linux_linker::link()
{
    std::stringstream linkCmd;
    linkCmd << "ld " << m_out_name << ".o -o " << m_out_name;
    std::cout << linkCmd.str() << std::endl;
    std::system(linkCmd.str().c_str());
}
