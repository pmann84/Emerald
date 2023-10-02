#pragma once

#include <Token.hpp>

#include <variant>
#include <vector>

namespace Node {
    struct IntLiteral
    {
        Token intLit;
    };

    struct Identifier
    {
        Token identifier;
    };

    struct Expr;

    struct BinExprAdd
    {
        Expr *lhs, *rhs;
    };

    struct BinExprMult
    {
        Expr *lhs, *rhs;
    };

    struct BinExprMinus
    {
        Expr *lhs, *rhs;
    };

    struct BinExprDiv
    {
        Expr *lhs, *rhs;
    };

    struct BinExpr
    {
        std::variant<BinExprAdd*, BinExprMult*, BinExprMinus*, BinExprDiv*> expr;
    };

    struct TermParen
    {
        Expr* expr;
    };

    struct Term
    {
        std::variant<IntLiteral*, Identifier*, TermParen*> expr;
    };

    struct Expr
    {
        std::variant<Term*, BinExpr*> expr;
    };

    struct StatementReturn
    {
        Expr* returnExpr;
    };

    struct StatementLet
    {
        Token identifier;
        Expr* letExpr;
    };

    struct Statement;
    struct Scope
    {
        std::vector<Statement*> statements;
    };

    struct StatementIf
    {
        Expr* expr;
        Scope* scope;
    };

    struct Statement
    {
        std::variant<StatementReturn*, StatementLet*, Scope*, StatementIf*> statement;
    };

    struct Program
    {
        std::vector<Statement*> statements;
    };
}