#include "debug.h"
#include "grammar.h"

using namespace std;

int test_case(Grammar &grammar, string fn);


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

  test_case(grammar, "test-cases/0001_minimal-program.c");
  test_case(grammar, "test-cases/0002_variables.c");
  test_case(grammar, "test-cases/0003_assignment.c");
  test_case(grammar, "test-cases/0004_initialization.c");
}

int test_case(Grammar &grammar, string fn) {
  vector<LexedToken> tokens;
  try {
    tokens = grammar.lex(fn);
  } catch (runtime_error e) {
    cout << "🟥" << fn << endl;
    cout << e.what() << endl;
    return 1;
  }
  cout << "🟩" << fn << endl;
  for (auto tk : tokens) {
    DEBUG(cout << tk.name << " \"" << tk.contents << "\"" << endl);
  }
  return 0;
}
