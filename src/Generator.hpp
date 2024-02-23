#pragma once

#include "Parser.hpp"
#include "variable.hpp"
#include "Nodes.hpp"
#include "label_manager.hpp"
#include "assembly_builder.hpp"

#include <sstream>
#include <map>

class Generator
{
public:
    explicit Generator(Node::Program root, ErrorHandler& errorHandler);

    [[nodiscard]] std::string generateProgram();
    assembly_builder& output();
    std::vector<variable>& variables();
    std::vector<size_t>& scopes();
    ErrorHandler& errors();
    size_t stackLocation() const;

    void generate_statement(const Node::Stmt* statement);
    void generate_expr(const Node::Expr* expr);
    void generate_bin_exp(const Node::BinExpr* binExpr);
    void generate_rel_exp(const Node::RelExpr* relExpr);
    void generate_eql_exp(const Node::EqlExpr* eqlExpr);
    void generate_term(const Node::Term* term);
    void generate_scope(const Node::Scope* scope);
    void generate_if_predicate(const Node::IfPredicate* ifPredicate, const std::string& endLabel);

    void push(const std::string& reg);
    void pop(const std::string& reg);
    std::string next_label();

    void begin_scope();
    void end_scope();

private:
    label_manager m_labels;
    assembly_builder m_builder;
    const Node::Program m_root;
    size_t m_stackLocation = 0;
    std::vector<variable> m_variables;
    std::vector<size_t> m_scopes;
    ErrorHandler& m_errorHandler;
};
