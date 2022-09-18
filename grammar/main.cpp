#include "grammar.h"

using namespace std;

int main() {
  auto fn = "test-cases/c.gram";
  Grammar grammar;
  try {
    grammar = move(Grammar::fromFile(fn));
  } catch (runtime_error e) {
    cout << e.what() << endl;
    return 1;
  }
  if (grammar.validate()) {
    cout << "grammar \"" << fn << "\" valid!" << endl;
  } else {
    cout << "grammar \"" << fn << "\" invalid :(" << endl;
    return 1;
  }
  cout << endl;

  vector<LexedToken> tokens;
  try {
    tokens = grammar.lex("test-cases/0001_minimal-program.c");
  } catch (runtime_error e) {
    cout << e.what() << endl;
    return 1;
  }
  for (auto tk : tokens) {
    cout << tk.name << " \"" << tk.contents << "\"" << endl;
  }
  return 0;
}
