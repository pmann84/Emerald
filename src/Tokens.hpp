#pragma once

#include <optional>
#include <string>
#include <map>

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
    Minus,
    ForwardSlash
};

// Regex to match
static std::map<char, TokenType> SymbolTokenMap = {
    {';', TokenType::ExprEnd},
    {'(', TokenType::OpenParen},
    {')', TokenType::CloseParen},
    {'=', TokenType::Equals},
    {'+', TokenType::Plus},
    {'*', TokenType::Asterisk},
    {'-', TokenType::Minus},
    {'/', TokenType::ForwardSlash},
};

inline std::optional<uint8_t> binaryPrecedence(TokenType tokenType)
{
    switch (tokenType)
    {
        case TokenType::Minus:
        case TokenType::Plus:
            return 0;
        case TokenType::Asterisk:
        case TokenType::ForwardSlash:
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

using TokenVector = std::vector<Token>;