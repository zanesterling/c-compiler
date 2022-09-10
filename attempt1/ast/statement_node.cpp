#include "ast.h"

StatementNode* StatementNode::try_parse(Parser& parser) {
	StatementNode* node = ReturnStatementNode::try_parse(parser);
	if (node != nullptr) return node;

	node = VarDeclStatementNode::try_parse(parser);
	if (node != nullptr) return node;

	// TODO: Add more statements, eg. assignment and decl statements.

	return nullptr;
}

void StatementNode::ugly_print() {
	cout << "STMT_NODE" << endl;
}
