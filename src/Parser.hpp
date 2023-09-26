#pragma once

#include <variant>
#include "Tokeniser.hpp"
#include "ArenaAllocator.hpp"
#include "Nodes.hpp"

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens);

    Result<Node::Program> parse();

private:
    [[nodiscard]] std::optional<Token> peek(size_t offset = 0) const;
    Token consume();
    std::optional<Token> tryConsume(TokenType tType, const std::string& error);
    std::optional<Token> tryConsume(TokenType tType);

    void addError(const std::string& message);

    std::optional<Node::Term*> parseTerm();
//    std::optional<Node::BinExpr*> parseBinExpr();
    std::optional<Node::Expr*> parseExpr(int minPrecedence = 0);
    std::optional<Node::Statement*> parseStatement();

private:
    std::vector<std::string> m_errors;
    const std::vector<Token> m_tokens;
    size_t m_tokenPos = 0;
    ArenaAllocator m_allocator;
};
