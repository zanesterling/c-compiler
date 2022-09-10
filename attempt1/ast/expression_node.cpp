#include "ast.h"

ExpressionNode* ExpressionNode::try_parse(Parser& parser) {
	auto int_literal = IntLiteralNode::try_parse(parser);
	if (int_literal != nullptr) return int_literal;

	// TODO: Add more expressions, eg. binop expr, func appl expr.

	return nullptr;
}
