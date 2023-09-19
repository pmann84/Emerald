#pragma once

#include <string>
#include <sstream>

class Generator;

class NodeVisitor
{
public:
    explicit NodeVisitor(Generator& generator) : m_generator(generator) {}

protected:
    Generator& generator() { return m_generator; }
    std::stringstream& output() { return m_generator.output(); }

private:
    Generator& m_generator;
};