#include "Generator.hpp"
#include "NodeVisitor.hpp"

#include <sstream>

Generator::Generator(Node::Program root) : m_root(std::move(root))
{

}

std::string Generator::generateProgram()
{
    output() <<"global _start\n\n_start:\n";

    if (m_root.statements.size() > 0) {
        for (const auto &statement: m_root.statements) {
            generateStatement(statement);
        }
    }
    else {
        // TODO: Consider whether this needs to be done all the time or not as a fallback?
        // Handle empty program
        output() << "\tmov rax, 60\n";
        output() << "\tmov rdi, 0\n";
        output() << "\tsyscall\n";
    }
    return output().str();
}

void Generator::generateStatement(const Node::Statement &statement)
{
    class StatementVisitor : public NodeVisitor
    {
    public:
        explicit StatementVisitor(Generator& generator): NodeVisitor(generator) {}
        void operator()(const Node::StatementLet& letStatement)
        {
        }
        void operator()(const Node::StatementReturn& returnStatement)
        {
            generator().generateExpr(returnStatement.returnExpr);
            output() << "\tmov rax, 60\n";
            output() << "\tpop rdi\n";
            output() << "\tsyscall\n";
        }
    } statementVisitor(*this);

    std::visit(statementVisitor, statement.statement);
}

void Generator::generateExpr(const Node::Expr &expr)
{
    class ExprVisitor : public NodeVisitor
    {
    public:
        explicit ExprVisitor(Generator& generator): NodeVisitor(generator) {}
        void operator()(const Node::Identifier& indentExpr)
        {
        }
        void operator()(const Node::IntLiteral& intLitExpr)
        {
            output() << "\tmov rax, " << intLitExpr.intLit.value.value() << "\n";
            output() << "\tpush rax\n";
        }
    } exprVisitor(*this);

    std::visit(exprVisitor, expr.expr);
}

std::stringstream &Generator::output()
{
    return m_outputStream;
}
