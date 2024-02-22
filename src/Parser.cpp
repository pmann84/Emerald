#include <cassert>
#include "rules.hpp"
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

Token Parser::peek_value(int64_t offset) const
{
    return peek(offset).value();
}

bool Parser::peek_has_value(int64_t offset) const
{
    return peek(offset).has_value();
}

bool Parser::peek_for_kind(Token::Kind tType, int64_t offset) const
{
    return peek_has_value(offset) && peek_value(offset).kind() == tType;
}

bool Parser::peek_for_rule(std::vector<Token::Kind> rule) const
{
    bool peek_successful = true;
    for (auto i = 0; i < rule.size(); ++i){
        const auto tKind = rule.at(i);
        peek_successful &= peek_for_kind(tKind, i);
    }
    return peek_successful;
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
    if (peek_has_value() && peek_value().kind() == tType)
    {
        return consume();
    }
    return {};
}

std::optional<Token> Parser::tryConsume(const std::vector<Token::Kind>& tTypes, const std::string& error) {
    std::optional<Token> token;
    for(const auto t : tTypes) {
        token = tryConsume(t);
    }
    if (!token.has_value())
    {
        if (peek_has_value(-1)) {
            add_error(peek_value(-1).info().value(), error);
        }
    }
    return token;
}

std::optional<Token> Parser::tryConsume(Token::Kind tType, const std::string& error)
{
    auto token = tryConsume(tType);
    if (!token.has_value())
    {
        if (peek_has_value(-1)) {
            add_error(peek_value(-1).info().value(), error);
        }
    }
    return token;
}

Node::Program Parser::parse()
{
    Node::Program program;
    while (peek_has_value())
    {
        if (!tryConsume(Token::Kind::Comment)) {
            if (auto statement = parseStatement()) {
                program.statements.push_back(statement.value());
            } else {
                add_error(peek_value().info().value(), "Invalid statement.");
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
            add_error(peek_value().info().value(), "Unable to parse expression");
        }
        else
        {
            auto expr = m_allocator.alloc<Node::BinExpr>();
            auto nodeLhsExpr = m_allocator.alloc<Node::Expr>();
            if (op.kind() == Token::Kind::Plus)
            {
                auto addNode = m_allocator.alloc<Node::BinaryExpr::Add>();
                nodeLhsExpr->expr = exprLhs->expr;
                addNode->lhs = nodeLhsExpr;
                addNode->rhs = exprRhs.value();
                *expr = addNode;
            }
            else if (op.kind() == Token::Kind::Asterisk)
            {
                auto multNode = m_allocator.alloc<Node::BinaryExpr::Multiply>();
                nodeLhsExpr->expr = exprLhs->expr;
                multNode->lhs = nodeLhsExpr;
                multNode->rhs = exprRhs.value();
                *expr = multNode;
            }
            else if (op.kind() == Token::Kind::Minus)
            {
                auto minusNode = m_allocator.alloc<Node::BinaryExpr::Minus>();
                nodeLhsExpr->expr = exprLhs->expr;
                minusNode->lhs = nodeLhsExpr;
                minusNode->rhs = exprRhs.value();
                *expr = minusNode;
            }
            else if (op.kind() == Token::Kind::ForwardSlash)
            {
                auto divNode = m_allocator.alloc<Node::BinaryExpr::Divide>();
                nodeLhsExpr->expr = exprLhs->expr;
                divNode->lhs = nodeLhsExpr;
                divNode->rhs = exprRhs.value();
                *expr = divNode;
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

std::optional<Node::Stmt*> Parser::parseStatement()
{
    auto stmt = m_allocator.alloc<Node::Stmt>();
    if (tryConsume(Token::Kind::Return)) {
        auto nodeReturn = m_allocator.alloc<Node::Statement::Return>();
        if (auto nodeExpr = parseExpr()) {
            nodeReturn->returnExpr = nodeExpr.value();
        } else {
            add_error(peek_value().info().value(), "Invalid expression after return.");
        }

        tryConsume(Token::Kind::SemiColon, "Expected ; after expression.");
        *stmt = nodeReturn;
        return stmt;
    } else if (peek_for_rule(emerald_grammar_rules.at("variable_assign"))) {
        consume();
        auto letStatement = m_allocator.alloc<Node::Statement::Let>();
        letStatement->identifier = consume();
        consume(); // consume equals
        if (auto expr = parseExpr()) {
            letStatement->letExpr = expr.value();
        } else {
            add_error(peek_value().info().value(), "Invalid expression in variable definition.");
        }

        tryConsume(Token::Kind::SemiColon, "Expected ; after expression.");
        *stmt = letStatement;
        return stmt;
    } else if(peek_for_rule(emerald_grammar_rules.at("variable_reassign"))) {
        auto assignStatement = m_allocator.alloc<Node::Statement::Assign>();
        assignStatement->identifier = consume();
        consume(); // consume equals
        if (auto expr = parseExpr()) {
            assignStatement->assignExpr = expr.value();
        } else {
            add_error(peek_value().info().value(), "Invalid expression in variable assignment.");
        }

        tryConsume(Token::Kind::SemiColon, "Expected ; after expression.");
        *stmt = assignStatement;
        return stmt;
    } else if (peek_for_kind(Token::Kind::OpenCurly)) {
        if (auto scope = parseScope()) {
            *stmt = scope.value();
            return stmt;
        }
        else
        {
            add_error(peek_value().info().value(), "Invalid scope.");
        }
    }
    else if (auto ifStatement = tryConsume(Token::Kind::If))
    {
        tryConsume(Token::Kind::OpenParen, "Expected ( after if");
        auto ifStmt = m_allocator.alloc<Node::Statement::If>();
        if (auto expr = parseExpr())
        {
            ifStmt->expr = expr.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value().info().value(), "Invalid scope in if block.");
            }
        }
        tryConsume(Token::Kind::CloseParen, "Expected ) after if expression");
        if (auto scope = parseScope())
        {
            ifStmt->scope = scope.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value().info().value(), "Invalid scope.");
            }
        }
        ifStmt->pred = parseIfPredicate();

        *stmt = ifStmt;
        return stmt;
    } else if (auto whileStatement = tryConsume(Token::Kind::While)) {
        tryConsume(Token::Kind::OpenParen, "Expected ( after while declaration");
        auto whileStmt = m_allocator.alloc<Node::Statement::While>();
        if (auto expr = parseExpr())
        {
            whileStmt->expr = expr.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value().info().value(), "Invalid expression in while block.");
            }
        }
        tryConsume(Token::Kind::CloseParen, "Expected ) after while expression");
        if (auto scope = parseScope())
        {
            whileStmt->scope = scope.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value().info().value(), "Invalid scope.");
            }
        }

        *stmt = whileStmt;
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
        *term = expr;
        return term;
    }
    else if (auto ident = tryConsume(Token::Kind::Identifier))
    {
        auto expr = m_allocator.alloc<Node::Identifier>();
        expr->identifier = ident.value();
        auto term = m_allocator.alloc<Node::Term>();
        *term = expr;
        return term;
    }
    else if (auto openParen = tryConsume(Token::Kind::OpenParen))
    {
        auto expr = parseExpr();
        if (!expr.has_value())
        {
            add_error(peek_value(-1).info().value(), "Expected expression after open parenthesis.");
        }
        tryConsume(Token::Kind::CloseParen, "Expected close parenthesis.");
        auto termParen = m_allocator.alloc<Node::TermParen>();
        termParen->expr = expr.value();
        auto term = m_allocator.alloc<Node::Term>();
        *term = termParen;
        return term;
    }
    return {};
}

