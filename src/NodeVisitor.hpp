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
    assembly_builder& output() { return m_generator.output(); }
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
            const auto error = make_error({}, "Undeclared variable ", identifier);
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
        output().move("rax", intLitExpr->intLit.value().value());
        generator().push("rax");
    }
    void operator()(const Node::TermParen* parenTerm)
    {
        generator().generate_expr(parenTerm->expr);
    }
};

class ExprVisitor : public NodeVisitor
{
public:
    explicit ExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::Term* term)
    {
        generator().generate_term(term);
    }
    void operator()(const Node::BinExpr* binExpr)
    {
        generator().generate_bin_exp(binExpr);
    }
    void operator()(const Node::RelExpr* relExpr) {
        generator().generate_rel_exp(relExpr);
    }
    void operator()(const Node::EqlExpr* eqlExpr) {
        generator().generate_eql_exp(eqlExpr);
    }
};

class BinExprVisitor : public NodeVisitor
{
public:
    explicit BinExprVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::BinaryExpr::Add* add)
    {
        // This pushes both onto the stop of the stack
        generator().generate_expr(add->rhs);
        generator().generate_expr(add->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output().add("rax", "rbx");
        generator().push("rax");
    }
    void operator()(const Node::BinaryExpr::Multiply* mult)
    {
        // This pushes both onto the stop of the stack
        generator().generate_expr(mult->rhs);
        generator().generate_expr(mult->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output().mul("rbx");
        generator().push("rax");
    }
    void operator()(const Node::BinaryExpr::Minus* minus)
    {
        // This pushes both onto the stop of the stack
        // Need to do RHS first otherwise we are minus-ing in the wrong order (associativity and all that)
        generator().generate_expr(minus->rhs);
        generator().generate_expr(minus->lhs);
        generator().pop("rax");
        generator().pop("rbx");
        output().sub("rax", "rbx");
        generator().push("rax");
    }
    void operator()(const Node::BinaryExpr::Divide* div)
    {
        // This pushes both onto the stop of the stack
        generator().generate_expr(div->rhs);
        generator().generate_expr(div->lhs);

        generator().pop("rax");
        generator().pop("rbx");
        output().div("rbx");
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
            const auto error = make_error(letStatement->identifier.info().value(), "Identifier ", letStatement->identifier.value().value(), " already used.");
            errors() << error;
        } else {
            variables().push_back({ .name = letStatement->identifier.value().value(), .stackPosition = generator().stackLocation() });
            generator().generate_expr(letStatement->letExpr); // This inserts the variable onto the stack
        }
    }
    void operator()(const Node::Statement::Assign* assignStatement) {
        const auto it = std::ranges::find_if(
                variables(),
                [&assignStatement](const Variable& var) {
                    return var.name == assignStatement->identifier.value().value();
                });
        if (it == variables().end()) {
            const auto error = make_error(assignStatement->identifier.info().value(), "Undeclared identifier ", assignStatement->identifier.value().value(), ".");
            errors() << error;
        } else {
            generator().generate_expr(assignStatement->assignExpr); // This evaluates the expression and inserts the variable onto the stack
            generator().pop("rax");

            const auto identLocation = it->stackPosition;
            const auto offset = generator().stackLocation() - identLocation - 1;
            output().move("[rsp + " + std::to_string(offset * 8) + "]", "rax");
        }
    }
    void operator()(const Node::Statement::Return* returnStatement)
    {
        generator().generate_expr(returnStatement->returnExpr);
        // TODO: Think of a way to abstract this to assembly_builder
        // this would involve moving the pop functionality out of here
#ifdef __WIN64
        output() << "\tcall ExitProcess\n";
#endif

#ifdef __linux__
        output().move("rax", "60");
        generator().pop("rdi");
        output().syscall();
#endif
    }
    void operator()(const Node::Scope* statementScope)
    {
        generator().generate_scope(statementScope);
    }
    void operator()(const Node::Statement::If* ifStatement)
    {
        // Puts the result of the expr on the top of the stack
        generator().generate_expr(ifStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
        const auto nextLabel = generator().next_label();
        const auto endLabel = generator().next_label();
        output().compare_and_jump("rax", "0", nextLabel, "begin if");
        generator().generate_scope(ifStatement->scope);
        output().jump_to_label(endLabel, "end if");
        output().write_label(nextLabel);
        if (ifStatement->pred.has_value()) {
            generator().generate_if_predicate(ifStatement->pred.value(), endLabel);
        }
        output().write_label(endLabel, "end of if block");
    }
    void operator()(const Node::Statement::While* whileStatement) {
        // Puts the result of the expr on the top of the stack
        generator().generate_expr(whileStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
        const auto whileLabel = generator().next_label();
        const auto endLabel = generator().next_label();
        output().write_label(whileLabel);
        output().compare_and_jump("rax", "0", endLabel, "begin while");
        generator().generate_scope(whileStatement->scope);
        output().jump_to_label(whileLabel);
        output().write_label(endLabel, "end of while block");
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
        generator().generate_expr(elseIfStatement->expr);
        // Pop the top of the stack (the result of the expr above) into rax
        generator().pop("rax");
        const auto nextLabel = generator().next_label();
        output().compare_and_jump("rax", "0", nextLabel, "begin else if");
        generator().generate_scope(elseIfStatement->scope);
        output().jump_to_label(m_endLabel, "end else if");
        output().write_label(nextLabel);
        if (elseIfStatement->pred.has_value()) {
            generator().generate_if_predicate(elseIfStatement->pred.value(), m_endLabel);
        }
    }
    void operator()(const Node::Statement::Else* elseStatement) {
        output().write_comment_and_end_line("begin else");
        generator().generate_scope(elseStatement->scope);
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