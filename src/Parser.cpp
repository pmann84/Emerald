#include <cassert>
#include "Parser.hpp"
#include "Nodes.hpp"

Parser::Parser(std::vector<Token> tokens, ErrorHandler& errorHandler) :
    m_tokens(std::move(tokens)),
    m_allocator(1024 * 1024 * 4),
    m_errorHandler(errorHandler)
{
}

std::optional<Token> Parser::peek(int64_t offset) const
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
    if (peek().has_value() && peek().value().Type == tType)
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
        addError(peek(-1).value().Info.value(), error);
    }
    return token;
}

Node::Program Parser::parse()
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
            addError(peek().value().Info.value(), "Invalid statement.");
        }
    }

    m_tokenPos = 0;
    return program;
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
            precedence = binaryPrecedence(currentToken->Type);
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
            addError(peek().value().Info.value(), "Unable to parse expression");
        }
        else
        {
            auto expr = m_allocator.alloc<Node::BinExpr>();
            auto nodeLhsExpr = m_allocator.alloc<Node::Expr>();
            if (op.Type == TokenType::Plus)
            {
                auto addNode = m_allocator.alloc<Node::BinExprAdd>();
                nodeLhsExpr->expr = exprLhs->expr;
                addNode->lhs = nodeLhsExpr;
                addNode->rhs = exprRhs.value();
                expr->expr = addNode;
            }
            else if (op.Type == TokenType::Asterisk)
            {
                auto multNode = m_allocator.alloc<Node::BinExprMult>();
                nodeLhsExpr->expr = exprLhs->expr;
                multNode->lhs = nodeLhsExpr;
                multNode->rhs = exprRhs.value();
                expr->expr = multNode;
            }
            else if (op.Type == TokenType::Minus)
            {
                auto minusNode = m_allocator.alloc<Node::BinExprMinus>();
                nodeLhsExpr->expr = exprLhs->expr;
                minusNode->lhs = nodeLhsExpr;
                minusNode->rhs = exprRhs.value();
                expr->expr = minusNode;
            }
            else if (op.Type == TokenType::ForwardSlash)
            {
                auto divNode = m_allocator.alloc<Node::BinExprDiv>();
                nodeLhsExpr->expr = exprLhs->expr;
                divNode->lhs = nodeLhsExpr;
                divNode->rhs = exprRhs.value();
                expr->expr = divNode;
            }
            else
            {
                assert(false);
            }
            exprLhs->expr = expr;
        }
    }
    return exprLhs;
}

std::optional<Node::Statement*> Parser::parseStatement()
{
    auto stmt = m_allocator.alloc<Node::Statement>();
    if (tryConsume(TokenType::Return)) {
        auto nodeReturn = m_allocator.alloc<Node::StatementReturn>();
        if (auto nodeExpr = parseExpr()) {
            nodeReturn->returnExpr = nodeExpr.value();
        } else {
            addError(peek().value().Info.value(), "Invalid expression after return.");
        }

        tryConsume(TokenType::ExprEnd, "Expected ; after expression.");
        stmt->statement = nodeReturn;
        return stmt;
    } else if (tryConsume(TokenType::Hash)) {
        // TODO: Comment so consume until the end of the line
    } else if (
            peek().value().Type == TokenType::Let &&
            peek(1).has_value() && peek(1).value().Type == TokenType::Identifier &&
            peek(2).has_value() && peek(2).value().Type == TokenType::Equals
            ) {
        consume();
        auto letStatement = m_allocator.alloc<Node::StatementLet>();
        letStatement->identifier = consume();
        consume();
        if (auto expr = parseExpr()) {
            letStatement->letExpr = expr.value();
        } else {
            addError(peek().value().Info.value(), "Invalid expression in variable definition.");
        }

        tryConsume(TokenType::ExprEnd, "Expected ; after expression.");
        stmt->statement = letStatement;
        return stmt;
    }
    else if (peek().has_value() && peek().value().Type == TokenType::OpenCurly) {
        if (auto scope = parseScope()) {
            stmt->statement = scope.value();
            return stmt;
        }
        else
        {
            addError(peek().value().Info.value(), "Invalid scope.");
        }
    }
    else if (auto ifStatement = tryConsume(TokenType::If))
    {
        tryConsume(TokenType::OpenParen, "Expected ( after if");
        auto ifStmt = m_allocator.alloc<Node::StatementIf>();
        if (auto expr = parseExpr())
        {
            ifStmt->expr = expr.value();
        }
        tryConsume(TokenType::CloseParen, "Expected ) after if expression");
        if (auto scope = parseScope())
        {
            ifStmt->scope = scope.value();
        }
        else
        {
            addError(peek().value().Info.value(), "Invalid scope.");
        }
        stmt->statement = ifStmt;
        return stmt;
    }
    // Return nothing which indicates an invalid statement
    return {};
}

std::optional<Node::Term*> Parser::parseTerm()
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
    else if (auto openParen = tryConsume(TokenType::OpenParen))
    {
        auto expr = parseExpr();
        if (!expr.has_value())
        {
            addError(peek(-1).value().Info.value(), "Expected expression after open parenthesis.");
        }
        tryConsume(TokenType::CloseParen, "Expected close parenthesis.");
        auto termParen = m_allocator.alloc<Node::TermParen>();
        termParen->expr = expr.value();
        auto term = m_allocator.alloc<Node::Term>();
        term->expr = termParen;
        return term;
    }
    return {};
}

std::optional<Node::Scope *> Parser::parseScope()
{
    if (!tryConsume(TokenType::OpenCurly, "Expected }"))
    {
        return {};
    }
    auto scope = m_allocator.alloc<Node::Scope>();
    while (auto stmt = parseStatement()) {
        scope->statements.push_back(stmt.value());
    }
    if (!tryConsume(TokenType::CloseCurly, "Expected }"))
    {
        return {};
    }
    return scope;
}

void Parser::addError(TokenInfo info, std::string message)
{
    consume();
    Error error = { .Info = info, .Message = std::move(message) };
    m_errorHandler << error;
}
