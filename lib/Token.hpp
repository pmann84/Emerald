#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

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
        Exclamation,
        Unexpected
    };

    explicit Token(Kind kind, std::optional<Info> info, std::optional<std::string> value);

    [[nodiscard]] Kind kind() const { return m_kind; }
    [[nodiscard]] std::optional<Info> info() const { return m_info; }
    [[nodiscard]] std::optional<std::string> value() const { return m_value; }

    [[nodiscard]] inline bool is_binary_operator() const {
        return kind() == Kind::Plus
            || kind() == Kind::Minus
            || kind() == Kind::Asterisk
            || kind() == Kind::ForwardSlash;
    }

    [[nodiscard]] inline bool is_relational_operator() const {
        return kind() == Kind::GreaterThan
            || kind() == Kind::LessThan;
    }

    [[nodiscard]] inline bool is_equality_operator() const {
        return kind() == Kind::Equals
            || kind() == Kind::Exclamation;
    }

    [[nodiscard]] inline std::optional<uint8_t> binary_precedence() const
    {
        switch (kind())
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

