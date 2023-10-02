#include "Token.hpp"

Token::Token(Token::Kind kind, std::optional<Info> info, std::optional<std::string> value)
    : m_kind(kind)
    , m_info(info)
    , m_value(std::move(value))
{

}
