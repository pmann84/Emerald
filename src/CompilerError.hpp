#pragma once

#include <string>
#include <vector>

struct Error
{
    std::string Message;
};

class ErrorHandler
{
public:
    

private:
    std::vector<Error> m_errors;
};