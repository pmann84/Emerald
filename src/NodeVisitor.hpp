#pragma once

#include <string>
#include <sstream>
#include <cassert>
#include <ranges>
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
    std::vector<size_t>& scopes() { return m_generator.scopes(); }
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
        std::string identifier = indentExpr->identifier.value().value();
        auto it = std::find_if(
                variables().begin(),
                variables().end(),
                [&identifier](const Variable& var) { return var.name == identifier; });
        if (it == variables().end())
        {
            std::stringstream errorSs;
            errorSs << "Undeclared variable " << identifier;
            const auto error = makeError({}, errorSs.str());
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
        output() << "\tmov rax, " << intLitExpr->intLit.value().value() << "\n";
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
    void operator()(const Node::RelExpr* relExpr) {
        generator().generateRelExp(relExpr);
    }
    void operator()(const Node::EqlExpr* eqlExpr) {
        generator().generateEqlExp(eqlExpr);
    }
};

class BinExprVisitor : public NodeVisitor
{
public:
    explicit BinExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::BinaryExpr::Add* add)
    {
        // This pushes both onto the stop of the stack
        generator().generateExpr(add->rhs);
        generator().generateExpr(add->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tadd rax, rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinaryExpr::Multiply* mult)
    {
        // This pushes both onto the stop of the stack
        generator().generateExpr(mult->rhs);
        generator().generateExpr(mult->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tmul rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinaryExpr::Minus* minus)
    {
        // This pushes both onto the stop of the stack
        // Need to do RHS first otherwise we are minus-ing in the wrong order (associativity and all that)
        generator().generateExpr(minus->rhs);
        generator().generateExpr(minus->lhs);
        generator().pop("rax");
        generator().pop("rbx");
        output() << "\tsub rax, rbx\n";
        generator().push("rax");
    }
    void operator()(const Node::BinaryExpr::Divide* div)
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
    void operator()(const Node::Statement::Let* letStatement)
    {
        size_t offset = 0;
        const auto ss = scopes();
        if (!scopes().empty())
        {
            offset = scopes().back();
            if (offset < 0 || offset > variables().size())
            {
                offset = 0;
            }
        }
        auto it = std::find_if(
                variables().begin() + offset,
                variables().end(),
                [&letStatement](const Variable& var) { return var.name == letStatement->identifier.value().value(); });
//        if (variables().contains(letStatement->identifier.Value.value()))
        if (it != variables().end())
        {
            std::stringstream errorSs;
            errorSs << "Identifier " << letStatement->identifier.value().value() << " already used.";
            const auto error = makeError(letStatement->identifier.info().value(), errorSs.str());
            errors() << error;
        } else {
            variables().push_back({ .name = letStatement->identifier.value().value(), .stackPosition = generator().stackLocation() });
            generator().generateExpr(letStatement->letExpr); // This inserts the variable onto the stack
        }
    }
    void operator()(const Node::Statement::Assign* assignStatement) {
        const auto it = std::ranges::find_if(
                variables(),
                [&assignStatement](const Variable& var) {
                    return var.name == assignStatement->identifier.value().value();
                });
        if (it == variables().end()) {
            std::stringstream errorSs;
            errorSs << "Undeclared identifier " << assignStatement->identifier.value().value() << ".";
            const auto error = makeError(assignStatement->identifier.info().value(), errorSs.str());
            errors() << error;
        } else {
            generator().generateExpr(assignStatement->assignExpr); // This evaluates the expression and inserts the variable onto the stack
            generator().pop("rax");

            const auto identLocation = it->stackPosition;
            const auto offset = generator().stackLocation() - identLocation - 1;
            generator().move("[rsp + " + std::to_string(offset * 8) + "]", "rax");
        }
    }
    void operator()(const Node::Statement::Return* returnStatement)
    {
        generator().generateExpr(returnStatement->returnExpr);
#ifdef __WIN64
        output() << "\tcall ExitProcess\n";
#endif

#ifdef __linux__
        generator().move("rax", "60");
        generator().pop("rdi");
        output() << "\tsyscall\n";
#endif
    }
    void operator()(const Node::Scope* statementScope)
    {
        generator().generateScope(statementScope);
    }
    void operator()(const Node::Statement::If* ifStatement)
    {
        // Puts the result of the expr on the top of the stack
        generator().generateExpr(ifStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
        const auto nextLabel = generator().createLabel();
        const auto endLabel = generator().createLabel();
        generator().compareAndJump("rax", "0", nextLabel, "begin if");
        generator().generateScope(ifStatement->scope);
        generator().jumpToLabel(endLabel, "end if");
        generator().writeLabel(nextLabel);
        if (ifStatement->pred.has_value()) {
            generator().generateIfPredicate(ifStatement->pred.value(), endLabel);
        }
        generator().writeLabel(endLabel, "end of if block");
    }
    void operator()(const Node::Statement::While* whileStatement) {
        // TODO: Generate asm for while statement
        // Puts the result of the expr on the top of the stack
        generator().generateExpr(whileStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
    }
};

class IfPredicateVisitor : public NodeVisitor
{
public:
    explicit IfPredicateVisitor(Generator& generator, const std::string& endLabel)
    : NodeVisitor(generator)
    , m_endLabel(endLabel)
    {}

    void operator()(const Node::Statement::ElseIf* elseIfStatement) {
        // Puts the result of the expr on the top of the stack
        generator().generateExpr(elseIfStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
        const auto nextLabel = generator().createLabel();
        output() << "\tcmp rax, 0 " << "; begin else if" << "\n";
        output() << "\tje " << nextLabel << "\n";
        generator().generateScope(elseIfStatement->scope);
        output() << "\tjmp " << m_endLabel << "; end else if" << "\n";
        output() << nextLabel << ":\n";
        if (elseIfStatement->pred.has_value()) {
            generator().generateIfPredicate(elseIfStatement->pred.value(), m_endLabel);
        }
    }
    void operator()(const Node::Statement::Else* elseStatement) {
        output() << "\t; begin else\n";
        generator().generateScope(elseStatement->scope);
    }

private:
    const std::string& m_endLabel;
};

class RelExprVisitor : public NodeVisitor {
public:
    explicit RelExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::RelationalExpr::LessThan* lessThanExpr) {
        // TODO: Generate asm
    }
    void operator()(const Node::RelationalExpr::GreaterThan* greaterThanExpr) {
        // TODO: Generate asm
    }
    void operator()(const Node::RelationalExpr::LessThanEqual* lessThanEqlExpr) {
        // TODO: Generate asm
    }
    void operator()(const Node::RelationalExpr::GreaterThanEqual* greaterThanEqlExpr) {
        // TODO: Generate asm
    }
};

class EqlExprVisitor : public NodeVisitor {
public:
    explicit EqlExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::EqualityExpr::Equal* eqlExpr) {
        // TODO: Generate asm
    }
    void operator()(const Node::EqualityExpr::NotEqual* notEqlExpr) {
        // TODO: Generate asm
    }
};