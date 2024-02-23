#include <cassert>
#include "rules.hpp"
#include "Parser.hpp"
#include "Nodes.hpp"

Parser::Parser(std::vector<Token> tokens, ErrorHandler& error_handler) :
        m_tokens(std::move(tokens)),
        m_allocator(1024 * 1024 * 4),
        m_error_handler(error_handler)
{
}

std::optional<Token> Parser::peek(int64_t offset) const
{
    if (m_token_pos + offset >= m_tokens.size())
    {
        return {};
    }
    return m_tokens.at(m_token_pos + offset);
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
    if (m_token_pos < m_tokens.size()) {
        return m_tokens.at(m_token_pos++);
    }
    return Token(Token::Kind::Unexpected, {}, {});
}

std::optional<Token> Parser::try_consume(Token::Kind tType)
{
    if (peek_has_value() && peek_value().kind() == tType)
    {
        return consume();
    }
    return {};
}

std::optional<Token> Parser::try_consume(const std::vector<Token::Kind>& tTypes, const std::string& error) {
    std::optional<Token> token;
    for(const auto t : tTypes) {
        token = try_consume(t);
    }
    if (!token.has_value())
    {
        if (peek_has_value(-1)) {
            add_error(peek_value(-1), error);
        }
    }
    return token;
}

std::optional<Token> Parser::try_consume(Token::Kind tType, const std::string& error)
{
    auto token = try_consume(tType);
    if (!token.has_value())
    {
        if (peek_has_value(-1)) {
            add_error(peek_value(-1), error);
        }
    }
    return token;
}

Node::Program Parser::parse()
{
    Node::Program program;
    while (peek_has_value())
    {
        if (!try_consume(Token::Kind::Comment)) {
            if (auto statement = parse_statement()) {
                program.statements.push_back(statement.value());
            } else {
                add_error(peek_value(), "Invalid statement.");
            }
        }
    }

    m_token_pos = 0;
    return program;
}

