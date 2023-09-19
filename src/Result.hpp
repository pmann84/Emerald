#pragma once

#include <string>
#include <vector>

template <typename T>
struct Result
{
    T result;
    bool success{};
    std::vector<std::string> errors;
};