#pragma once

#include <string>
#include <vector>
#include <optional>

class Token
{
public:
    struct Info
    {
        std::string File;
        size_t Line = 0;
        size_t Pos = 0;
    };

    enum class Kind
    {
        Return,
        Let,
        If,
        Else,
        For,
        While,
        IntLit,
        Identifier,
        OpenParen,
        CloseParen,
        OpenCurly,
        CloseCurly,
        OpenSquare,
        CloseSquare,
        Equals,
        Plus,
        Asterisk,
        Minus,
        ForwardSlash,
        BackSlash,
        LessThan,
        GreaterThan,
        Hash,
        SemiColon,
        Dot,
        Comma,
        Colon,
        SingleQuote,
        DoubleQuote,
        Pipe,
        Comment,
        Unexpected
    };

    explicit Token(Kind kind, std::optional<Info> info, std::optional<std::string> value);

    [[nodiscard]] Kind kind() const { return m_kind; }
    [[nodiscard]] std::optional<Info> info() const { return m_info; }
    [[nodiscard]] std::optional<std::string> value() const { return m_value; }

private:
    Kind m_kind;
    std::optional<Info> m_info;
    std::optional<std::string> m_value;
};

using TokenVector = std::vector<Token>;

inline Token makeToken(Token::Kind kind, std::optional<Token::Info> info, std::optional<std::string> value) {
    Token t(kind, info, std::move(value));
    return t;
}

inline Token makeToken(Token::Kind kind, std::optional<Token::Info> info) {
    Token t(kind, info, {});
    return t;
}

inline std::optional<uint8_t> binaryPrecedence(Token::Kind kind)
{
    switch (kind)
    {
        case Token::Kind::Minus:
        case Token::Kind::Plus:
            return 0;
        case Token::Kind::Asterisk:
        case Token::Kind::ForwardSlash:
            return 1;
        default:
            return {};
    }
}