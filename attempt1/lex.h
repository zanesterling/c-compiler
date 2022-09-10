#pragma once

#include <fstream>
#include <vector>

#include "token.h"

using namespace std;

class Lexer {
public:
	Lexer(ifstream&& source_file) : source_file(move(source_file)) {}

	vector<Token> lex();
	void setError(string&& errStr);
	bool hasError() { return this->hasErr; }
	string getError() { return this->error; }

private:
	ifstream source_file;

	int cur_token_start;
	int cur_token_end;

	bool hasErr = false;
	string error;
};

