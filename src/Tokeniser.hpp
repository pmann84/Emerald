#pragma once

#include "Result.hpp"
#include "Tokens.hpp"

#include <string>
#include <optional>
#include <vector>
#include <map>

//// Regex to match
//static std::map<std::string, TokenType> TokenMap = {
//        {"return", TokenType::return_},
//        {"[0-9]*", TokenType::int_lit},
//        {";", TokenType::expr_end},
//        {"(", TokenType::open_paren},
//        {")", TokenType::close_paren},
//        {"let", TokenType::let},
//        {"=", TokenType::equals},
//};

using TokenVector = std::vector<Token>;

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
