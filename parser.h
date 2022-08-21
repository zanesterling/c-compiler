#pragma once

#include <vector>

#include "ast/ast.h"
#include "lex.h"

using namespace std;

class Parser {
public:
	Parser(vector<Token>&& tokens) : tokens{tokens} {}

	// Returned ref must be deleted by caller.
	AstNode* parse();

private:
	vector<Token> tokens;
};
