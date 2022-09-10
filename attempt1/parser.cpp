#include "parser.h"

AstNode* Parser::parse() {
	vector<AstNode*> nodes;
	while (true) {
		AstNode* node;
		node = FunctionDeclNode::try_parse(*this);
		if (node != nullptr) {
			nodes.push_back(node);
			continue;
		}

		node = VarDeclNode::try_parse(*this);
		if (node != nullptr) {
			nodes.push_back(node);
			continue;
		}

		// TODO: What else can go at the root level of a program?

		// TODO: Switch this to an appropriate exception throw.
		if (this->has_next_token()) {
			cerr << "ERROR: Could not find a matching node at token_index: "
				<< this->next_token_index << endl;
			return nullptr;
		}
		break;
	}

	return new ProgramNode(move(nodes));
}

bool Parser::has_next_token() {
	return this->next_token_index < tokens.size();
}

// Could throw an access exception :/
Token& Parser::next_token() {
	return this->tokens[this->next_token_index++];
}

Token* Parser::assert_next_token(TokenType type) {
	if (!this->has_next_token()) return nullptr;
	if (type != this->tokens[this->next_token_index].type) {
		return nullptr;
	}
	auto token = &next_token();
	cout << "found token: " << token->toString() << endl;
	return token;
}
