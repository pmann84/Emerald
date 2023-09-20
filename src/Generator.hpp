#pragma once

#include "Parser.hpp"
#include "Variable.hpp"

#include <sstream>
#include <unordered_map>

class Generator
{
public:
    explicit Generator(Node::Program root);

    [[nodiscard]] Result<std::string> generateProgram();
    std::stringstream& output();
    std::unordered_map<std::string, Variable>& variables();
    std::vector<std::string>& errors();
    size_t stackLocation() const;

    void generateStatement(const Node::Statement& statement);
    void generateExpr(const Node::Expr& expr);

    void push(const std::string& reg);
    void pop(const std::string& reg);

private:
    const Node::Program m_root;
    std::stringstream m_outputStream;
    size_t m_stackLocation = 0;
    std::unordered_map<std::string, Variable> m_variables;
    std::vector<std::string> m_errors;
};