std::optional<Node::Expr*> Parser::parse_expr(int min_precedence)
{
    auto termLhs = parse_term();
    if (!termLhs.has_value())
    {
        return {};
    }

    auto exprLhs = m_allocator.alloc<Node::Expr>();
    exprLhs->expr = termLhs.value();

    // TODO: This function needs logic adding to handle equality and relational operators
//    if (currentToken.value().is_relational_operator()) {
//        Token op = consume();
//        auto expr = m_allocator.alloc<Node::RelExpr>();
//        auto nodeLhsExpr = m_allocator.alloc<Node::Expr>();
//        const auto nextToken = peek();
//        switch (op.kind()) {
//            case Token::Kind::LessThan:
//                if (nextToken.has_value() && nextToken.value().kind() == Token::Kind::Equals) {
//                    consume();
//                    // TODO: <=
//                    auto leqNode = m_allocator.alloc<Node::RelationalExpr::LessThanEqual>();
//                    auto exprRhs = parse_expr();
//                    if (!exprRhs.has_value())
//                    {
//                        add_error(peek_value(), "Unable to parse expression");
//                    }
//                    nodeLhsExpr->expr = exprLhs->expr;
//                    leqNode->lhs = nodeLhsExpr;
//                    leqNode->rhs = exprRhs.value();
//                    *expr = leqNode;
//                } else {
//                    // TODO: <
//                    auto ltNode = m_allocator.alloc<Node::RelationalExpr::LessThan>();
//                    auto exprRhs = parse_expr();
//                    if (!exprRhs.has_value())
//                    {
//                        add_error(peek_value(), "Unable to parse expression");
//                    }
//                    nodeLhsExpr->expr = exprLhs->expr;
//                    ltNode->lhs = nodeLhsExpr;
//                    ltNode->rhs = exprRhs.value();
//                    *expr = ltNode;
//                }
//                break;
//            case Token::Kind::GreaterThan:
//                if (nextToken.has_value() && nextToken.value().kind() == Token::Kind::Equals) {
//                    consume();
//                    // TODO: >=
//                    auto geqNode = m_allocator.alloc<Node::RelationalExpr::GreaterThanEqual>();
//                    auto exprRhs = parse_expr();
//                    if (!exprRhs.has_value())
//                    {
//                        add_error(peek_value(), "Unable to parse expression");
//                    }
//                    nodeLhsExpr->expr = exprLhs->expr;
//                    geqNode->lhs = nodeLhsExpr;
//                    geqNode->rhs = exprRhs.value();
//                    *expr = geqNode;
//                } else {
//                    // TODO: >
//                    auto gtNode = m_allocator.alloc<Node::RelationalExpr::GreaterThan>();
//                    auto exprRhs = parse_expr();
//                    if (!exprRhs.has_value())
//                    {
//                        add_error(peek_value(), "Unable to parse expression");
//                    }
//                    nodeLhsExpr->expr = exprLhs->expr;
//                    gtNode->lhs = nodeLhsExpr;
//                    gtNode->rhs = exprRhs.value();
//                    *expr = gtNode;
//                }
//                break;
//            default:
//                add_error(op, "Invalid relational operator ", op.value().value());
//        }
//        exprLhs->expr = expr;
//    } else if (currentToken.value().is_equality_operator()) {
//        auto expr = m_allocator.alloc<Node::EqlExpr>();
//        auto nodeLhsExpr = m_allocator.alloc<Node::Expr>();
//        Token op = consume();
//        const auto nextToken = peek();
//        switch (op.kind())
//        {
//            case Token::Kind::Exclamation:
//                if (nextToken.has_value() && nextToken.value().kind() == Token::Kind::Equals)
//                {
//                    consume();
//                    // TODO: !=
//                    auto neqNode = m_allocator.alloc<Node::EqualityExpr::NotEqual>();
//                    auto exprRhs = parse_expr();
//                    if (!exprRhs.has_value())
//                    {
//                        add_error(peek_value(), "Unable to parse expression");
//                    }
//                    nodeLhsExpr->expr = exprLhs->expr;
//                    neqNode->lhs = nodeLhsExpr;
//                    neqNode->rhs = exprRhs.value();
//                    *expr = neqNode;
//                } else
//                {
//                    add_error(op, "Invalid equality operator ", op.value().value(),
//                              nextToken.value().value().value());
//                }
//                break;
//            case Token::Kind::Equals:
//                if (nextToken.has_value() && nextToken.value().kind() == Token::Kind::Equals)
//                {
//                    consume();
//                    // TODO: ==
//                    auto eqNode = m_allocator.alloc<Node::EqualityExpr::Equal>();
//                    auto exprRhs = parse_expr();
//                    if (!exprRhs.has_value())
//                    {
//                        add_error(peek_value(), "Unable to parse expression");
//                    }
//                    nodeLhsExpr->expr = exprLhs->expr;
//                    eqNode->lhs = nodeLhsExpr;
//                    eqNode->rhs = exprRhs.value();
//                    *expr = eqNode;
//                } else
//                {
//                    add_error(op, "Invalid equality operator ", op.value().value(),
//                              nextToken.value().value().value());
//                }
//                break;
//            default:
//                add_error(op, "Invalid equality operator ", op.value().value());
//        }
//        exprLhs->expr = expr;
//    }
    while(true)
    {
        std::optional<Token> currentToken = peek();
        // Empty token - break out
        if (!currentToken.has_value()) break;

        // Check for the operators binary precedence
        std::optional<uint8_t> precedence = binaryPrecedence(currentToken->kind());
        if (!precedence.has_value() || precedence.value() < min_precedence) {
            break;
        } else {
            Token op = consume();
            auto nextMinPrecedence = precedence.value() + 1;
            auto exprRhs = parse_expr(nextMinPrecedence);
            if (!exprRhs.has_value())
            {
                add_error(peek_value(), "Unable to parse expression");
            } else
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
                } else if (op.kind() == Token::Kind::Asterisk)
                {
                    auto multNode = m_allocator.alloc<Node::BinaryExpr::Multiply>();
                    nodeLhsExpr->expr = exprLhs->expr;
                    multNode->lhs = nodeLhsExpr;
                    multNode->rhs = exprRhs.value();
                    *expr = multNode;
                } else if (op.kind() == Token::Kind::Minus)
                {
                    auto minusNode = m_allocator.alloc<Node::BinaryExpr::Minus>();
                    nodeLhsExpr->expr = exprLhs->expr;
                    minusNode->lhs = nodeLhsExpr;
                    minusNode->rhs = exprRhs.value();
                    *expr = minusNode;
                } else if (op.kind() == Token::Kind::ForwardSlash)
                {
                    auto divNode = m_allocator.alloc<Node::BinaryExpr::Divide>();
                    nodeLhsExpr->expr = exprLhs->expr;
                    divNode->lhs = nodeLhsExpr;
                    divNode->rhs = exprRhs.value();
                    *expr = divNode;
                } else
                {
                    add_error(op, "Unrecognised binary operator ", op.value().value());
                }
                exprLhs->expr = expr;
            }
        }
    }
    return exprLhs;
}

