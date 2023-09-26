#pragma once

#include <variant>
#include <vector>
#include "Tokens.hpp"

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

    struct BinExpr
    {
        std::variant<BinExprAdd*, BinExprMult*> expr;
    };

    struct Term
    {
        std::variant<IntLiteral*, Identifier*> expr;
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

    struct Statement
    {
        std::variant<StatementReturn*, StatementLet*> statement;
    };

    struct Program
    {
        std::vector<Statement*> statements;
    };
}