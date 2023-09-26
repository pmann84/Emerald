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
    std::unordered_map<std::string, Variable>& variables() { return m_generator.variables(); }
    std::vector<std::string>& errors() { return m_generator.errors(); }
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
        std::string identifier = indentExpr->identifier.value.value();
        if (!variables().contains(identifier)) {
            std::stringstream errorSs;
            errorSs << "Undeclared variable " << identifier;
            errors().push_back(errorSs.str());
            return;
        }

        const auto identLocation = variables().at(identifier).stackPosition;
        const auto offset = generator().stackLocation() - identLocation - 1;
        // This pushes a copy of the item in rsp + offset to the top of the stack.
        // We * 8 here because it's the size in bytes (64bit -> 8, 32bit -> 4)
        generator().push("QWORD [rsp + " + std::to_string(offset * 8) + "]");

    }
    void operator()(const Node::IntLiteral* intLitExpr)
    {
        output() << "\tmov rax, " << intLitExpr->intLit.value.value() << "\n";
        generator().push("rax");
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
        generator().generateExpr(add->lhs);
        generator().generateExpr(add->rhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tadd rax, rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinExprMult* mult)
    {
        // This pushes both onto the stop of the stack
        generator().generateExpr(mult->lhs);
        generator().generateExpr(mult->rhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tmul rbx\n";
        generator().push("rax");
    }
};

class StatementVisitor : public NodeVisitor
{
public:
    explicit StatementVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::StatementLet* letStatement)
    {
        if (variables().contains(letStatement->identifier.value.value()))
        {
            std::stringstream errorSs;
            errorSs << "Identifier " << letStatement->identifier.value.value() << " already used.";
            errors().push_back(errorSs.str());
        } else {
            variables().insert({
                                       letStatement->identifier.value.value(),
                                       Variable{ .stackPosition = generator().stackLocation() }
                               });
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
};