std::optional<Node::Stmt*> Parser::parse_statement()
{
    auto stmt = m_allocator.alloc<Node::Stmt>();
    if (try_consume(Token::Kind::Return)) {
        auto nodeReturn = m_allocator.alloc<Node::Statement::Return>();
        if (auto nodeExpr = parse_expr()) {
            nodeReturn->returnExpr = nodeExpr.value();
        } else {
            add_error(peek_value(), "Invalid expression after return.");
        }

        try_consume(Token::Kind::SemiColon, "Expected ; after expression.");
        *stmt = nodeReturn;
        return stmt;
    } else if (peek_for_rule(emerald_grammar_rules.at("variable_assign"))) {
        consume();
        auto letStatement = m_allocator.alloc<Node::Statement::Let>();
        letStatement->identifier = consume();
        consume(); // consume equals
        if (auto expr = parse_expr()) {
            letStatement->letExpr = expr.value();
        } else {
            add_error(peek_value(), "Invalid expression in variable definition.");
        }

        try_consume(Token::Kind::SemiColon, "Expected ; after expression.");
        *stmt = letStatement;
        return stmt;
    } else if(peek_for_rule(emerald_grammar_rules.at("variable_reassign"))) {
        auto assignStatement = m_allocator.alloc<Node::Statement::Assign>();
        assignStatement->identifier = consume();
        consume(); // consume equals
        if (auto expr = parse_expr()) {
            assignStatement->assignExpr = expr.value();
        } else {
            add_error(peek_value(), "Invalid expression in variable assignment.");
        }

        try_consume(Token::Kind::SemiColon, "Expected ; after expression.");
        *stmt = assignStatement;
        return stmt;
    } else if (peek_for_kind(Token::Kind::OpenCurly)) {
        if (auto scope = parse_scope()) {
            *stmt = scope.value();
            return stmt;
        }
        else
        {
            add_error(peek_value(), "Invalid scope.");
        }
    }
    else if (auto ifStatement = try_consume(Token::Kind::If))
    {
        try_consume(Token::Kind::OpenParen, "Expected ( after if");
        auto ifStmt = m_allocator.alloc<Node::Statement::If>();
        if (auto expr = parse_expr())
        {
            ifStmt->expr = expr.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value(), "Invalid scope in if block.");
            }
        }
        try_consume(Token::Kind::CloseParen, "Expected ) after if expression");
        if (auto scope = parse_scope())
        {
            ifStmt->scope = scope.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value(), "Invalid scope.");
            }
        }
        ifStmt->pred = parse_if_predicate();

        *stmt = ifStmt;
        return stmt;
    } else if (auto whileStatement = try_consume(Token::Kind::While)) {
        try_consume(Token::Kind::OpenParen, "Expected ( after while declaration");
        auto whileStmt = m_allocator.alloc<Node::Statement::While>();
        if (auto expr = parse_expr())
        {
            whileStmt->expr = expr.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value(), "Invalid expression in while block.");
            }
        }
        try_consume(Token::Kind::CloseParen, "Expected ) after while expression");
        if (auto scope = parse_scope())
        {
            whileStmt->scope = scope.value();
        }
        else
        {
            if (peek_has_value()) {
                add_error(peek_value(), "Invalid scope.");
            }
        }

        *stmt = whileStmt;
        return stmt;
    }
    // Return nothing which indicates an invalid statement
    return {};
}

