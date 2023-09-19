#pragma once

#include "Parser.hpp"

#include <sstream>

class Generator
{
public:
    explicit Generator(Node::Program root);

    [[nodiscard]] std::string generateProgram();
    std::stringstream& output();

private:
    void generateStatement(const Node::Statement& statement);
    void generateExpr(const Node::Expr& expr);
private:
    const Node::Program m_root;
    std::stringstream m_outputStream;
};
