#pragma once

#include <optional>
#include <string>

enum class TokenType
{
    Return,
    IntLit,
    ExprEnd,
    OpenParen,
    CloseParen,
    Identifier,
    Let,
    Equals,
    Plus,
    Asterisk,
};

inline std::optional<uint8_t> binaryPrecedence(TokenType tokenType)
{
    switch (tokenType)
    {
        case TokenType::Plus:
            return 0;
        case TokenType::Asterisk:
            return 1;
        default:
            return {};
    }
}

struct Token
{
    TokenType token;
    std::optional<std::string> value;
};