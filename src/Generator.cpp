#include "Generator.hpp"
#include "NodeVisitor.hpp"

#include <sstream>

Generator::Generator(Node::Program root) : m_root(std::move(root))
{

}

Result<std::string> Generator::generateProgram()
{
    output() <<"global _start\n\n_start:\n";

    if (!m_root.statements.empty()) {
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
    return { .result = output().str(), .success=m_errors.empty(), .errors=m_errors };
}

void Generator::generateStatement(const Node::Statement &statement)
{
    StatementVisitor statementVisitor(*this);
    std::visit(statementVisitor, statement.statement);
}

void Generator::generateExpr(const Node::Expr &expr)
{
    ExprVisitor exprVisitor(*this);
    std::visit(exprVisitor, expr.expr);
}

std::stringstream &Generator::output()
{
    return m_outputStream;
}

void Generator::push(const std::string& reg)
{
    output() << "\tpush " << reg << "\n";
    m_stackLocation++;
}

void Generator::pop(const std::string& reg)
{
    output() << "\tpop " << reg << "\n";
    m_stackLocation--;
}

std::unordered_map<std::string, Variable> &Generator::variables()
{
    return m_variables;
}

std::vector<std::string> &Generator::errors()
{
    return m_errors;
}

size_t Generator::stackLocation() const
{
    return m_stackLocation;
}
