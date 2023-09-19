#pragma once


#include <string>

class Linker
{
public:
    explicit Linker(const std::string& outName);

    void link();

private:
    std::string m_outName;
};
