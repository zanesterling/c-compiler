#pragma once

#include <string>

using namespace std;

enum class TokenType {
	identifier,
	open_paren,
	close_paren,
	open_curly,
	close_curly,
	int_literal,
	semicolon,
};

class Token {
public:
	Token(TokenType type, string contents) : type{type}, contents{contents} {}

	const TokenType type;
	const string contents; // used for storing eg. the name of a type

	string toString();
};
