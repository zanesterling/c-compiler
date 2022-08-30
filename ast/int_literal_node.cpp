#include "ast.h"

IntLiteralNode* IntLiteralNode::try_parse(Parser& parser) {
	auto int_literal = parser.assert_next_token(TokenType::int_literal);
	if (int_literal == nullptr) return nullptr;
	auto node = new IntLiteralNode(*int_literal);
	return node;
}

void IntLiteralNode::ugly_print() {
	cout << "INT_LITERAL_NODE{" << this->val << "}";
}
