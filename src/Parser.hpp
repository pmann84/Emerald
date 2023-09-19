#pragma once

#include <variant>
#include "Tokeniser.hpp"

namespace Node {
    struct IntLiteral
    {
        Token intLit;
    };

    struct Identifier
    {
        Token identifier;
    };

    struct Expr
    {
        std::variant<IntLiteral, Identifier> expr;
    };

    struct StatementReturn
    {
        Expr returnExpr;
    };

    struct StatementLet
    {
        Token identifier;
        Expr letExpr;
    };

    struct Statement
    {
        std::variant<StatementReturn, StatementLet> statement;
    };

    struct Program
    {
        std::vector<Statement> statements;
    };
}

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens);

    Result<Node::Program> parse();

private:
    [[nodiscard]] std::optional<Token> peek(size_t offset = 0) const;
    Token consume();

    std::optional<Node::Expr> parseExpr();
    std::optional<Node::Statement> parseStatement();

private:
    std::vector<std::string> m_errors;
    const std::vector<Token> m_tokens;
    size_t m_tokenPos = 0;
};
