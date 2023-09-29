#include <sstream>
#include "Tokeniser.hpp"

Tokeniser::Tokeniser(std::string source, ErrorHandler& errorHandler) : m_src(std::move(source)), m_errorHandler(errorHandler)
{
}

TokenVector Tokeniser::tokenise()
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
                tokens.push_back({ .Type = keywordToken, .Info = getCurrentTokenInfo() });
                buf.str("");
            }
            else
            {
                tokens.push_back({ .Type = TokenType::Identifier, .Info = getCurrentTokenInfo(), .Value = buf.str() });
                buf.str("");
            }
        }
        else if (std::isdigit(peek().value()))
        {
            const TokenInfo info = getCurrentTokenInfo();
            buf << consume();
            while (peek().has_value() && std::isdigit(peek().value())) {
                buf << consume();
            }
            tokens.push_back({ .Type = TokenType::IntLit, .Info = info, .Value = buf.str() });
            buf.str("");
        }
        else if (std::isspace(peek().value()))
        {
            // Ignore whitespace
            if (peek().value() == '\n' || peek().value() == '\r')
            {
                m_lineNo++;
                m_posInLine = 0;
            }
            consume();
        }
        else if (SymbolTokenMap.contains(peek().value()))
        {
            // Single symbol
            auto symToken = SymbolTokenMap.at(peek().value());
            const TokenInfo info = getCurrentTokenInfo();
            consume();
            tokens.push_back({ .Type = symToken, .Info = info });
        }
    }
    // Reset the position into the source in case tokenise is called twice
    m_srcPos = 0;
    return tokens;
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
    m_posInLine++;
   return m_src.at(m_srcPos++);
}

TokenInfo Tokeniser::getCurrentTokenInfo() const
{
    TokenInfo info = { .File = "", .Line = m_lineNo, .Pos = m_posInLine - 1 };
    return info;
}
