#pragma once

#include "Token.hpp"
#include "CompilerError.hpp"

#include <string>
#include <optional>
#include <vector>
#include <map>

class Tokeniser
{
public:
    explicit Tokeniser(std::string source, std::string filename, ErrorHandler& errorHandler);

    [[nodiscard]] TokenVector tokenise();

private:
    [[nodiscard]] std::optional<char> peek(size_t offset = 0) const;
    char consume();
    [[nodiscard]] Token::Info getCurrentTokenInfo() const;
    bool isNewline() const;

private:
    const std::string m_src;
    const std::string m_filename;
    size_t m_srcPos = 0;
    size_t m_posInLine = 0;
    size_t m_lineNo = 0;
    ErrorHandler& m_errorHandler;

    static std::map<char, Token::Kind> SymbolTokenMap;
    static std::map<std::string, Token::Kind> KeywordTokenMap;
};
