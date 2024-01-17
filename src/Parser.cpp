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
    if (m_tokenPos < m_tokens.size()) {
        return m_tokens.at(m_tokenPos++);
    }
    return Token(Token::Kind::Unexpected, {}, {});
}

std::optional<Token> Parser::tryConsume(Token::Kind tType)
{
    if (peek().has_value() && peek().value().kind() == tType)
    {
        return consume();
    }
    return {};
}

std::optional<Token> Parser::tryConsume(Token::Kind tType, const std::string& error)
{
    auto token = tryConsume(tType);
    if (!token.has_value())
    {
        if (peek(-1).has_value()) {
            addError(peek(-1).value().info().value(), error);
        }

    }
    return token;
}

Node::Program Parser::parse()
{
    Node::Program program;
    while (peek().has_value())
    {
        if (!tryConsume(Token::Kind::Comment)) {
            if (auto statement = parseStatement()) {
                program.statements.push_back(statement.value());
            } else {
                addError(peek().value().info().value(), "Invalid statement.");
            }
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
            precedence = binaryPrecedence(currentToken->kind());
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
            addError(peek().value().info().value(), "Unable to parse expression");
        }
        else
        {
            auto expr = m_allocator.alloc<Node::BinExpr>();
            auto nodeLhsExpr = m_allocator.alloc<Node::Expr>();
            if (op.kind() == Token::Kind::Plus)
            {
                auto addNode = m_allocator.alloc<Node::BinExprAdd>();
                nodeLhsExpr->expr = exprLhs->expr;
                addNode->lhs = nodeLhsExpr;
                addNode->rhs = exprRhs.value();
                expr->expr = addNode;
            }
            else if (op.kind() == Token::Kind::Asterisk)
            {
                auto multNode = m_allocator.alloc<Node::BinExprMult>();
                nodeLhsExpr->expr = exprLhs->expr;
                multNode->lhs = nodeLhsExpr;
                multNode->rhs = exprRhs.value();
                expr->expr = multNode;
            }
            else if (op.kind() == Token::Kind::Minus)
            {
                auto minusNode = m_allocator.alloc<Node::BinExprMinus>();
                nodeLhsExpr->expr = exprLhs->expr;
                minusNode->lhs = nodeLhsExpr;
                minusNode->rhs = exprRhs.value();
                expr->expr = minusNode;
            }
            else if (op.kind() == Token::Kind::ForwardSlash)
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
    if (tryConsume(Token::Kind::Return)) {
        auto nodeReturn = m_allocator.alloc<Node::StatementReturn>();
        if (auto nodeExpr = parseExpr()) {
            nodeReturn->returnExpr = nodeExpr.value();
        } else {
            addError(peek().value().info().value(), "Invalid expression after return.");
        }

        tryConsume(Token::Kind::SemiColon, "Expected ; after expression.");
        stmt->statement = nodeReturn;
        return stmt;
    } else if (
            peek().has_value() && peek().value().kind() == Token::Kind::Let &&
            peek(1).has_value() && peek(1).value().kind() == Token::Kind::Identifier &&
            peek(2).has_value() && peek(2).value().kind() == Token::Kind::Equals
    ) {
        consume();
        auto letStatement = m_allocator.alloc<Node::StatementLet>();
        letStatement->identifier = consume();
        consume(); // consume equals
        if (auto expr = parseExpr()) {
            letStatement->letExpr = expr.value();
        } else {
            addError(peek().value().info().value(), "Invalid expression in variable definition.");
        }

        tryConsume(Token::Kind::SemiColon, "Expected ; after expression.");
        stmt->statement = letStatement;
        return stmt;
    } else if(
            peek().has_value() && peek().value().kind() == Token::Kind::Identifier &&
            peek(1).has_value() && peek(1).value().kind() == Token::Kind::Equals
    ) {
        auto assignStatement = m_allocator.alloc<Node::StatementAssign>();
        assignStatement->identifier = consume();
        consume(); // consume equals
        if (auto expr = parseExpr()) {
            assignStatement->assignExpr = expr.value();
        } else {
            addError(peek().value().info().value(), "Invalid expression in variable assignment.");
        }

        tryConsume(Token::Kind::SemiColon, "Expected ; after expression.");
        stmt->statement = assignStatement;
        return stmt;
    } else if (peek().has_value() && peek().value().kind() == Token::Kind::OpenCurly) {
        if (auto scope = parseScope()) {
            stmt->statement = scope.value();
            return stmt;
        }
        else
        {
            addError(peek().value().info().value(), "Invalid scope.");
        }
    }
    else if (auto ifStatement = tryConsume(Token::Kind::If))
    {
        tryConsume(Token::Kind::OpenParen, "Expected ( after if");
        auto ifStmt = m_allocator.alloc<Node::StatementIf>();
        if (auto expr = parseExpr())
        {
            ifStmt->expr = expr.value();
        }
        tryConsume(Token::Kind::CloseParen, "Expected ) after if expression");
        if (auto scope = parseScope())
        {
            ifStmt->scope = scope.value();
        }
        else
        {
            if (peek().has_value()) {
                addError(peek().value().info().value(), "Invalid scope.");
            }
        }
        stmt->statement = ifStmt;
        return stmt;
    }
    // Return nothing which indicates an invalid statement
    return {};
}

std::optional<Node::Term*> Parser::parseTerm()
{
    if (auto intLit = tryConsume(Token::Kind::IntLit))
    {
        auto expr = m_allocator.alloc<Node::IntLiteral>();
        expr->intLit = intLit.value();
        auto term = m_allocator.alloc<Node::Term>();
        term->expr = expr;
        return term;
    }
    else if (auto ident = tryConsume(Token::Kind::Identifier))
    {
        auto expr = m_allocator.alloc<Node::Identifier>();
        expr->identifier = ident.value();
        auto term = m_allocator.alloc<Node::Term>();
        term->expr = expr;
        return term;
    }
    else if (auto openParen = tryConsume(Token::Kind::OpenParen))
    {
        auto expr = parseExpr();
        if (!expr.has_value())
        {
            addError(peek(-1).value().info().value(), "Expected expression after open parenthesis.");
        }
        tryConsume(Token::Kind::CloseParen, "Expected close parenthesis.");
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
    if (!tryConsume(Token::Kind::OpenCurly, "Expected }"))
    {
        return {};
    }
    auto scope = m_allocator.alloc<Node::Scope>();
    while (auto stmt = parseStatement()) {
        scope->statements.push_back(stmt.value());
    }
    if (!tryConsume(Token::Kind::CloseCurly, "Expected }"))
    {
        return {};
    }
    return scope;
}

void Parser::addError(Token::Info info, std::string message)
{
    consume();
    const auto error = makeError(info, message);
    m_errorHandler << error;
}
