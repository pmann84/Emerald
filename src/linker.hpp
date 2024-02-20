#pragma once

#include <string>

class linker
{
public:
    virtual ~linker() = default;
    virtual void link() = 0;
};
