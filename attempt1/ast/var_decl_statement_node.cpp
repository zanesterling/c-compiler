#include "ast.h"

VarDeclStatementNode* VarDeclStatementNode::try_parse(Parser& parser) {
	auto type = TypeNode::try_parse(parser);
	if (type == nullptr) return nullptr;
	auto name = parser.assert_next_token(TokenType::identifier);
	if (name == nullptr) return nullptr;
	auto semi = parser.assert_next_token(TokenType::semicolon);
	if (semi == nullptr) return nullptr;

	return new VarDeclStatementNode(type->type_name, name->contents);
}
