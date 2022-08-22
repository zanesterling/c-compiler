#include "ast.h"

using namespace std;

FunctionDeclNode* FunctionDeclNode::try_parse(Parser& parser) {
	// FIXME: IRL the type signature is more complicated than this, I think.
	TypeNode* type = TypeNode::try_parse(parser);

	Token* func_name = parser.assert_next_token(TokenType::identifier);
	Token* lparen = parser.assert_next_token(TokenType::open_paren);
	// TODO: Parse arglist.
	Token* rparen = parser.assert_next_token(TokenType::close_paren);
	Token* lcurly = parser.assert_next_token(TokenType::open_curly);
	if (func_name == nullptr
		|| lparen == nullptr
		|| rparen == nullptr
		|| lcurly == nullptr) {
		// FIXME: Need to pop the parser back to its previous state.
		return nullptr;
	}

	vector<AstNode*> body_statements;
	while (true) {
		StatementNode* stmt = StatementNode::try_parse(parser);
		if (stmt == nullptr) break;
		body_statements.push_back(stmt);
	}

	Token* rcurly = parser.assert_next_token(TokenType::close_curly);
	if (rcurly == nullptr) {
		// FIXME: Need to pop the parser back to its previous state.
		return nullptr;
	}

	return new FunctionDeclNode(move(body_statements));
}

void FunctionDeclNode::ugly_print() {
	cout << "FUNCTION_DECL_NODE:";
	for (auto stmt : this->body_statements) {
		cout << " ";
		stmt->ugly_print();
	}
}
