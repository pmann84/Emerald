#pragma once

#include "linker.hpp"

class windows_linker: public linker
{
public:
    explicit windows_linker(const std::string& out_name);

    void link() override;
private:
    // TODO: Make this more dynamic
    std::string m_linker_path = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\link.exe)";
    std::string m_out_name;
};
