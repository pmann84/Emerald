#pragma once

#include "linker.hpp"

class windows_linker: public linker
{
public:
    explicit windows_linker(const std::string& out_name);

    void link() override;
private:
    std::string m_out_name;
};
