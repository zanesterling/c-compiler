#include "ast.h"

using namespace std;

ProgramNode* ProgramNode::try_parse(Parser& parser) {
	return nullptr;
}

void ProgramNode::ugly_print() {
	cout << "PROGRAM_NODE:";
	for (auto child : children) {
		cout << " ";
		child->ugly_print();
	}
	cout << endl;
}
