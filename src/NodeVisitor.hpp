#pragma once

#include <string>
#include <sstream>

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

class ExprVisitor : public NodeVisitor
{
public:
    explicit ExprVisitor(Generator& generator): NodeVisitor(generator) {}

    void operator()(const Node::Identifier& indentExpr)
    {
        std::string identifier = indentExpr.identifier.value.value();
        if (!variables().contains(identifier)) {
            std::stringstream errorSs;
            errorSs << "Undeclared variable " << identifier;
            errors().push_back(errorSs.str());
            return;
        }

        const auto identLocation = variables().at(identifier).stackPosition;
        const auto offset = generator().stackLocation() - identLocation - 1;
        // This pushes a copy of the item in rsp + offset to the top of the stack.
        // We * 8 here because its the size in bytes (64bit -> 8, 32bit -> 4)
        generator().push("QWORD [rsp + " + std::to_string(offset * 8) + "]");

    }
    void operator()(const Node::IntLiteral& intLitExpr)
    {
        output() << "\tmov rax, " << intLitExpr.intLit.value.value() << "\n";
        generator().push("rax");
    }
};

class StatementVisitor : public NodeVisitor
{
public:
    explicit StatementVisitor(Generator& generator): NodeVisitor(generator) {}
    void operator()(const Node::StatementLet& letStatement)
    {
        if (variables().contains(letStatement.identifier.value.value()))
        {
            std::stringstream errorSs;
            errorSs << "Identifier " << letStatement.identifier.value.value() << " already used.";
            errors().push_back(errorSs.str());
        } else {
            variables().insert({
                                       letStatement.identifier.value.value(),
                                       Variable{ .stackPosition = generator().stackLocation() }
                               });
            generator().generateExpr(letStatement.letExpr); // This inserts the variable onto the stack
        }
    }
    void operator()(const Node::StatementReturn& returnStatement)
    {
        generator().generateExpr(returnStatement.returnExpr);
        output() << "\tmov rax, 60\n";
        generator().pop("rdi");
        output() << "\tsyscall\n";
    }
};