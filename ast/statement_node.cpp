#include "ast.h"

StatementNode* StatementNode::try_parse(Parser& parser) {
	auto ret = ReturnStatementNode::try_parse(parser);
	if (ret != nullptr) return ret;

	// TODO: Add more statements, eg. assignment and decl statements.

	return nullptr;
}

void StatementNode::ugly_print() {
	cout << "STMT_NODE" << endl;
}
