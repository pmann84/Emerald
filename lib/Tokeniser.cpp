#include <sstream>
#include "Tokeniser.hpp"

std::map<char, Token::Kind> Tokeniser::SymbolTokenMap = {
    {';', Token::Kind::SemiColon},
    {'(', Token::Kind::OpenParen},
    {')', Token::Kind::CloseParen},
    {'=', Token::Kind::Equals},
    {'+', Token::Kind::Plus},
    {'*', Token::Kind::Asterisk},
    {'-', Token::Kind::Minus},
    {'/', Token::Kind::ForwardSlash},
    {'\\', Token::Kind::BackSlash},
    {'{', Token::Kind::OpenCurly},
    {'}', Token::Kind::CloseCurly},
    {'[', Token::Kind::OpenSquare},
    {']', Token::Kind::CloseSquare},
    {'<', Token::Kind::LessThan},
    {'>', Token::Kind::GreaterThan},
    {'.', Token::Kind::Dot},
    {',', Token::Kind::Comma},
    {':', Token::Kind::Colon},
    {'\'', Token::Kind::SingleQuote},
    {'\"', Token::Kind::DoubleQuote},
    {'|', Token::Kind::Pipe}
};

std::map<std::string, Token::Kind> Tokeniser::KeywordTokenMap = {
    {"return", Token::Kind::Return},
    {"let", Token::Kind::Let},
    {"if", Token::Kind::If},
    {"for", Token::Kind::For},
    {"else", Token::Kind::Else},
    {"while", Token::Kind::While}
};

Tokeniser::Tokeniser(std::string source, std::string filename, ErrorHandler& errorHandler)
    : m_src(std::move(source))
    , m_filename(std::move(filename))
    , m_errorHandler(errorHandler)
{
}

TokenVector Tokeniser::tokenise()
{
    std::vector<Token> tokens{};
    std::vector<std::string> errors{};
    std::stringstream buf;

    while(peek().has_value())
    {
        if (std::isspace(peek().value()))
        {
            // Ignore whitespace but calculate the correct position in the source code
            if (isNewline())
            {
                m_lineNo++;
                m_posInLine = 0;
                consumeNewline();
            } else {
                consume();
            }

            // Go to the next character
            continue;
        }


        if (std::isalpha(peek().value()))
        {
            const auto infoAtStart = getCurrentTokenInfo();
            buf << consume();
            while (peek().has_value() && std::isalnum(peek().value())) {
                buf << consume();
            }

            // Check if it's a key word
            if(KeywordTokenMap.contains(buf.str()))
            {
                auto keywordToken = KeywordTokenMap.at(buf.str());
                tokens.push_back(makeToken(keywordToken, infoAtStart));
                buf.str("");
            }
            else
            {
                tokens.push_back(makeToken(Token::Kind::Identifier, infoAtStart, buf.str()));
                buf.str("");
            }
        }
        else if (std::isdigit(peek().value()))
        {
            const Token::Info info = getCurrentTokenInfo();
            buf << consume();
            while (peek().has_value() && std::isdigit(peek().value())) {
                buf << consume();
            }
            tokens.push_back(makeToken(Token::Kind::IntLit, info, buf.str()));
            buf.str("");
        }
        else if (peek().value() == '#') {
            if (peek(1).value() == '*') {
                // Parse any multi-line comments
                const Token::Info info = getCurrentTokenInfo();
                buf << consume() << consume();
                // Parse until we hit another */
                while (peek().has_value() && peek().value() != '*' && peek(1).has_value() && peek(1).value() != '#') {
                    buf << consume();
                }
                buf << consume() << consume();
                auto comment = buf.str();
                comment = comment.erase(0, 2);
                comment = comment.erase(comment.size() - 2, 2);
                //tokens.push_back(makeToken(Token::Kind::Comment, info, comment));
                buf.str("");
            } else {
                // Parse any single line comments
                const Token::Info info = getCurrentTokenInfo();
                buf << consume();
                while (peek().has_value() && !isNewline()) {
                    buf << consume();
                }
                auto comment = buf.str();
                comment = comment.erase(0, 2);
                //tokens.push_back(makeToken(Token::Kind::Comment, info, comment));
                buf.str("");
            }
        }
        else if (SymbolTokenMap.contains(peek().value()))
        {
            // Single symbol
            auto symToken = SymbolTokenMap.at(peek().value());
            const Token::Info info = getCurrentTokenInfo();
            consume();
            tokens.push_back(makeToken(symToken, info));
        }
        else
        {
            tokens.push_back(makeToken(Token::Kind::Unexpected, getCurrentTokenInfo()));
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

void Tokeniser::consumeNewline()
{
    if (peek().value() == '\n') {
        consume();
    } else if (peek().value() == '\r' && peek(1).value() == '\n') {
        consume();
        consume();
    }
}

Token::Info Tokeniser::getCurrentTokenInfo() const
{
    Token::Info info = { .File = m_filename, .Line = m_lineNo, .Pos = m_posInLine };
    return info;
}

bool Tokeniser::isNewline() const
{
    return  peek().value() == '\n' || (peek().value() == '\r' && peek(1).value() == '\n');
}