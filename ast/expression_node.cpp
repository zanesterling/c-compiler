#include "ast.h"

ExpressionNode* ExpressionNode::try_parse(Parser& parser) {
	auto int_literal = parser.assert_next_token(TokenType::int_literal);
	if (int_literal != nullptr) return new IntLiteralNode(*int_literal);

	// TODO: Add more expressions, eg. binop expr, func appl expr.

	return nullptr;
}
