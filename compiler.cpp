#include <fstream>
#include <iostream>
#include <vector>

#include "ast/ast.h"
#include "lex.h"
#include "parser.h"

using namespace std;

constexpr bool DEBUG_LEX = true;

int usage(char** argv) {
	cerr << "usage: " << argv[0] << " SOURCE_FILE" << endl;
	return 1;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		return usage(argv);
	}

	ifstream sourceFile(argv[1]);
	if (!sourceFile.is_open()) {
		cerr << "error opening file: " << argv[1] << endl;
	}

	// TODO: add macro preprocess

	Lexer lexer(move(sourceFile));
	vector<Token> tokens = lexer.lex();
	if (DEBUG_LEX) for (Token t : tokens) cout << t.toString() << endl;

	Parser parser(move(tokens));
	AstNode* ast = parser.parse();
	if (ast == nullptr) {
		cerr << "error parsing ast" << endl;
		return 1;
	}
	cout << endl;
	ast->ugly_print();

	// TODO: interpret the C code?
	// TODO: generate machine code

	delete ast;
	sourceFile.close();
	return 0;
}
