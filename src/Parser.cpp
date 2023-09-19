#include "Parser.hpp"

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens))
{
}

std::optional<Token> Parser::peek(size_t offset) const
{
    if (m_tokenPos + offset >= m_tokens.size())
    {
        return {};
    }
    return m_tokens.at(m_tokenPos + offset);
}

Token Parser::consume()
{
    return m_tokens.at(m_tokenPos++);
}

Result<Node::Program> Parser::parse()
{
    Node::Program program;
    while (peek().has_value())
    {
        if (auto statement = parseStatement())
        {
            program.statements.push_back(statement.value());
        }
        else
        {
            m_errors.emplace_back("Invalid statement!");
        }
    }

    m_tokenPos = 0;
    return { .result = program, .success = m_errors.empty(), .errors = m_errors };
}

std::optional<Node::Expr> Parser::parseExpr()
{
    if (peek().has_value() && peek().value().token == TokenType::int_lit)
    {
        return Node::Expr{ .expr = Node::IntLiteral{ .intLit = consume() } };
    }
    else if (peek().has_value() && peek().value().token == TokenType::identifier)
    {
        return Node::Expr{ .expr = Node::Identifier{ .identifier = consume() } };
    }
    else
    {
        m_errors.emplace_back("Invalid expression - must be an integer literal.");
    }
    return {};
}

std::optional<Node::Statement> Parser::parseStatement()
{
    if (peek().value().token == TokenType::return_)
    {
        consume();
        Node::StatementReturn returnStatement;
        if (auto nodeExpr = parseExpr())
        {
            returnStatement = Node::StatementReturn{ .returnExpr = nodeExpr.value() };
        }
        else
        {
            m_errors.emplace_back("Invalid expression after return.");
        }

        if (peek().has_value() && peek().value().token == TokenType::expr_end) {
            consume();
        }
        else
        {
            m_errors.emplace_back("Expected semi-colon after expression.");
        }
        return Node::Statement { .statement = returnStatement };
    }
    else if (
            peek().value().token == TokenType::let &&
            peek(1).has_value() && peek(1).value().token == TokenType::identifier &&
            peek(2).has_value() && peek(2).value().token == TokenType::equals
    )
    {
        consume();
        auto letStatement = Node::StatementLet{ .identifier = consume() };
        consume();
        if (auto expr = parseExpr())
        {
            letStatement.letExpr = expr.value();
        }
        else
        {
            m_errors.emplace_back("Invalid expression in variable definition.");
        }

        if (peek().has_value() && peek().value().token == TokenType::expr_end)
        {
            consume();
        }
        else
        {
            m_errors.emplace_back("Expected semi-colon after expression.");
        }
        return Node::Statement { .statement = letStatement };
    }
    else
    {
        m_errors.emplace_back("Unable to find valid return statement.");
    }
    return {};
}