std::optional<Node::Scope *> Parser::parseScope()
{
    if (!tryConsume(Token::Kind::OpenCurly, "Expected { "))
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

void Parser::add_error(Token::Info info, std::string message)
{
    consume();
    const auto error = make_error(info, message);
    m_errorHandler << error;
}

std::optional<Node::IfPredicate*> Parser::parseIfPredicate() {

    if (tryConsume(Token::Kind::Else)) {
        auto ifPredicate = m_allocator.alloc<Node::IfPredicate>();
        if (tryConsume(Token::Kind::If)) {
            tryConsume(Token::Kind::OpenParen, "Expected ( after if");
            auto elseIfStmt = m_allocator.alloc<Node::Statement::ElseIf>();
            if (auto expr = parseExpr())
            {
                elseIfStmt->expr = expr.value();
            }
            tryConsume(Token::Kind::CloseParen, "Expected ) after if expression");
            if (auto scope = parseScope())
            {
                elseIfStmt->scope = scope.value();
            }
            else
            {
                if (peek_has_value()) {
                    add_error(peek_value().info().value(), "Invalid scope.");
                }
            }
            elseIfStmt->pred = parseIfPredicate();

            *ifPredicate = elseIfStmt;
        } else {
            auto elseStmt = m_allocator.alloc<Node::Statement::Else>();
            if (auto scope = parseScope())
            {
                elseStmt->scope = scope.value();
            }
            else
            {
                if (peek_has_value()) {
                    add_error(peek_value().info().value(), "Invalid scope.");
                }
            }
            *ifPredicate = elseStmt;
        }
        return ifPredicate;
    }
    return {};
}

