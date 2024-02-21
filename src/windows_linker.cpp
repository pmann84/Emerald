#include "windows_linker.hpp"

#include <sstream>
#include <iostream>

windows_linker::windows_linker(const std::string& out_name) : m_out_name(out_name)
{
}

void windows_linker::link()
{
    std::stringstream linkCmd;
    linkCmd << "\"" << m_linker_path << "\" " << m_out_name << ".obj /OUT:" << m_out_name << ".exe /SUBSYSTEM:CONSOLE /ENTRY:start";
    std::cout << linkCmd.str() << std::endl;
    std::system(linkCmd.str().c_str());
}