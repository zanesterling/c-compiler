#include "ast.h"

ReturnStatementNode* ReturnStatementNode::try_parse(Parser& parser) {
	auto ret_ident = parser.assert_next_token(TokenType::identifier);
	if (ret_ident == nullptr || ret_ident->contents != "return") return nullptr;

	auto ret_expr = ExpressionNode::try_parse(parser);
	if (ret_expr == nullptr) return nullptr;

	auto semi = parser.assert_next_token(TokenType::semicolon);
	if (semi == nullptr) return nullptr;

	return new ReturnStatementNode(ret_expr);
}

void ReturnStatementNode::ugly_print() {
	cout << "RETURN_STMT_NODE: ";
	this->expr->ugly_print();
}
