#pragma once

#include <string>
#include <vector>

struct Error
{
    std::string File;
    std::string Line;
    std::string Pos;
    std::string Message;
};

class ErrorHandler
{
public:
    

private:
    std::vector<Error> m_errors;
};