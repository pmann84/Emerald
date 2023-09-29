#include "Generator.hpp"
#include "NodeVisitor.hpp"

#include <sstream>

Generator::Generator(Node::Program root, ErrorHandler& errorHandler) : m_root(std::move(root)), m_errorHandler(errorHandler)
{

}

std::string Generator::generateProgram()
{
    output() <<"global _start\n\n_start:\n";

    if (!m_root.statements.empty()) {
        for (const auto statement: m_root.statements) {
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

void Generator::generateStatement(const Node::Statement* statement)
{
    StatementVisitor statementVisitor(*this);
    std::visit(statementVisitor, statement->statement);
}

void Generator::generateExpr(const Node::Expr* expr)
{
    ExprVisitor exprVisitor(*this);
    std::visit(exprVisitor, expr->expr);
}

void Generator::generateBinExp(const Node::BinExpr *binExpr)
{
    BinExprVisitor binExprVisitor(*this);
    std::visit(binExprVisitor, binExpr->expr);
}

void Generator::generateTerm(const Node::Term *term)
{
    TermVisitor termVisitor(*this);
    std::visit(termVisitor, term->expr);
}

void Generator::generateScope(const Node::Scope *scope)
{
    beginScope();
    for (const auto* stmt : scope->statements)
    {
        generateStatement(stmt);
    }
    endScope();
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

std::vector<Variable> &Generator::variables()
{
    return m_variables;
}

size_t Generator::stackLocation() const
{
    return m_stackLocation;
}

ErrorHandler &Generator::errors()
{
    return m_errorHandler;
}

void Generator::beginScope()
{
    m_scopes.push_back(m_variables.size());
}

void Generator::endScope()
{
    size_t popCount = m_variables.size() - m_scopes.back();
    // stack grows from the top to adding is popping off!
    m_outputStream << "\tadd rsp, " << popCount * 8 << "\n";
    m_stackLocation -= popCount;
//    m_variables = std::vector<Variable>(m_variables.begin(), m_variables.begin() + popCount);
    for (auto i = 0; i < popCount; ++i)
    {
        m_variables.pop_back();
    }
    m_scopes.pop_back();
}

std::string Generator::createLabel()
{
    std::stringstream ss;
    ss << "label" << m_labelIndex++;
    return ss.str();
}

