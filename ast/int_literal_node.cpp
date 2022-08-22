#include "ast.h"

IntLiteralNode* try_parse(Parser& parser) {
	// NOT IMPLEMENTED YET. Not sure if we need to implement it tbh.
	return nullptr;
}

void IntLiteralNode::ugly_print() {
	cout << "INT_LITERAL_NODE{" << this->val << "}";
}
