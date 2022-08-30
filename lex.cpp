#include "lex.h"
#include <iostream>

using namespace std;

enum class LexerState {
};

bool is_alpha(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool is_num(char c) {
	return '0' <= c && c <= '9';
}

bool is_alphanum(char c) {
	return is_alpha(c) || is_num(c);
}

bool is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

vector<Token> Lexer::lex() {
	std::vector<Token> tokens;

	string line;
	while (getline(this->source_file, line)) {
		for (int i = 0; i < line.size(); i++) {
			char c = line[i];
			if (is_whitespace(c)) continue;

			if (c == '(') {
				tokens.push_back(Token{ TokenType::open_paren, string(1, c) });
			} else if (c == ')') {
				tokens.push_back(Token{ TokenType::close_paren, string(1, c) });
			} else if (c == '{') {
				tokens.push_back(Token{ TokenType::open_curly, string(1, c) });
			} else if (c == '}') {
				tokens.push_back(Token{ TokenType::close_curly, string(1, c) });
			} else if (c == ';') {
				tokens.push_back(Token{ TokenType::semicolon, string(1, c) });
			} else if (is_alpha(c)) {
				int j;
				for (j = i+1; is_alphanum(line[j]); j++);
				tokens.push_back(Token{ TokenType::identifier, line.substr(i, j-i) });
				i = j-1; // Advance past the scanned identifier.
			} else if (is_num(c)) {
				// TODO: This doesn't account for float literals.
				int j;
				for (j = i+1; is_num(line[j]); j++);
				tokens.push_back(Token{ TokenType::int_literal, line.substr(i, j-i) });
				i = j-1; // Advance past the scanned numeric literal.
			} else {
				string errStr = "unexpected character '";
				errStr += c;
				errStr += "'";
				cerr << "LEX_ERR: " << errStr << endl;
				setError(move(errStr));
				goto ERROR;
			}
		}
	}
	ERROR:

	return tokens;
}

void Lexer::setError(string&& errStr) {
	this->hasErr = true;
	this->error = errStr;
}
