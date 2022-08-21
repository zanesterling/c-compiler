#pragma once

#include <iostream>
#include <vector>

using namespace std;

enum class AstNodeType {
	program,
	function_definition,
	return_statement,
};

class AstNode {
public:
	virtual ~AstNode() {}

	AstNodeType getType();

	// static AstNode& try_parse();

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

	static ProgramNode* try_parse();

	void ugly_print();

private:
	vector<AstNode*> children;
};

class FunctionDeclNode : public AstNode {
public:
	~FunctionDeclNode();

	static FunctionDeclNode* try_parse();

	void ugly_print();
};

class VarDeclNode : public AstNode {
public:
	~VarDeclNode();

	static VarDeclNode* try_parse();

	void ugly_print();
};
