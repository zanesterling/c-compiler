#pragma once

#include <fstream>
#include <vector>

#include "token.h"

using namespace std;

class Lexer {
public:
	Lexer(ifstream&& source_file) : source_file(move(source_file)) {}

	vector<Token> lex();

private:
	ifstream source_file;

	int cur_token_start;
	int cur_token_end;
};

