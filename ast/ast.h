#pragma once

#include <iostream>
#include <vector>

#include "../parser.h"
#include "../token.h"

using namespace std;

class Parser; // Forward declaration to resolve circular dep.

enum class AstNodeType {
	program,
	function_definition,
	return_statement,
};

class AstNode {
public:
	virtual ~AstNode() {}

	AstNodeType getType();

	// static AstNode& try_parse(Parser& parser);

	virtual void ugly_print()=0;
	// virtual void pretty_print()=0;

private:
	AstNodeType type;
};

class ProgramNode : public AstNode {
public:
	ProgramNode(vector<AstNode*>&& children) : children{children} {}
	~ProgramNode() {
		for (AstNode* p : children) delete p;
	}

	static ProgramNode* try_parse(Parser& parser);

	void ugly_print();

private:
	// Should only contain FunctionDeclNode, VarDeclNode.
	vector<AstNode*> children;
};

using Type = string;

class FunctionDeclNode : public AstNode {
public:
	string name;
	Type type;
	vector<AstNode*> body_statements;

	FunctionDeclNode(Type type, string name, vector<AstNode*> body)
		: type{type}, name{name}, body_statements{body} {}
	~FunctionDeclNode() {}

	static FunctionDeclNode* try_parse(Parser& parser);

	void ugly_print();
};

class VarDeclNode : public AstNode {
public:
	~VarDeclNode();

	static VarDeclNode* try_parse(Parser& parser);

	void ugly_print();
};

class TypeNode : public AstNode {
public:
	string type_name;

	TypeNode(string type_name) : type_name{type_name} {}
	~TypeNode();

	static TypeNode* try_parse(Parser& parser);

	void ugly_print();
};

class StatementNode : public AstNode {
public:
	~StatementNode() {}

	static StatementNode* try_parse(Parser& parser);

	void ugly_print();
};

class ExpressionNode;
class ReturnStatementNode : public StatementNode {
public:
	ExpressionNode* expr; // FIXME: Delete, memory leak.

	ReturnStatementNode(ExpressionNode* expr) : expr{expr} {}
	~ReturnStatementNode() {}

	static ReturnStatementNode* try_parse(Parser& parser);

	void ugly_print();
};

class ExpressionNode : public AstNode {
public:
	ExpressionNode() {}
	~ExpressionNode() {}

	static ExpressionNode* try_parse(Parser& parser);

	virtual void ugly_print()=0;
};

class IntLiteralNode : public ExpressionNode {
public:
	int val;

	IntLiteralNode(Token token) : val{stoi(token.contents)} {}
	~IntLiteralNode() {}

	static IntLiteralNode* try_parse(Parser& parser);

	void ugly_print();
};