std::optional<Node::Term*> Parser::parse_term()
{
    if (auto intLit = try_consume(Token::Kind::IntLit))
    {
        auto expr = m_allocator.alloc<Node::IntLiteral>();
        expr->intLit = intLit.value();
        auto term = m_allocator.alloc<Node::Term>();
        *term = expr;
        return term;
    }
    else if (auto ident = try_consume(Token::Kind::Identifier))
    {
        auto expr = m_allocator.alloc<Node::Identifier>();
        expr->identifier = ident.value();
        auto term = m_allocator.alloc<Node::Term>();
        *term = expr;
        return term;
    }
    else if (auto openParen = try_consume(Token::Kind::OpenParen))
    {
        auto expr = parse_expr();
        if (!expr.has_value())
        {
            add_error(peek_value(-1), "Expected expression after open parenthesis.");
        }
        try_consume(Token::Kind::CloseParen, "Expected close parenthesis.");
        auto termParen = m_allocator.alloc<Node::TermParen>();
        termParen->expr = expr.value();
        auto term = m_allocator.alloc<Node::Term>();
        *term = termParen;
        return term;
    }
    return {};
}

std::optional<Node::Scope *> Parser::parse_scope()
{
    if (!try_consume(Token::Kind::OpenCurly, "Expected { "))
    {
        return {};
    }
    auto scope = m_allocator.alloc<Node::Scope>();
    while (auto stmt = parse_statement()) {
        scope->statements.push_back(stmt.value());
    }
    if (!try_consume(Token::Kind::CloseCurly, "Expected }"))
    {
        return {};
    }
    return scope;
}

template<typename ...DescArgT>
void Parser::add_error(const Token& token, DescArgT ...description)
{
    consume();
    const auto error = make_error(token, description...);
    m_error_handler << error;
}

std::optional<Node::IfPredicate*> Parser::parse_if_predicate() {

    if (try_consume(Token::Kind::Else)) {
        auto ifPredicate = m_allocator.alloc<Node::IfPredicate>();
        if (try_consume(Token::Kind::If)) {
            try_consume(Token::Kind::OpenParen, "Expected ( after if");
            auto elseIfStmt = m_allocator.alloc<Node::Statement::ElseIf>();
            if (auto expr = parse_expr())
            {
                elseIfStmt->expr = expr.value();
            }
            try_consume(Token::Kind::CloseParen, "Expected ) after if expression");
            if (auto scope = parse_scope())
            {
                elseIfStmt->scope = scope.value();
            }
            else
            {
                if (peek_has_value()) {
                    add_error(peek_value(), "Invalid scope.");
                }
            }
            elseIfStmt->pred = parse_if_predicate();

            *ifPredicate = elseIfStmt;
        } else {
            auto elseStmt = m_allocator.alloc<Node::Statement::Else>();
            if (auto scope = parse_scope())
            {
                elseStmt->scope = scope.value();
            }
            else
            {
                if (peek_has_value()) {
                    add_error(peek_value(), "Invalid scope.");
                }
            }
            *ifPredicate = elseStmt;
        }
        return ifPredicate;
    }
    return {};
}