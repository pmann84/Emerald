#pragma once

#include <variant>
#include <Tokeniser.hpp>
#include <CompilerError.hpp>
#include "ArenaAllocator.hpp"
#include "Nodes.hpp"

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens, ErrorHandler& error_handler);

    Node::Program parse();

private:
    [[nodiscard]] std::optional<Token> peek(int64_t offset = 0) const;
    [[nodiscard]] Token peek_value(int64_t offset = 0) const;
    [[nodiscard]] bool peek_has_value(int64_t offset = 0) const;
    [[nodiscard]] bool peek_for_kind(Token::Kind tType, int64_t offset = 0) const;
    [[nodiscard]] bool peek_for_rule(std::vector<Token::Kind> rule) const;

    Token consume();
    std::optional<Token> try_consume(const std::vector<Token::Kind>& tTypes, const std::string& error);
    std::optional<Token> try_consume(Token::Kind tType, const std::string& error);
    std::optional<Token> try_consume(Token::Kind tType);

    template<typename ...DescArgT>
    void add_error(const Token& token, DescArgT ...description);

    std::optional<Node::Term*> parse_term();
    std::optional<Node::Expr*> parse_expr(int min_precedence = 0);
    std::optional<Node::Stmt*> parse_statement();
    std::optional<Node::Scope*> parse_scope();
    std::optional<Node::IfPredicate*> parse_if_predicate();

private:
    const std::vector<Token> m_tokens;
    size_t m_token_pos = 0;
    ArenaAllocator m_allocator;
    ErrorHandler& m_error_handler;
};
