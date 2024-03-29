#pragma once

#include "Parser.hpp"
#include "Variable.hpp"
#include "Nodes.hpp"

#include <sstream>
#include <map>

class Generator
{
public:
    explicit Generator(Node::Program root, ErrorHandler& errorHandler);

    [[nodiscard]] std::string generateProgram();
    std::stringstream& output();
    std::vector<Variable>& variables();
    std::vector<size_t>& scopes();
    ErrorHandler& errors();
    size_t stackLocation() const;

    void generateStatement(const Node::Stmt* statement);
    void generateExpr(const Node::Expr* expr);
    void generateBinExp(const Node::BinExpr* binExpr);
    void generateRelExp(const Node::RelExpr* relExpr);
    void generateEqlExp(const Node::EqlExpr* eqlExpr);
    void generateTerm(const Node::Term* term);
    void generateScope(const Node::Scope* scope);
    void generateIfPredicate(const Node::IfPredicate* ifPredicate, const std::string& endLabel);

    void push(const std::string& reg);
    void pop(const std::string& reg);
    void move(const std::string& dest, const std::string& src);
    std::string createLabel();

    void beginScope();
    void endScope();

private:
    const Node::Program m_root;
    std::stringstream m_outputStream;
    size_t m_stackLocation = 0;
    std::vector<Variable> m_variables;
    std::vector<size_t> m_scopes;
    size_t m_labelIndex = 0;
    ErrorHandler& m_errorHandler;
};
