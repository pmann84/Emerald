#include "Parser.hpp"
#include "Nodes.hpp"

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 4)
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

std::optional<Token> Parser::tryConsume(TokenType tType)
{
    if (peek().has_value() && peek().value().token == tType)
    {
        return consume();
    }
    return {};
}

std::optional<Token> Parser::tryConsume(TokenType tType, const std::string& error)
{
    auto token = tryConsume(tType);
    if (!token.has_value())
    {
        addError(error);
    }
    return token;
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
            addError("Invalid statement!");
        }
    }

    m_tokenPos = 0;
    return { .result = program, .success = m_errors.empty(), .errors = m_errors };
}

std::optional<Node::Expr*> Parser::parseExpr(int minPrecedence)
{
    auto termLhs = parseTerm();
    if (!termLhs.has_value())
    {
        return {};
    }

    auto exprLhs = m_allocator.alloc<Node::Expr>();
    exprLhs->expr = termLhs.value();

    while(true)
    {
        std::optional<Token> currentToken = peek();
        std::optional<uint8_t> precedence;
        if (currentToken.has_value())
        {
            precedence = binaryPrecedence(currentToken->token);
            if (!precedence.has_value() || precedence.value() < minPrecedence) {
                break;
            }
        }
        else
        {
            break;
        }
        Token op = consume();
        auto nextMinPrecedence = precedence.value() + 1;
        auto exprRhs = parseExpr(nextMinPrecedence);
        if (!exprRhs.has_value())
        {
            addError("Unable to parse expression");
        }
        else
        {
            auto expr = m_allocator.alloc<Node::BinExpr>();
            auto nodeLhsExpr = m_allocator.alloc<Node::Expr>();
            if (op.token == TokenType::Plus)
            {
                auto addNode = m_allocator.alloc<Node::BinExprAdd>();
                nodeLhsExpr->expr = exprLhs->expr;
                addNode->lhs = nodeLhsExpr;
                addNode->rhs = exprRhs.value();
                expr->expr = addNode;
            }
            else if (op.token == TokenType::Asterisk)
            {
                auto multNode = m_allocator.alloc<Node::BinExprMult>();
                nodeLhsExpr->expr = exprLhs->expr;
                multNode->lhs = nodeLhsExpr;
                multNode->rhs = exprRhs.value();
                expr->expr = multNode;
            }
            exprLhs->expr = expr;
        }
    }
    return exprLhs;
}

std::optional<Node::Statement*> Parser::parseStatement()
{
    auto stmt = m_allocator.alloc<Node::Statement>();
    if (tryConsume(TokenType::Return))
    {
        auto nodeReturn = m_allocator.alloc<Node::StatementReturn>();
        if (auto nodeExpr = parseExpr())
        {
            nodeReturn->returnExpr = nodeExpr.value();
        }
        else
        {
            addError("Invalid expression after return.");
        }

        tryConsume(TokenType::ExprEnd, "Expected ; after expression.");
        stmt->statement = nodeReturn;
        return stmt;
    }
    else if (
            peek().value().token == TokenType::Let &&
            peek(1).has_value() && peek(1).value().token == TokenType::Identifier &&
            peek(2).has_value() && peek(2).value().token == TokenType::Equals
    )
    {
        consume();
        auto letStatement = m_allocator.alloc<Node::StatementLet>();
        letStatement->identifier = consume();
        consume();
        if (auto expr = parseExpr())
        {
            letStatement->letExpr = expr.value();
        }
        else
        {
            addError("Invalid expression in variable definition.");
        }

        tryConsume(TokenType::ExprEnd, "Expected ; after expression.");
        stmt->statement = letStatement;
        return stmt;
    }
    else
    {
        addError("Unable to find valid statement.");
    }
    return {};
}

std::optional<Node::Term *> Parser::parseTerm()
{
    if (auto intLit = tryConsume(TokenType::IntLit))
    {
        auto expr = m_allocator.alloc<Node::IntLiteral>();
        expr->intLit = intLit.value();
        auto term = m_allocator.alloc<Node::Term>();
        term->expr = expr;
        return term;
    }
    else if (auto ident = tryConsume(TokenType::Identifier))
    {
        auto expr = m_allocator.alloc<Node::Identifier>();
        expr->identifier = ident.value();
        auto term = m_allocator.alloc<Node::Term>();
        term->expr = expr;
        return term;
    }
    return {};
}

void Parser::addError(const std::string &message)
{
    consume();
    m_errors.emplace_back(message);
}
