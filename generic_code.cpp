#include <optional>
#include <regex>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

static const bool _DEBUG = false;
#define DEBUG(x) if (_DEBUG) { x; }

bool matchstr(string &s, string &line, int i);

typedef int TokenType;
class Token {
 public:
  const TokenType type;
  const string heart;
};

typedef optional<Token> (*KwMatchFunc)(int lineNum, string &line, int &i);


/* INSERT GENERATED CODE HERE */


// ===== GENERIC CODE ===== //
#include <fstream>
#include <iostream>
#include <stdexcept>

runtime_error lineErr(int lineNum, string msg, int i, string line) {
  auto errMsg = to_string(lineNum) + "at index " + to_string(i) + ": "
                + msg + ": \"" + line + "\"";
  return runtime_error(msg);
}

bool matchstr(string &s, string &line, int i) {
  // TODO: If you need to match multiline strings, this will need work.
  // NOTE: This does not account for escape characters (null, tab),
  //       but they shouldn't show up in a keyword.
  return line.compare(i, s.size(), s)==0;
}

void skipAnySpaces(string& line, int& i) {
  for (; i < line.size() && isspace(line[i]); i++);
}

vector<Token> lex(ifstream& f) {
  vector<Token> tokens;
  string line;
  int lineNum = 0;
  while (getline(f, line)) {
    lineNum++;
    int i = 0;
    while (i < line.size()) {
      skipAnySpaces(line, i);
      if (i == line.size()) break;

      for (auto kw_match_func : kw_match_funcs) {
        optional<Token> optToken = kw_match_func(lineNum, line, i);
        if (!optToken.has_value()) continue;
        DEBUG(cout << "found kw token: " << optToken.value().heart << endl);
        tokens.push_back(optToken.value());
        goto foundMatch;
      }

      // TODO: foreach regex token, try parse token
      for (auto rx_match_func : rx_match_funcs) {
        optional<Token> optToken = rx_match_func(lineNum, line, i);
        if (!optToken.has_value()) continue;
        DEBUG(cout << "found rg token: " << optToken.value().heart << endl);
        tokens.push_back(optToken.value());
        goto foundMatch;
      }

      throw lineErr(lineNum, "could not find token match", i, line);
   foundMatch:;
    }
  }

  return tokens;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: %s SOURCE_FILE.c", argv[0]);
  }

  ifstream f((string(argv[1])));
  vector<Token> tokens = lex(f);
  for (auto tok : tokens) cout << " " << tok.heart;
  cout << endl;
  return 0;
}
