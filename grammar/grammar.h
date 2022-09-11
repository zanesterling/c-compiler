#pragma once

#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

using namespace std;

enum MinalKind {
	terminal,
	nonterminal
};
class Minal {
public:
	const MinalKind kind;
	const std::string heart;

	Minal(MinalKind kind, std::string heart) : kind(kind), heart(heart) {}
};

class Production {
public:
	std::string head;
	vector<Minal> body;
};

class Grammar {
public:
	// TODO: Should these be const to avoid bad access?
	set<string> terminals;
	vector<Production> productions;
	int startProduction = -1;

	Grammar() {}

	static Grammar parse(string filename);

	bool validate();
};
