#pragma once

#include "Result.hpp"
#include "Tokens.hpp"

#include <string>
#include <optional>
#include <vector>
#include <map>

class Tokeniser
{
public:
    explicit Tokeniser(std::string source);

    [[nodiscard]] Result<TokenVector> tokenise();

private:
    [[nodiscard]] std::optional<char> peek(size_t offset = 0) const;
    char consume();

private:
    const std::string m_src;
    size_t m_srcPos = 0;
};
