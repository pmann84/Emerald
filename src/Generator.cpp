#include "Generator.hpp"
#include "NodeVisitor.hpp"

#include <sstream>

Generator::Generator(Node::Program root, ErrorHandler& errorHandler) : m_root(std::move(root)), m_errorHandler(errorHandler)
{

}

std::string Generator::generateProgram()
{
    // This section is required on windows compilations
    // but not on linux, we can probably make this better
    output() << "global start\n\n";

#ifdef __WIN64
    output() << "section .text\n\n";
    output() << "extern ExitProcess\n\n";
#endif

    output() << "start:\n";

    if (!m_root.statements.empty()) {
        for (const auto statement: m_root.statements) {
            generateStatement(statement);
        }
    }
    else {
        // TODO: Consider whether this needs to be done all the time or not as a fallback?
#ifdef __WIN64
        move("rax", "0");
        push("rax");
        output() << "\tcall ExitProcess\n";
#endif

#ifdef __linux__
        move("rax", "60");
        move("rdi", "0");
        output() << "\tsyscall\n";
#endif
    }
    return output().str();
}

void Generator::generateStatement(const Node::Stmt* statement)
{
    StatementVisitor statementVisitor(*this);
    std::visit(statementVisitor, *statement);
}

void Generator::generateExpr(const Node::Expr* expr)
{
    ExprVisitor exprVisitor(*this);
    std::visit(exprVisitor, expr->expr);
}

void Generator::generateBinExp(const Node::BinExpr *binExpr)
{
    BinExprVisitor binExprVisitor(*this);
    std::visit(binExprVisitor, *binExpr);
}

void Generator::generateRelExp(const Node::RelExpr *relExpr) {
    RelExprVisitor relExprVisitor(*this);
    std::visit(relExprVisitor, *relExpr);
}

void Generator::generateEqlExp(const Node::EqlExpr *eqlExpr) {
    EqlExprVisitor eqlExprVisitor(*this);
    std::visit(eqlExprVisitor, *eqlExpr);
}

void Generator::generateTerm(const Node::Term *term)
{
    TermVisitor termVisitor(*this);
    std::visit(termVisitor, *term);
}

void Generator::generateScope(const Node::Scope *scope)
{
    if (scope) {
        beginScope();
        for (const auto *stmt: scope->statements) {
            generateStatement(stmt);
        }
        endScope();
    }
}

void Generator::generateIfPredicate(const Node::IfPredicate *ifPredicate, const std::string& endLabel) {
    IfPredicateVisitor ifPredicateVisitor(*this, endLabel);
    std::visit(ifPredicateVisitor, *ifPredicate);
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

void Generator::move(const std::string& dest, const std::string& src) {
    output() << "\tmov " << dest << ", " << src << "\n";
}

std::vector<Variable> &Generator::variables()
{
    return m_variables;
}

std::vector<size_t> &Generator::scopes()
{
    return m_scopes;
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
    ss << "L" << m_labelIndex++;
    return ss.str();
}

