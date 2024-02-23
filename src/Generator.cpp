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
    output().write_header();

    if (!m_root.statements.empty()) {
        for (const auto statement: m_root.statements) {
            generate_statement(statement);
        }
    }
    else {
        // TODO: Consider whether this needs to be done all the time or not as a fallback?
        output().empty_program();
    }
    return output().str();
}

void Generator::generate_statement(const Node::Stmt* statement)
{
    StatementVisitor statementVisitor(*this);
    std::visit(statementVisitor, *statement);
}

void Generator::generate_expr(const Node::Expr* expr)
{
    ExprVisitor exprVisitor(*this);
    std::visit(exprVisitor, expr->expr);
}

void Generator::generate_bin_exp(const Node::BinExpr *binExpr)
{
    BinExprVisitor binExprVisitor(*this);
    std::visit(binExprVisitor, *binExpr);
}

void Generator::generate_rel_exp(const Node::RelExpr *relExpr) {
    RelExprVisitor relExprVisitor(*this);
    std::visit(relExprVisitor, *relExpr);
}

void Generator::generate_eql_exp(const Node::EqlExpr *eqlExpr) {
    EqlExprVisitor eqlExprVisitor(*this);
    std::visit(eqlExprVisitor, *eqlExpr);
}

void Generator::generate_term(const Node::Term *term)
{
    TermVisitor termVisitor(*this);
    std::visit(termVisitor, *term);
}

void Generator::generate_scope(const Node::Scope *scope)
{
    if (scope) {
        begin_scope();
        for (const auto *stmt: scope->statements) {
            generate_statement(stmt);
        }
        end_scope();
    }
}

void Generator::generate_if_predicate(const Node::IfPredicate *ifPredicate, const std::string& endLabel) {
    IfPredicateVisitor ifPredicateVisitor(*this, endLabel);
    std::visit(ifPredicateVisitor, *ifPredicate);
}

assembly_builder& Generator::output()
{
    return m_builder;
}

void Generator::push(const std::string& reg)
{
    output().push(reg);
    m_stackLocation++;
}

void Generator::pop(const std::string& reg)
{
    output().pop(reg);
    m_stackLocation--;
}

std::vector<variable> &Generator::variables()
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

void Generator::begin_scope()
{
    m_scopes.push_back(m_variables.size());
}

void Generator::end_scope()
{
    size_t popCount = m_variables.size() - m_scopes.back();
    // stack grows from the top to adding is popping off!
    output().add("rsp", popCount * 8);
    m_stackLocation -= popCount;
//    m_variables = std::vector<Variable>(m_variables.begin(), m_variables.begin() + popCount);
    for (auto i = 0; i < popCount; ++i)
    {
        m_variables.pop_back();
    }
    m_scopes.pop_back();
}

std::string Generator::next_label()
{
    return m_labels.next();
}
