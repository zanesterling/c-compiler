#include "parser.h"

AstNode* Parser::parse() {
	int token_index = 0;
	vector<AstNode*> nodes;
	while (true) {
		AstNode* node;
		node = FunctionDeclNode::try_parse();
		if (node != nullptr) {
			nodes.push_back(node);
			continue;
		}

		node = VarDeclNode::try_parse();
		if (node != nullptr) {
			nodes.push_back(node);
			continue;
		}

		// TODO: What else can go at the root level of a program?

		// TODO: Switch this to an appropriate exception throw.
		cerr << "ERROR: Could not find a matching node at token_index: " << token_index << endl;
		return nullptr;
	}

	return new ProgramNode(move(nodes));
}
