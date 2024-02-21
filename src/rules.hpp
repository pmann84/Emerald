#pragma once

#include "Token.hpp"

#include <map>
#include <string>
#include <vector>

const std::map<std::string, std::vector<Token::Kind>> emerald_grammar_rules {
    {"variable_assign", {Token::Kind::Let, Token::Kind::Identifier, Token::Kind::Equals}},
    {"variable_reassign", {Token::Kind::Identifier, Token::Kind::Equals}}
};