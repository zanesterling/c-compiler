#pragma once

#include <fstream>
#include <vector>

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

	TokenType type;
	string contents; // used for storing eg. the name of a type

	string toString();
};

class Lexer {
public:
	Lexer(ifstream&& source_file) : source_file(move(source_file)) {}

	vector<Token> lex();

private:
	ifstream source_file;

	int cur_token_start;
	int cur_token_end;
};

