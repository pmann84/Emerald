#pragma once

#include <variant>
#include "../lib/Tokeniser.hpp"
#include "ArenaAllocator.hpp"
#include "Nodes.hpp"

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens, ErrorHandler& errorHandler);

    Node::Program parse();

private:
    [[nodiscard]] std::optional<Token> peek(int64_t offset = 0) const;
    Token consume();
    std::optional<Token> tryConsume(const std::vector<Token::Kind>& tTypes, const std::string& error);
    std::optional<Token> tryConsume(Token::Kind tType, const std::string& error);
    std::optional<Token> tryConsume(Token::Kind tType);

    void addError(Token::Info info, std::string message);

    std::optional<Node::Term*> parseTerm();
    std::optional<Node::Expr*> parseExpr(int minPrecedence = 0);
    std::optional<Node::Stmt*> parseStatement();
    std::optional<Node::Scope*> parseScope();
    std::optional<Node::IfPredicate*> parseIfPredicate();

private:
    const std::vector<Token> m_tokens;
    size_t m_tokenPos = 0;
    ArenaAllocator m_allocator;
    ErrorHandler& m_errorHandler;
};
