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

    namespace BinaryExpr {
        struct Add
        {
            Expr *lhs, *rhs;
        };

        struct Multiply
        {
            Expr *lhs, *rhs;
        };

        struct Minus
        {
            Expr *lhs, *rhs;
        };

        struct Divide
        {
            Expr *lhs, *rhs;
        };
    }

    using BinExpr = std::variant<BinaryExpr::Add*, BinaryExpr::Multiply*, BinaryExpr::Minus*, BinaryExpr::Divide*>;

    namespace RelationalExpr {
        struct LessThan {
            Expr *lhs, *rhs;
        };
        struct GreaterThan {
            Expr *lhs, *rhs;
        };
        struct LessThanEqual {
            Expr *lhs, *rhs;
        };
        struct GreaterThanEqual {
            Expr *lhs, *rhs;
        };
    }

    using RelExpr = std::variant<RelationalExpr::LessThan*, RelationalExpr::GreaterThan*, RelationalExpr::LessThanEqual*, RelationalExpr::GreaterThanEqual*>;

    namespace EqualityExpr {
        struct Equal {
            Expr *lhs, *rhs;
        };
        struct NotEqual {
            Expr *lhs, *rhs;
        };
    }

    using EqlExpr = std::variant<EqualityExpr::Equal*, EqualityExpr::NotEqual*>;

    struct TermParen
    {
        Expr* expr;
    };

    using Term = std::variant<IntLiteral*, Identifier*, TermParen*>;

    struct Expr
    {
        std::variant<Term*, BinExpr*, EqlExpr*, RelExpr*> expr;
    };

    struct Scope;
    namespace Statement {
        struct If;
        struct While;

        struct Return
        {
            Expr* returnExpr;
        };

        struct Let
        {
            Token identifier;
            Expr* letExpr;
        };

        struct Assign
        {
            Token identifier;
            Expr* assignExpr;
        };
    }
    using Stmt = std::variant<Statement::Return*, Statement::Let*, Scope*, Statement::If*, Statement::Assign*, Statement::While*>;

    struct Scope
    {
        std::vector<Stmt*> statements;
    };

    namespace Statement {
        struct ElseIf;
        struct Else;
    }
    using IfPredicate = std::variant<Statement::ElseIf*, Statement::Else*>;

    namespace Statement {
        struct ElseIf
        {
            Expr* expr;
            Scope* scope;
            std::optional<IfPredicate*> pred;
        };

        struct If
        {
            Expr* expr;
            Scope* scope;
            std::optional<IfPredicate*> pred;
        };

        struct Else
        {
            Scope* scope;
        };

        struct While {
            Expr* expr;
            Scope* scope;
        };
    }


    struct Program
    {
        std::vector<Stmt*> statements;
    };
}