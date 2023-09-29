#pragma once

#include "Tokens.hpp"
#include "CompilerError.hpp"

#include <string>
#include <optional>
#include <vector>
#include <map>

class Tokeniser
{
public:
    explicit Tokeniser(std::string source, ErrorHandler& errorHandler);

    [[nodiscard]] TokenVector tokenise();

private:
    [[nodiscard]] std::optional<char> peek(size_t offset = 0) const;
    char consume();
    TokenInfo getCurrentTokenInfo() const;

private:
    const std::string m_src;
    size_t m_srcPos = 0;
    size_t m_posInLine = 0;
    size_t m_lineNo = 1;
    ErrorHandler& m_errorHandler;
};
