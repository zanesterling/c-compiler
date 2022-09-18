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
  if (grammar.validate()) {
    cout << "grammar \"" << fn << "\" valid!" << endl;
  } else {
    cout << "grammar \"" << fn << "\" invalid :(" << endl;
  }
  cout << endl;

  vector<LexedToken> tokens;
  try {
    tokens = grammar.lex("grammar/expr.txt");
  } catch (runtime_error e) {
    cout << e.what() << endl;
    return 1;
  }
  for (auto tk : tokens) {
    cout << tk.name << " \"" << tk.contents << "\"" << endl;
  }
  return 0;
}
