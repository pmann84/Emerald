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

    using BinExpr = std::variant<BinExprAdd*, BinExprMult*, BinExprMinus*, BinExprDiv*>;

    struct TermParen
    {
        Expr* expr;
    };

    using Term = std::variant<IntLiteral*, Identifier*, TermParen*>;

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

    struct StatementAssign
    {
        Token identifier;
        Expr* assignExpr;
    };

    struct Scope;
    struct StatementIf;
    using Statement = std::variant<StatementReturn*, StatementLet*, Scope*, StatementIf*, StatementAssign*>;

    struct Scope
    {
        std::vector<Statement*> statements;
    };

    struct StatementElseIf;
    struct StatementElse;
    using IfPredicate = std::variant<StatementElseIf*, StatementElse*>;

    struct StatementElseIf
    {
        Expr* expr;
        Scope* scope;
        std::optional<IfPredicate*> pred;
    };

    struct StatementIf
    {
        Expr* expr;
        Scope* scope;
        std::optional<IfPredicate*> pred;
    };

    struct StatementElse
    {
        Scope* scope;
    };

    struct Program
    {
        std::vector<Statement*> statements;
    };
}