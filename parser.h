#pragma once

#include <vector>

#include "ast/ast.h"
#include "lex.h"

using namespace std;

class AstNode; // Forward declaration b/c of circular reference.

class Parser {
public:
	Parser(vector<Token>&& tokens) : tokens{tokens}, next_token_index{0} {}

	// Returned ref must be deleted by caller.
	AstNode* parse();

	bool has_next_token();
	Token& next_token();
	Token* assert_next_token(TokenType);

private:
	vector<Token> tokens;
	int next_token_index;
};
