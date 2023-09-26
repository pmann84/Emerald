#include <sstream>
#include "Tokeniser.hpp"

Tokeniser::Tokeniser(std::string source) : m_src(std::move(source))
{
}

Result<TokenVector> Tokeniser::tokenise()
{
    std::vector<Token> tokens{};
    std::vector<std::string> errors{};
    std::stringstream buf;
    while(peek().has_value())
    {
        if (std::isalpha(peek().value()))
        {
            buf << consume();
            while (peek().has_value() && std::isalnum(peek().value())) {
                buf << consume();
            }

            // Check if it's a key word
            if(KeywordTokenMap.contains(buf.str()))
            {
                auto keywordToken = KeywordTokenMap.at(buf.str());
                tokens.push_back({ .token = keywordToken });
                buf.str("");
            }
            else
            {
                tokens.push_back({ .token = TokenType::Identifier, .value = buf.str() });
                buf.str("");
            }
        }
        else if (std::isdigit(peek().value()))
        {
            buf << consume();
            while (peek().has_value() && std::isdigit(peek().value())) {
                buf << consume();
            }
            tokens.push_back({ .token = TokenType::IntLit, .value = buf.str() });
            buf.str("");
        }
        else if (std::isspace(peek().value()))
        {
            // Ignore whitespace
            consume();
        }
        else if (SymbolTokenMap.contains(peek().value()))
        {
            // Single symbol
            auto symToken = SymbolTokenMap.at(peek().value());
            consume();
            tokens.push_back({ .token = symToken });
        }
    }
    // Reset the position into the source in case tokenise is called twice
    m_srcPos = 0;
    return { .result = tokens, .success = errors.empty(), .errors = errors };
}

std::optional<char> Tokeniser::peek(size_t offset) const
{
    if (m_srcPos + offset >= m_src.size())
    {
        return {};
    }
    return m_src.at(m_srcPos + offset);
}

char Tokeniser::consume()
{
   return m_src.at(m_srcPos++);
}
