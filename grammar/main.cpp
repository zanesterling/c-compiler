#include "grammar.h"

using namespace std;

int main() {
	auto fn = "grammar/expr.gram";
	Grammar grammar;
	try {
		grammar = move(Grammar::parse(fn));
	} catch (runtime_error e) {
		cout << e.what() << endl;
		return 1;
	}
	if (!grammar.validate()) {
		cout << "grammar \"" << fn << "\" valid!" << endl;
	} else {
		cout << "grammar \"" << fn << "\" invalid :(" << endl;
	}
	return 0;
}
