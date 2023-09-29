#pragma once

#include <optional>
#include <string>
#include <map>
#include <vector>

enum class TokenType
{
    Return,
    Let,
    IntLit,
    Identifier,
    ExprEnd,
    OpenParen,
    CloseParen,
    Equals,
    Plus,
    Asterisk,
    Minus,
    ForwardSlash,
    OpenCurly,
    CloseCurly,
    Hash
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
    {'{', TokenType::OpenCurly},
    {'}', TokenType::CloseCurly},
    {'#', TokenType::Hash}
};

static std::map<std::string, TokenType> KeywordTokenMap = {
    {"return", TokenType::Return},
    {"let", TokenType::Let},
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

struct TokenInfo
{
    std::string File;
    size_t Line;
    size_t Pos;
};

struct Token
{
    TokenType Type;
    std::optional<TokenInfo> Info;
    std::optional<std::string> Value;
};

using TokenVector = std::vector<Token>;