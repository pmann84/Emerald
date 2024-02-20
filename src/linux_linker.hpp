#pragma once

#include "linker.hpp"

class linux_linker: public linker
{
public:
    explicit linux_linker(const std::string& out_name);

    void link() override;
private:
    std::string m_out_name;
};