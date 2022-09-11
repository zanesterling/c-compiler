#include "grammar.h"

runtime_error lineErr(int lineNum, string msg, string line) {
	return runtime_error(to_string(lineNum)+": " + msg+": \"" + line + "\"");
}

Grammar Grammar::parse(string filename) {
	ifstream f(filename);
	if (!f.is_open()) {
		throw runtime_error("error opening file: " + filename);
	}

	Grammar grammar;
	string line;
	size_t lineNum = 0;
	while (getline(f, line)) {
		if (line.size() == 0) continue;
		if (line[0] == '%') { // Terminal declaration.
			for (int i = 1; i < line.size(); i++) {
				if (line[i] < 'A' || 'Z' < line[i]) {
					throw lineErr(lineNum, "invalid token spec", line);
				}
			}
			grammar.terminals.insert(line.substr(1));
		} else if (line.find("->") != string::npos) { // Production.
			int i = 0;
			for (; i < line.size() && isalpha(line[i]); i++);
			auto headEnd = i;

			if (line.compare(headEnd, 3, " ->") != 0) {
				throw lineErr(lineNum, "expected \" ->\" at index " + to_string(i), line);
			}
			i += 3;

			vector<Minal> minals;
			while (i+1 < line.size()) {
				if (line[i] != ' ' || !isalpha(line[++i])) {
					throw lineErr(lineNum, "expected minal at index " + to_string(i), line);
				}
				int minalStart = i;
				for (; i < line.size() && isalpha(line[i]); i++);
				auto kind = isupper(line[i]) ? MinalKind::terminal : MinalKind::nonterminal;
				minals.push_back(Minal{kind, line.substr(minalStart, i)});
			}
		} else {
			throw lineErr(lineNum, "bad line layout", line);
		}
		lineNum++;
	}

	return grammar;
}

bool Grammar::validate() {
	set<string> nonTerminals;
	for (auto production : productions) {
		nonTerminals.insert(production.head);
	}
	for (auto production : productions) {
		for (auto minal : production.body) {
			if (minal.kind == MinalKind::terminal) continue;
			if (nonTerminals.count(minal.heart) == 0) return false;
		}
	}
	return true;
}
