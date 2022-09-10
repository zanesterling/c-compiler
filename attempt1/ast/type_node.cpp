#include "ast.h"

TypeNode* TypeNode::try_parse(Parser& parser) {
	if (!parser.has_next_token()) return nullptr;
	Token& token = parser.next_token();
	if (token.type != TokenType::identifier) return nullptr;

	cout << "parsed a TYPE_NODE!" << endl;
	return new TypeNode(token.contents);

	// TODO: What about pointers, arrays, pointers to pointers? Structs?
}

TypeNode::~TypeNode() {}

void TypeNode::ugly_print() {
	cout << "TYPE_NODE: " << this->type_name << endl;
}
