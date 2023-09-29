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
    std::map<std::string, Variable>& variables();
    ErrorHandler& errors();
    size_t stackLocation() const;

    void generateStatement(const Node::Statement* statement);
    void generateExpr(const Node::Expr* expr);
    void generateBinExp(const Node::BinExpr* binExpr);
    void generateTerm(const Node::Term* term);

    void push(const std::string& reg);
    void pop(const std::string& reg);

private:
    const Node::Program m_root;
    std::stringstream m_outputStream;
    size_t m_stackLocation = 0;
    std::map<std::string, Variable> m_variables;
    ErrorHandler& m_errorHandler;
};
