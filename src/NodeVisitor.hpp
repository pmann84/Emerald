#pragma once

#include <string>
#include <sstream>
#include <cassert>
#include "Nodes.hpp"

class Generator;

class NodeVisitor
{
public:
    explicit NodeVisitor(Generator& generator) : m_generator(generator) {}

protected:
    Generator& generator() { return m_generator; }
    std::stringstream& output() { return m_generator.output(); }
    std::vector<Variable>& variables() { return m_generator.variables(); }
    ErrorHandler& errors() { return m_generator.errors(); }
    size_t stackLocation() { return m_generator.stackLocation(); }

private:
    Generator& m_generator;
};

class TermVisitor : public NodeVisitor
{
public:
    explicit TermVisitor(Generator& generator): NodeVisitor((generator)) {}

    void operator()(const Node::Identifier* indentExpr)
    {
        std::string identifier = indentExpr->identifier.Value.value();
        auto it = std::find_if(
                variables().begin(),
                variables().end(),
                [&identifier](const Variable& var) { return var.name == identifier; });
        if (it == variables().end())
        {
            std::stringstream errorSs;
            errorSs << "Undeclared variable " << identifier;
            Error error = { .Message = errorSs.str() };
            errors() << error;
            return;
        }

        const auto identLocation = it->stackPosition;
        const auto offset = generator().stackLocation() - identLocation - 1;
        // This pushes a copy of the item in rsp + offset to the top of the stack.
        // We * 8 here because it's the size in bytes (64bit -> 8, 32bit -> 4)
        generator().push("QWORD [rsp + " + std::to_string(offset * 8) + "]");

    }
    void operator()(const Node::IntLiteral* intLitExpr)
    {
        output() << "\tmov rax, " << intLitExpr->intLit.Value.value() << "\n";
        generator().push("rax");
    }
    void operator()(const Node::TermParen* parenTerm)
    {
        generator().generateExpr(parenTerm->expr);
    }
};

class ExprVisitor : public NodeVisitor
{
public:
    explicit ExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::Term* term)
    {
        generator().generateTerm(term);
    }
    void operator()(const Node::BinExpr* binExpr)
    {
        generator().generateBinExp(binExpr);
    }
};

class BinExprVisitor : public NodeVisitor
{
public:
    explicit BinExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::BinExprAdd* add)
    {
        // This pushes both onto the stop of the stack
        generator().generateExpr(add->rhs);
        generator().generateExpr(add->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tadd rax, rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinExprMult* mult)
    {
        // This pushes both onto the stop of the stack
        generator().generateExpr(mult->rhs);
        generator().generateExpr(mult->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tmul rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinExprMinus* minus)
    {
        // This pushes both onto the stop of the stack
        // Need to do RHS first otherwise we are minusing in the wrong order (associativity and all that)
        generator().generateExpr(minus->rhs);
        generator().generateExpr(minus->lhs);
        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tsub rax, rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinExprDiv* div)
    {
        // This pushes both onto the stop of the stack
        generator().generateExpr(div->rhs);
        generator().generateExpr(div->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tdiv rbx\n";
        generator().push("rax");
    }
};

class StatementVisitor : public NodeVisitor
{
public:
    explicit StatementVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::StatementLet* letStatement)
    {
        auto it = std::find_if(
                variables().begin(),
                variables().end(),
                [&letStatement](const Variable& var) { return var.name == letStatement->identifier.Value.value(); });
//        if (variables().contains(letStatement->identifier.Value.value()))
        if (it != variables().end())
        {
            std::stringstream errorSs;
            errorSs << "Identifier " << letStatement->identifier.Value.value() << " already used.";
            Error error = { .Message = errorSs.str() };
            errors() << error;
        } else {
            variables().push_back({ .name = letStatement->identifier.Value.value(), .stackPosition = generator().stackLocation() });
            generator().generateExpr(letStatement->letExpr); // This inserts the variable onto the stack
        }
    }
    void operator()(const Node::StatementReturn* returnStatement)
    {
        generator().generateExpr(returnStatement->returnExpr);
        output() << "\tmov rax, 60\n";
        generator().pop("rdi");
        output() << "\tsyscall\n";
    }
    void operator()(const Node::Scope* statementScope)
    {
        generator().generateScope(statementScope);
    }
    void operator()(const Node::StatementIf* ifStatement)
    {
        // Puts the result of the expr on the top of the stack
        generator().generateExpr(ifStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
        const auto label = generator().createLabel();
        output() << "\ttest rax, rax\n";
        output() << "\tjz " << label << "\n";
        generator().generateScope(ifStatement->scope);
        output() << label << ":\n";
    }
};